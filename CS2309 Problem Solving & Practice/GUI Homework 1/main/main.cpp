/*
 *     Platform:          Windows
 *     Compiler:          MinGW gcc version 13.1.0 x86_64-posix-seh-rev1
 *     Cpp:               requires standard c++20
 *     Headers:           FLTK headers, poly.h, superellipse.h
 *     Sources:           main.cpp, poly.cpp, superellipse.cpp
 *     Static libraries:  FLTK libraries
 *     Dynamic libraries: gdiplus, comctl32, ws2_32
 */

#include "poly.h"
#include "superellipse.h"

constexpr bool showHomeWorkOne = true;
constexpr bool showHomeWorkTwo = true;

int main()
{
    /*
     * README
     *     Console Window:
     *         A/B/M/N Slider:           Alter the a/b/m/n parameter of the superellipse.
     *         Points Slider:            Alter the number of curving points of the superellipse.
     *                                   Warning: changing points to over 50 may cause performance problems with All Branches on.
     *         Show All Branches Button: Toggle whether to show all branches. 
     *     Other:
     *         Window Resize:            Main window can be resized with all widgets resizing proportionally.
     */
    if constexpr (showHomeWorkOne)
    {
        SuperellipseWindow *window = new SuperellipseWindow(1280, 720, "Window Task 1");
        window->show();
    }
    /*
     * README
     *     Console Window:
     *         Vertices Browser:         This widget shows all vertices of the current polygon.
     *         X/Y Input Boxes:          Provide x and y input from text.
     *         Add Vertex Button:        Add the coordinate of x/y input boxes to the polygon.
     *                                   Default input is 0 if input box is left blank.
     *                                   Input will be read till the first invalid char.
     *         Show Axis Button:         Toggle whether to show the x and y axis.
     *         Clear Polygon Button:     Clear all vertices of the polygon.
     *     Graph Window:
     *         Click Left Mouse Button:  Add the pointing coordinate to the polygon vertices.
     *         Scroll Mousr Scroller:    Zoom up or down with the origin point always in the middle of the graph window.
     *         Click Right Mouse Button: Remove the last vertex added to the polygon.
     *     Other:
     *         Window Resize:            Main window can be resized with all widgets resizing proportionally.
     */
    if constexpr (showHomeWorkTwo)
    {
        PolyWindow *window = new PolyWindow(1280, 720, "Window Task 2");
        window->show();
    }
    return Fl::run();
}