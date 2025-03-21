#pragma once

#include <string>

#include <FL/Fl_Group.H>
#include <FL/Fl_Round_Button.H>

namespace VisualQuadtree
{
    class RadioButton : public Fl_Round_Button
    {
    public:
        explicit RadioButton(float x, float y, float width, float height, std::string_view label, Fl_Group *group);
        virtual ~RadioButton() = default;

        virtual void draw() override;
        virtual int  handle(int event) override;
    };

} // namespace VisualQuadtree