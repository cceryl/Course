#include "TLB.h"

#include <stdio.h>
#include <stdlib.h>

static bool TLB_entry_equal(LRU_data *entry_1, LRU_data *entry_2)
{
    TLB_entry_t *a = (TLB_entry_t *)entry_1;
    TLB_entry_t *b = (TLB_entry_t *)entry_2;
    return a->page_number == b->page_number;
}

static void print_TLB_entry(LRU_data *entry)
{
    TLB_entry_t *TLB_entry = (TLB_entry_t *)entry;
    printf("  Page number: %zu, Frame number: %zu\n", TLB_entry->page_number, TLB_entry->frame_number);
}

void TLB_create(TLB_t *TLB, size_t capacity)
{
    TLB->entries = (TLB_entry_t *)malloc(capacity * sizeof(TLB_entry_t));
    for (size_t i = 0; i < capacity; ++i)
    {
        TLB->entries[i].page_number  = (size_t)-1;
        TLB->entries[i].frame_number = (size_t)-1;
        TLB->entries[i].valid        = false;
    }

    LRU_cache_create(&TLB->cache, capacity, TLB_entry_equal);
    TLB->hit_count  = 0;
    TLB->miss_count = 0;
}

void TLB_destroy(TLB_t *TLB)
{
    free(TLB->entries);
    TLB->entries = NULL;
    LRU_cache_destroy(&TLB->cache);
}

void TLB_clear(TLB_t *TLB)
{
    for (size_t i = 0; i < TLB->cache.capacity; ++i)
    {
        TLB->entries[i].page_number  = (size_t)-1;
        TLB->entries[i].frame_number = (size_t)-1;
        TLB->entries[i].valid        = false;
    }

    LRU_cache_clear(&TLB->cache);
    TLB->hit_count  = 0;
    TLB->miss_count = 0;
}

int TLB_query(TLB_t *TLB, size_t page_number)
{
    TLB_entry_t  key        = {page_number, 0, false};
    TLB_entry_t *query_data = LRU_cache_query(&TLB->cache, &key);
    if (query_data == NULL)
    {
        ++TLB->miss_count;
        return -1;
    }

    ++TLB->hit_count;
    return query_data->frame_number;
}

void TLB_insert(TLB_t *TLB, size_t page_number, size_t frame_number)
{
    TLB_entry_t *entry_to_insert = NULL;

    if (TLB_full(TLB))
    {
        TLB_entry_t *least_recently_used = LRU_cache_least_recently_used(&TLB->cache);
        LRU_cache_remove(&TLB->cache, least_recently_used);
        entry_to_insert = least_recently_used;
    }
    else
        for (size_t i = 0; i < TLB->cache.capacity; ++i)
            if (!TLB->entries[i].valid)
            {
                entry_to_insert = &TLB->entries[i];
                break;
            }

    entry_to_insert->page_number  = page_number;
    entry_to_insert->frame_number = frame_number;
    entry_to_insert->valid        = true;

    LRU_cache_insert(&TLB->cache, entry_to_insert);
}

bool TLB_remove(TLB_t *TLB, size_t page_number)
{
    TLB_entry_t  key           = {page_number, 0, false};
    TLB_entry_t *removed_entry = LRU_cache_remove(&TLB->cache, &key);
    if (removed_entry == NULL)
        return false;

    removed_entry->page_number  = -1;
    removed_entry->frame_number = -1;
    removed_entry->valid        = false;
    return true;
}

size_t TLB_size(TLB_t *TLB) { return LRU_cache_size(&TLB->cache); }

size_t TLB_capacity(TLB_t *TLB) { return LRU_cache_capacity(&TLB->cache); }

bool TLB_empty(TLB_t *TLB) { return LRU_cache_empty(&TLB->cache); }

bool TLB_full(TLB_t *TLB) { return LRU_cache_full(&TLB->cache); }

size_t TLB_hit_count(TLB_t *TLB) { return TLB->hit_count; }

size_t TLB_miss_count(TLB_t *TLB) { return TLB->miss_count; }

void TLB_print(TLB_t *TLB)
{
    printf("TLB:\n");
    for (size_t i = 0; i < TLB->cache.capacity; ++i)
    {
        printf("  Entry %zu: ", i);
        if (TLB->entries[i].valid)
            printf("Page number: %zu, Frame number: %zu\n", TLB->entries[i].page_number, TLB->entries[i].frame_number);
        else
            printf("Invalid\n");
    }
    printf("Internal LRU cache:\n");
    LRU_cache_print(&TLB->cache, print_TLB_entry);
}