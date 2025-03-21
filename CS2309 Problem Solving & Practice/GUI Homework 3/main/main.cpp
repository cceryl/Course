/*
 *     Platform:          Windows
 *     Compiler:          MinGW gcc version 13.1.0 x86_64-posix-seh-rev1
 *     Cpp:               requires standard c++20
 *     Headers:           FLTK headers, image_button.h, analog_clock.h
 *     Sources:           main.cpp, image_button.cpp, analog_clock.cpp
 *     Static libraries:  FLTK libraries
 *     Dynamic libraries: gdiplus, comctl32, ws2_32
 */

constexpr bool showHomeworkOne = true;
constexpr bool showHomeworkTwo = true;

#include "analog_clock.h"
#include "image_button.h"

/* See README for more information */

int main()
{
    if constexpr (showHomeworkOne)
    {
        ImageButtonWindow *imageButtonWindow = new ImageButtonWindow(800, 600, "Image Button");
        imageButtonWindow->show();
    }
    if constexpr (showHomeworkTwo)
    {
        ClockWindow *clockWindow = new ClockWindow(800, 600, "Analog Clock");
        clockWindow->show();
    }
    return Fl::run();
}