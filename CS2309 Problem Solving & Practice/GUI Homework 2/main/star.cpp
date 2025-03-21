#include "star.h"

#include <FL/fl_draw.H>

using std::ranges::views::iota;

Star::Star(int vertex, int density, int angle) : mVertex(vertex), mDensity(density), mAngle(angle), mDensityMode(false)
{
    this->calcTheta();
}

void Star::updateVertex(int vertex)
{
    if (mVertex != vertex)
    {
        mVertex = vertex;
        this->calcTheta();
    }
}

void Star::updateAngle(int angle)
{
    if (mAngle != angle)
    {
        mAngle = angle;
        this->calcRadius();
    }
}

void Star::updateDensity(int density)
{
    if (mDensity != density)
    {
        mDensity = density;
        this->calcRadius();
    }
}

std::vector<Point> Star::getVertices() const
{
    std::vector<Point> result;
    result.reserve(2 * mVertex);
    for (int i : iota(0, mVertex))
    {
        result.push_back({2 * i * mThetaStride, sOuterRadius});
        result.push_back({(2 * i + 1) * mThetaStride, mInnerRadius});
    }
    return result;
}

void Star::calcTheta()
{
    mThetaStride = 180.0 / mVertex;
    this->calcRadius();
}

void Star::calcRadius()
{
    if (mDensityMode)
    {
        double halfAngle = (90.0 - 180.0 * mDensity / mVertex) * M_PI / 180.0;
        double stride    = mThetaStride * M_PI / 180.0;
        mInnerRadius     = sOuterRadius / std::sin(M_PI - halfAngle - stride) * std::sin(halfAngle);
    }
    else
    {
        double halfAngle = mAngle * M_PI / 360.0;
        double stride    = mThetaStride * M_PI / 180.0;
        mInnerRadius     = sOuterRadius / std::sin(halfAngle) * std::sin(M_PI - halfAngle - stride);
    }
}

StarWindow::StarWindow(int width, int height, std::string_view name) : Fl_Double_Window(width, height, name.data())
{
    this->border(true);
    this->box(FL_NO_BOX);
    this->clip_children(true);
    this->color(FL_BLACK);
    this->resizable(this);
    this->size_range(800, 600);
    this->begin();

    mVertex           = 5;
    mAngle            = 252;
    mDensity          = 1;
    mRotate           = 90;
    mRegular          = false;
    mRemoveAngleLimit = false;
    mLineColor        = fl_rgb_color(0x00, 0x00, 0xFF);
    mFillColor        = fl_rgb_color(0x00, 0x7F, 0xFF);

    mConsoleWindow = new ConsoleWindow(0, 0, this->w() * 0.25, this->h(), this);
    this->setupConsole();

    mGraphWindow = new GraphWindow(this->w() * 0.25, 0, this->w() * 0.75, this->h(), this);
    this->setupGraph();

    this->end();
}

void StarWindow::draw()
{
    this->checkRegular();
    this->checkRemoveAngleLimit();

    this->checkVertex();
    if (mRegular)
        this->checkDensity();
    else
        this->checkAngle();
    this->checkRotate();

    this->checkColor();

    Fl_Double_Window::draw();
}

