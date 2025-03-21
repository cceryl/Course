#ifndef SJTU_LINKEDHASHMAP_HPP
#define SJTU_LINKEDHASHMAP_HPP

#include "exceptions.hpp"
#include "utility.hpp"
#include <cstddef>
#include <functional>

namespace sjtu
{
    template <typename Key, typename T, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
    class linked_hashmap
    {
    public:
        typedef pair<const Key, T> value_type;

    private:
        struct node
        {
            value_type *val;
            node       *next;
            node       *before;
            node       *after;
            node() : val(nullptr), next(nullptr), before(nullptr), after(nullptr) {}
            node(const value_type &_val, node *_next, node *_before, node *_after) :
                val(new value_type(_val)), next(_next), before(_before), after(_after)
            {}
            ~node() { delete val; }
            node(const node &other) : val(new value_type(*other.val)), next(nullptr), before(nullptr), after(nullptr) {}
        };
        static constexpr size_t defaultCapacity   = 128;
        static constexpr double defaultLoadFactor = 0.75;
        Hash                    hash;
        Equal                   equal;

        node  *table;
        node  *head;
        size_t capacity;
        size_t numOfElements;
        double loadFactor;

        void resize()
        {
            if (numOfElements > capacity * loadFactor)
            {
                capacity       = capacity << 1;
                node *newTable = new node[capacity];
                for (node *p = head->after, *q; p != head; p = p->after)
                {
                    q       = &newTable[hash(p->val->first) & (capacity - 1)];
                    p->next = q->next, q->next = p;
                }
                delete[] table;
                table = newTable;
            }
        }

    public:
        class const_iterator;
        class iterator
        {
            friend class linked_hashmap;
            friend class const_iterator;

        private:
            node           *target;
            linked_hashmap *hashmap;

        public:
            using difference_type   = std::ptrdiff_t;
            using value_type        = typename linked_hashmap::value_type;
            using pointer           = value_type *;
            using reference         = value_type &;
            using iterator_category = std::output_iterator_tag;

            iterator(node *_target = nullptr, linked_hashmap *_hashmap = nullptr) : target(_target), hashmap(_hashmap) {}
            iterator(const iterator &other) : target(other.target), hashmap(other.hashmap) {}
            iterator operator++(int)
            {
                if (target == hashmap->head)
                    throw invalid_iterator();
                iterator tmp(target, hashmap);
                target = target->after;
                return tmp;
            }
            iterator &operator++()
            {
                if (target == hashmap->head)
                    throw invalid_iterator();
                target = target->after;
                return *this;
            }
            iterator operator--(int)
            {
                if (target == hashmap->head->after)
                    throw invalid_iterator();
                iterator tmp(target, hashmap);
                target = target->before;
                return tmp;
            }
            iterator &operator--()
            {
                if (target == hashmap->head->after)
                    throw invalid_iterator();
                target = target->before;
                return *this;
            }
            bool        operator==(const iterator &rhs) const noexcept { return target == rhs.target; }
            bool        operator==(const const_iterator &rhs) const noexcept { return target == rhs.target; }
            bool        operator!=(const iterator &rhs) const noexcept { return target != rhs.target; }
            bool        operator!=(const const_iterator &rhs) const noexcept { return target != rhs.target; }
            value_type &operator*() const
            {
                if (target == hashmap->head)
                    throw invalid_iterator();
                return *target->val;
            }
            value_type *operator->() const noexcept { return target->val; }
        };
        class const_iterator
        {
            friend class linked_hashmap;

        private:
            node                 *target;
            const linked_hashmap *hashmap;

        public:
            const_iterator(node *_target = nullptr, const linked_hashmap *_hashmap = nullptr) : target(_target), hashmap(_hashmap) {}
            const_iterator(const const_iterator &other) : target(other.target), hashmap(other.hashmap) {}
            const_iterator(const iterator &other) : target(other.target), hashmap(other.hashmap) {}
            const_iterator operator++(int)
            {
                if (target == hashmap->head)
                    throw invalid_iterator();
                const_iterator tmp(target, hashmap);
                target = target->after;
                return tmp;
            }
            const_iterator &operator++()
            {
                if (target == hashmap->head)
                    throw invalid_iterator();
                target = target->after;
                return *this;
            }
            const_iterator operator--(int)
            {
                if (target == hashmap->head->after)
                    throw invalid_iterator();
                const_iterator tmp(target, hashmap);
                target = target->before;
                return tmp;
            }
            const_iterator &operator--()
            {
                if (target == hashmap->head->after)
                    throw invalid_iterator();
                target = target->before;
                return *this;
            }
            bool              operator==(const iterator &rhs) const noexcept { return target == rhs.target; }
            bool              operator==(const const_iterator &rhs) const noexcept { return target == rhs.target; }
            bool              operator!=(const iterator &rhs) const noexcept { return target != rhs.target; }
            bool              operator!=(const const_iterator &rhs) const noexcept { return target != rhs.target; }
            const value_type &operator*() const
            {
                if (target == hashmap->head)
                    throw invalid_iterator();
                return *target->val;
            }
            const value_type *operator->() const noexcept { return target->val; }
        };

