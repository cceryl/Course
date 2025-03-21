#include "RadioButton.h"

#include <FL/fl_draw.H>

namespace VisualQuadtree
{
    RadioButton::RadioButton(float x, float y, float width, float height, std::string_view label, Fl_Group *group) :
        Fl_Round_Button(x * group->w(), y * group->h(), width * group->w(), height * group->h(), label.data())
    {
        this->align(FL_ALIGN_CENTER);
        this->box(FL_GTK_ROUND_UP_BOX);
        this->labelfont(FL_TIMES);
        this->labelsize(22);
        this->labelcolor(fl_rgb_color(0x3F, 0x3F, 0x3F));
        this->type(102);

        this->callback([](Fl_Widget *widget, void *data) { widget->parent()->redraw(); });
    }

    void RadioButton::draw()
    {
        fl_line_style(FL_SOLID, 1);
        Fl_Round_Button::draw();
    }

    int RadioButton::handle(int event) { return Fl_Round_Button::handle(event); }
    
} // namespace VisualQuadtree