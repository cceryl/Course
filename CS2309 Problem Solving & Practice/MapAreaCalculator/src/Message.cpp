#include "Message.h"

#include <FL/Fl_Group.H>
#include <FL/fl_draw.H>

namespace MapAreaCalculator
{
    Message::Message(float x, float y, float width, float height, std::string_view message, Fl_Double_Window *window, bool alignCenter,
                     int fontSize) :
        Fl_Box(x * window->w(), y * window->h(), width * window->w(), height * window->h(), message.data())
    {
        if (alignCenter)
            this->align(FL_ALIGN_CENTER);
        else
            this->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        this->box(FL_NO_BOX);
        this->labelfont(FL_TIMES);
        this->labelsize(fontSize);
        this->labelcolor(FL_BLACK);

        mX      = x;
        mY      = y;
        mWidth  = width;
        mHeight = height;

        mMessage = message;
    }

    void Message::draw()
    {
        fl_line_style(FL_SOLID, 1);
        Fl_Group *parent = this->parent();
        this->resize(mX * parent->w(), mY * parent->h(), mWidth * parent->w(), mHeight * parent->h());
        Fl_Box::draw();
    }

    int Message::handle(int event) { return Fl_Box::handle(event); }

    void Message::updateMessage(std::string_view message)
    {
        mMessage = message;
        this->label(mMessage.data());
        this->redraw();
        this->redraw_label();
    }

} // namespace MapAreaCalculator