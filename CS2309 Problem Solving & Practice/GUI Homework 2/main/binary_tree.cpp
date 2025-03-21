#include "binary_tree.h"

#include <chrono>
#include <ranges>

#include <FL/fl_draw.H>

using std::ranges::views::iota;

std::vector<Node> BinaryTree::getNodes() const
{
    std::vector<Node> nodes;
    nodes.reserve((1 << mLevel) - 1);
    int numHighestLevelNodes = 1 << (mLevel - 1);

    for (int level : iota(0, mLevel))
    {
        int numNodes = 1 << level;

        double stride = numHighestLevelNodes / numNodes * mBasicStride;
        double startX = -stride * (-0.5 + numNodes / 2.0);

        for (int i : iota(0, numNodes))
            nodes.push_back({startX + i * stride, mBasicHeight * level});
    }

    return nodes;
}

BinaryTreeWindow::BinaryTreeWindow(int width, int height, std::string_view name) : Fl_Double_Window(width, height, name.data())
{
    this->border(true);
    this->box(FL_NO_BOX);
    this->clip_children(true);
    this->color(FL_BLACK);
    this->resizable(this);
    this->size_range(800, 600);
    this->begin();

    mConsoleWindow = new ConsoleWindow(0, 0, this->w() * 0.25, this->h(), this);
    this->setupConsole();

    mGraphWindow = new GraphWindow(this->w() * 0.25, 0, this->w() * 0.75, this->h(), this);
    this->setupGraph();

    mBinaryTree.updateLevel(3);
    mLevel       = 3;
    mBasicStride = 10;
    mBasicHeight = 10;
    mNodeRadius  = 10;
    mLineColor   = fl_rgb_color(0x00, 0x00, 0xFF);
    mNodeColor   = fl_rgb_color(0x00, 0x7F, 0xFF);

    this->end();
}

void BinaryTreeWindow::draw()
{
    static int      lastLevel, lastStride, lastHeight, lastRadius;
    static Fl_Color lastLineColor, lastNodeColor;

    if (lastLevel != mLevel)
    {
        lastLevel = mLevel;
        mBinaryTree.updateLevel(mLevel);
        mGraphWindow->redraw();
    }
    if (lastStride != mBasicStride)
    {
        lastStride = mBasicStride;
        mBinaryTree.updateStride(mBasicStride / 10.0);
        mGraphWindow->redraw();
    }
    if (lastHeight != mBasicHeight)
    {
        lastHeight = mBasicHeight;
        mBinaryTree.updateHeight(mBasicHeight / 10.0);
        mGraphWindow->redraw();
    }
    if (lastRadius != mNodeRadius)
    {
        lastRadius = mNodeRadius;
        mGraphWindow->redraw();
    }
    if (lastLineColor != mLineColor)
    {
        lastLineColor = mLineColor;
        mGraphWindow->redraw();
    }
    if (lastNodeColor != mNodeColor)
    {
        lastNodeColor = mNodeColor;
        mGraphWindow->redraw();
    }

    Fl_Double_Window::draw();
}

void BinaryTreeWindow::setupConsole()
{
    mConsoleWindow->begin();

    (new Slider(0.1, 0.1, 0.8, 0.05, mConsoleWindow, "Level"))->setValue(0, 8, 1, 3, &mLevel);

    (new Slider(0.1, 0.2, 0.8, 0.05, mConsoleWindow, "Stride"))->setValue(0, 100, 1, 10, &mBasicStride);

    (new Slider(0.1, 0.3, 0.8, 0.05, mConsoleWindow, "Height"))->setValue(0, 100, 1, 10, &mBasicHeight);

    (new Slider(0.1, 0.4, 0.8, 0.05, mConsoleWindow, "Radius"))->setValue(0, 100, 1, 10, &mNodeRadius);

    (new ColorChooser(0.1, 0.5, 0.8, 0.2, mConsoleWindow, "Line Color"))->setValue(0x0000FF, &mLineColor);

    (new ColorChooser(0.1, 0.75, 0.8, 0.2, mConsoleWindow, "Node Color"))->setValue(0x007FFF, &mNodeColor);

    mConsoleWindow->end();
}

void BinaryTreeWindow::setupGraph()
{
    mGraphWindow->begin();

    new GraphWindow::WindowEventHandler(mGraphWindow);

    mGraphWindow->end();
}

BinaryTreeWindow::GraphWindow::GraphWindow(int x, int y, int width, int height, BinaryTreeWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_BLACK);

    mScaler = 100.0;
    mShift  = {width / 2.0, height / 2.0};
}

void BinaryTreeWindow::GraphWindow::draw()
{
    Fl_Double_Window::draw();

    std::vector<Node> nodes = mMainWindow->mBinaryTree.getNodes();

    fl_line_style(FL_SOLID, 3);
    fl_push_matrix();
    fl_translate(mShift.x, mShift.y);
    fl_scale(mScaler);

    fl_color(mMainWindow->mLineColor);
    for (int i : iota(0ull, nodes.size() / 2))
    {
        fl_begin_line();
        fl_vertex(nodes[2 * i + 1].x, nodes[2 * i + 1].y);
        fl_vertex(nodes[i].x, nodes[i].y);
        fl_vertex(nodes[2 * i + 2].x, nodes[2 * i + 2].y);
        fl_end_line();
    }

    for (int i : iota(0ull, nodes.size()))
    {
        double radius = mMainWindow->mNodeRadius / 100.0;
        fl_color(mMainWindow->mNodeColor);
        fl_pie((nodes[i].x - radius) * mScaler + mShift.x, (nodes[i].y - radius) * mScaler + mShift.y, 2 * radius * mScaler,
               2 * radius * mScaler, 0, 360);
        fl_color(mMainWindow->mLineColor);
        fl_circle(nodes[i].x, nodes[i].y, radius);
    }

    fl_pop_matrix();
}