void StarWindow::setupConsole()
{
    mConsoleWindow->begin();

    mVertexSlider = new Slider(0.10, 0.05, 0.80, 0.05, mConsoleWindow, "Vertex");
    mVertexSlider->setValue(3, 24, 1, 5, &mVertex);

    mAngleSlider = new Slider(0.10, 0.15, 0.80, 0.05, mConsoleWindow, "Angle");
    mAngleSlider->setValue(180, 360, 1, 252, &mAngle);

    mDensitySlider = new Slider(0.10, 0.25, 0.80, 0.05, mConsoleWindow, "Density");
    mDensitySlider->setValue(1, 10, 1, 1, &mDensity);
    mDensitySlider->deactivate();

    mRotateSlider = new Slider(0.10, 0.35, 0.80, 0.05, mConsoleWindow, "Rotate");
    mRotateSlider->setValue(0, 360, 1, 90, &mRotate);

    new RadioButton(0.10, 0.45, 0.80, 0.05, mConsoleWindow, "Regular Star Polygon", &mRegular);

    new RadioButton(0.10, 0.55, 0.80, 0.05, mConsoleWindow, "Remove Angle Limit", &mRemoveAngleLimit);

    ColorChooser *lineColorChooser = new ColorChooser(0.20, 0.65, 0.60, 0.10, mConsoleWindow, "Line Color");
    lineColorChooser->setValue(0x0000FF, &mLineColor);

    ColorChooser *fillColorChooser = new ColorChooser(0.20, 0.80, 0.60, 0.10, mConsoleWindow, "Fill Color");
    fillColorChooser->setValue(0x007FFF, &mFillColor);

    mConsoleWindow->end();
}

void StarWindow::setupGraph()
{
    mGraphWindow->begin();
    mGraphWindow->end();
}

void StarWindow::checkRegular()
{
    static bool lastRegular;
    if (mRegular != lastRegular)
    {
        lastRegular = mRegular;

        if (mRegular)
        {
            mAngleSlider->deactivate();
            mDensitySlider->activate();
            mStar.densityMode(true);

            mDensitySlider->updateRange(1, (mVertex - 1) / 2);
            mDensity = std::min(mDensity, (mVertex - 1) / 2);
            mStar.updateDensity(mDensity);
        }
        else
        {
            mAngleSlider->activate();
            mDensitySlider->deactivate();
            mStar.densityMode(false);

            if (mRemoveAngleLimit)
                mAngleSlider->updateRange(5, 355);
            else
                mAngleSlider->updateRange(180, std::ceil(360.0 - 360.0 / mVertex));
        }

        mConsoleWindow->redraw();
        mGraphWindow->redraw();
    }
}

void StarWindow::checkRemoveAngleLimit()
{
    static bool lastRemoveAngleLimit;
    if (mRemoveAngleLimit != lastRemoveAngleLimit)
    {
        lastRemoveAngleLimit = mRemoveAngleLimit;

        if (mRemoveAngleLimit)
            mAngleSlider->updateRange(5, 355);
        else
        {
            mAngleSlider->updateRange(180, std::ceil(360.0 - 360.0 / mVertex));
            mAngle = std::min(mAngle, static_cast<int>(std::ceil(360.0 - 360.0 / mVertex)));
            mStar.updateAngle(mAngle);
        }

        mConsoleWindow->redraw();
        mGraphWindow->redraw();
    }
}

void StarWindow::checkVertex()
{
    static int lastVertex;
    if (mVertex != lastVertex)
    {
        lastVertex = mVertex;
        mStar.updateVertex(mVertex);

        if (mRegular)
        {
            mDensitySlider->updateRange(1, (mVertex - 1) / 2);
            mDensity = std::min(mDensity, (mVertex - 1) / 2);
            mStar.updateDensity(mDensity);
        }
        else if (!mRemoveAngleLimit)
        {
            mAngleSlider->updateRange(180, std::ceil(360.0 - 360.0 / mVertex));
            mAngle = std::min(mAngle, static_cast<int>(std::ceil(360.0 - 360.0 / mVertex)));
            mStar.updateAngle(mAngle);
        }

        mConsoleWindow->redraw();
        mGraphWindow->redraw();
    }
}

void StarWindow::checkAngle()
{
    static int lastAngle;
    if (mAngle != lastAngle)
    {
        lastAngle = mAngle;
        mStar.updateAngle(mAngle);
        mGraphWindow->redraw();
    }
}

void StarWindow::checkDensity()
{
    static int lastDensity;
    if (mDensity != lastDensity)
    {
        lastDensity = mDensity;
        mStar.updateDensity(mDensity);
        mGraphWindow->redraw();
    }
}

