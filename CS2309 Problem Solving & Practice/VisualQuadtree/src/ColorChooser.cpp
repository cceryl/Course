#include "ColorChooser.h"

#include <FL/fl_draw.H>

namespace VisualQuadtree
{
    ColorChooser::ColorChooser(float x, float y, float width, float height, std::string_view label, Fl_Double_Window *window,
                               Fl_Color &value) :
        Fl_Color_Chooser(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
    {
        this->box(FL_GTK_DOWN_BOX);
        this->labelfont(FL_TIMES);
        this->align(FL_ALIGN_BOTTOM);

        this->rgb(0x00 / 255.0, 0x7F / 255.0, 0xFF / 255.0);
        this->user_data(&value);
        this->callback(
            [](Fl_Widget *widget, void *data)
            {
                Fl_Color_Chooser *colorChooser = dynamic_cast<Fl_Color_Chooser *>(widget);
                colorChooser->redraw();
                *reinterpret_cast<Fl_Color *>(data) =
                    fl_rgb_color(colorChooser->r() * 255, colorChooser->g() * 255, colorChooser->b() * 255);
            });
    }

    void ColorChooser::draw()
    {
        fl_line_style(FL_SOLID, 1);
        Fl_Color_Chooser::draw();
    }

    int ColorChooser::handle(int event) { return Fl_Color_Chooser::handle(event); }

} // namespace VisualQuadtree