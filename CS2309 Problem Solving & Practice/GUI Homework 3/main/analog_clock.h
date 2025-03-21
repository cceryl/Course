#pragma once

#include <string>

#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>

class ClockWindow : public Fl_Double_Window
{
public:
    ClockWindow(int width, int height, std::string_view title);
    void draw() override { Fl_Double_Window::draw();}
    int  handle(int event) override { return Fl_Double_Window::handle(event); }

    class Clock : public Fl_Box
    {
    public:
        Clock(int x, int y, int width, int height);
        void draw() override;
        int  handle(int event) override { return Fl_Box::handle(event); }
    };
};