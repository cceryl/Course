#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include "exceptions.hpp"
#include "utility.hpp"
#include <cstddef>
#include <functional>

namespace sjtu
{
    template <class Key, class T, class Compare = std::less<Key>>
    class map
    {
    public:
        typedef pair<const Key, T> value_type;

    private:
        enum Color { red = false, black = true };
        struct node
        {
            value_type *val;
            Color       color;
            node       *parent;
            node       *left;
            node       *right;
            node() : val(nullptr), parent(nullptr), left(nullptr), right(nullptr) {}
            node(const value_type *const _val, Color _color, node *_parent) :
                val(new value_type(*_val)), color(_color), parent(_parent), left(nullptr), right(nullptr)
            {}
            ~node() { delete val; }
        };

        node    head;
        size_t  numOfElements;
        Compare cmp;

        void copy(node *root, const node *const other) noexcept
        {
            if (other->left != nullptr)
            {
                root->left = new node(other->left->val, other->left->color, root);
                copy(root->left, other->left);
            }
            if (other->right != nullptr)
            {
                root->right = new node(other->right->val, other->right->color, root);
                copy(root->right, other->right);
            }
        }
        void remove(node *root) noexcept
        {
            if (root == nullptr)
                return;
            remove(root->left), remove(root->right);
            delete root;
        }

        void LLrotate(node *root, node *L1, node *L2) noexcept
        {
            if (root == root->parent->left)
                root->parent->left = L1;
            else
                root->parent->right = L1;
            L1->parent = root->parent, root->parent = L1;
            root->left = L1->right, L1->right = root;
            if (root->left != nullptr)
                root->left->parent = root;
        }
        void RRrotate(node *root, node *R1, node *R2) noexcept
        {
            if (root == root->parent->left)
                root->parent->left = R1;
            else
                root->parent->right = R1;
            R1->parent = root->parent, root->parent = R1;
            root->right = R1->left, R1->left = root;
            if (root->right != nullptr)
                root->right->parent = root;
        }
        void LRrotate(node *root, node *L, node *R) noexcept
        {
            L->parent = R, R->parent = root;
            root->left = R, L->right = R->left, R->left = L;
            if (L->right != nullptr)
                L->right->parent = L;
            LLrotate(root, R, L);
        }
        void RLrotate(node *root, node *R, node *L) noexcept
        {
            R->parent = L, L->parent = root;
            root->right = L, R->left = L->right, L->right = R;
            if (R->left != nullptr)
                R->left->parent = R;
            RRrotate(root, L, R);
        }

        void adjustInsert(node *target) noexcept
        {
            node *parent = target->parent;
            if (parent->color == black)
                return;
            node *root = parent->parent, *other = (parent == root->left ? root->right : root->left);
            if (other == nullptr || other->color == black)
            {
                if (parent == root->left && target == parent->left)
                    LLrotate(root, parent, target), parent->color = black, root->color = red;
                if (parent == root->right && target == parent->right)
                    RRrotate(root, parent, target), parent->color = black, root->color = red;
                if (parent == root->left && target == parent->right)
                    LRrotate(root, parent, target), target->color = black, root->color = red;
                if (parent == root->right && target == parent->left)
                    RLrotate(root, parent, target), target->color = black, root->color = red;
            }
            else
            {
                root->color = red, parent->color = other->color = black;
                if (root->parent == &head)
                    root->color = black;
                else if (root->parent->color == red)
                    adjustInsert(root);
            }
        }
        void adjustErase(node *target) noexcept
        {
            while (target != head.left)
            {
                node *parent = target->parent, *other = (target == parent->left ? parent->right : parent->left);
                if (other->color == red)
                {
                    other->color = black, parent->color = red;
                    if (other == parent->left)
                        LLrotate(parent, other, other->left);
                    else
                        RRrotate(parent, other, other->right);
                    other = (target == parent->left ? parent->right : parent->left);
                }
                if (other->left && other->left->color == red)
                {
                    Color parentColor = parent->color;
                    parent->color     = black;
                    if (parent->left == target)
                        RLrotate(parent, other, other->left);
                    else
                    {
                        other->left->color = black;
                        LLrotate(parent, other, other->left);
                    }
                    parent->parent->color = parentColor;
                    return;
                }
                else if (other->right && other->right->color == red)
                {
                    Color parentColor = parent->color;
                    parent->color     = black;
                    if (parent->left == target)
                    {
                        other->right->color = black;
                        RRrotate(parent, other, other->right);
                    }
                    else
                        LRrotate(parent, other, other->right);
                    parent->parent->color = parentColor;
                    return;
                }
                if (parent->color == red)
                {
                    parent->color = black, other->color = red;
                    return;
                }
                else
                {
                    other->color = red;
                    target       = parent;
                }
            }
        }

