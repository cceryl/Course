#include "Window.h"

namespace VisualQuadtree
{
    Window::Window(int width, int height, std::string_view title)
    {
        mWindow = std::make_unique<MainWindow>(width, height, title);
        mWindow->show();
        Fl::run();
    }

} // namespace VisualQuadtree