#pragma once

#include <string>
#include <vector>

#include <FL/Fl_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hor_Slider.H>

struct Node
{
    double x;
    double y;
};

class BinaryTree
{
public:
    explicit BinaryTree(int level = 0) : mLevel(level), mBasicStride(0.1), mBasicHeight(0.1) {}
    virtual ~BinaryTree()                     = default;
    BinaryTree(const BinaryTree &)            = default;
    BinaryTree &operator=(const BinaryTree &) = default;
    BinaryTree(BinaryTree &&)                 = default;
    BinaryTree &operator=(BinaryTree &&)      = default;

    void updateLevel(int level) { mLevel = level; }
    void updateStride(double stride) { mBasicStride = stride; }
    void updateHeight(double height) { mBasicHeight = height; }

    std::vector<Node> getNodes() const;

private:
    int    mLevel;
    double mBasicStride = 1.0;
    double mBasicHeight = 1.0;
};

class BinaryTreeWindow : public Fl_Double_Window
{
public:
    explicit BinaryTreeWindow(int width, int height, std::string_view name);
    virtual void draw() override;
    virtual int  handle(int event) override { return Fl_Double_Window::handle(event); }

private:
    class GraphWindow : public Fl_Double_Window
    {
    public:
        explicit GraphWindow(int x, int y, int width, int height, BinaryTreeWindow *mainWindow);
        virtual void draw() override;
        virtual int  handle(int event) override { return Fl_Double_Window::handle(event); }

        void   updateScaler(double mult) { mScaler *= mult; }
        double getScaler() const { return mScaler; }
        void   updateShift(double dx, double dy) { mShift.x += dx, mShift.y += dy; }
        double getShiftX() const { return mShift.x; }
        double getShiftY() const { return mShift.y; }

        class WindowEventHandler : public Fl_Button
        {
        public:
            explicit WindowEventHandler(GraphWindow *window);
            virtual void draw() override;
            virtual int  handle(int event) override;

        private:
            GraphWindow *mWindow;
        };

    private:

        BinaryTreeWindow *mMainWindow;
        double            mScaler;
        Node              mShift;
    };

    class ConsoleWindow : public Fl_Double_Window
    {
    public:
        explicit ConsoleWindow(int x, int y, int width, int height, BinaryTreeWindow *mainWindow);
        virtual void draw() override { Fl_Double_Window::draw(); }
        virtual int  handle(int event) override { return Fl_Double_Window::handle(event); }

    private:
        BinaryTreeWindow *mMainWindow;
    };

    class Slider : public Fl_Hor_Slider
    {
    public:
        explicit Slider(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label);
        virtual void draw() override;

        void setValue(int min, int max, int step, int init, int *value);

    private:
        std::string mName;

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

    BinaryTree mBinaryTree;
    int        mLevel;
    int        mBasicStride;
    int        mBasicHeight;
    int        mNodeRadius;
    Fl_Color   mLineColor;
    Fl_Color   mNodeColor;
};

/* Debug */
#include <iostream>