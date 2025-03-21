cmake_minimum_required(VERSION 3.27)
project(VisualQuadtree)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-static)
# add_definitions(-D DEBUG)

include_directories(VisualQuadtree)
include_directories(.)
include_directories(./include)

aux_source_directory(. SOURCES)
aux_source_directory(./src SOURCES)

file(GLOB LIBRARIES "./lib/*.a")

set(EXECUTABLE_OUTPUT_PATH ..)
add_executable(VisualQuadtree ${SOURCES})
set_property(TARGET VisualQuadtree PROPERTY WIN32_EXECUTABLE true)

target_link_libraries(VisualQuadtree PRIVATE ${LIBRARIES})

target_link_directories(VisualQuadtree PRIVATE .)
target_link_libraries(VisualQuadtree PRIVATE gdiplus)
target_link_libraries(VisualQuadtree PRIVATE comctl32)
target_link_libraries(VisualQuadtree PRIVATE ws2_32)