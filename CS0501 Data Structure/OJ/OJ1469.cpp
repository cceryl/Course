// template class ArrayList

#ifndef _ARRAYLIST_H
#define _ARRAYLIST_H

#ifndef USE_OS
    #define USE_OS
#endif

#include <iostream>

class error : public std::exception
{
private:
    std::string msg;

public:
    explicit error(const char *_msg_) : msg(_msg_) {}

    const char *toString() { return msg.c_str(); }
};

template <typename T>
class ArrayList
{
    friend std::ostream &operator<<(std::ostream &os, const ArrayList<T> &obj)
    {
        for (int i = 0; i < obj.length; i++)
            os << obj.arr[i] << ' ';
        return os;
    }

private:
    T  *arr;
    int length;

public:
    ArrayList(const T *_arr, int _length) : length(_length)
    {
        if (length < 0)
            throw error("invalid length");
        arr = new T[length];
        for (int i = 0; i < length; i++)
            arr[i] = _arr[i];
    }
    ArrayList(int _length) : length(_length)
    {
        if (length < 0)
            throw error("invalid length");
        arr = new T[length];
    }
    ArrayList(const ArrayList &obj) : ArrayList(obj.arr, obj.length) {}
    ArrayList(ArrayList &&obj) = delete;
    ~ArrayList() { delete[] arr; }
    int size() const { return length; }
    T  &operator[](int index)
    {
        if (index < 0 || index >= length)
            throw error("index out of bound");
        return arr[index];
    }
    T operator[](int index) const
    {
        if (index < 0 || index >= length)
            throw error("index out of bound");
        return arr[index];
    }
    ArrayList operator+(const ArrayList &obj) const
    {
        ArrayList tmp(length + obj.length);
        for (int i = 0; i < length; i++)
            tmp.arr[i] = arr[i];
        for (int i = 0; i < obj.length; i++)
            tmp.arr[i + length] = obj.arr[i];
        return tmp;
    }
    ArrayList &operator=(const ArrayList &obj)
    {
        if (this == &obj)
            return *this;
        length = obj.length;
        delete[] arr;
        arr = new T[length];
        for (int i = 0; i < length; i++)
            arr[i] = obj.arr[i];
        return *this;
    }
    bool operator==(const ArrayList &obj) const
    {
        if (length != obj.length)
            return false;
        for (int i = 0; i < length; i++)
            if (!(arr[i] == obj.arr[i]))
                return false;
        return true;
    }
    bool operator!=(const ArrayList &obj) const { return !(*this == obj); }
};

template <typename T>
void printList(ArrayList<T> &list)
{
#ifdef USE_OS
    std::cout << list << "\n";
#else
    list.print();
    std::cout << "\n";
#endif
}

#endif