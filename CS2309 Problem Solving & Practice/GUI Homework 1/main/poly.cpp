#include "poly.h"

Poly::Poly(const std::vector<Point> &vertices)
{
    mVertices.reserve(vertices.size());
    for (const auto &vertex : vertices)
        mVertices.push_back(vertex);
    if (detectFailure())
    {
        mVertices.clear();
        mFailureSides.clear();
        mFailureVertices.clear();
        throw std::invalid_argument("Polygon: invalid polygon.");
    }
}

void Poly::push(Point point)
{
    mVertices.push_back(point);
    calcFailureSides(mVertices.size() - 1, true);
    calcFailureVertices(mVertices.size() - 1, true);
}

void Poly::pop()
{
    if (mVertices.empty())
        return;
    calcFailureSides(mVertices.size() - 1, false);
    calcFailureVertices(mVertices.size() - 1, false);
    mVertices.pop_back();
}

void Poly::clear()
{
    mVertices.clear();
    mFailureSides.clear();
    mFailureVertices.clear();
}

bool Poly::detectFailure()
{
    for (std::size_t i = 0; i < mVertices.size(); ++i)
    {
        calcFailureSides(i, true);
        calcFailureVertices(i, true);
        if (!mFailureVertices.empty() || !mFailureSides.empty())
            return true;
    }
    return false;
}

void Poly::calcFailureSides(std::size_t index, bool insert)
{
    if (index == 0)
        return;
    if (insert)
    {
        Point oldBack = mVertices[index - 1], newBack = mVertices[index];
        while (!mFailureSides.empty() && mFailureSides.back().c == oldBack && mFailureSides.back().d == mVertices.front())
            mFailureSides.pop_back();
        for (std::size_t a = 0, b = 1, c = index - 1, d = index; b < c; ++a, ++b)
            if (intersect(mVertices[a], mVertices[b], mVertices[c], mVertices[d]))
                mFailureSides.push_back({mVertices[a], mVertices[b], mVertices[c], mVertices[d], FailureSide::Concurrent});
        if (index >= 2 && parallel(mVertices[index - 2], mVertices[index - 1], mVertices[index - 1], mVertices[index]))
            mFailureSides.push_back(
                {mVertices[index - 2], mVertices[index - 1], mVertices[index - 1], mVertices[index], FailureSide::Collinear});
        for (std::size_t a = 1, b = 2, c = index, d = 0; b < c; ++a, ++b)
            if (intersect(mVertices[a], mVertices[b], mVertices[c], mVertices[d]))
                mFailureSides.push_back({mVertices[a], mVertices[b], mVertices[c], mVertices[d], FailureSide::Concurrent});
        if (index >= 2 && parallel(mVertices[index], mVertices[0], mVertices[0], mVertices[1]))
            mFailureSides.push_back({mVertices[0], mVertices[1], mVertices[index], mVertices[0], FailureSide::Collinear});
        if (index >= 2 && parallel(mVertices[index - 1], mVertices[index], mVertices[index], mVertices[0]))
            mFailureSides.push_back({mVertices[index - 1], mVertices[index], mVertices[index], mVertices[0], FailureSide::Collinear});
    }
    else
    {
        Point oldBack = mVertices[index], newBack = mVertices[index - 1];
        while (!mFailureSides.empty() && mFailureSides.back().c == mVertices[index] && mFailureSides.back().d == mVertices[0])
            mFailureSides.pop_back();
        while (!mFailureSides.empty() && mFailureSides.back().c == mVertices[index - 1] && mFailureSides.back().d == mVertices[index])
            mFailureSides.pop_back();
        for (std::size_t a = 1, b = 2, c = index - 1, d = 0; b < c; ++a, ++b)
            if (intersect(mVertices[a], mVertices[b], mVertices[c], mVertices[d]))
                mFailureSides.push_back({mVertices[a], mVertices[b], mVertices[c], mVertices[d], FailureSide::Concurrent});
        if (index >= 2 && parallel(mVertices[index - 2], mVertices[index - 1], mVertices[index - 1], mVertices[0]))
            mFailureSides.push_back(
                {mVertices[index - 2], mVertices[index - 1], mVertices[index - 1], mVertices[0], FailureSide::Collinear});
        if (index >= 2 && parallel(mVertices[index - 1], mVertices[0], mVertices[0], mVertices[1]))
            mFailureSides.push_back({mVertices[0], mVertices[1], mVertices[index - 1], mVertices[0], FailureSide::Collinear});
    }
}

