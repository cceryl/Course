#pragma once

#include <array>
#include <memory>
#include <vector>

#include "Circle.h"

namespace VisualQuadtree
{
    class Quadtree
    {
    public:
        explicit Quadtree(int width, int height);
        virtual ~Quadtree()                   = default;
        Quadtree(const Quadtree &)            = delete;
        Quadtree &operator=(const Quadtree &) = delete;
        Quadtree(Quadtree &&)                 = delete;
        Quadtree &operator=(Quadtree &&)      = delete;

        /// @brief Insert a circle into the quadtree. Do not check collision.
        /// @return True if the circle was inserted, false otherwise.
        bool insert(const std::shared_ptr<Circle> &circle);

        /// @brief Check if the circle will collide with any other circle in quadtree.
        bool collideCheck(const std::shared_ptr<Circle> &circle) const;

        /// @brief Update the quadtree based on the current state of the circles and simulate collision.
        void update();

        /// @brief Clear the quadtree.
        void clear();

        /// @brief Draw the quadtree. Do not draw circles.
        void draw() const;
        
        /// @brief Set the circle limit for the quadtree.
        static void setCircleLimit(int circleLimit);

    private:
        struct Node
        {
        public:
            explicit Node(int x1, int x2, int y1, int y2);
            virtual ~Node()               = default;
            Node(const Node &)            = delete;
            Node &operator=(const Node &) = delete;
            Node(Node &&)                 = delete;
            Node &operator=(Node &&)      = delete;

            /// @brief Insert a circle into the node. Do not check collision.
            /// @return True if the circle was inserted, false otherwise.
            bool insert(const std::shared_ptr<Circle> &circle);

            /// @brief Recursively check if the circle collide with any other circle in the node.
            bool collideCheck(const std::shared_ptr<Circle> &circle) const;

            /// @brief Update the node based on the current state of the circles.
            /// @return Circles that are out of the node.
            std::vector<std::shared_ptr<Circle>> update(std::vector<std::shared_ptr<Circle>> parentCircles = {});

            /// @brief Recursively draw the node.
            void draw(int depth) const;

            /// @brief Set the circle limit for the node.
            static void setCircleLimit(int circleLimit);

        private:
            enum { TopRight = 0, TopLeft = 1, BottomLeft = 2, BottomRight = 3 };
            std::array<std::unique_ptr<Node>, 4> mChildren;
            std::vector<std::shared_ptr<Circle>> mCircles;
            int                                  mNumCircles;

            int mX1, mX2;
            int mY1, mY2;

            inline static int sCircleLimit = 20;

            /// @brief Check if the node contains the given circle.
            bool contain(const std::shared_ptr<Circle> &circle) const;

            /// @brief Check if the node is a leaf.
            bool isLeaf() const;

            /// @brief Split the node into 4 children.
            void split();
            /// @brief Merge the node's children into the node.
            void merge();
        };

        std::unique_ptr<Node> mRoot;
        int                   mWidth;
        int                   mHeight;
    };

} // namespace VisualQuadtree