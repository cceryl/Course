#ifndef SRC_HPP
#define SRC_HPP
namespace sjtu
{
    template <class T>
    void sort(T *first, T *last)
    {
        // TODO
        // O(nlogn)
        // quickSort
        if (first >= last - 1)
            return;
        T *F = first, *L = last;
        T  pivot = *first;
        while (first != last - 1)
        {
            while (first != last - 1)
            {
                if (*(last - 1) < pivot)
                {
                    *first++ = *(last - 1);
                    break;
                }
                else
                    last--;
            }
            while (first != last - 1)
            {
                if (!(*first < pivot))
                {
                    *(last-- - 1) = *first;
                    break;
                }
                else
                    first++;
            }
        }
        *first = pivot;
        sort(F, first);
        sort(last, L);
    }

    template <class T>
    T *lower_bound(T *first, T *last, const T &value)
    {
        // TODO
        // O(logn)
        // binarySearch
        if (first == last)
            return last;
        T *mid = first + (last - first) / 2;
        if (*mid < value)
            return lower_bound(mid + 1, last, value);
        else
            return lower_bound(first, mid, value);
    }

    template <class T>
    T *upper_bound(T *first, T *last, const T &value)
    {
        // TODO
        // O(logn)
        if (first == last)
            return last;
        T *mid = first + (last - first) / 2;
        if (!(value < *mid))
            return upper_bound(mid + 1, last, value);
        else
            return upper_bound(first, mid, value);
    }
} // namespace sjtu
#endif