        void swap(node *target, node *replace)
        {
            bool replace_left = replace == replace->parent->left, replace_right = replace == replace->parent->right;
            std::swap(target->color, replace->color);
            if (target == target->parent->left)
                target->parent->left = replace;
            else
                target->parent->right = replace;
            if (replace->left != nullptr)
                replace->left->parent = target;
            if (replace->right != nullptr)
                replace->right->parent = target;
            if (target->left != nullptr && replace_right)
                target->left->parent = replace;
            if (target->right != nullptr && replace_left)
                target->right->parent = replace;
            if (replace_left)
                std::swap(target->right, replace->right);
            else
                std::swap(target->left, replace->left);
            if (replace->parent == target)
            {
                if (replace_left)
                    target->left = replace->left, replace->left = target;
                else
                    target->right = replace->right, replace->right = target;
                replace->parent = target->parent, target->parent = replace;
            }
            else
            {
                if (target->left != nullptr && replace_left)
                    target->left->parent = replace;
                if (target->right != nullptr && replace_right)
                    target->right->parent = replace;
                if (replace_left)
                    replace->parent->left = target;
                else
                    replace->parent->right = target;
                std::swap(target->parent, replace->parent);
                if (replace_left)
                    std::swap(target->left, replace->left);
                else
                    std::swap(target->right, replace->right);
            }
        }

    public:
        class const_iterator;
        class iterator
        {
            friend class const_iterator;
            friend class map;

        private:
            node *target;
            map  *container;

        public:
            iterator(node *_target = nullptr, map *_container = nullptr) : target(_target), container(_container) {}
            iterator(const iterator &other) : target(other.target), container(other.container) {}

            iterator operator++(int)
            {
                if (target == nullptr || target == container->end().target)
                    throw invalid_iterator();
                node *tmp = target;
                if (target->right != nullptr)
                {
                    target = target->right;
                    while (target->left != nullptr)
                        target = target->left;
                }
                else if (target == target->parent->left)
                    target = target->parent;
                else
                {
                    while (target == target->parent->right)
                        target = target->parent;
                    target = target->parent;
                }
                return iterator(tmp, container);
            }
            iterator &operator++()
            {
                if (target == nullptr || target == container->end().target)
                    throw invalid_iterator();
                if (target->right != nullptr)
                {
                    target = target->right;
                    while (target->left != nullptr)
                        target = target->left;
                }
                else if (target == target->parent->left)
                    target = target->parent;
                else
                {
                    while (target == target->parent->right)
                        target = target->parent;
                    target = target->parent;
                }
                return *this;
            }
            iterator operator--(int)
            {
                if (target == nullptr || target == container->begin().target)
                    throw invalid_iterator();
                node *tmp = target;
                if (target->left != nullptr)
                {
                    target = target->left;
                    while (target->right != nullptr)
                        target = target->right;
                }
                else if (target == target->parent->right)
                    target = target->parent;
                else
                {
                    while (target == target->parent->left)
                        target = target->parent;
                    target = target->parent;
                }
                return iterator(tmp, container);
            }
            iterator &operator--()
            {
                if (target == nullptr || target == container->begin().target)
                    throw invalid_iterator();
                if (target->left != nullptr)
                {
                    target = target->left;
                    while (target->right != nullptr)
                        target = target->right;
                }
                else if (target == target->parent->right)
                    target = target->parent;
                else
                {
                    while (target == target->parent->left)
                        target = target->parent;
                    target = target->parent;
                }
                return *this;
            }

