/*
 *     Platform:          Windows
 *     Compiler:          MinGW gcc version 13.1.0 x86_64-posix-seh-rev1
 *     Cpp:               requires standard c++20
 *     Headers:           FLTK headers, star.h, binary_tree.h
 *     Sources:           main.cpp, star.cpp, binary_tree.cpp
 *     Static libraries:  FLTK libraries
 *     Dynamic libraries: gdiplus, comctl32, ws2_32
 */

#include "binary_tree.h"
#include "star.h"

constexpr bool showHomeworkOne = true;
constexpr bool showHomeworkTwo = true;

/* See README for more information */

int main()
{
    if constexpr (showHomeworkOne)
    {
        StarWindow *window = new StarWindow(1280, 720, "Star");
        window->show();
    }
    if constexpr (showHomeworkTwo)
    {
        BinaryTreeWindow *window = new BinaryTreeWindow(1280, 720, "Binary Tree");
        window->show();
    }
    return Fl::run();
}