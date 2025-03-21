cmake_minimum_required(VERSION 3.27)
project(MapAreaCalculator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-static)

include_directories(MapAreaCalculator)
include_directories(.)
include_directories(./include)

aux_source_directory(. SOURCES)
aux_source_directory(./src SOURCES)

file(GLOB LIBRARIES "./lib/*.a")

set(EXECUTABLE_OUTPUT_PATH ..)
add_executable(MapAreaCalculator ${SOURCES})
set_property(TARGET MapAreaCalculator PROPERTY WIN32_EXECUTABLE true)

target_link_libraries(MapAreaCalculator PRIVATE ${LIBRARIES})

target_link_directories(MapAreaCalculator PRIVATE .)
target_link_libraries(MapAreaCalculator PRIVATE gdiplus)
target_link_libraries(MapAreaCalculator PRIVATE comctl32)
target_link_libraries(MapAreaCalculator PRIVATE ws2_32)