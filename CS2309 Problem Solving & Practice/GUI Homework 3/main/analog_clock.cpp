#include "analog_clock.h"

#include <chrono>
#include <cmath>
#include <functional>

#include <FL/Fl.H>
#include <FL/fl_draw.H>

static void timerCallback(void *data)
{
    reinterpret_cast<ClockWindow *>(data)->redraw();
    Fl::repeat_timeout(0.1, timerCallback, data);
}

ClockWindow::ClockWindow(int width, int height, std::string_view title) : Fl_Double_Window(width, height, title.data())
{
    this->border(true);
    this->box(FL_GTK_THIN_DOWN_BOX);
    this->clip_children(false);
    this->color(FL_GRAY);
    this->resizable(this);
    this->size_range(800, 600);

    this->begin();

    int    side  = std::min(width, height) * 0.8;
    Clock *clock = new Clock((width - side) / 2, (height - side) / 2, side, side);
    Fl::add_timeout(0.1, timerCallback, this);

    this->end();
}

ClockWindow::Clock::Clock(int x, int y, int width, int height) : Fl_Box(x, y, width, height, nullptr)
{
    this->box(FL_NO_BOX);
    this->color(FL_GRAY, FL_GRAY);
}

void ClockWindow::Clock::draw()
{
    double side = std::min(this->parent()->w(), this->parent()->h()) * 0.8;
    this->resize((this->parent()->w() - side) / 2, (this->parent()->h() - side) / 2, side, side);
    Fl_Box::draw();

    fl_line_style(FL_SOLID, 2);
    fl_color(FL_BLACK);
    double originX = this->x() + side / 2;
    double originY = this->y() + side / 2;
    double radius  = side / 2;

    fl_circle(originX, originY, radius);
    for (int i = 0; i < 12; ++i)
    {
        double theta = i * 30 * M_PI / 180;
        fl_line(originX + radius * cos(theta), originY + radius * sin(theta), originX + radius * 0.9 * cos(theta),
                originY + radius * 0.9 * sin(theta));
    }

    fl_font(FL_HELVETICA_BOLD, 20);
    for (int i = 0; i < 12; ++i)
    {
        double theta = i * 30 * M_PI / 180 - M_PI / 3;
        fl_draw(std::to_string(i + 1).c_str(), originX + radius * 1.1 * cos(theta) - 5 * (i >= 9) - 5,
                originY + radius * 1.1 * sin(theta) + 5);
    }

    std::chrono::zoned_time time("Asia/Shanghai", std::chrono::high_resolution_clock::now());
    auto                    timeStamp = time.get_local_time().time_since_epoch();

    auto hour   = std::chrono::duration_cast<std::chrono::hours>(timeStamp).count() % 12;
    auto minute = std::chrono::duration_cast<std::chrono::minutes>(timeStamp).count() % 60;
    auto second = std::chrono::duration_cast<std::chrono::seconds>(timeStamp).count() % 60;

    double hourAngle   = (hour + minute / 60.0) * 30 * M_PI / 180 - M_PI / 2;
    double minuteAngle = (minute + second / 60.0) * 6 * M_PI / 180 - M_PI / 2;
    double secondAngle = second * 6 * M_PI / 180 - M_PI / 2;

    fl_line_style(FL_SOLID, 4);
    fl_line(originX, originY, originX + radius * 0.5 * cos(hourAngle), originY + radius * 0.5 * sin(hourAngle));
    fl_line_style(FL_SOLID, 3);
    fl_line(originX, originY, originX + radius * 0.7 * cos(minuteAngle), originY + radius * 0.7 * sin(minuteAngle));
    fl_line_style(FL_SOLID, 2);
    fl_color(FL_RED);
    fl_line(originX, originY, originX + radius * 0.9 * cos(secondAngle), originY + radius * 0.9 * sin(secondAngle));
}