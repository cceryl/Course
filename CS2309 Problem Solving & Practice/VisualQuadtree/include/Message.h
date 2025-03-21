#pragma once

#include <string>

#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>

namespace VisualQuadtree
{
    class Message : public Fl_Box
    {
    public:
        explicit Message(float x, float y, float width, float height, std::string_view message, Fl_Double_Window *window,
                         bool alignCenter = true, int fontSize = 22);
        virtual ~Message() = default;

        virtual void draw() override;
        virtual int  handle(int event) override;

        void updateMessage(std::string_view message);

        std::string mMessage;
    };

} // namespace VisualQuadtree