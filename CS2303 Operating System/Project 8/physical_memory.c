#include "physical_memory.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void physical_memory_create(physical_memory_t *memory, void *memory_ptr, size_t memory_size, size_t frame_size)
{
    memory->memory      = memory_ptr;
    memory->memory_size = memory_size;

    memory->frames = (physical_memory_frame_t *)malloc(memory_size / frame_size * sizeof(physical_memory_frame_t));
    for (int i = 0; i < memory_size / frame_size; ++i)
    {
        memory->frames[i].frame_number = i;
        memory->frames[i].address      = i * frame_size;
        memory->frames[i].valid        = false;
        memory->frames[i].dirty        = false;
    }
    memory->frame_size = frame_size;
}

void physical_memory_destroy(physical_memory_t *memory)
{
    free(memory->frames);
    memory->memory      = NULL;
    memory->frames      = NULL;
    memory->memory_size = 0;
    memory->frame_size  = 0;
}

void physical_memory_read(physical_memory_t *memory, size_t frame_number, ptrdiff_t offset, void *buffer, size_t byte_size)
{
    memcpy(buffer, physical_memory_get_frame_address(memory, frame_number) + offset, byte_size);
}

void physical_memory_write(physical_memory_t *memory, size_t frame_number, ptrdiff_t offset, const void *buffer, size_t byte_size)
{
    memcpy(physical_memory_get_frame_address(memory, frame_number) + offset, buffer, byte_size);
    memory->frames[frame_number].dirty = true;
}

physical_memory_frame_t *physical_memory_get_frame(physical_memory_t *memory, int frame_number) { return &memory->frames[frame_number]; }

void *physical_memory_get_frame_address(physical_memory_t *memory, int frame_number)
{
    return memory->memory + memory->frames[frame_number].address;
}

bool physical_memory_swap_frame_data(physical_memory_t *memory, int frame_number, void *data_swap_buffer)
{
    memory->frames[frame_number].valid = true;
    memory->frames[frame_number].dirty = false;
    if (memory->frames[frame_number].valid && memory->frames[frame_number].dirty)
    {
        uint8_t *data_ptr    = physical_memory_get_frame_address(memory, frame_number);
        uint8_t *temp_buffer = malloc(memory->frame_size);
        memcpy(temp_buffer, data_ptr, memory->frame_size);
        memcpy(data_ptr, data_swap_buffer, memory->frame_size);
        memcpy(data_swap_buffer, temp_buffer, memory->frame_size);
        free(temp_buffer);
        return true;
    }
    else
    {
        memcpy(data_swap_buffer, physical_memory_get_frame_address(memory, frame_number), memory->frame_size);
        return false;
    }
}