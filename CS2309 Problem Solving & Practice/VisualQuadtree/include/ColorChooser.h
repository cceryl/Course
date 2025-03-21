#pragma once

#include <string>

#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Double_Window.H>

namespace VisualQuadtree
{
    class ColorChooser : public Fl_Color_Chooser
    {
    public:
        explicit ColorChooser(float x, float y, float width, float height, std::string_view label, Fl_Double_Window *window,
                              Fl_Color &value);
        virtual ~ColorChooser() = default;

        virtual void draw() override;
        virtual int  handle(int event) override;
    };

} // namespace VisualQuadtree