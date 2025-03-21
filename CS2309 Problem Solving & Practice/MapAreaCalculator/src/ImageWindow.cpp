#include "Window.h"

#include "Platform.h"

namespace MapAreaCalculator
{
    ImageWindow::ImageWindow(int x, int y, int width, int height, std::string_view title) :
        Fl_Double_Window(x, y, width, height, title.data())
    {
        this->box(FL_THIN_DOWN_BOX);
        this->color(47);

        this->begin();
        mImage = std::make_unique<Image>(0, 0, width, height);
        this->end();
    }

    void ImageWindow::linkConsoleWindow(std::shared_ptr<ConsoleWindow> consoleWindow) { mConsoleWindow = consoleWindow; }

    void ImageWindow::draw() { Fl_Double_Window::draw(); }

    int ImageWindow::handle(int event)
    {
        if (event == FL_PUSH && (Fl::event_button() == FL_LEFT_MOUSE || Fl::event_button() == FL_RIGHT_MOUSE))
            mConsoleWindow.lock()->redraw();
        return Fl_Double_Window::handle(event);
    }

    void ImageWindow::importImage()
    {
        static bool called = false;
        if (called)
            return;
        called = true;
        mImage->loadImage(importImagePath());
        called = false;
    }

    void ImageWindow::importVertices()
    {
        static bool called = false;
        if (called)
            return;
        called = true;
        mImage->importVertices(importVerticesPath());
        mConsoleWindow.lock()->redraw();
        called = false;
    }

    void ImageWindow::exportResultImage()
    {
        static bool called = false;
        if (called)
            return;
        called = true;
        mImage->exportResultImage(exportImagePath());
        called = false;
    }

    void ImageWindow::exportVertices()
    {
        static bool called = false;
        if (called)
            return;
        mImage->exportVertices(exportVerticesPath());
        called = false;
    }

    void ImageWindow::selectScale() { mImage->selectScale(); }

    double ImageWindow::getScaler() const { return mImage->getScaler(); }

    double ImageWindow::calcArea() const { return mImage->calcArea(); }

} // namespace MapAreaCalculator