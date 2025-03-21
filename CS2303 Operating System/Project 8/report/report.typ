#set align(center + horizon)
#set text(size: 40pt)
= Project 8
#set text(size: 24pt)
Virtual Memory Manager\
#set align(start + top)
#set text(size: 14pt)
#pagebreak(weak: true)

#set page(header: context{
  if counter(page).get().first() > 1 [
    *Project 8*
    #h(1fr)
    _Virtual Memory Manager_
  ]
}, numbering: "1")
#counter(page).update(1)

= Introduction
In this project, I implemented a virtual memory manager that provides read and write access to a virtual address space. The main program uses the virtual memory manager to translate virtual addresses in file `addresses.txt` to physical addresses and read raw data in file `BACKING_STORE.bin`. The program then writes the virtual address, translated physical address, and the value stored at the physical address to file `answers.txt`.\

The virtual memory manager:
- Use page table to translate virtual addresses to physical addresses.
- Use TLB to accelerate the translation.
- Use frame table to manage the physical memory.
- Use LRU algorithm to replace TLB entries when TLB miss occurs.
- Use LRU algorithm to replace frames when page fault occurs.
- Use dirty bit to track the modification of frames.
- Use an on-stack buffer to simulate the physical memory.
#figure(
  image("correctness.png", width: 100%),
  caption: "Correctness of the virtual memory manager"
)
#pagebreak(weak: true)

= Overview
The virtual memory manager consists of the following components:
- LRU cache: a generic LRU cache implemented using a doubly linked list.
- TLB: a cache that stores the most recent translations from virtual addresses to physical addresses.
- Physical memory: a memory manager that provides block read and write operations.
- Virtual memory: a memory manager that implements the page table and integrates the TLB and physical memory.
The structure of the virtual memory manager is shown below:
#figure(
  image("structure.png", width: 100%),
  caption: "Structure of the project"
)
#pagebreak(weak: true)

= Implementation
== LRU Cache
The LRU cache is implemented using a circular doubly linked list. The node closest to the head is the least recently used, and the node closest to the tail is the most recently used. In order to support different types of data, the LRU cache is implemented as a generic class using `void *` to represent data and requires a comparison function to compare two data items.\

The data structure of the LRU cache is:
```c
typedef void LRU_data;
typedef bool (*equal_t)(LRU_data *a, LRU_data *b);

/// @brief LRU Cache node
typedef struct LRU_cache_node
{
    LRU_data              *data;
    struct LRU_cache_node *prev;
    struct LRU_cache_node *next;
} LRU_cache_node_t;

/// @brief LRU Cache
typedef struct LRU_cache
{
    LRU_cache_node_t *head;
    size_t            size;
    size_t            capacity;
    equal_t           data_equal;
} LRU_cache_t;
```

The LRU cache provides the following main operations:
```c
/// @brief Query the cache for the cache data that matches the key
/// @return The cache data if found, NULL otherwise
LRU_data *LRU_cache_query(LRU_cache_t *LRU, LRU_data *key);
/// @brief Insert the data into the LRU cache
///        If there exists equivalent data, it will be updated
/// @return Removed data if the LRU cache is full, NULL otherwise
LRU_data *LRU_cache_insert(LRU_cache_t *LRU, LRU_data *data);
/// @brief Get the LRU data, will not update the LRU cache
/// @return The LRU if the LRU cache is not empty, NULL otherwise
LRU_data *LRU_cache_least_recently_used(LRU_cache_t *LRU);
/// @brief Remove the data from the LRU cache
/// @return The removed data if found, NULL otherwise
LRU_data *LRU_cache_remove(LRU_cache_t *LRU, LRU_data *key);
```

The query function is implemented by traversing the list and using the comparison function to compare the data. If the data is found, it is moved to the tail of the list to indicate that it is the most recently used.\

The insert function first queries the cache for the data. If the data is found, it is updated and moved to the tail of the list. The function then checks if the cache is full. If so, the least recently used data is removed from the head of the list. The new data is then inserted at the tail of the list.\

The least recently used function returns the data at the head of the list without updating the list.\

The remove function is similar to the query function but removes the data from the list if found.\

