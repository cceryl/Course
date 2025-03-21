#include "superellipse.h"
#include "poly.h"

Superellipse::Superellipse(double a, double b, double m, double n) { this->changeParams(a, b, m, n); }

void Superellipse::changeParams(double a, double b, double m, double n)
{
    if (a == 0)
        throw std::invalid_argument("Superellipse: 'a' cannot be 0.");
    if (b == 0)
        throw std::invalid_argument("Superellipse: 'b' cannot be 0.");
    if (m <= 0)
        throw std::invalid_argument("Superellipse: 'm' must be positive.");
    if (n <= 0)
        throw std::invalid_argument("Superellipse: 'n' must be positive.");

    a = std::abs(a);
    b = std::abs(b);

    mParams = {a, b, m, n};
    calcPerimeter(std::min(a, b) * gResolutionFactor);
}

std::vector<Superellipse::Point> Superellipse::genCurvePoints(int curvePointNum)
{
    if (curvePointNum < 3)
        throw std::invalid_argument("Superellipse: the number of curve points must be at least 3.");
    std::vector<Point> points;
    double             stride     = mPerimeter.back() / curvePointNum;
    double             resolution = std::min(mParams.a, mParams.b) * gResolutionFactor;
    int                segments   = mPerimeter.size() / 4;

    auto genPoint = [segments, resolution, &points](int seg, double a, double b, double m, double n)
    {
        int quadrant = seg / segments + 1;
        switch (quadrant)
        {
        case 1:
        {
            double x = seg * resolution;
            double y = b * std::pow(1.0 - std::pow(x / a, m), 1.0 / n);
            points.push_back({x, y});
            break;
        }
        case 2:
        {
            double x = a - (seg - segments) * resolution;
            double y = -b * std::pow(1.0 - std::pow(x / a, m), 1.0 / n);
            points.push_back({x, y});
            break;
        }
        case 3:
        {
            double x = -(seg - 2 * segments) * resolution;
            double y = -b * std::pow(1.0 - std::pow(-x / a, m), 1.0 / n);
            points.push_back({x, y});
            break;
        }
        case 4:
        {
            double x = -a + (seg - 3 * segments) * resolution;
            double y = b * std::pow(1.0 - std::pow(-x / a, m), 1.0 / n);
            points.push_back({x, y});
            break;
        }
        default: break;
        }
    };

    for (int seg = 0, point = 0; seg < mPerimeter.size() && point < curvePointNum; ++seg)
        if (mPerimeter[seg] >= point * stride)
        {
            genPoint(seg, mParams.a, mParams.b, mParams.m, mParams.n);
            ++point;
        }

    return points;
}

void Superellipse::calcPerimeter(double resolution)
{
    double perimeter = 0.0;
    mPerimeter.clear();
    mPerimeter.reserve(4 * (mParams.a / resolution + 1));
    mPerimeter.push_back(0);

    for (double x = resolution, y1, y2 = mParams.b; x <= mParams.a; x += resolution)
    {
        y1 = y2;
        y2 = mParams.b * std::pow(1.0 - std::pow(x / mParams.a, mParams.m), 1.0 / mParams.n);

        double dx       = resolution;
        double dy       = y1 - y2;
        double distance = std::sqrt(dx * dx + dy * dy);

        perimeter += distance;
        mPerimeter.push_back(perimeter);
    }

    int    segments      = mPerimeter.size();
    double quadPerimeter = mPerimeter.back();
    for (int i = 1; i <= segments; ++i)
        mPerimeter.push_back(2 * quadPerimeter - mPerimeter[segments - i]);
    for (int i = 0; i < 2 * segments; ++i)
        mPerimeter.push_back(mPerimeter[i] + 2 * quadPerimeter);
}

SuperellipseWindow::SuperellipseWindow(int width, int height, std::string_view name) :
    Fl_Double_Window(width, height, name.data()), mSuperellipse(1.0, 1.0, 2.0, 2.0)
{
    this->border(true);
    this->box(FL_NO_BOX);
    this->clip_children(true);
    this->color(FL_BLACK);
    this->resizable(this);
    this->size_range(800, 600);
    this->begin();

    mA = 1.0, mB = 1.0, mM = 2.0, mN = 2.0;
    mCurvePointNum = 50;
    mAllBranch     = false;

    mConsoleWindow = new ConsoleWindow(0, 0, this->w() * 0.25, this->h(), this);
    this->setupConsole();

    mGraphWindow = new GraphWindow(this->w() * 0.25, 0, this->w() * 0.75, this->h(), this);
    this->setupGraph();

    this->end();
}

void SuperellipseWindow::draw()
{
    static double lastA, lastB, lastM, lastN;
    int           lastPointNum;
    if (lastA != mA || lastB != mB || lastM != mM || lastN != mN)
    {
        lastA = mA, lastB = mB, lastM = mM, lastN = mN, lastPointNum = mCurvePointNum;
        mSuperellipse.changeParams(mA, mB, mM, mN);
        mGraphWindow->redraw();
    }
    else if (lastPointNum != mCurvePointNum)
    {
        lastPointNum = mCurvePointNum;
        mGraphWindow->redraw();
    }

    Fl_Double_Window::draw();
}

