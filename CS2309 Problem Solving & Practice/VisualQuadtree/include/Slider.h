#pragma once

#include <string>

#include <FL/FL_Hor_Slider.H>
#include <FL/Fl_Double_Window.H>

namespace VisualQuadtree
{
    class Slider : public Fl_Hor_Slider
    {
    public:
        explicit Slider(float x, float y, float width, float height, std::string_view label, Fl_Double_Window *window);
        virtual ~Slider() = default;

        virtual void draw() override;
        virtual int  handle(int event) override;

        void setValue(int min, int max, int step, int init, int &value);
        void updateRange(int min, int max);

    private:
        std::string mName;
    };

} // namespace VisualQuadtree