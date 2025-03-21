#include "Slider.h"

#include <FL/Fl_Group.H>
#include <FL/fl_draw.H>

#include <format>

namespace VisualQuadtree
{
    Slider::Slider(float x, float y, float width, float height, std::string_view label, Fl_Double_Window *window) :
        Fl_Hor_Slider(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
    {
        this->box(FL_GTK_DOWN_BOX);
        this->labelfont(FL_TIMES);

        mName = label.data();
    }

    void Slider::draw()
    {
        static std::string label;

        fl_line_style(FL_SOLID, 1);
        label = std::format("{} = {}", mName, this->value());
        this->label(label.c_str());

        Fl_Hor_Slider::draw();
    }

    int Slider::handle(int event) { return Fl_Hor_Slider::handle(event); }

    void Slider::setValue(int min, int max, int step, int init, int &value)
    {
        this->range(min, max);
        this->step(step);
        this->value(init);

        this->user_data(&value);
        this->callback(
            [](Fl_Widget *widget, void *value)
            {
                Fl_Slider *slider               = dynamic_cast<Fl_Slider *>(widget);
                *reinterpret_cast<int *>(value) = slider->value();
                slider->redraw();
                slider->redraw_label();
                slider->parent()->redraw();
            });
    }

    void Slider::updateRange(int min, int max)
    {
        this->range(min, max);

        if (this->value() < min)
            this->value(min);
        if (this->value() > max)
            this->value(max);

        this->redraw();
    }

} // namespace VisualQuadtree