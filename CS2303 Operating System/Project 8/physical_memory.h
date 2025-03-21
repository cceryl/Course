#pragma once

#include <stdbool.h>
#include <stddef.h>

/// @brief Memory frame structure
typedef struct physical_memory_frame
{
    int       frame_number;
    ptrdiff_t address;
    bool      valid;
    bool      dirty;
} physical_memory_frame_t;

/// @brief Physical memory structure
typedef struct physical_memory
{
    void                    *memory;
    size_t                   memory_size;
    physical_memory_frame_t *frames;
    size_t                   frame_size;
} physical_memory_t;

/// @brief Create physical memory
void physical_memory_create(physical_memory_t *memory, void *memory_ptr, size_t memory_size, size_t frame_size);

/// @brief Destroy physical memory
void physical_memory_destroy(physical_memory_t *memory);

/// @brief Read from a memory frame, do not check address range and validity
void physical_memory_read(physical_memory_t *memory, size_t frame_number, ptrdiff_t offset, void *buffer, size_t byte_size);

/// @brief Write to a memory frame, do not check address range and validity
void physical_memory_write(physical_memory_t *memory, size_t frame_number, ptrdiff_t offset, const void *buffer, size_t byte_size);

/// @brief Get physical memory frame
physical_memory_frame_t *physical_memory_get_frame(physical_memory_t *memory, int frame_number);

/// @brief Get physical memory frame real address
void *physical_memory_get_frame_address(physical_memory_t *memory, int frame_number);

/// @brief If the frame is valid and dirty, swap the frame data with the swap buffer, otherwise write the swap buffer to the frame
/// @return True if the swap buffer holds the data of the frame, false otherwise
bool physical_memory_swap_frame_data(physical_memory_t *memory, int frame_number, void *data_swap_buffer);