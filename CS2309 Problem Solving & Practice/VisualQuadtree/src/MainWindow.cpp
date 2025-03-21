#include "Window.h"

namespace VisualQuadtree
{
    MainWindow::MainWindow(int width, int height, std::string_view title) : Fl_Double_Window(width, height, title.data())
    {
        this->border(true);
        this->box(FL_NO_BOX);
        this->clip_children(true);
        this->color(FL_BLACK);

        this->begin();
        mConsoleWindow = std::make_unique<ConsoleWindow>(0, 0, width / 4, height, "Console Window");
        mGraphWindow   = std::make_unique<GraphWindow>(width / 4, 0, width / 4 * 3, height, "Image Window");
        mConsoleWindow->linkGraphWindow(mGraphWindow);
        mGraphWindow->linkConsoleWindow(mConsoleWindow);
        this->end();
    }

    void MainWindow::draw() { Fl_Double_Window::draw(); }

    int MainWindow::handle(int event) { return Fl_Double_Window::handle(event); }

} // namespace VisualQuadtree