void Poly::calcFailureVertices(std::size_t index, bool insert)
{
    if (insert)
    {
        if (auto iter = mFailureVertices.find(mVertices[index]); iter != mFailureVertices.end())
            ++iter->second;
        else if (std::find(mVertices.cbegin(), mVertices.cend() - 1, mVertices[index]) != mVertices.cend() - 1)
            mFailureVertices.insert({mVertices[index], 2});
    }
    else if (auto iter = mFailureVertices.find(mVertices[index]); iter != mFailureVertices.end())
    {
        if (iter->second == 2)
            mFailureVertices.erase(iter);
        else
            --iter->second;
    }
}

bool Poly::parallel(const Point &a, const Point &b, const Point &c, const Point &d)
{
    double x1 = b.x - a.x, y1 = b.y - a.y;
    double x2 = d.x - c.x, y2 = d.y - c.y;
    return (x1 * y2 - x2 * y1) == 0;
}

bool Poly::intersect(const Point &a, const Point &b, const Point &c, const Point &d)
{
    double x1 = b.x - a.x, y1 = b.y - a.y;
    double x2 = d.x - c.x, y2 = d.y - c.y;
    double cross = x1 * y2 - x2 * y1;

    Point inter = {(x2 * (a.y - c.y) - y2 * (a.x - c.x)) / cross, (x1 * (a.y - c.y) - y1 * (a.x - c.x)) / cross};

    return inter.x >= 0 && inter.x <= 1 && inter.y >= 0 && inter.y <= 1;
}

PolyWindow::PolyWindow(int width, int height, std::string_view name) : Fl_Double_Window(width, height, name.data())
{
    this->border(true);
    this->box(FL_NO_BOX);
    this->clip_children(true);
    this->color(FL_BLACK);
    this->resizable(this);
    this->size_range(800, 600);
    this->begin();

    mVertexUpdate     = false;
    mLeftClickUpdate  = false;
    mRightClickUpdate = false;
    mShowAxisUpdate   = false;
    mShowAxis         = false;
    mClearUpdate      = false;

    mConsoleWindow = new ConsoleWindow(0, 0, this->w() * 0.25, this->h(), this);
    this->setupConsole();

    mGraphWindow = new GraphWindow(this->w() * 0.25, 0, this->w() * 0.75, this->h(), this);
    this->setupGraph();

    this->end();
}

void PolyWindow::draw()
{
    if (mVertexUpdate)
    {
        mVertexUpdate = false;
        mPoly.push({mX, mY});
        mVertexBrowser->add(std::format("@f({:10.3f}, {:10.3f})", mX, mY).data());
        mVertexBrowser->redraw();
        mGraphWindow->redraw();
    }
    if (mLeftClickUpdate)
    {
        mLeftClickUpdate = false;
        mPoly.push({mClickX, mClickY});
        mVertexBrowser->add(std::format("@f({:10.3f}, {:10.3f})", mClickX, mClickY).data());
        mVertexBrowser->redraw();
        mGraphWindow->redraw();
    }
    if (mRightClickUpdate)
    {
        mRightClickUpdate = false;
        mPoly.pop();
        mVertexBrowser->remove(mVertexBrowser->size());
        mVertexBrowser->redraw();
        mGraphWindow->redraw();
    }
    if (mScrollUpdate)
    {
        mScrollUpdate = false;
        mGraphWindow->changeScaler(mScrollUp);
        mGraphWindow->redraw();
    }
    if (mShowAxisUpdate)
    {
        mShowAxisUpdate = false;
        mShowAxis       = !mShowAxis;
        mGraphWindow->redraw();
    }
    if (mClearUpdate)
    {
        mClearUpdate = false;
        mPoly.clear();
        mVertexBrowser->clear();
        mGraphWindow->redraw();
    }
    Fl_Double_Window::draw();
}

void PolyWindow::setupConsole()
{
    mConsoleWindow->begin();

    mVertexBrowser = new Browser(0.1, 0.1, 0.8, 0.4, mConsoleWindow, "Vertices ");

    InputField *xInput = new InputField(0.15, 0.6, 0.3, 0.05, mConsoleWindow, "x ", &mX);

    InputField *yInput = new InputField(0.55, 0.6, 0.3, 0.05, mConsoleWindow, "y ", &mY);

    Button *addButton = new Button(0.15, 0.7, 0.7, 0.05, mConsoleWindow, "Add Vertex ", &mVertexUpdate);

    Button *axisButton = new Button(0.15, 0.8, 0.3, 0.05, mConsoleWindow, "Show Axis ", &mShowAxisUpdate);

    Button *clearButton = new Button(0.55, 0.8, 0.3, 0.05, mConsoleWindow, "Clear Polygon ", &mClearUpdate);

    mConsoleWindow->end();
}

