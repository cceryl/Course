#include "Window.h"

namespace MapAreaCalculator
{
    Window::Window(int width, int height, std::string_view title)
    {
        mWindow = std::make_unique<MainWindow>(width, height, title);
        mWindow->show();
        Fl::run();
    }

} // namespace MapAreaCalculator