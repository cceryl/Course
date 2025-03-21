#include "Vec2.h"

#include <cmath>
#include <stdexcept>

namespace VisualQuadtree
{
    Vec2::Vec2(float x, float y) : x(x), y(y) {}

    Vec2::Vec2(std::initializer_list<float> list)
    {
#ifdef DEBUG
        if (list.size() != 2)
            throw std::runtime_error("Vec2 initializer list must have 2 elements");
#endif
        auto iter = list.begin();
        x         = *iter;
        y         = *(++iter);
    }

    bool Vec2::operator==(const Vec2 &rhs) const { return x == rhs.x && y == rhs.y; }
    bool Vec2::operator!=(const Vec2 &rhs) const { return x != rhs.x || y != rhs.y; }

    Vec2  Vec2::operator+(const Vec2 &rhs) const { return {x + rhs.x, y + rhs.y}; }
    Vec2 &Vec2::operator+=(const Vec2 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    Vec2  VisualQuadtree::Vec2::operator-(const Vec2 &rhs) const { return {x - rhs.x, y - rhs.y}; }
    Vec2 &VisualQuadtree::Vec2::operator-=(const Vec2 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vec2  VisualQuadtree::Vec2::operator*(float rhs) const { return {x * rhs, y * rhs}; }
    Vec2 &VisualQuadtree::Vec2::operator*=(float rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }
    Vec2  VisualQuadtree::Vec2::operator/(float rhs) const { return {x / rhs, y / rhs}; }
    Vec2 &VisualQuadtree::Vec2::operator/=(float rhs)
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    Vec2  operator*(float lhs, const Vec2 &rhs) { return {lhs * rhs.x, lhs * rhs.y}; }
    float VisualQuadtree::Vec2::operator*(const Vec2 &rhs) const { return x * rhs.x + y * rhs.y; }

    float VisualQuadtree::Vec2::magnitude() const { return std::sqrt(x * x + y * y); }
    Vec2 &Vec2::normalize() { return *this /= this->magnitude(); }

} // namespace VisualQuadtree