== TLB
The TLB is implemented as a cache that stores the most recent translations from virtual addresses to physical addresses. The TLB is implemented using the LRU cache. The data structure of the TLB is:
```c
/// @brief TLB entry structure, the "LRU_data" in LRU cache
typedef struct TLB_entry
{
    size_t page_number;
    size_t frame_number;
    bool   valid;
} TLB_entry_t;

/// @brief TLB structure
typedef struct TLB
{
    TLB_entry_t *entries;
    LRU_cache_t  cache;
    size_t       hit_count;
    size_t       miss_count;
} TLB_t;
```

The TLB provides the following main operations:
```c
/// @brief Query for the frame number of a page number in the TLB
/// @return The frame number if TLB hits, -1 otherwise
int TLB_query(TLB_t *TLB, size_t page_number);

/// @brief Insert an entry into the TLB
void TLB_insert(TLB_t *TLB, size_t page_number, size_t frame_number);

/// @brief Remove an entry from the TLB
/// @return True if the page number was removed
///         False if such page number was not in the TLB
bool TLB_remove(TLB_t *TLB, size_t page_number);

/// @brief The equal function for TLB entries
static bool TLB_entry_equal(LRU_data *entry_1, LRU_data *entry_2)
{
    TLB_entry_t *a = (TLB_entry_t *)entry_1;
    TLB_entry_t *b = (TLB_entry_t *)entry_2;
    return a->page_number == b->page_number;
}
```

The query function constructs a key as ```{page_number, 0, false}``` and queries the LRU cache. If the LRU cache hits, the frame number is returned. Otherwise, -1 is returned.\

The insert function first judges if the LRU cache is full. If so, the least recently used entry is removed from the LRU cache and marked as "entry to insert", otherwise it traverses the entries to find an invalid entry to be "entry to insert". Then the "entry to insert" is initialized with parameters and inserted into the LRU cache.\

The remove function constructs a key as ```{page_number, 0, false}``` and calls the remove function of the LRU cache.\

== Physical Memory
The physical memory is implemented as a memory manager that provides read and write operations of frames. The physical memory is implemented using an on-stack buffer given as a parameter. The data structure of the physical memory is:
```c
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
```

The physical memory provides the following main operations:
```c
/// @brief Read from a memory frame, do not check address range and validity
void physical_memory_read(physical_memory_t *memory, size_t frame_number, ptrdiff_t offset, void *buffer, size_t byte_size);

/// @brief Write to a memory frame, do not check address range and validity
void physical_memory_write(physical_memory_t *memory, size_t frame_number, ptrdiff_t offset, const void *buffer, size_t byte_size);
```

The read function reads data from the memory buffer at the specified frame number and offset.\

The write function writes data to the memory buffer at the specified frame number and offset. The dirty bit of the frame is set to true after writing.\

== Virtual Memory
The virtual memory is the core component of the virtual memory manager. It integrates the LRU cache, TLB, and physical memory to provide read and write operations of virtual addresses. It holds a page table managed by LRU cache. In order to write answers, the virtual memory manager also holds a file pointer to the `answers.txt` file. The data structure of the virtual memory is:
```c
/// @brief The virtual memory page table entry
typedef struct virtual_memory_page_table_entry
{
    size_t page_number;
    size_t frame_number;
    bool   valid;
} virtual_memory_page_table_entry_t;

/// @brief The virtual memory
typedef struct virtual_memory
{
    size_t                             page_size;
    virtual_memory_page_table_entry_t *page_table;
    size_t                             page_table_capacity;

    LRU_cache_t       page_cache;
    TLB_t             TLB;
    physical_memory_t physical_memory;
    FILE             *backing_store;

    size_t page_faults;
    size_t total_accesses;

    bool  log_enabled;
    FILE *log_file;
} virtual_memory_t;
```

The virtual memory provides the following main operations:
```c
/// @brief Read a byte from the virtual memory
int8_t virtual_memory_read(virtual_memory_t *virtual_memory, ptrdiff_t address);

/// @brief Write a byte to the virtual memory, do nothing if the logical address is invalid
void virtual_memory_write(virtual_memory_t *virtual_memory, ptrdiff_t address, int8_t value);

/// @brief Get physical address from virtual address
ptrdiff_t virtual_memory_get_physical_address(virtual_memory_t *virtual_memory, ptrdiff_t address);
```