            bool operator==(const iterator &rhs) const noexcept { return target == rhs.target; }
            bool operator==(const const_iterator &rhs) const noexcept { return target == rhs.target; }
            bool operator!=(const iterator &rhs) const noexcept { return target != rhs.target; }
            bool operator!=(const const_iterator &rhs) const noexcept { return target != rhs.target; }

            value_type &operator*() const
            {
                if (target == nullptr || target == container->end().target)
                    throw invalid_iterator();
                return *target->val;
            }
            value_type *operator->() const
            {
                if (target == nullptr || target == container->end().target)
                    throw invalid_iterator();
                return target->val;
            }
        };
        class const_iterator
        {
            friend class iterator;
            friend class map;

        private:
            const node *target;
            const map  *container;

        public:
            const_iterator(const node *_target = nullptr, const map *_container = nullptr) : target(_target), container(_container) {}
            const_iterator(const const_iterator &other) : target(other.target), container(other.container) {}
            const_iterator(const iterator &other) : target(other.target), container(other.container) {}

            const_iterator operator++(int)
            {
                if (target == nullptr || target == container->cend().target)
                    throw invalid_iterator();
                const node *tmp = target;
                if (target->right != nullptr)
                {
                    target = target->right;
                    while (target->left != nullptr)
                        target = target->left;
                }
                else if (target == target->parent->left)
                    target = target->parent;
                else
                {
                    while (target == target->parent->right)
                        target = target->parent;
                    target = target->parent;
                }
                return const_iterator(tmp, container);
            }
            const_iterator &operator++()
            {
                if (target == nullptr || target == container->cend().target)
                    throw invalid_iterator();
                if (target->right != nullptr)
                {
                    target = target->right;
                    while (target->left != nullptr)
                        target = target->left;
                }
                else if (target == target->parent->left)
                    target = target->parent;
                else
                {
                    while (target == target->parent->right)
                        target = target->parent;
                    target = target->parent;
                }
                return *this;
            }
            const_iterator operator--(int)
            {
                if (target == nullptr || target == container->cbegin().target)
                    throw invalid_iterator();
                const node *tmp = target;
                if (target->left != nullptr)
                {
                    target = target->left;
                    while (target->right != nullptr)
                        target = target->right;
                }
                else if (target == target->parent->right)
                    target = target->parent;
                else
                {
                    while (target == target->parent->left)
                        target = target->parent;
                    target = target->parent;
                }
                return const_iterator(tmp, container);
            }
            const_iterator &operator--()
            {
                if (target == nullptr || target == container->cbegin().target)
                    throw invalid_iterator();
                if (target->left != nullptr)
                {
                    target = target->left;
                    while (target->right != nullptr)
                        target = target->right;
                }
                else if (target == target->parent->right)
                    target = target->parent;
                else
                {
                    while (target == target->parent->left)
                        target = target->parent;
                    target = target->parent;
                }
                return *this;
            }

            bool operator==(const iterator &rhs) const noexcept { return target == rhs.target; }
            bool operator==(const const_iterator &rhs) const noexcept { return target == rhs.target; }
            bool operator!=(const iterator &rhs) const noexcept { return target != rhs.target; }
            bool operator!=(const const_iterator &rhs) const noexcept { return target != rhs.target; }

            value_type &operator*() const
            {
                if (target == nullptr || target == container->cend().target)
                    throw invalid_iterator();
                return *target->val;
            }
            value_type *operator->() const
            {
                if (target == nullptr || target == container->cend().target)
                    throw invalid_iterator();
                return target->val;
            }
        };

        map() : numOfElements(0) {}
        map(const map &other) : numOfElements(other.numOfElements), cmp(other.cmp) { copy(&head, &other.head); }
        map &operator=(const map &other)
        {
            if (this == &other)
                return *this;
            remove(head.left), head.left = nullptr;
            numOfElements = other.numOfElements, cmp = other.cmp;
            copy(&head, &other.head);
            return *this;
        }
        ~map() { remove(head.left); }

