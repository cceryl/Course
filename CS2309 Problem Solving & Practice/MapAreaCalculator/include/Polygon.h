#pragma once

#include <string>
#include <vector>

namespace MapAreaCalculator
{
    class Polygon
    {
    public:
        explicit Polygon();
        virtual ~Polygon()                  = default;
        Polygon(const Polygon &)            = delete;
        Polygon &operator=(const Polygon &) = delete;
        Polygon(Polygon &&)                 = delete;
        Polygon &operator=(Polygon &&)      = delete;

        void push(int x, int y);
        void pop();

        void clear();

        void importVertices(std::string_view verticesPath, int width, int height);
        void exportVertices(std::string_view verticesPath) const;

        void   selectScale();
        double calcArea() const;
        double getScaler() const;
        void   draw() const;

    private:
        struct Point
        {
            int x;
            int y;

            bool operator==(const Point &rhs) const { return x == rhs.x && y == rhs.y; }
        };

        struct Conflict
        {
            Point a1, a2;
            Point b1, b2;
        };

        bool mSelectScale;

        std::vector<Point>    mVertices;
        std::vector<Conflict> mConflicts;
        std::vector<Point>    mScale;

        bool intersect(std::size_t a1, std::size_t a2, std::size_t b1, std::size_t b2) const;
    };

} // namespace MapAreaCalculator