The read function calls the `virtual_memory_get_physical_address` function to get the physical address and calls the `physical_memory_read` function to read the data from the physical memory. If log is enabled, the function writes the log to the log file.\

The write function calls the `virtual_memory_get_physical_address` function to get the physical address and calls the `physical_memory_write` function to write the data to the physical memory. If log is enabled, the function writes the log to the log file.\

The get physical address function is the core function of the virtual memory manager. The detailed implementation is as follows:
```c
ptrdiff_t virtual_memory_get_physical_address(virtual_memory_t *virtual_memory, ptrdiff_t address)
{
  // Extract page number and offset according to the requirement
  size_t page_number = (address & 0xFF00) >> 8;
  size_t offset      = address & 0x00FF;
  ++virtual_memory->total_accesses;

  // Query TLB for the frame number
  // If hit, return the physical address
  int TLB_query_frame = 
    TLB_query(&virtual_memory->TLB, page_number);
  if (TLB_query_frame != -1)
    return TLB_query_frame * virtual_memory->page_size + offset;

  // Query page table for the frame number
  // If no page fault occurs, insert the entry into TLB
  // Then return the physical address
  virtual_memory_page_table_entry_t key = {page_number, 0, false};
  virtual_memory_page_table_entry_t *page_table_query_frame =
    LRU_cache_query(&virtual_memory->page_cache, &key);
  if (page_table_query_frame != NULL)
  {
    TLB_insert(&virtual_memory->TLB, page_number,
               page_table_query_frame->frame_number);
    return page_table_query_frame->frame_number *
           virtual_memory->page_size + offset;
  }

  // TLB and page table miss, page fault occurs
  // Evict the least recently used frame
  // If the frame is dirty, write back to the backing store
  // Then read the page from the backing store
  // Update the page table and TLB
  // Return the physical address
  ++virtual_memory->page_faults;

  // Find the frame to insert
  // If the LRU cache is full, evict the least recently used frame
  // Otherwise, find an invalid frame
  physical_memory_frame_t *frame_to_insert = NULL;

  // If the LRU cache is full, evict the least recently used frame
  if (LRU_cache_full(&virtual_memory->page_cache))
  {
    // Find the least recently used frame
    virtual_memory_page_table_entry_t *least_recently_used_frame =
      LRU_cache_least_recently_used(&virtual_memory->page_cache);
    physical_memory_frame_t *frame_to_evict =
      physical_memory_get_frame(&virtual_memory->physical_memory,
                         least_recently_used_frame->frame_number);

    // Write back to the backing store if the frame is dirty
    if (frame_to_evict->dirty)
    {
      fseek(virtual_memory->backing_store,
            least_recently_used_frame->page_number * 
            virtual_memory->page_size, SEEK_SET);
      fwrite(physical_memory_get_frame_address(
               &virtual_memory->physical_memory,
               frame_to_evict->frame_number),
             virtual_memory->page_size, 1, 
             virtual_memory->backing_store);
      frame_to_evict->dirty = false;
    }

    // Evict the least recently used frame and reset its value
    LRU_cache_remove(&virtual_memory->page_cache, 
                     least_recently_used_frame);
    TLB_remove(&virtual_memory->TLB, 
               least_recently_used_frame->page_number);
    least_recently_used_frame->frame_number = (size_t)-1;
    least_recently_used_frame->valid        = false;
    frame_to_insert                         = frame_to_evict;
    }
  // Otherwise, the LRU cache is not full
  else
  {
    // Traverse the physical memory to find an invalid frame
    for (size_t i = 0; 
         i < virtual_memory->physical_memory.memory_size / 
         virtual_memory->page_size; ++i)
    {
      physical_memory_frame_t *frame = 
        physical_memory_get_frame(
          &virtual_memory->physical_memory, i);
      if (!frame->valid)
      {
        frame_to_insert = frame;
        break;
      }
    }
  }

  // The frame to insert is found
  // Read the page from the backing store
  // Then update the page table
  fseek(virtual_memory->backing_store, 
        page_number * virtual_memory->page_size, SEEK_SET);
  fread(physical_memory_get_frame_address(
          &virtual_memory->physical_memory, 
          frame_to_insert->frame_number), 
        virtual_memory->page_size, 1,
        virtual_memory->backing_store);
  virtual_memory->page_table[page_number].frame_number = 
    frame_to_insert->frame_number;
  virtual_memory->page_table[page_number].valid = true;

  // Insert the entry into the LRU cache and TLB
  frame_to_insert->valid = true;
  frame_to_insert->dirty = false;
  LRU_cache_insert(&virtual_memory->page_cache,
                   &virtual_memory->page_table[page_number]);
  TLB_insert(&virtual_memory->TLB, page_number, 
             frame_to_insert->frame_number);

  // Return the physical address
  return frame_to_insert->frame_number * 
         virtual_memory->page_size + offset;
}
```

