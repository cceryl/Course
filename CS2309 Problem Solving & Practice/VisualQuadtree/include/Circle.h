#pragma once

#include <FL/fl_draw.H>

#include "Vec2.h"

namespace VisualQuadtree
{
    class Circle
    {
    public:
        Circle(const Vec2 &center, const Vec2 &velocity, float radius, Fl_Color color);
        virtual ~Circle()                 = default;
        Circle(const Circle &)            = default;
        Circle &operator=(const Circle &) = default;
        Circle(Circle &&)                 = default;
        Circle &operator=(Circle &&)      = default;

        /// @brief Check if the circle is inside the given rectangle.
        bool inside(float x1, float x2, float y1, float y2) const;

        /// @brief Check if the circle collides with another circle.
        bool collideCheck(const Circle &other) const;
        /// @brief Simulate collision with another circle.
        void collide(Circle &other);
        /// @brief Simulate collision with boundaries.
        void collide(float x1, float x2, float y1, float y2);

        /// @brief Update the circle's position.
        void update(double time);

        /// @brief Draw the circle.
        void draw(bool fill) const;

        static void  setRestitution(float restitution);
        static float getRestitution();

    private:
        Vec2     mCenter;
        Vec2     mVelocity;
        float    mRadius;
        Fl_Color mColor;

        inline static float       sRestitution = 1.0f;
        inline static const float epsilon      = 0.01f;
    };

} // namespace VisualQuadtree