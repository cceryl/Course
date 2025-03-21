#pragma once

#include <string>

#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

namespace MapAreaCalculator
{
    class Button : public Fl_Button
    {
    public:
        explicit Button(float x, float y, float width, float height, std::string_view label, Fl_Double_Window *window, bool &value);
        virtual ~Button() = default;

        virtual void draw() override;
        virtual int  handle(int event) override;

    private:
        float mX, mY;
        float mWidth, mHeight;
    };

} // namespace MapAreaCalculator