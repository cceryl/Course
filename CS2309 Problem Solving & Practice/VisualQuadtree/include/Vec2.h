#pragma once

#include <initializer_list>

namespace VisualQuadtree
{
    struct Vec2
    {
        Vec2(float x = 0.0f, float y = 0.0f);
        Vec2(std::initializer_list<float> list);
        virtual ~Vec2()               = default;
        Vec2(const Vec2 &)            = default;
        Vec2 &operator=(const Vec2 &) = default;
        Vec2(Vec2 &&)                 = default;
        Vec2 &operator=(Vec2 &&)      = default;

        bool operator==(const Vec2 &rhs) const;
        bool operator!=(const Vec2 &rhs) const;

        Vec2  operator+(const Vec2 &rhs) const;
        Vec2 &operator+=(const Vec2 &rhs);
        Vec2  operator-(const Vec2 &rhs) const;
        Vec2 &operator-=(const Vec2 &rhs);

        Vec2  operator*(float rhs) const;
        Vec2 &operator*=(float rhs);
        Vec2  operator/(float rhs) const;
        Vec2 &operator/=(float rhs);

        friend Vec2 operator*(float lhs, const Vec2 &rhs);
        float       operator*(const Vec2 &rhs) const;

        /// @brief The magnitude of the vector.
        float magnitude() const;
        /// @brief Normalize the vector.
        Vec2 &normalize();

        float x;
        float y;
    };

} // namespace VisualQuadtree