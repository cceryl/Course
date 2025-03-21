#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "algorithm.hpp"
#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu
{
    template <typename T>
    class list
    {
    protected:
        class node
        {
        public:
            T    *_data;
            node *_prev;
            node *_next;
            node() : _data(nullptr), _prev(nullptr), _next(nullptr) {}
            node(T *data, node *prev, node *next) : _data(data), _prev(prev), _next(next) {}
            node(const node &other) : _data(new T(*other._data)), _prev(nullptr), _next(nullptr) {}
            ~node() { delete _data; }
        };
        node  *_node;
        size_t _size;

        node *insert(node *pos, node *cur) noexcept
        {
            pos->_prev->_next = cur, cur->_next = pos;
            cur->_prev = pos->_prev, pos->_prev = cur;
            ++_size;
            return cur;
        }
        node *erase(node *pos) noexcept
        {
            pos->_prev->_next = pos->_next;
            pos->_next->_prev = pos->_prev;
            --_size;
            return pos;
        }
        void sort(node *begin, node *end) noexcept
        {
            if (begin->_next == end || begin == end)
                return;
            node *i = begin, *j = end->_prev;
            T    *pivot = begin->_data;
            while (i != j)
            {
                while (i != j)
                {
                    if (*j->_data < *pivot)
                    {
                        i->_data = j->_data, i = i->_next;
                        break;
                    }
                    else
                        j = j->_prev;
                }
                while (i != j)
                {
                    if (!(*i->_data < *pivot))
                    {
                        j->_data = i->_data, j = j->_prev;
                        break;
                    }
                    else
                        i = i->_next;
                }
            }
            i->_data = pivot;
            sort(begin, i);
            sort(j->_next, end);
        }

    public:
        class const_iterator;
        class iterator
        {
            friend class const_iterator;
            friend class list;

        private:
            node       *_node;
            const list *_list;

        public:
            iterator(node *node = nullptr, const list *list = nullptr) : _node(node), _list(list) {}
            iterator operator++(int)
            {
                if (_node == _list->_node)
                    throw invalid_iterator();
                return iterator((_node = _node->_next)->_prev, _list);
            }
            iterator &operator++()
            {
                if (_node == _list->_node)
                    throw invalid_iterator();
                _node = _node->_next;
                return *this;
            }
            iterator operator--(int)
            {
                if (_node == _list->_node->_next)
                    throw invalid_iterator();
                return iterator((_node = _node->_prev)->_next, _list);
            }
            iterator &operator--()
            {
                if (_node == _list->_node->_next)
                    throw invalid_iterator();
                _node = _node->_prev;
                return *this;
            }
            T &operator*() const
            {
                if (_node->_data == nullptr)
                    throw invalid_iterator();
                return *(_node->_data);
            }
            T *operator->() const
            {
                if (_node->_data == nullptr)
                    throw invalid_iterator();
                return _node->_data;
            }
            bool operator==(const iterator &rhs) const noexcept { return _node == rhs._node; }
            bool operator==(const const_iterator &rhs) const noexcept { return _node == rhs._node; }
            bool operator!=(const iterator &rhs) const noexcept { return _node != rhs._node; }
            bool operator!=(const const_iterator &rhs) const noexcept { return _node != rhs._node; }
        };
        class const_iterator
        {
            friend class iterator;
            friend class list;

        private:
            node       *_node;
            const list *_list;

        public:
            const_iterator(node *node = nullptr, const list *list = nullptr) : _node(node), _list(list) {}
            const_iterator(const iterator &iter) : _node(iter._node), _list(iter._list) {}
            const_iterator operator++(int)
            {
                if (_node == _list->_node)
                    throw invalid_iterator();
                return const_iterator((_node = _node->_next)->_prev, _list);
            }
            const_iterator &operator++()
            {
                if (_node == _list->_node)
                    throw invalid_iterator();
                _node = _node->_next;
                return *this;
            }
            const_iterator operator--(int)
            {
                if (_node == _list->_node->_next)
                    throw invalid_iterator();
                return const_iterator((_node = _node->_prev)->_next, _list);
            }
            const_iterator &operator--()
            {
                if (_node == _list->_node->_next)
                    throw invalid_iterator();
                return _node = _node->_prev;
                return *this;
            }
            const T &operator*() const
            {
                if (_node->_data == nullptr)
                    throw invalid_iterator();
                return *(_node->_data);
            }
            const T *operator->() const
            {
                if (_node->_data == nullptr)
                    throw invalid_iterator();
                return _node->_data;
            }
            bool operator==(const iterator &rhs) const noexcept { return _node == rhs._node; }
            bool operator==(const const_iterator &rhs) const noexcept { return _node == rhs._node; }
            bool operator!=(const iterator &rhs) const noexcept { return _node != rhs._node; }
            bool operator!=(const const_iterator &rhs) const noexcept { return _node != rhs._node; }
        };
        list() : _size(0)
        {
            _node        = new node;
            _node->_prev = _node->_next = _node;
        }
        list(const list &other) : _size(other._size)
        {
            _node        = new node;
            _node->_next = _node;
            node *p = _node, *q = other._node->_next;
            while (q != other._node)
            {
                p->_next = new node(new T(*(q->_data)), p, _node);
                p = p->_next, q = q->_next;
            }
            _node->_prev = p;
        }
        list(list &&other) : _node(other._node), _size(other._size) { other._node = nullptr; }
        virtual ~list()
        {
            if (_node != nullptr)
                for (node *p = _node->_next, *q; p != _node; p = q)
                {
                    q = p->_next;
                    delete p;
                }
            delete _node;
        }
        list &operator=(const list &other) noexcept
        {
            for (node *p = _node->_next, *q; p != _node; p = q)
            {
                q = p->_next;
                delete p;
            }
            _node->_next = _node;
            node *p = _node, *q = other._node->_next;
            while (q != other._node)
            {
                p->_next = new node(new T(*(q->_data)), p, _node);
                p = p->_next, q = q->_next;
            }
            _node->_prev = p;
            _size        = other._size;
            return *this;
        }
        const T &front() const
        {
            if (_size == 0)
                throw container_is_empty();
            return *(_node->_next->_data);
        }
        const T &back() const
        {
            if (_size == 0)
                throw container_is_empty();
            return *(_node->_prev->_data);
        }
        iterator       begin() noexcept { return iterator(_node->_next, this); }
        const_iterator cbegin() const noexcept { return const_iterator(_node->_next, this); }
        iterator       end() noexcept { return iterator(_node, this); }
        const_iterator cend() const noexcept { return const_iterator(_node, this); }
        virtual bool   empty() const noexcept { return !_size; }
        virtual size_t size() const noexcept { return _size; }
        virtual void   clear() noexcept
        {
            for (node *p = _node->_next, *q; p != _node; p = q)
            {
                q = p->_next;
                delete p;
            }
            _node->_prev = _node->_next = _node;
            _size                       = 0;
        }
        virtual iterator insert(const iterator pos, const T &value)
        {
            if (pos._list != this)
                throw invalid_iterator();
            ++_size;
            return iterator(pos._node->_prev = pos._node->_prev->_next = new node(new T(value), pos._node->_prev, pos._node), this);
        }
        virtual iterator erase(const iterator pos)
        {
            if (!_size)
                throw container_is_empty();
            if (pos._node == _node)
                throw invalid_iterator();
            --_size;
            pos._node->_prev->_next = pos._node->_next;
            pos._node->_next->_prev = pos._node->_prev;
            iterator tmp(pos._node->_next, this);
            delete pos._node;
            return tmp;
        }
        void push_back(const T &value) noexcept
        {
            _node->_prev = _node->_prev->_next = new node(new T(value), _node->_prev, _node);
            ++_size;
        }
        void pop_back()
        {
            if (!_size)
                throw container_is_empty();
            node *tmp                          = _node->_prev;
            (_node->_prev = tmp->_prev)->_next = _node;
            delete tmp;
            --_size;
        }
        void push_front(const T &value) noexcept
        {
            _node->_next = _node->_next->_prev = new node(new T(value), _node, _node->_next);
            ++_size;
        }
        void pop_front()
        {
            if (!_size)
                throw container_is_empty();
            node *tmp                          = _node->_next;
            (_node->_next = tmp->_next)->_prev = _node;
            delete tmp;
            --_size;
        }
        void sort() noexcept { sort(_node->_next, _node); }
        void merge(list &other) noexcept
        {
            node *p = _node->_next, *q = other._node->_next;
            while (p != _node && q != other._node)
            {
                if (!(*p->_data < *q->_data))
                {
                    q = q->_next;
                    insert(p, q->_prev);
                }
                else
                    p = p->_next;
            }
            if (p == _node)
                while (q != other._node)
                {
                    q = q->_next;
                    insert(_node, q->_prev);
                }
            other._node->_prev = other._node->_next = other._node;
            other._size                             = 0;
        }
        void reverse() noexcept
        {
            T *tmp;
            for (node *p = _node->_next, *q = _node->_prev; p != q && p->_prev != q; p = p->_next, q = q->_prev)
                tmp = p->_data, p->_data = q->_data, q->_data = tmp;
        }
        void unique() noexcept
        {
            for (node *p = _node->_next; p != _node && p->_next != _node; p = p->_next)
                while (p->_next != _node && *p->_data == *p->_next->_data)
                    delete erase(p->_next);
        }
    };
} // namespace sjtu

#endif