        iterator begin()
        {
            node *target = &head;
            while (target->left != nullptr)
                target = target->left;
            return iterator(target, this);
        }
        const_iterator cbegin() const
        {
            const node *target = &head;
            while (target->left != nullptr)
                target = target->left;
            return const_iterator(target, this);
        }
        iterator       end() { return iterator(&head, this); }
        const_iterator cend() const { return const_iterator(&head, this); }

        bool   empty() const noexcept { return !numOfElements; }
        size_t size() const noexcept { return numOfElements; }
        void   clear()
        {
            remove(head.left), head.left = nullptr;
            numOfElements = 0;
        }

        T &at(const Key &key)
        {
            node *target = head.left;
            while (target != nullptr)
            {
                if (cmp(key, target->val->first))
                    target = target->left;
                else if (cmp(target->val->first, key))
                    target = target->right;
                else
                    return target->val->second;
            }
            throw index_out_of_bound();
        }
        const T &at(const Key &key) const
        {
            const node *target = head.left;
            while (target != nullptr)
            {
                if (cmp(key, target->val->first))
                    target = target->left;
                else if (cmp(target->val->first, key))
                    target = target->right;
                else
                    return target->val->second;
            }
            throw index_out_of_bound();
        }
        T &operator[](const Key &key) noexcept
        {
            node *target = head.left;
            while (target != nullptr)
            {
                if (cmp(key, target->val->first))
                    target = target->left;
                else if (cmp(target->val->first, key))
                    target = target->right;
                else
                    return target->val->second;
            }
            return insert(pair(key, T())).first->second;
        }
        const T &operator[](const Key &key) const { return at(key); }

        pair<iterator, bool> insert(const value_type &value) noexcept
        {
            if (numOfElements == 0)
            {
                head.left = new node(&value, black, &head);
                ++numOfElements;
                return pair(iterator(head.left, this), true);
            }
            node *target = head.left, *prev = &head;
            while (target != nullptr)
            {
                if (cmp(value.first, target->val->first))
                    prev = target, target = target->left;
                else if (cmp(target->val->first, value.first))
                    prev = target, target = target->right;
                else
                    return pair(iterator(target, this), false);
            }
            if (cmp(value.first, prev->val->first))
                target = prev->left = new node(&value, red, prev);
            else
                target = prev->right = new node(&value, red, prev);
            adjustInsert(target);
            ++numOfElements;
            return pair(iterator(target, this), true);
        }
        void erase(iterator pos)
        {
            if (pos.container != this || pos.target == end().target)
                throw index_out_of_bound();
            --numOfElements;
            node *target = pos.target, *replace;
            while (target->left != nullptr || target->right != nullptr)
            {
                if (target->left == nullptr)
                    replace = target->right;
                else if (target->right == nullptr)
                    replace = target->left;
                else
                {
                    replace = target->left;
                    while (replace->right != nullptr)
                        replace = replace->right;
                }
                swap(target, replace);
            }
            if (target->color == black)
                adjustErase(target);
            if (target == head.left)
            {
                head.left = nullptr;
                delete target;
                return;
            }
            if (target->parent->left == target)
                target->parent->left = nullptr;
            else
                target->parent->right = nullptr;
            delete target;
        }

        iterator find(const Key &key) noexcept
        {
            node *target = head.left;
            while (target != nullptr)
            {
                if (cmp(key, target->val->first))
                    target = target->left;
                else if (cmp(target->val->first, key))
                    target = target->right;
                else
                    return iterator(target, this);
            }
            return end();
        }
        const_iterator find(const Key &key) const noexcept
        {
            node *target = head.left;
            while (target != nullptr)
            {
                if (cmp(key, target->val->first))
                    target = target->left;
                else if (cmp(target->val->first, key))
                    target = target->right;
                else
                    return const_iterator(target, this);
            }
            return cend();
        }
        size_t count(const Key &key) const noexcept { return find(key) != cend(); }
    };
} // namespace sjtu

#endif