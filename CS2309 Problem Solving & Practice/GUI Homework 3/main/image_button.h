#pragma once

#include <string>
#include <unordered_map>

#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hor_Slider.H>

class ImageButtonWindow : public Fl_Double_Window
{
    class ImageButton;

public:
    explicit ImageButtonWindow(int width, int height, std::string_view name);
    virtual void draw() override { Fl_Double_Window::draw(); }
    virtual int  handle(int event) override { return Fl_Double_Window::handle(event); }

private:
    class GraphWindow : public Fl_Double_Window
    {
    public:
        explicit GraphWindow(int x, int y, int width, int height, ImageButtonWindow *mainWindow);
        virtual void draw() override;
        virtual int  handle(int event) override { return Fl_Double_Window::handle(event); }

        ImageButtonWindow *mMainWindow;

        ImageButton *mImageButton;
        bool         mRandom;
    };

    class ConsoleWindow : public Fl_Double_Window
    {
    public:
        explicit ConsoleWindow(int x, int y, int width, int height, ImageButtonWindow *mainWindow);
        virtual void draw() override;
        virtual int  handle(int event) override { return Fl_Double_Window::handle(event); }

        bool        mImportImage;
        std::string mImagePath;

        ImageButtonWindow *mMainWindow;
    };

    class Button : public Fl_Button
    {
    public:
        explicit Button(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label, bool *data);
        virtual void draw() override;
        virtual int  handle(int event) override { return Fl_Button::handle(event); }

    private:
        double mX, mY, mWidth, mHeight;
    };

    class ImageButton : public Fl_Button
    {
    public:
        explicit ImageButton(double x, double y, double width, double height, GraphWindow *window, std::string_view label, bool *data);
        virtual void draw() override;
        virtual int  handle(int event) override { return Fl_Button::handle(event); }

        int *getScaler() { return &mScaler; }

    private:
        GraphWindow *mWindow;
        double       mX, mY, mWidth, mHeight;

        std::string   mImagePath;
        Fl_RGB_Image *mImage;
        int           mScaler;
    };

    class Slider : public Fl_Hor_Slider
    {
    public:
        explicit Slider(float x, float y, float width, float height, std::string_view label, Fl_Double_Window *window, std::string_view label);
        virtual void draw() override;

        void setValue(int min, int max, int step, int init, int *value);
        void updateRange(int min, int max);

    private:
        std::string mName;

        double mX, mY, mWidth, mHeight;
    };

    ConsoleWindow *mConsoleWindow;
    GraphWindow   *mGraphWindow;

    void setupConsole();
    void setupGraph();
};