SuperellipseWindow::GraphWindow::GraphWindow(int x, int y, int width, int height, SuperellipseWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow), mScaler(100.0)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_BLACK);
}

void SuperellipseWindow::GraphWindow::draw()
{
    Fl_Double_Window::draw();

    auto &&vertices = mMainWindow->mSuperellipse.genCurvePoints(mMainWindow->mCurvePointNum);

    fl_color(0x00, 0x00, 0xFF);
    fl_line_style(FL_SOLID, 1);
    if (mMainWindow->mAllBranch)
    {
        double originX = this->w() / 2.0;
        double originY = this->h() / 2.0;
        for (auto i = vertices.cbegin(); i != vertices.cend(); ++i)
            for (auto j = i + 1; j != vertices.end(); ++j)
                fl_line(i->x * mScaler + originX, i->y * mScaler + originY, j->x * mScaler + originX, j->y * mScaler + originY);
    }
    else
    {
        fl_push_matrix();
        fl_translate(this->w() / 2, this->h() / 2);
        fl_scale(mScaler);
        fl_begin_line();
        for (const auto &vertex : vertices)
            fl_vertex(vertex.x, vertex.y);
        fl_vertex(vertices.front().x, vertices.front().y);
        fl_end_line();
        fl_pop_matrix();
    }
}

SuperellipseWindow::ConsoleWindow::ConsoleWindow(int x, int y, int width, int height, SuperellipseWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_GRAY);
}

void SuperellipseWindow::ConsoleWindow::draw() { Fl_Double_Window::draw(); }

SuperellipseWindow::Slider::Slider(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label,
                                   bool integral) :
    Fl_Hor_Slider(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);
    mName     = label.data();
    mIntegral = integral;
    mX        = x;
    mY        = y;
    mWidth    = width;
    mHeight   = height;
}

void SuperellipseWindow::Slider::draw()
{
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    if (mIntegral)
        mLabel = std::format("{} = {} ", mName, (int)this->value());
    else
        mLabel = std::format("{} = {:.2f} ", mName, this->value());
    this->label(mLabel.c_str());
    Fl_Hor_Slider::draw();
}

void SuperellipseWindow::Slider::setValue(double min, double max, double step, double init, void *value)
{
    this->range(min, max);
    this->step(step);
    this->value(init);
    this->user_data(value);
    if (mIntegral)
        this->callback(
            [](Fl_Widget *slider, void *value)
            {
                dynamic_cast<Fl_Slider *>(slider)->redraw();
                dynamic_cast<Fl_Slider *>(slider)->redraw_label();
                *reinterpret_cast<int *>(value) = (int)dynamic_cast<Fl_Slider *>(slider)->value();
                slider->parent()->parent()->redraw();
            });
    else
        this->callback(
            [](Fl_Widget *slider, void *value)
            {
                dynamic_cast<Fl_Slider *>(slider)->redraw();
                dynamic_cast<Fl_Slider *>(slider)->redraw_label();
                *reinterpret_cast<double *>(value) = dynamic_cast<Fl_Slider *>(slider)->value();
                slider->parent()->parent()->redraw();
            });
}

SuperellipseWindow::Button::Button(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label) :
    Fl_Round_Button(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);
    this->align(FL_ALIGN_CENTER);
    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;
}

void SuperellipseWindow::Button::draw()
{
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    Fl_Round_Button::draw();
}

void SuperellipseWindow::Button::setValue(double init, void *value)
{
    this->user_data(value);
    this->callback(
        [](Fl_Widget *button, void *value)
        {
            dynamic_cast<Fl_Round_Button *>(button)->redraw();
            *reinterpret_cast<bool *>(value) = (bool)dynamic_cast<Fl_Round_Button *>(button)->value();
            button->parent()->parent()->redraw();
        });
}

void SuperellipseWindow::setupConsole()
{
    mConsoleWindow->begin();

    Slider *sliderA = new Slider(0.1, 0.1, 0.8, 0.05, mConsoleWindow, "A", false);
    sliderA->setValue(0.01, 5.00, 0.01, 1.00, &mA);

    Slider *sliderB = new Slider(0.1, 0.2, 0.8, 0.05, mConsoleWindow, "B", false);
    sliderB->setValue(0.01, 5.00, 0.01, 1.00, &mB);

    Slider *sliderM = new Slider(0.1, 0.3, 0.8, 0.05, mConsoleWindow, "M", false);
    sliderM->setValue(0.1, 10.00, 0.1, 2.00, &mM);

    Slider *sliderN = new Slider(0.1, 0.4, 0.8, 0.05, mConsoleWindow, "N", false);
    sliderN->setValue(0.1, 10.00, 0.1, 2.00, &mN);

    Slider *sliderPoints = new Slider(0.1, 0.5, 0.8, 0.05, mConsoleWindow, "Points", true);
    sliderPoints->setValue(3, 100, 1, 50, &mCurvePointNum);

    Button *branchButton = new Button(0.1, 0.7, 0.8, 0.05, mConsoleWindow, "Show All Branches ");
    branchButton->setValue(false, &mAllBranch);

    mConsoleWindow->end();
}

void SuperellipseWindow::setupGraph()
{
    mGraphWindow->begin();
    mGraphWindow->end();
}