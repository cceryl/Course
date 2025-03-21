#include "Polygon.h"

#include <cmath>
#include <format>
#include <fstream>
#include <regex>

#include <FL/fl_draw.H>

#include "Platform.h"

namespace MapAreaCalculator
{
    Polygon::Polygon() : mSelectScale(false) {}

    void Polygon::push(int x, int y)
    {
        if (mSelectScale)
        {
            mScale.push_back({x, y});
            if (mScale.size() == 2)
                mSelectScale = false;
            return;
        }

        mVertices.push_back({x, y});

        if (mVertices.size() <= 3)
            return;
        std::size_t last    = mVertices.size() - 1;
        Point       oldBack = mVertices[last - 1], newBack = mVertices.back();

        while (!mConflicts.empty() && mConflicts.back().b1 == oldBack && mConflicts.back().b2 == mVertices.front())
            mConflicts.pop_back();
        for (std::size_t i = 0; i + 1 < last - 1; ++i)
            if (this->intersect(i, i + 1, last - 1, last))
                mConflicts.push_back({mVertices[i], mVertices[i + 1], oldBack, newBack});
        for (std::size_t i = 1; i + 1 < last; ++i)
            if (this->intersect(i, i + 1, last, 0))
                mConflicts.push_back({mVertices[i], mVertices[i + 1], newBack, mVertices.front()});
    }

    void Polygon::pop()
    {
        if (mSelectScale)
        {
            if (!mScale.empty())
                mScale.pop_back();
            return;
        }

        if (mVertices.empty())
            return;

        if (mVertices.size() > 3)
        {
            std::size_t last    = mVertices.size() - 1;
            Point       oldBack = mVertices.back(), newBack = mVertices[last - 1];

            while (!mConflicts.empty() && mConflicts.back().b1 == oldBack && mConflicts.back().b2 == mVertices.front())
                mConflicts.pop_back();
            while (!mConflicts.empty() && mConflicts.back().b1 == newBack && mConflicts.back().b2 == oldBack)
                mConflicts.pop_back();
            for (std::size_t i = 1; i + 1 < last - 1; ++i)
                if (this->intersect(i, i + 1, last - 1, 0))
                    mConflicts.push_back({mVertices[i], mVertices[i + 1], newBack, mVertices.front()});
        }

        mVertices.pop_back();
    }

    void Polygon::clear()
    {
        mVertices.clear();
        mConflicts.clear();
        mScale.clear();
    }

    void Polygon::importVertices(std::string_view verticesPath, int width, int height)
    {
        if (verticesPath.empty())
            return;
        mVertices.clear();
        mConflicts.clear();

        std::fstream file(verticesPath.data(), std::ios::in);
        if (!file.is_open())
        {
            errorMessage(std::format(R"(Failed to open file "{}".)", std::string(verticesPath)));
            return;
        }

        static const std::regex regex(R"(^\s*(\d+)\s+(\d+)\s*$)");
        std::string             line;
        try
        {
            while (std::getline(file, line))
            {
                std::smatch match;
                if (!std::regex_match(line, match, regex))
                {
                    errorMessage(std::format(R"(Invalid format in file "{}".)", std::string(verticesPath)));
                    mVertices.clear();
                    mConflicts.clear();
                    return;
                }
                int x = std::stoi(match[1]), y = std::stoi(match[2]);
                if (x < 0 || x > width || y < 0 || y > height)
                {
                    errorMessage(std::format(R"(Vertex ({}, {}) is out of range.)", x, y));
                    mVertices.clear();
                    mConflicts.clear();
                    return;
                }
                else
                    this->push(x, y);
            }
        }
        catch (const std::exception &exception)
        {
            errorMessage(std::format(R"(Invalid format in file "{}": {}.)", std::string(verticesPath), exception.what()));
            mVertices.clear();
            mConflicts.clear();
            return;
        }
    }

    void Polygon::exportVertices(std::string_view verticesPath) const
    {
        std::fstream file(verticesPath.data(), std::ios::out);
        if (!file.is_open())
        {
            errorMessage(std::format(R"(Failed to open file "{}".)", std::string(verticesPath)));
            return;
        }

        for (const auto &vertex : mVertices)
            file << vertex.x << ' ' << vertex.y << '\n';
    }

    void Polygon::selectScale()
    {
        mScale.clear();
        mSelectScale = true;
    }

    double Polygon::calcArea() const
    {
        if (mVertices.size() < 3 || !mConflicts.empty())
            return -1.0;

        double area  = 0.0;
        auto   cross = [](const Point &a, const Point &b) { return a.x * b.y - a.y * b.x; };

        for (int i = 0; i < mVertices.size(); ++i)
            area += cross(mVertices[i], mVertices[(i + 1) % mVertices.size()]);

        area = std::abs(area) / 2.0;
        return area;
    }

    double Polygon::getScaler() const
    {
        if (mScale.size() != 2)
            return -1.0;
        int dx = mScale.back().x - mScale.front().x, dy = mScale.back().y - mScale.front().y;
        return std::sqrt(dx * dx + dy * dy);
    }

    void Polygon::draw() const
    {
        fl_line_style(FL_SOLID, 4);

        fl_color(fl_rgb_color(0x00, 0x00, 0xFF));
        fl_begin_loop();
        for (const auto &vertex : mVertices)
            fl_vertex(vertex.x, vertex.y);
        fl_end_loop();

        fl_color(fl_rgb_color(0xFF, 0x00, 0x00));
        for (const auto &[a1, a2, b1, b2] : mConflicts)
        {
            fl_line(a1.x, a1.y, a2.x, a2.y);
            fl_line(b1.x, b1.y, b2.x, b2.y);
        }

        fl_color(fl_rgb_color(0x00, 0x7F, 0xFF));
        fl_begin_polygon();
        for (const auto &vertex : mVertices)
            fl_vertex(vertex.x, vertex.y);
        fl_end_polygon();

        fl_color(fl_rgb_color(0x7F, 0xFF, 0x00));
        if (mScale.size() == 2)
            fl_line(mScale.front().x, mScale.front().y, mScale.back().x, mScale.back().y);
    }

    bool Polygon::intersect(std::size_t a1, std::size_t a2, std::size_t b1, std::size_t b2) const
    {
        double x1 = mVertices[a2].x - mVertices[a1].x, y1 = mVertices[a2].y - mVertices[a1].y;
        double x2 = mVertices[b2].x - mVertices[b1].x, y2 = mVertices[b2].y - mVertices[b1].y;
        double cross = x1 * y2 - x2 * y1;

        double interX = (x2 * (mVertices[a1].y - mVertices[b1].y) - y2 * (mVertices[a1].x - mVertices[b1].x)) / cross;
        double interY = (x1 * (mVertices[a1].y - mVertices[b1].y) - y1 * (mVertices[a1].x - mVertices[b1].x)) / cross;

        return 0 <= interX && interX <= 1 && 0 <= interY && interY <= 1;
    }

} // namespace MapAreaCalculator