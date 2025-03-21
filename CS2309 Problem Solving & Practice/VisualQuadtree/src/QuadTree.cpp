#include "Quadtree.h"

#include <algorithm>
#include <stdexcept>

#include <FL/fl_draw.H>

namespace VisualQuadtree
{
    Quadtree::Node::Node(int x1, int x2, int y1, int y2) : mNumCircles(0)
    {
#ifdef DEBUG
        if (x1 > x2)
            throw std::invalid_argument("x1 must be less than x2");
        if (y1 > y2)
            throw std::invalid_argument("y1 must be less than y2");
#endif
        mX1 = x1, mX2 = x2;
        mY1 = y1, mY2 = y2;
    }

    bool Quadtree::Node::insert(const std::shared_ptr<Circle> &circle)
    {
        if (!this->contain(circle))
            return false;
        ++mNumCircles;

        if (this->isLeaf())
        {
            if (mNumCircles < sCircleLimit)
            {
                mCircles.push_back(circle);
                return true;
            }
            this->split();
        }

        if (std::none_of(mChildren.begin(), mChildren.end(), [&circle](const auto &child) { return child->insert(circle); }))
            mCircles.push_back(circle);
        return true;
    }

    bool Quadtree::Node::collideCheck(const std::shared_ptr<Circle> &circle) const
    {
        for (const auto &other : mCircles)
            if (circle->collideCheck(*other))
                return true;

        if (this->isLeaf())
            return false;

        for (const auto &child : mChildren)
            if (child->contain(circle))
                return child->collideCheck(circle);

        return std::any_of(mChildren.begin(), mChildren.end(), [&circle](const auto &child) { return child->collideCheck(circle); });
    }

    std::vector<std::shared_ptr<Circle>> Quadtree::Node::update(std::vector<std::shared_ptr<Circle>> parentCircles)
    {
        std::vector<std::shared_ptr<Circle>> outOfBoundsCircles;

        std::erase_if(mCircles,
                      [this, &outOfBoundsCircles](auto &circle)
                      {
                          if (!this->contain(circle))
                          {
                              outOfBoundsCircles.push_back(circle);
                              return true;
                          }
                          return false;
                      });

        if (!this->isLeaf())
        {
            std::erase_if(
                mCircles, [this](auto &circle)
                { return std::any_of(mChildren.begin(), mChildren.end(), [&circle](auto &child) { return child->insert(circle); }); });

            std::copy(mCircles.begin(), mCircles.end(), std::back_inserter(parentCircles));

            for (auto &child : mChildren)
            {
                auto &&childOutOfBoundsCircles = child->update(parentCircles);
                for (auto &circle : childOutOfBoundsCircles)
                    if (this->contain(circle))
                        mCircles.push_back(circle);
                    else
                        outOfBoundsCircles.push_back(circle);
            }
        }

        for (std::size_t i = 0; i < mCircles.size(); ++i)
            for (std::size_t j = i + 1; j < mCircles.size(); ++j)
                if (mCircles[i]->collideCheck(*mCircles[j]))
                    mCircles[i]->collide(*mCircles[j]);

        for (std::size_t i = 0; i < parentCircles.size(); ++i)
            for (std::size_t j = i + 1; j < parentCircles.size(); ++j)
                if (parentCircles[i]->collideCheck(*parentCircles[j]))
                    parentCircles[i]->collide(*parentCircles[j]);

        for (auto &circle : mCircles)
            for (auto &other : parentCircles)
                if (circle->collideCheck(*other))
                    circle->collide(*other);

        mNumCircles -= outOfBoundsCircles.size();
        if (mNumCircles > sCircleLimit && this->isLeaf())
            this->split();
        else if (mNumCircles < sCircleLimit * 0.75)
            this->merge();

        return outOfBoundsCircles;
    }

    void Quadtree::Node::draw(int depth) const
    {
        if (this->isLeaf())
            return;

        fl_color(FL_RED);
        fl_line_style(FL_SOLID, std::max(1, 4 - depth));
        fl_line(mX1, (mY1 + mY2) / 2, mX2, (mY1 + mY2) / 2);
        fl_line((mX1 + mX2) / 2, mY1, (mX1 + mX2) / 2, mY2);
        for (const auto &child : mChildren)
            child->draw(depth + 1);
    }

    void Quadtree::Node::setCircleLimit(int circleLimit) { sCircleLimit = circleLimit; }

    bool Quadtree::Node::contain(const std::shared_ptr<Circle> &circle) const { return circle->inside(mX1, mX2, mY1, mY2); }

    bool Quadtree::Node::isLeaf() const { return mChildren.front() == nullptr; }

    void Quadtree::Node::split()
    {
#ifdef DEBUG
        if (!this->isLeaf())
            throw std::runtime_error("Cannot split a non-leaf node");
#endif
        int xMid = (mX1 + mX2) / 2;
        int yMid = (mY1 + mY2) / 2;

        mChildren[TopRight]    = std::make_unique<Node>(xMid, mX2, mY1, yMid);
        mChildren[TopLeft]     = std::make_unique<Node>(mX1, xMid, mY1, yMid);
        mChildren[BottomLeft]  = std::make_unique<Node>(mX1, xMid, yMid, mY2);
        mChildren[BottomRight] = std::make_unique<Node>(xMid, mX2, yMid, mY2);

        std::erase_if(mCircles,
                      [this](auto &circle) {
                          return std::any_of(mChildren.begin(), mChildren.end(), [&circle](auto &child) { return child->insert(circle); });
                      });
    }

    void Quadtree::Node::merge()
    {
        if (this->isLeaf())
            return;

        for (auto &child : mChildren)
        {
            child->merge();
            std::move(child->mCircles.begin(), child->mCircles.end(), std::back_inserter(mCircles));
            child.reset();
        }
    }

    Quadtree::Quadtree(int width, int height) : mWidth(width), mHeight(height) { mRoot = std::make_unique<Node>(0, width, 0, height); }

    bool Quadtree::insert(const std::shared_ptr<Circle> &circle) { return mRoot->insert(circle); }

    bool Quadtree::collideCheck(const std::shared_ptr<Circle> &circle) const { return mRoot->collideCheck(circle); }

    void Quadtree::update()
    {
        auto &&outOfBoundsCircles = mRoot->update();
        for (auto &circle : outOfBoundsCircles)
        {
            circle->collide(0, mWidth, 0, mHeight);
            mRoot->insert(circle);
        }
    }

    void Quadtree::clear()
    {
        mRoot.reset();
        mRoot = std::make_unique<Node>(0, mWidth, 0, mHeight);
    }

    void Quadtree::draw() const { mRoot->draw(0); }

    void Quadtree::setCircleLimit(int circleLimit) { Node::setCircleLimit(circleLimit); }

} // namespace VisualQuadtree