== Main Program
The main program reads the virtual addresses from the `addresses.txt` file and calls the `virtual_memory_read` function to read the data from the virtual memory. The main program then writes the virtual address, translated physical address, and the value stored at the physical address to the `answers.txt` file by setting the log file of the virtual memory manager. The main program also prints the page fault rate and TLB hit rate to the console. The program finally checks the correctness of the virtual memory manager by comparing `answers.txt` with `correct.txt`.\
#pagebreak(weak: true)

= Correctness
The correctness of the virtual memory manager is tested by comparing the output of the main program with the correct output. The correctness test is performed by running the main program with the provided `BACKING_STORE.bin`, `addresses.txt`, and `correct.txt`. The correctness test is passed if the output of the main program matches the correct output.\

The correctness of the virtual memory manager is shown in the figure below. Only the last few lines of the output are shown for brevity. The correctness test is passed, as the output of the virtual memory manager matches the correct output.\
#figure(
  image("output.png", width: 100%),
  caption: "The output of the virtual memory manager
   (page number: 256, frame number: 256)"
)
#figure(
  image("output_hit_rate.png", width: 100%),
  caption: "The TLB hit rate and page fault rate
   (page number: 256, frame number: 256)"
)

In the bonus test, we change the number of frames from 256 to 128.
- The "Physical address" is different from the correct answer because there will be page faults and the frames are different.
- The "Value" remains the same because the data in the backing store is the same. 
- The page fault rate is significantly higher because the number of frames is reduced. 
- The TLB hit rate is the same because the TLB is not affected by the number of frames as long as the capacity of TLB is smaller than the number of frames.
- The correctness test is not passed at line 172, where the 129th page is accessed and the manager must evict the least recently used frame.\
#figure(
  image("output_change.png", width: 100%),
  caption: "The output of the virtual memory manager
   (page number: 256, frame number: 128)"
)
#figure(
  image("output_hit_rate_change.png", width: 100%),
  caption: "The TLB hit rate and page fault rate
   (page number: 256, frame number: 128)"
)


#pagebreak(weak: true)

= Conclusion
In this project, I implemented a virtual memory manager that provides read and write access to a virtual address space.\

The virtual memory manager implements the page table, TLB, physical memory, and virtual memory, and it uses the LRU cache to manage the page table and TLB.\

The virtual memory manager is tested for correctness by comparing the output of the main program with the correct output. The correctness test is passed, as the output of the virtual memory manager matches the correct output.\

In the bonus test, we change the number of frames from 256 to 128. The same value is read from the backing store, but the physical address is different due to page faults. The page fault rate is significantly higher, and the correctness test is not passed, as is expected.\

The virtual memory manager is a fundamental component of modern operating systems and provides a convenient and efficient way to translate virtual addresses to physical addresses. The virtual memory manager plays a crucial role in providing a stable and secure environment for running applications.\

In this project, I learned how to implement a virtual memory manager and gained a deeper understanding of the memory management system in modern operating systems. I also learned how to implement template-like data structures in C and got familiar with the LRU algorithm and its applications in cache management.\

The project may be further improved by splitting the page table into a independent structure to provide more flexibility and scalability, especially when the page table may use different structures like two-level page table or inverted page table. The project may also be improved by adding more detailed logs to help debug and analyze the performance of the virtual memory manager.\