void StarWindow::checkRotate()
{
    static int lastRotate;
    if (mRotate != lastRotate)
    {
        lastRotate = mRotate;
        mGraphWindow->setRotate(mRotate);
        mGraphWindow->redraw();
    }
}

void StarWindow::checkColor()
{
    static Fl_Color lastLineColor;
    if (mLineColor != lastLineColor)
    {
        lastLineColor = mLineColor;
        mGraphWindow->redraw();
    }

    static Fl_Color lastFillColor;
    if (mFillColor != lastFillColor)
    {
        lastFillColor = mFillColor;
        mGraphWindow->redraw();
    }
}

StarWindow::GraphWindow::GraphWindow(int x, int y, int width, int height, StarWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_BLACK);

    mScaler = 100.0;
    mRotate = 90;
}

void StarWindow::GraphWindow::draw()
{
    Fl_Double_Window::draw();
    auto     vertices  = mMainWindow->mStar.getVertices();
    Fl_Color lineColor = mMainWindow->mLineColor;
    Fl_Color fillColor = mMainWindow->mFillColor;

    fl_line_style(FL_SOLID, 3);
    fl_push_matrix();
    fl_translate(this->w() / 2, this->h() / 2);
    fl_scale(mScaler);
    fl_rotate(mRotate);

    fl_color(lineColor);
    fl_begin_loop();
    for (const auto &[theta, radius] : vertices)
        fl_vertex(radius * std::cos(theta * M_PI / 180), -radius * std::sin(theta * M_PI / 180));
    fl_end_loop();

    fl_color(fillColor);
    fl_begin_polygon();
    for (const auto &[theta, radius] : vertices)
        fl_vertex(radius * std::cos(theta * M_PI / 180), -radius * std::sin(theta * M_PI / 180));
    fl_end_polygon();

    fl_pop_matrix();
}

int StarWindow::GraphWindow::handle(int event)
{
    if (Fl::event_inside(this->x(), this->y(), this->w(), this->h()) && event == FL_MOUSEWHEEL)
    {
        mScaler *= Fl::event_dy() < 0 ? 1.1 : 0.9;
        this->redraw();
    }
    return Fl_Double_Window::handle(event);
}

StarWindow::ConsoleWindow::ConsoleWindow(int x, int y, int width, int height, StarWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_GRAY);
}

StarWindow::Slider::Slider(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label) :
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

void StarWindow::Slider::draw()
{
    static std::string label;

    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    label = std::format("{} = {} ", mName, this->value());
    this->label(label.c_str());

    Fl_Hor_Slider::draw();
}

void StarWindow::Slider::setValue(int min, int max, int step, int init, int *value)
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

void StarWindow::Slider::updateRange(int min, int max)
{
    this->range(min, max);

    if (this->value() < min)
        this->value(min);
    if (this->value() > max)
        this->value(max);

    this->redraw();
}

StarWindow::RadioButton::RadioButton(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label,
                                     bool *value) :
    Fl_Round_Button(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);
    this->align(FL_ALIGN_CENTER);

    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;

    this->user_data(value);
    this->callback(
        [](Fl_Widget *button, void *value)
        {
            dynamic_cast<Fl_Round_Button *>(button)->redraw();
            *reinterpret_cast<bool *>(value) = (bool)dynamic_cast<Fl_Round_Button *>(button)->value();
            button->parent()->parent()->redraw();
        });
}

void StarWindow::RadioButton::draw()
{
    fl_line_style(FL_SOLID, 1);
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    Fl_Round_Button::draw();
}

StarWindow::ColorChooser::ColorChooser(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label) :
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

void StarWindow::ColorChooser::draw()
{
    fl_line_style(FL_SOLID, 1);
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    Fl_Color_Chooser::draw();
}

void StarWindow::ColorChooser::setValue(int init, Fl_Color *value)
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