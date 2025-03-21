cmake_minimum_required(VERSION 3.27)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(main)
include_directories(.)

aux_source_directory(. SOURCES)
aux_source_directory(./main SOURCES)

file(GLOB LIBRARIES "./lib/*.a")

set(EXECUTABLE_OUTPUT_PATH ../main)
add_executable(main ${SOURCES})

target_link_libraries(main PRIVATE ${LIBRARIES})

target_link_directories(main PRIVATE .)
target_link_libraries(main PRIVATE gdiplus)
target_link_libraries(main PRIVATE comctl32)
target_link_libraries(main PRIVATE ws2_32)