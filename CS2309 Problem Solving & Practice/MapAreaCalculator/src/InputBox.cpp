#include "InputBox.h"

#include <FL/Fl_Group.H>
#include <FL/fl_draw.H>

namespace MapAreaCalculator
{
    InputBox::InputBox(float x, float y, float width, float height, Fl_Double_Window *window, std::string_view label, std::string &value) :
        Fl_Float_Input(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
    {
        this->box(FL_GTK_DOWN_BOX);
        this->labelfont(FL_TIMES);
        this->align(FL_ALIGN_RIGHT);

        mX      = x;
        mY      = y;
        mWidth  = width;
        mHeight = height;

        this->user_data(&value);
        this->callback(
            [](Fl_Widget *input, void *value)
            {
                *reinterpret_cast<std::string *>(value) = dynamic_cast<Fl_Float_Input *>(input)->value();
                input->redraw();
            });
    }

    void InputBox::draw()
    {
        fl_line_style(FL_SOLID, 1);
        Fl_Group *parent = this->parent();
        this->resize(mX * parent->w(), mY * parent->h(), mWidth * parent->w(), mHeight * parent->h());
        Fl_Float_Input::draw();
    }

    int InputBox::handle(int event) { return Fl_Float_Input::handle(event); }

} // namespace MapAreaCalculator