void PolyWindow::setupGraph()
{
    mGraphWindow->begin();
    mGraphWindow->end();
}

PolyWindow::GraphWindow::GraphWindow(int x, int y, int width, int height, PolyWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow), mScaler(100.0)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_BLACK);
}

void PolyWindow::GraphWindow::draw()
{
    Fl_Double_Window::draw();

    Poly  &poly    = mMainWindow->mPoly;
    double originX = this->w() / 2;
    double originY = this->h() / 2;

    fl_line_style(FL_SOLID, 2);

    fl_push_matrix();
    fl_translate(originX, originY);
    fl_scale(mScaler);

    fl_color(0x00, 0x00, 0xFF);
    fl_begin_loop();
    for (const auto &vertex : poly.vertices())
        fl_vertex(vertex.x, -vertex.y);
    fl_end_loop();

    fl_color(0x00, 0x7F, 0xFF);
    fl_begin_polygon();
    for (const auto &vertex : poly.vertices())
        fl_vertex(vertex.x, -vertex.y);
    fl_end_polygon();

    fl_pop_matrix();

    fl_color(0xFF, 0x00, 0x00);
    for (const auto &side : poly.failureSides())
    {
        fl_line(side.a.x * mScaler + originX, -side.a.y * mScaler + originY, side.b.x * mScaler + originX,
                -side.b.y * mScaler + originY);
        fl_line(side.c.x * mScaler + originX, -side.c.y * mScaler + originY, side.d.x * mScaler + originX,
                -side.d.y * mScaler + originY);
    }
    for (const auto &[vertex, count] : poly.failureVertices())
        fl_point(vertex.x * mScaler + originX, -vertex.y * mScaler + originY);

    if (mMainWindow->mShowAxis)
    {
        fl_color(0xFF, 0xFF, 0xFF);
        fl_line_style(FL_SOLID, 2);
        fl_line(originX * 0.2, originY, originX * 1.8, originY);
        fl_line(originX, originY * 0.2, originX, originY * 1.8);
        fl_begin_polygon();
        fl_vertex(originX * 1.8, originY * 1.012);
        fl_vertex(originX * 1.83, originY);
        fl_vertex(originX * 1.8, originY * 0.988);
        fl_end_polygon();
        fl_begin_polygon();
        fl_vertex(originX * 0.988, originY * 0.2);
        fl_vertex(originX, originY * 0.17);
        fl_vertex(originX * 1.012, originY * 0.2);
        fl_end_polygon();
    }
}

int PolyWindow::GraphWindow::handle(int event)
{
    if (event == FL_PUSH && Fl::event_state() == FL_BUTTON1)
    {
        mMainWindow->mClickX          = (Fl::event_x() - this->w() / 2) / mScaler;
        mMainWindow->mClickY          = -(Fl::event_y() - this->h() / 2) / mScaler;
        mMainWindow->mLeftClickUpdate = true;
        mMainWindow->redraw();
    }
    else if (event == FL_PUSH && Fl::event_state() == FL_BUTTON3)
    {
        mMainWindow->mRightClickUpdate = true;
        mMainWindow->redraw();
    }
    else if (event == FL_MOUSEWHEEL)
    {
        mMainWindow->mScrollUp     = Fl::event_dy() < 0;
        mMainWindow->mScrollUpdate = true;
        mMainWindow->redraw();
    }
    return Fl_Double_Window::handle(event);
}

PolyWindow::ConsoleWindow::ConsoleWindow(int x, int y, int width, int height, PolyWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_GRAY);
}

PolyWindow::Browser::Browser(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label) :
    Fl_Browser(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);
    this->align(FL_ALIGN_TOP);

    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;
}

void PolyWindow::Browser::draw()
{
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    Fl_Browser::draw();
}

PolyWindow::Button::Button(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label, bool *flag) :
    Fl_Button(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);
    this->align(FL_ALIGN_CENTER);

    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;

    this->user_data(flag);
    this->callback(
        [](Fl_Widget *button, void *flag)
        {
            *reinterpret_cast<bool *>(flag) = true;
            button->parent()->parent()->redraw();
        });
}

void PolyWindow::Button::draw()
{
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    Fl_Button::draw();
}

PolyWindow::InputField::InputField(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label,
                                   double *value) :
    Fl_Float_Input(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);
    this->align(FL_ALIGN_TOP);

    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;

    this->user_data(value);
    this->callback(
        [](Fl_Widget *input, void *value)
        {
            *reinterpret_cast<double *>(value) = std::stod(dynamic_cast<Fl_Float_Input *>(input)->value());
            input->parent()->parent()->redraw();
        });
}

void PolyWindow::InputField::draw()
{
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    Fl_Float_Input::draw();
}