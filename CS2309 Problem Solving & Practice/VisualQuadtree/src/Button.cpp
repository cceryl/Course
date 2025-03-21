#include "Button.h"

#include <FL/fl_draw.H>

namespace VisualQuadtree
{
    Button::Button(float x, float y, float width, float height, std::string_view label, Fl_Double_Window *window,
                   bool &value) :
        Fl_Button(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
    {
        this->align(FL_ALIGN_CENTER);
        this->box(FL_GTK_ROUND_UP_BOX);
        this->labelfont(FL_TIMES);
        this->labelsize(22);
        this->labelcolor(fl_rgb_color(0x3F, 0x3F, 0x3F));

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
        Fl_Button::draw();
    }

    int Button::handle(int event) { return Fl_Button::handle(event); }

    void Button::setLabel(std::string_view label)
    {
        this->label(label.data());
        this->redraw();
        this->redraw_label();
    }

} // namespace VisualQuadtree