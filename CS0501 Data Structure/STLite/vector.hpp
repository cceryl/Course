#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu
{
    template <typename DataType>
    class vector
    {
    private:
        DataType *_first;
        DataType *_last;
        DataType *_endOfStorage;

        void expand() noexcept
        {
            DataType *tmp = (DataType *)malloc(2 * sizeof(DataType) * (_endOfStorage - _first));
            for (DataType *p = tmp, *q = _first; q < _last; ++p, ++q)
            {
                new (p) DataType(*q);
                q->~DataType();
            }
            _endOfStorage = tmp + 2 * (_endOfStorage - _first);
            _last         = tmp + (_last - _first);
            free(_first);
            _first = tmp;
        }

    public:
        class const_iterator;
        class iterator
        {
        private:
            const vector *_vec;
            DataType     *_ptr;

        public:
            iterator(const vector *vec, DataType *ptr) : _vec(vec), _ptr(ptr) {}
            iterator operator+(const int &n) const noexcept { return iterator(_vec, _ptr + n); }
            iterator operator-(const int &n) const noexcept { return iterator(_vec, _ptr - n); }
            int      operator-(const iterator &rhs) const
            {
                if (_vec != rhs._vec)
                    throw invalid_iterator();
                return (_ptr - rhs._ptr);
            }
            iterator &operator+=(const int &n) noexcept
            {
                _ptr += n;
                return *this;
            }
            iterator &operator-=(const int &n) noexcept
            {
                _ptr -= n;
                return *this;
            }
            iterator operator++(int) noexcept
            {
                iterator tmp(_vec, _ptr);
                ++_ptr;
                return tmp;
            }
            iterator &operator++() noexcept
            {
                ++_ptr;
                return *this;
            }
            iterator operator--(int) noexcept
            {
                iterator tmp(_vec, _ptr);
                --_ptr;
                return tmp;
            }
            iterator &operator--() noexcept
            {
                --_ptr;
                return *this;
            }
            DataType &operator*() const noexcept { return *_ptr; }
            bool      operator==(const iterator &rhs) const noexcept { return _ptr == rhs._ptr; }
            bool      operator==(const const_iterator &rhs) const noexcept { return _ptr == rhs._ptr; }
            bool      operator!=(const iterator &rhs) const noexcept { return _ptr != rhs._ptr; }
            bool      operator!=(const const_iterator &rhs) const noexcept { return _ptr != rhs._ptr; }
        };
        class const_iterator
        {
        private:
            const vector *_vec;
            DataType     *_ptr;

        public:
            const_iterator(const vector *vec, DataType *ptr) : _vec(vec), _ptr(ptr) {}
            const_iterator operator+(const int &n) const noexcept { return const_iterator(_vec, _ptr + n); }
            const_iterator operator-(const int &n) const noexcept { return const_iterator(_vec, _ptr - n); }
            int            operator-(const const_iterator &rhs) const
            {
                if (_vec != rhs._vec)
                    throw invalid_iterator();
                return (_ptr - rhs._ptr);
            }
            const_iterator &operator+=(const int &n) noexcept
            {
                _ptr += n;
                return *this;
            }
            const_iterator &operator-=(const int &n) noexcept
            {
                _ptr -= n;
                return *this;
            }
            const_iterator operator++(int) noexcept
            {
                const_iterator tmp(_vec, _ptr);
                ++_ptr;
                return tmp;
            }
            const_iterator &operator++() noexcept
            {
                ++_ptr;
                return *this;
            }
            const_iterator operator--(int) noexcept
            {
                const_iterator tmp(_vec, _ptr);
                --_ptr;
                return tmp;
            }
            const_iterator &operator--() noexcept
            {
                --_ptr;
                return *this;
            }
            const DataType &operator*() const noexcept { return *_ptr; }
            bool            operator==(const iterator &rhs) const noexcept { return _ptr == rhs._ptr; }
            bool            operator==(const const_iterator &rhs) const noexcept { return _ptr == rhs._ptr; }
            bool            operator!=(const iterator &rhs) const noexcept { return _ptr != rhs._ptr; }
            bool            operator!=(const const_iterator &rhs) const noexcept { return _ptr != rhs._ptr; }
        };
        vector()
        {
            _first        = (DataType *)malloc(sizeof(DataType));
            _last         = _first;
            _endOfStorage = _first + 1;
        }
        vector(const vector &other)
        {
            _first        = (DataType *)malloc(sizeof(DataType) * (other._endOfStorage - other._first));
            _last         = _first + (other._last - other._first);
            _endOfStorage = _first + (other._endOfStorage - other._first);
            for (DataType *p = _first, *q = other._first; q < other._last; ++p, ++q)
                new (p) DataType(*q);
        }
        ~vector()
        {
            for (DataType *p = _first; p != _last; ++p)
                p->~DataType();
            free(_first);
        }
        vector &operator=(const vector &other) noexcept
        {
            if (&other == this)
                return *this;
            for (DataType *p = _first; p != _last; ++p)
                p->~DataType();
            free(_first);
            _first        = (DataType *)malloc(sizeof(DataType) * (other._endOfStorage - other._first));
            _last         = _first + (other._last - other._first);
            _endOfStorage = _first + (other._endOfStorage - other._first);
            for (DataType *p = _first, *q = other._first; q < other._last; ++p, ++q)
                new (p) DataType(*q);
            return *this;
        }
        DataType &at(const size_t &pos)
        {
            if (pos < 0 || pos + _first >= _last)
                throw index_out_of_bound();
            return _first[pos];
        }
        const DataType &at(const size_t &pos) const
        {
            if (pos < 0 || pos + _first >= _last)
                throw index_out_of_bound();
            return _first[pos];
        }
        DataType &operator[](const size_t &pos)
        {
            if (pos < 0 || pos + _first >= _last)
                throw index_out_of_bound();
            return _first[pos];
        }
        const DataType &operator[](const size_t &pos) const
        {
            if (pos < 0 || pos + _first >= _last)
                throw index_out_of_bound();
            return _first[pos];
        }
        const DataType &front() const
        {
            if (_first == _last)
                throw container_is_empty();
            return *_first;
        }
        const DataType &back() const
        {
            if (_first == _last)
                throw container_is_empty();
            return *(_last - 1);
        }
        iterator       begin() { return iterator(this, _first); }
        const_iterator cbegin() const { return const_iterator(this, _first); }
        iterator       end() { return iterator(this, _last); };
        const_iterator cend() const { return const_iterator(this, _last); }
        bool           empty() const noexcept { return _last > _first; }
        size_t         size() const noexcept { return (_last - _first); }
        void           clear() noexcept
        {
            for (DataType *p = _first; p != _last; ++p)
                p->~DataType();
            free(_first);
            _first        = (DataType *)malloc(sizeof(DataType));
            _last         = _first;
            _endOfStorage = _first + 1;
        }
        iterator insert(iterator pos, const DataType &value)
        {
            if (_last == _endOfStorage)
            {
                size_t tmp = pos - iterator(this, _first);
                expand();
                pos = iterator(this, tmp + _first);
            }
            new (_last) DataType(value);
            for (iterator p = this->end(); p != pos; --p)
                *p = *(p - 1);
            *pos = value;
            ++_last;
            return pos;
        }
        iterator insert(const size_t &ind, const DataType &value)
        {
            if (ind < 0 || ind + _first >= _last)
                throw index_out_of_bound();
            new (_last) DataType(value);
            for (DataType *p = _last; p != _first + ind; ++p)
                *p = *(p - 1);
            *(_first + ind) = value;
            ++_last;
            return iterator(this, _first + ind);
        }
        iterator erase(iterator pos)
        {
            for (iterator p = pos; p != this->end(); ++p)
                *p = *(p + 1);
            --_last;
            _last->~DataType();
            return pos;
        }
        iterator erase(const size_t &ind)
        {
            if (ind < 0 || ind + _first >= _last)
                throw index_out_of_bound();
            for (DataType *p = _first + ind; p < _last; ++p)
                *p = *(p + 1);
            --_last;
            _last->~DataType();
            return iterator(this, _first + ind);
        }
        void push_back(const DataType &value) noexcept
        {
            if (_last == _endOfStorage)
                expand();
            new (_last++) DataType(value);
        }
        void pop_back()
        {
            if (_first == _last)
                throw container_is_empty();
            --_last;
            _last->~DataType();
        }
    };
} // namespace sjtu

#endif