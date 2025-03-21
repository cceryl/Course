#pragma once

#include <cmath>
#include <format>
#include <ranges>
#include <vector>

#include <FL/Fl_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Round_Button.H>

struct Point
{
    double theta  = 0.0;
    double radius = 0.0;
};

class Star
{
public:
    explicit Star(int vertex = 5, int density = 2, int angle = 252);
    virtual ~Star()               = default;
    Star(const Star &)            = default;
    Star &operator=(const Star &) = default;
    Star(Star &&)                 = default;
    Star &operator=(Star &&)      = default;

    void updateVertex(int vertex);
    void updateAngle(int angle);
    void updateDensity(int density);

    void densityMode(bool mode) { mDensityMode = mode; }

    std::vector<Point> getVertices() const;

private:
    int mVertex;
    int mAngle;
    int mDensity;

    bool mDensityMode;

    double                     mThetaStride;
    double                     mInnerRadius;
    inline static const double sOuterRadius = 1.0;

    void calcTheta();
    void calcRadius();
};

class StarWindow : public Fl_Double_Window
{
public:
    explicit StarWindow(int width, int height, std::string_view name);
    virtual void draw() override;
    virtual int  handle(int event) override { return Fl_Double_Window::handle(event); }

private:
    class GraphWindow : public Fl_Double_Window
    {
    public:
        explicit GraphWindow(int x, int y, int width, int height, StarWindow *mainWindow);
        virtual void draw() override;
        virtual int  handle(int event) override;

        void setRotate(int rotate) { mRotate = rotate; }

    private:
        StarWindow *mMainWindow;
        double      mScaler;
        int         mRotate;
    };

    class ConsoleWindow : public Fl_Double_Window
    {
    public:
        explicit ConsoleWindow(int x, int y, int width, int height, StarWindow *mainWindow);
        virtual void draw() override { Fl_Double_Window::draw(); }
        virtual int  handle(int event) override { return Fl_Double_Window::handle(event); }

    private:
        StarWindow *mMainWindow;
    };

    class Slider : public Fl_Hor_Slider
    {
    public:
        explicit Slider(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label);
        virtual void draw() override;

        void setValue(int min, int max, int step, int init, int *value);
        void updateRange(int min, int max);

    private:
        std::string mName;

        double mX, mY, mWidth, mHeight;
    };

    class RadioButton : public Fl_Round_Button
    {
    public:
        explicit RadioButton(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label,
                             bool *value);
        virtual void draw() override;

    private:
        double mX, mY, mWidth, mHeight;
    };

    class ColorChooser : public Fl_Color_Chooser
    {
    public:
        explicit ColorChooser(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label);
        virtual void draw() override;

        void setValue(int init, Fl_Color *value);

    private:
        double mX, mY, mWidth, mHeight;
    };

    ConsoleWindow *mConsoleWindow;
    GraphWindow   *mGraphWindow;

    void setupConsole();
    void setupGraph();

    Star mStar;
    int  mVertex;
    int  mAngle;
    int  mDensity;
    int  mRotate;

    Fl_Color mLineColor;
    Fl_Color mFillColor;

    bool mRegular;
    bool mRemoveAngleLimit;

    Slider *mVertexSlider;
    Slider *mAngleSlider;
    Slider *mDensitySlider;
    Slider *mRotateSlider;

    void checkRegular();
    void checkRemoveAngleLimit();
    void checkVertex();
    void checkAngle();
    void checkDensity();
    void checkRotate();
    void checkColor();
};