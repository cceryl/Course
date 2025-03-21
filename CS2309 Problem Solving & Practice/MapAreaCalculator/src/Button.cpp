#include "Button.h"

#include <FL/Fl_Group.H>
#include <FL/fl_draw.H>

namespace MapAreaCalculator
{
    Button::Button(float x, float y, float width, float height, std::string_view label, Fl_Double_Window *window, bool &value) :
        Fl_Button(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
    {
        this->align(FL_ALIGN_CENTER);
        this->box(FL_GTK_ROUND_UP_BOX);
        this->labelfont(FL_TIMES);
        this->labelsize(22);
        this->labelcolor(fl_rgb_color(0x3F, 0x3F, 0x3F));

        mX      = x;
        mY      = y;
        mWidth  = width;
        mHeight = height;

        this->user_data(&value);
        this->callback(
            [](Fl_Widget *widget, void *data)
            {
                *reinterpret_cast<bool *>(data) = true;
                widget->parent()->redraw();
            });
    }

    void Button::draw()
    {
        fl_line_style(FL_SOLID, 1);
        Fl_Group *parent = this->parent();
        this->resize(mX * parent->w(), mY * parent->h(), mWidth * parent->w(), mHeight * parent->h());
        Fl_Button::draw();
    }

    int Button::handle(int event) { return Fl_Button::handle(event); }

} // namespace MapAreaCalculator