#include "virtual_memory.h"

static bool page_table_entry_equal(void *entry_1, void *entry_2)
{
    virtual_memory_page_table_entry_t *a = (virtual_memory_page_table_entry_t *)entry_1;
    virtual_memory_page_table_entry_t *b = (virtual_memory_page_table_entry_t *)entry_2;
    return a->page_number == b->page_number;
}

void virtual_memory_create(virtual_memory_t *virtual_memory, size_t page_size, size_t page_table_capacity, size_t TLB_capacity,
                           size_t physical_memory_capacity, void *physical_memory_ptr, const char *backing_store_filename)
{
    virtual_memory->page_size  = page_size;
    virtual_memory->page_table = malloc(page_table_capacity * sizeof(virtual_memory_page_table_entry_t));
    for (size_t i = 0; i < page_table_capacity; ++i)
    {
        virtual_memory->page_table[i].page_number  = i;
        virtual_memory->page_table[i].frame_number = (size_t)-1;
        virtual_memory->page_table[i].valid        = false;
    }
    virtual_memory->page_table_capacity = page_table_capacity;

    LRU_cache_create(&virtual_memory->page_cache, physical_memory_capacity / page_size, page_table_entry_equal);
    TLB_create(&virtual_memory->TLB, TLB_capacity);
    physical_memory_create(&virtual_memory->physical_memory, physical_memory_ptr, physical_memory_capacity, page_size);
    virtual_memory->backing_store = fopen(backing_store_filename, "rb");

    virtual_memory->page_faults    = 0;
    virtual_memory->total_accesses = 0;

    virtual_memory->log_enabled = false;
    virtual_memory->log_file    = NULL;
}

void virtual_memory_destroy(virtual_memory_t *virtual_memory)
{
    for (size_t i = 0; i < virtual_memory->page_table_capacity; ++i)
    {
        if (!virtual_memory->page_table[i].valid)
            continue;
        physical_memory_frame_t *frame =
            physical_memory_get_frame(&virtual_memory->physical_memory, virtual_memory->page_table[i].frame_number);
        if (frame->valid && frame->dirty)
        {
            fseek(virtual_memory->backing_store, virtual_memory->page_table[i].page_number * virtual_memory->page_size, SEEK_SET);
            fwrite(physical_memory_get_frame_address(&virtual_memory->physical_memory, frame->frame_number), virtual_memory->page_size, 1,
                   virtual_memory->backing_store);
        }
    }

    free(virtual_memory->page_table);
    LRU_cache_destroy(&virtual_memory->page_cache);
    TLB_destroy(&virtual_memory->TLB);
    physical_memory_destroy(&virtual_memory->physical_memory);
    fclose(virtual_memory->backing_store);
    if (virtual_memory->log_file != NULL)
        fclose(virtual_memory->log_file);
}

int8_t virtual_memory_read(virtual_memory_t *virtual_memory, ptrdiff_t address)
{
    ptrdiff_t physical_address = virtual_memory_get_physical_address(virtual_memory, address);
    size_t    frame_number     = physical_address / virtual_memory->page_size;
    ptrdiff_t offset           = physical_address % virtual_memory->page_size;

    int8_t value;
    physical_memory_read(&virtual_memory->physical_memory, frame_number, offset, &value, sizeof(int8_t));
    if (virtual_memory->log_enabled)
        fprintf(virtual_memory->log_file, "Virtual address: %td Physical address: %td Value: %d\n", address, physical_address, value);
    return value;
}

void virtual_memory_write(virtual_memory_t *virtual_memory, ptrdiff_t address, int8_t value)
{
    ptrdiff_t physical_address = virtual_memory_get_physical_address(virtual_memory, address);
    size_t    frame_number     = physical_address / virtual_memory->page_size;
    ptrdiff_t offset           = physical_address % virtual_memory->page_size;

    physical_memory_write(&virtual_memory->physical_memory, frame_number, offset, &value, sizeof(int8_t));
    if (virtual_memory->log_enabled)
        fprintf(virtual_memory->log_file, "Virtual address: %td Physical address: %td Value: %d\n", address, physical_address, value);
}

