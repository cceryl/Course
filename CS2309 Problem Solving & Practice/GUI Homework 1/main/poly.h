#pragma once

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Float_Input.H>
#include <FL/fl_draw.H>

class Poly
{
public:
    struct Point
    {
        double x;
        double y;

        bool operator==(const Point &rhs) const { return x == rhs.x && y == rhs.y; }

        struct Hash
        {
            std::size_t operator()(const Point &point) const
            {
                std::hash<double> hasher;
                return hasher(point.x) ^ hasher(point.y);
            }
        };
    };

    struct FailureSide
    {
        enum Reason { Concurrent, Collinear };
        Point  a, b;
        Point  c, d;
        Reason reason;
    };

    using FailureVertex = Point;

    explicit Poly() = default;
    explicit Poly(const std::vector<Point> &vertices);
    virtual ~Poly()               = default;
    Poly(const Poly &)            = delete;
    Poly &operator=(const Poly &) = delete;
    Poly(Poly &&)                 = delete;
    Poly &operator=(Poly &&)      = delete;

    void push(Point point);
    void pop();
    void clear();

    const std::vector<Point>                                  &vertices() { return mVertices; }
    const std::vector<FailureSide>                            &failureSides() { return mFailureSides; }
    const std::unordered_map<FailureVertex, int, Point::Hash> &failureVertices() { return mFailureVertices; }

private:
    std::vector<Point>                                  mVertices;
    std::vector<FailureSide>                            mFailureSides;
    std::unordered_map<FailureVertex, int, Point::Hash> mFailureVertices;

    bool detectFailure();
    void calcFailureSides(std::size_t index, bool insert);
    void calcFailureVertices(std::size_t index, bool insert);

    bool parallel(const Point &a, const Point &b, const Point &c, const Point &d);
    bool intersect(const Point &a, const Point &b, const Point &c, const Point &d);
};

class PolyWindow : public Fl_Double_Window
{
public:
    explicit PolyWindow(int width, int height, std::string_view name);
    virtual void draw() override;

private:
    class GraphWindow : public Fl_Double_Window
    {
    public:
        explicit GraphWindow(int x, int y, int width, int height, PolyWindow *mainWindow);
        virtual void draw() override;
        virtual int  handle(int event) override;

        void changeScaler(bool enlarge) { mScaler *= enlarge ? 1.1 : 0.9; }

    private:
        PolyWindow *mMainWindow;
        double      mScaler;
    };

    class ConsoleWindow : public Fl_Double_Window
    {
    public:
        explicit ConsoleWindow(int x, int y, int width, int height, PolyWindow *mainWindow);

    private:
        PolyWindow *mMainWindow;
    };

    class InputField : public Fl_Float_Input
    {
    public:
        explicit InputField(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label,
                            double *value);
        virtual void draw() override;

    private:
        double mX, mY, mWidth, mHeight;
    };

    class Browser : public Fl_Browser
    {
    public:
        explicit Browser(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label);
        virtual void draw() override;

    private:
        double mX, mY, mWidth, mHeight;
    };

    class Button : public Fl_Button
    {
    public:
        explicit Button(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label, bool *flag);
        virtual void draw() override;

    private:
        double mX, mY, mWidth, mHeight;
    };

    bool   mVertexUpdate;
    double mX, mY;

    bool   mLeftClickUpdate;
    double mClickX, mClickY;

    bool mRightClickUpdate;

    bool mScrollUpdate;
    bool mScrollUp;

    bool mShowAxisUpdate;
    bool mShowAxis;

    bool mClearUpdate;

    ConsoleWindow *mConsoleWindow;
    GraphWindow   *mGraphWindow;

    Poly     mPoly;
    Browser *mVertexBrowser;

    void setupConsole();
    void setupGraph();
};