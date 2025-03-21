#pragma once

#include <cmath>
#include <format>
#include <stdexcept>
#include <string>
#include <vector>

#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Round_Button.H>
#include <FL/fl_draw.H>

class Superellipse
{
public:
    explicit Superellipse(double a = 1.0, double b = 1.0, double m = 2.0, double n = 2.0);
    virtual ~Superellipse() = default;

    struct Point
    {
        double x;
        double y;
    };

    void               changeParams(double a, double b, double m, double n);
    std::vector<Point> genCurvePoints(int curvePointNum);

private:
    struct Params
    {
        double a, b;
        double m, n;
    };

    Params              mParams;
    std::vector<double> mPerimeter;

    inline static constexpr double gResolutionFactor = 0.001;

    void calcPerimeter(double resolution);
};

class SuperellipseWindow : public Fl_Double_Window
{
public:
    explicit SuperellipseWindow(int width, int height, std::string_view name);
    virtual void draw() override;

private:
    class GraphWindow : public Fl_Double_Window
    {
    public:
        explicit GraphWindow(int x, int y, int width, int height, SuperellipseWindow *mainWindow);
        virtual void draw() override;

    private:
        SuperellipseWindow *mMainWindow;
        double              mScaler;
    };

    class ConsoleWindow : public Fl_Double_Window
    {
    public:
        explicit ConsoleWindow(int x, int y, int width, int height, SuperellipseWindow *mainWindow);
        virtual void draw() override;

    private:
        SuperellipseWindow *mMainWindow;
    };

    class Slider : public Fl_Hor_Slider
    {
    public:
        explicit Slider(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label, bool integral);
        virtual void draw() override;

        void setValue(double min, double max, double step, double init, void *value);

    private:
        bool        mIntegral;
        std::string mName;
        std::string mLabel;

        double mX, mY, mWidth, mHeight;
    };

    class Button : public Fl_Round_Button
    {
    public:
        explicit Button(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label);
        virtual void draw() override;

        void setValue(double init, void *value);

    private:
        double mX, mY, mWidth, mHeight;
    };

    ConsoleWindow *mConsoleWindow;
    GraphWindow   *mGraphWindow;

    Superellipse mSuperellipse;
    double       mA, mB, mM, mN;
    int          mCurvePointNum;
    bool         mAllBranch;

    void setupConsole();
    void setupGraph();
};