BinaryTreeWindow::GraphWindow::WindowEventHandler::WindowEventHandler(GraphWindow *window) : Fl_Button(0, 0, window->w(), window->h())
{
    mWindow = window;
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_BLACK, FL_BLACK);
}

void BinaryTreeWindow::GraphWindow::WindowEventHandler::draw()
{
    this->resize(0, 0, this->parent()->w(), this->parent()->h());
    Fl_Button::draw();
}

int BinaryTreeWindow::GraphWindow::WindowEventHandler::handle(int event)
{
    if (Fl::event_inside(this->x(), this->y(), this->w(), this->h()))
        if (event == FL_DRAG && Fl::event_state() == FL_BUTTON1)
        {
            static int  lastX, lastY;
            static auto lastDragTime = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds(1000);
            auto        currentTime  = std::chrono::high_resolution_clock::now();
            auto        deltaTime    = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastDragTime);
            if (deltaTime > std::chrono::milliseconds(100))
            {
                lastX = Fl::event_x();
                lastY = Fl::event_y();
            }
            mWindow->updateShift(Fl::event_x() - lastX, Fl::event_y() - lastY);
            lastX        = Fl::event_x();
            lastY        = Fl::event_y();
            lastDragTime = currentTime;
            lastDragTime = std::chrono::high_resolution_clock::now();
            this->redraw();
        }
        else if (event == FL_MOUSEWHEEL)
        {
            static bool imageCall = true;
            if (!imageCall)
            {
                imageCall = true;
                return Fl_Button::handle(event);
            }
            imageCall        = false;
            double oldScaler = mWindow->getScaler();
            mWindow->updateScaler(Fl::event_dy() > 0 ? 0.9 : 1.1);
            double mouseX       = Fl::event_x();
            double mouseY       = Fl::event_y();
            double realCoordX   = (mouseX - mWindow->getShiftX()) / oldScaler;
            double realCoordY   = (mouseY - mWindow->getShiftY()) / oldScaler;
            double scaledCoordX = realCoordX * mWindow->getScaler() + mWindow->getShiftX();
            double scaledCoordY = realCoordY * mWindow->getScaler() + mWindow->getShiftY();
            double deltaX       = mouseX - scaledCoordX;
            double deltaY       = mouseY - scaledCoordY;
            mWindow->updateShift(deltaX, deltaY);
            this->redraw();
        }
    return Fl_Button::handle(event);
}

BinaryTreeWindow::ConsoleWindow::ConsoleWindow(int x, int y, int width, int height, BinaryTreeWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_GRAY);
}

BinaryTreeWindow::Slider::Slider(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label) :
    Fl_Hor_Slider(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);

    mName   = label.data();
    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;
}

void BinaryTreeWindow::Slider::draw()
{
    static std::string label;

    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    label = std::format("{} = {} ", mName, this->value());
    this->label(label.c_str());

    Fl_Hor_Slider::draw();
}

void BinaryTreeWindow::Slider::setValue(int min, int max, int step, int init, int *value)
{
    this->range(min, max);
    this->step(step);
    this->value(init);

    this->user_data(value);
    this->callback(
        [](Fl_Widget *slider, void *value)
        {
            dynamic_cast<Fl_Slider *>(slider)->redraw();
            dynamic_cast<Fl_Slider *>(slider)->redraw_label();
            *reinterpret_cast<int *>(value) = (int)dynamic_cast<Fl_Slider *>(slider)->value();
            slider->parent()->parent()->redraw();
        });
}

BinaryTreeWindow::ColorChooser::ColorChooser(double x, double y, double width, double height, ConsoleWindow *window,
                                             std::string_view label) :
    Fl_Color_Chooser(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);
    this->align(FL_ALIGN_BOTTOM);

    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;
}

void BinaryTreeWindow::ColorChooser::draw()
{
    fl_line_style(FL_SOLID, 1);
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    Fl_Color_Chooser::draw();
}

void BinaryTreeWindow::ColorChooser::setValue(int init, Fl_Color *value)
{
    this->rgb((init >> 16 & 0xFF) / 255.0, (init >> 8 & 0xFF) / 255.0, (init & 0xFF) / 255.0);
    this->user_data(value);
    this->callback(
        [](Fl_Widget *chooser, void *value)
        {
            Fl_Color_Chooser *colorChooser = dynamic_cast<Fl_Color_Chooser *>(chooser);
            colorChooser->redraw();
            *reinterpret_cast<Fl_Color *>(value) =
                fl_rgb_color(colorChooser->r() * 255, colorChooser->g() * 255, colorChooser->b() * 255);
            colorChooser->parent()->parent()->redraw();
        });
}