        linked_hashmap(size_t _capacity = defaultCapacity, double _loadFactor = defaultLoadFactor) :
            table(new node[_capacity]), head(new node), capacity(_capacity), numOfElements(0), loadFactor(_loadFactor)
        {
            head->before = head->after = head;
        }
        linked_hashmap(const linked_hashmap &other) :
            table(new node[other.capacity]),
            head(new node),
            capacity(other.capacity),
            numOfElements(other.numOfElements),
            loadFactor(other.loadFactor)
        {
            head->before = head->after = head;
            for (node *p = other.head->after, *q; p != other.head; p = p->after)
            {
                q = &table[hash(p->val->first) & (capacity - 1)];
                q = q->next      = new node(*p->val, q->next, head->before, head);
                q->before->after = q, head->before = q;
            }
        }

        linked_hashmap &operator=(const linked_hashmap &other)
        {
            if (&other == this)
                return *this;
            for (node *p = head->after->after; p != head; p = p->after)
                delete p->before;
            head->before = head->after = head;
            if (capacity != other.capacity)
            {
                delete[] table;
                capacity = other.capacity;
                table    = new node[capacity];
            }
            else if (numOfElements != 0)
                memset(table, 0, capacity * sizeof(node));
            numOfElements = other.numOfElements, loadFactor = other.loadFactor;
            for (node *p = other.head->after, *q; p != other.head; p = p->after)
            {
                q = &table[hash(p->val->first) & (capacity - 1)];
                q = q->next      = new node(*p->val, q->next, head->before, head);
                q->before->after = q, head->before = q;
            }
            return *this;
        }

        ~linked_hashmap()
        {
            if (numOfElements != 0)
            {
                for (node *p = head->after->after; p != head; p = p->after)
                    delete p->before;
                delete head->before;
            }
            delete[] table;
            delete head;
        }

        T &at(const Key &key)
        {
            node *p = table[hash(key) & (capacity - 1)].next;
            while (p != nullptr)
            {
                if (equal(p->val->first, key))
                    return p->val->second;
                p = p->next;
            }
            throw index_out_of_bound();
        }
        const T &at(const Key &key) const
        {
            node *p = table[hash(key) & (capacity - 1)].next;
            while (p != nullptr)
            {
                if (equal(p->val->first, key))
                    return p->val->second;
                p = p->next;
            }
            throw index_out_of_bound();
        }

        T &operator[](const Key &key) noexcept
        {
            node *p = &table[hash(key) & (capacity - 1)];
            while (p->next != nullptr)
            {
                if (equal(p->next->val->first, key))
                    return p->next->val->second;
                p = p->next;
            }
            p = p->next      = new node(value_type(key, T()), nullptr, head->before, head);
            p->before->after = p, head->before = p;
            ++numOfElements;
            resize();
            return p->val->second;
        }
        const T &operator[](const Key &key) const
        {
            node *p = table[hash(key) & (capacity - 1)].next;
            while (p != nullptr)
            {
                if (equal(p->val->first, key))
                    return p->val->second;
                p = p->next;
            }
            throw index_out_of_bound();
        }

        iterator       begin() noexcept { return iterator(head->after, this); }
        const_iterator cbegin() const noexcept { return const_iterator(head->after, this); }
        iterator       end() noexcept { return iterator(head, this); }
        const_iterator cend() const noexcept { return const_iterator(head, this); }
        bool           empty() const noexcept { return !numOfElements; }
        size_t         size() const noexcept { return numOfElements; }

        void clear()
        {
            if (numOfElements == 0)
                return;
            for (node *p = head->after->after; p != head; p = p->after)
                delete p->before;
            delete head->before;
            head->before = head->after = head;
            memset(table, 0, capacity * sizeof(node));
            numOfElements = 0;
        }

        pair<iterator, bool> insert(const value_type &value) noexcept
        {
            node *pos = &table[hash(value.first) & (capacity - 1)];
            while (pos->next != nullptr)
            {
                if (equal(pos->next->val->first, value.first))
                    return pair<iterator, bool>(iterator(pos->next, this), false);
                pos = pos->next;
            }
            pos = pos->next    = new node(value, nullptr, head->before, head);
            pos->before->after = pos, head->before = pos;
            ++numOfElements;
            resize();
            return pair<iterator, bool>(iterator(pos, this), true);
        }

        void erase(iterator pos)
        {
            if (pos.target == head || pos.hashmap != this)
                throw invalid_iterator();
            pos.target->before->after = pos.target->after, pos.target->after->before = pos.target->before;
            node *p = &table[hash(pos.target->val->first) & (capacity - 1)];
            while (p->next != pos.target)
                p = p->next;
            p->next = pos.target->next;
            delete pos.target;
            --numOfElements;
        }

        size_t count(const Key &key) const noexcept
        {
            node *p = table[hash(key) & (capacity - 1)].next;
            while (p != nullptr)
            {
                if (equal(p->val->first, key))
                    return 1;
                p = p->next;
            }
            return 0;
        }

        iterator find(const Key &key) noexcept
        {
            node *p = table[hash(key) & (capacity - 1)].next;
            while (p != nullptr)
            {
                if (equal(p->val->first, key))
                    return iterator(p, this);
                p = p->next;
            }
            return iterator(head, this);
        }
        const_iterator find(const Key &key) const noexcept
        {
            node *p = table[hash(key) & (capacity - 1)].next;
            while (p != nullptr)
            {
                if (equal(p->val->first, key))
                    return const_iterator(p, this);
                p = p->next;
            }
            return const_iterator(head, this);
        }
    };
} // namespace sjtu

#endif