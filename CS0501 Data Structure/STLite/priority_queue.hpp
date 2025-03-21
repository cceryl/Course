#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <functional>

namespace sjtu
{
    template <typename T, class Compare = std::less<T>>
    class priority_queue
    {
        struct node
        {
            T     *key;
            size_t degree;
            node  *child;
            node  *next;
            node(T *_key) : key(_key), degree(0), child(nullptr), next(nullptr) {}
            node(const node &other) : key(new T(*other.key)), degree(other.degree), child(nullptr), next(nullptr) {}
            ~node() { delete key; }
        };

    private:
        node   *root;
        Compare cmp;
        size_t  length;
        node   *prior;
        node   *link(node *root_x, node *root_y)
        {
            if (root_x == nullptr)
                return root_y;
            if (root_y == nullptr)
                return root_x;
            node *x = root_x, *y = root_y, *root_m = nullptr, *current;
            while (x != nullptr && y != nullptr)
            {
                if (x->degree <= y->degree)
                {
                    if (root_m == nullptr)
                        root_m = current = x;
                    else
                        current->next = x, current = x;
                    x = x->next;
                }
                else
                {
                    if (root_m == nullptr)
                        root_m = current = y;
                    else
                        current->next = y, current = y;
                    y = y->next;
                }
            }
            while (x != nullptr)
                current->next = x, current = x, x = x->next;
            while (y != nullptr)
                current->next = y, current = y, y = y->next;
            return root_m;
        }
        void merge()
        {
            if (root == nullptr || root->next == nullptr)
                return;
            node *current = root;
            while (root->next != nullptr && root->degree == root->next->degree)
            {
                if (cmp(*root->key, *root->next->key))
                {
                    root = root->next, current->next = root->child, root->child = current;
                    ++root->degree;
                    current = root;
                }
                else
                {
                    current = root->next, root->next = current->next, current->next = root->child, root->child = current;
                    ++root->degree;
                    current = root;
                }
            }
            if (current->next == nullptr)
                return;
            node *prev = root;
            current    = current->next;
            while (current != nullptr && current->next != nullptr)
            {
                if (current->degree == current->next->degree)
                    if (current->next->next != nullptr && current->next->degree == current->next->next->degree)
                        current = current->next, prev = prev->next;
                    else
                    {
                        if (cmp(*current->key, *current->next->key))
                        {
                            prev->next = current->next, current->next = prev->next->child, prev->next->child = current;
                            ++prev->next->degree;
                            prev = prev->next, current = prev->next;
                        }
                        else
                        {
                            prev = prev->next, current = current->next;
                            prev->next = current->next, current->next = prev->child, prev->child = current;
                            ++prev->degree;
                            current = prev->next;
                        }
                    }
                else
                    current = current->next, prev = prev->next;
            }
        }
        void remove(node *root_m)
        {
            if (root_m == nullptr)
                return;
            remove(root_m->child), remove(root_m->next);
            delete root_m;
        }
        void copy(node *root_m, node *other)
        {
            if (other->child != nullptr)
            {
                root_m->child = new node(*other->child);
                copy(root_m->child, other->child);
            }
            if (other->next != nullptr)
            {
                root_m->next = new node(*other->next);
                copy(root_m->next, other->next);
            }
        }

    public:
        priority_queue() : root(nullptr), length(0), prior(nullptr) {}
        priority_queue(const priority_queue &other) : cmp(other.cmp), length(other.length), prior(nullptr)
        {
            if (length != 0)
            {
                root = new node(*other.root);
                copy(root, other.root);
            }
        }
        ~priority_queue() { remove(root); }
        priority_queue &operator=(const priority_queue &other)
        {
            if (this == &other)
                return *this;
            remove(root);
            cmp = other.cmp, length = other.length, prior = nullptr;
            if (length != 0)
            {
                root = new node(*other.root);
                copy(root, other.root);
            }
            return *this;
        }
        const T &top()
        {
            if (length == 0)
                throw container_is_empty();
            if (prior != nullptr)
                return *prior->key;
            T *t = root->key;
            for (node *p = root; p != nullptr; p = p->next)
                if (cmp(*t, *p->key))
                    t = p->key, prior = p;
            return *t;
        }
        void push(const T &e) noexcept
        {
            root = link(root, new node(new T(e)));
            merge();
            ++length, prior = nullptr;
        }
        void pop()
        {
            if (length == 0)
                throw container_is_empty();
            node *prev = root, *current;
            prior      = nullptr;
            for (node *p = root; p->next != nullptr; p = p->next)
                if (cmp(*prev->next->key, *p->next->key))
                    prev = p;
            if (prev->next != nullptr && cmp(*root->key, *prev->next->key))
                current = prev->next, prev->next = current->next;
            else
                current = root, root = root->next;
            for (node *p = current->child, *q; p != nullptr; p = q)
            {
                q = p->next, p->next = nullptr;
                root = link(root, p);
                merge();
            }
            delete current;
            --length;
        }
        size_t size() const noexcept { return length; }
        bool   empty() const noexcept { return !length; }
        void   merge(priority_queue &other) noexcept
        {
            root = link(root, other.root);
            merge();
            length += other.length, prior = nullptr;
            other.length = 0, other.root = nullptr;
        }
    };
} // namespace sjtu

#endif