ptrdiff_t virtual_memory_get_physical_address(virtual_memory_t *virtual_memory, ptrdiff_t address)
{
    size_t page_number = (address & 0xFF00) >> 8;
    size_t offset      = address & 0x00FF;
    ++virtual_memory->total_accesses;

    int TLB_query_frame = TLB_query(&virtual_memory->TLB, page_number);
    if (TLB_query_frame != -1)
        return TLB_query_frame * virtual_memory->page_size + offset;

    virtual_memory_page_table_entry_t  key                    = {page_number, 0, false};
    virtual_memory_page_table_entry_t *page_table_query_frame = LRU_cache_query(&virtual_memory->page_cache, &key);
    if (page_table_query_frame != NULL)
    {
        TLB_insert(&virtual_memory->TLB, page_number, page_table_query_frame->frame_number);
        return page_table_query_frame->frame_number * virtual_memory->page_size + offset;
    }

    ++virtual_memory->page_faults;
    physical_memory_frame_t *frame_to_insert = NULL;
    if (LRU_cache_full(&virtual_memory->page_cache))
    {
        virtual_memory_page_table_entry_t *least_recently_used_frame = LRU_cache_least_recently_used(&virtual_memory->page_cache);
        physical_memory_frame_t           *frame_to_evict =
            physical_memory_get_frame(&virtual_memory->physical_memory, least_recently_used_frame->frame_number);

        if (frame_to_evict->dirty)
        {
            fseek(virtual_memory->backing_store, least_recently_used_frame->page_number * virtual_memory->page_size, SEEK_SET);
            fwrite(physical_memory_get_frame_address(&virtual_memory->physical_memory, frame_to_evict->frame_number),
                   virtual_memory->page_size, 1, virtual_memory->backing_store);
            frame_to_evict->dirty = false;
        }

        LRU_cache_remove(&virtual_memory->page_cache, least_recently_used_frame);
        TLB_remove(&virtual_memory->TLB, least_recently_used_frame->page_number);
        least_recently_used_frame->frame_number = (size_t)-1;
        least_recently_used_frame->valid        = false;
        frame_to_insert                         = frame_to_evict;
    }
    else
        for (size_t i = 0; i < virtual_memory->physical_memory.memory_size / virtual_memory->page_size; ++i)
        {
            physical_memory_frame_t *frame = physical_memory_get_frame(&virtual_memory->physical_memory, i);
            if (!frame->valid)
            {
                frame_to_insert = frame;
                break;
            }
        }

    fseek(virtual_memory->backing_store, page_number * virtual_memory->page_size, SEEK_SET);
    fread(physical_memory_get_frame_address(&virtual_memory->physical_memory, frame_to_insert->frame_number), virtual_memory->page_size, 1,
          virtual_memory->backing_store);
    virtual_memory->page_table[page_number].frame_number = frame_to_insert->frame_number;
    virtual_memory->page_table[page_number].valid        = true;

    frame_to_insert->valid = true;
    frame_to_insert->dirty = false;
    LRU_cache_insert(&virtual_memory->page_cache, &virtual_memory->page_table[page_number]);
    TLB_insert(&virtual_memory->TLB, page_number, frame_to_insert->frame_number);

    return frame_to_insert->frame_number * virtual_memory->page_size + offset;
}

void virtual_memory_set_log(virtual_memory_t *virtual_memory, bool enabled, const char *filename)
{
    if (virtual_memory->log_file != NULL)
        fclose(virtual_memory->log_file);

    virtual_memory->log_enabled = enabled;
    if (enabled)
        virtual_memory->log_file = fopen(filename, "a");
    else
        virtual_memory->log_file = NULL;
}