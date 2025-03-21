#pragma once

#include <string>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Float_Input.H>

namespace MapAreaCalculator
{
    class InputBox : public Fl_Float_Input
    {
    public:
        explicit InputBox(float x, float y, float width, float height, Fl_Double_Window *window, std::string_view label,
                          std::string &value);
        virtual ~InputBox() = default;

        virtual void draw() override;
        virtual int  handle(int event) override;

    private:
        float mX, mY;
        float mWidth, mHeight;
    };

} // namespace MapAreaCalculator