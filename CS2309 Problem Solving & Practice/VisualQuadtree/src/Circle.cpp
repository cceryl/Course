#include "Circle.h"

#include <stdexcept>

namespace VisualQuadtree
{
    Circle::Circle(const Vec2 &center, const Vec2 &velocity, float radius, Fl_Color color) : mCenter(center), mVelocity(velocity)
    {
#ifdef DEBUG
        if (radius <= 0)
            throw std::invalid_argument("Circle radius must be positive");
#endif
        mRadius = radius;
        mColor  = color;
    }

    bool Circle::inside(float x1, float x2, float y1, float y2) const
    {
#ifdef DEBUG
        if (x1 >= x2)
            throw std::invalid_argument("x1 must be less than x2");
        if (y1 >= y2)
            throw std::invalid_argument("y1 must be less than y2");
#endif
        return x1 <= mCenter.x - mRadius && mCenter.x + mRadius <= x2 && y1 <= mCenter.y - mRadius && mCenter.y + mRadius <= y2;
    }

    bool Circle::collideCheck(const Circle &other) const
    {
        if (this == &other)
            return false;
        return (mCenter - other.mCenter).magnitude() < mRadius + other.mRadius;
    }

    void Circle::collide(Circle &other)
    {
#ifdef DEBUG
        if (!this->collideCheck(other))
            throw std::runtime_error("Circles do not collide");
#endif

        Vec2  normal   = other.mCenter - mCenter;
        float distance = normal.magnitude();
        normal.normalize();
        float m1 = mRadius * mRadius;
        float m2 = other.mRadius * other.mRadius;

        Vec2 v1n         = normal * (mVelocity * normal);
        Vec2 v2n         = normal * (other.mVelocity * normal);
        mVelocity       -= (1.0f + sRestitution) * m2 / (m1 + m2) * (v1n - v2n);
        other.mVelocity -= (1.0f + sRestitution) * m1 / (m1 + m2) * (v2n - v1n);

        float overlap  = mRadius + other.mRadius - distance;
        Vec2  offset   = normal * overlap * (0.5f + epsilon);
        mCenter       -= offset;
        other.mCenter += offset;
    }

    void Circle::collide(float x1, float x2, float y1, float y2)
    {
#ifdef DEBUG
        if (x1 >= x2)
            throw std::invalid_argument("x1 must be less than x2");
        if (y1 >= y2)
            throw std::invalid_argument("y1 must be less than y2");
#endif
        if (mCenter.x - mRadius < x1)
        {
            mVelocity.x = -mVelocity.x * sRestitution;
            mCenter.x   = x1 + mRadius + epsilon;
        }
        else if (mCenter.x + mRadius > x2)
        {
            mVelocity.x = -mVelocity.x * sRestitution;
            mCenter.x   = x2 - mRadius - epsilon;
        }
        if (mCenter.y - mRadius < y1)
        {
            mVelocity.y = -mVelocity.y * sRestitution;
            mCenter.y   = y1 + mRadius + epsilon;
        }
        else if (mCenter.y + mRadius > y2)
        {
            mVelocity.y = -mVelocity.y * sRestitution;
            mCenter.y   = y2 - mRadius - epsilon;
        }
    }

    void Circle::update(double time) { mCenter += mVelocity * time; }

    void Circle::draw(bool fill) const
    {
        fl_color(mColor);
        fl_line_style(FL_SOLID, 3);
        if (fill)
            fl_pie(mCenter.x - mRadius, mCenter.y - mRadius, mRadius * 2, mRadius * 2, 0, 360);
        else
            fl_circle(mCenter.x, mCenter.y, mRadius);
    }

    void Circle::setRestitution(float restitution) { sRestitution = restitution; }

    float Circle::getRestitution() { return sRestitution; }

} // namespace VisualQuadtree