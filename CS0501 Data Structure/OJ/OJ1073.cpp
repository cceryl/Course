// date 1900/1/1 - 2030/12/31

#ifndef _DATE_H
#define _DATE_H

#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

const int month_days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

class Date
{
private:
    int _year;
    int _month;
    int _day;

public:
    // 构造函数
    Date() : _year(1900), _month(1), _day(1) {}
    Date(int year, int month, int day) : _year(year), _month(month), _day(day)
    {
        if (year < 1900 || year > 2030 || month < 1 || month > 12)
        {
            _year  = 1900;
            _month = 1;
            _day   = 1;
            return;
        }
        if (day < 1 || day > month_days[_month] + (_month == 2 && isLeap()))
        {
            _year  = 1900;
            _month = 1;
            _day   = 1;
        }
    }
    // 判断是否为闰年
    bool isLeap() const { return _year % 400 == 0 || (_year % 100 != 0 && _year % 4 == 0); }
    bool isLeap(int year) const { return year % 400 == 0 || (year % 100 != 0 && year % 4 == 0); }
    // 一个日期加上一个天数
    Date operator+(int days) const
    {
        Date tmp(_year, _month, _day);
        /* if (tmp._month == 2 && tmp._day == 29)
        {
            tmp._day = 28;
            ++days;
        }
        while (days >= 366)
        {
            ++tmp._year;
            days -= 366;
        }
        if (_year != tmp._year)
            for (int year = _year + (_month >= 3); year <= (tmp._year - (_month <= 2)); ++year)
                days += !isLeap(year); */
        for (; days > 0; --days)
            ++tmp;
        return tmp;
    }
    // 一个日期减去一个天数
    Date operator-(int days) const
    {
        Date tmp(_year, _month, _day);
        /* if (tmp._month == 2 && tmp._day == 29)
        {
            tmp._day = 28;
            --days;
        }
        while (days >= 365)
        {
            --tmp._year;
            days -= 365;
        }
        if (_year != tmp._year)
            for (int year = _year - (_month <= 2); year >= (tmp._year + (_month >= 3)); --year)
                days += isLeap(year); */
        for (; days > 0; --days)
            --tmp;
        return tmp;
    }
    // 一个日期减去一个日期
    int operator-(const Date &obj) const
    {
        int days = 0;
        if (_year < obj._year)
        {
            int y = _year, m = _month, d = _day;
            if (m == 2 && d == 29)
            {
                d = 28;
                ++days;
            }
            while (y != obj._year)
            {
                ++y;
                days += 365 + (m <= 2 ? isLeap(y - 1) : isLeap(y));
            }
            Date tmp(y, m, d);
            while (tmp < obj)
            {
                ++tmp;
                ++days;
            }
            while (obj < tmp)
            {
                --tmp;
                --days;
            }
        }
        else if (_year > obj._year)
            return (obj - *this);
        else
        {
            Date tmp(_year, _month, _day);
            while (tmp < obj)
            {
                ++tmp;
                ++days;
            }
            while (obj < tmp)
            {
                --tmp;
                --days;
            }
            days = days < 0 ? -days : days;
        }
        return days;
    }
    // 前置++
    Date &operator++()
    {
        if (++_day > (month_days[_month] + (_month == 2 && isLeap())))
        {
            _day = 1;
            ++_month;
        }
        _year += _month == 13 ? _month %= 12 : 0;
        return *this;
    }
    // 后置++
    Date operator++(int postfix)
    {
        Date tmp(_year, _month, _day);
        if (++_day > (month_days[_month] + (_month == 2 && isLeap())))
        {
            _day = 1;
            ++_month;
        }
        _year += _month == 13 ? _month %= 12 : 0;
        return tmp;
    }
    // 前置--
    Date &operator--()
    {
        if (!--_day)
        {
            if (--_month)
                _day = month_days[_month] + (_month == 2 && isLeap());
            else
            {
                --_year;
                _month = 12;
                _day   = 31;
            }
        }
        return *this;
    }
    // 后置--
    Date operator--(int postfix)
    {
        Date tmp(_year, _month, _day);
        if (!--_day)
        {
            if (--_month)
                _day = month_days[_month] + (_month == 2 && isLeap());
            else
            {
                --_year;
                _month = 12;
                _day   = 31;
            }
        }
        return tmp;
    }
    //<重载
    bool operator<(const Date &obj) const
    {
        return _year == obj._year ? _month == obj._month ? _day < obj._day : _month < obj._month : _year < obj._year;
    }
    // 重载输出运算符
    friend ostream &operator<<(ostream &os, const Date &_date)
    {
        os << _date._year << '-' << _date._month << '-' << _date._day;
        return os;
    }
    // 以上仅为提示，不代表你需要完成所有，你也可以添加其他需要的函数。
};

void Test()
{
    int op;
    cin >> op;
    int yy, mm, dd;
    if (op == 1 || op == 0)
    {
        Date d0;
        Date d1(2000, 2, 29);
        Date d2(1900, 2, 29);
        cout << d0 << endl;
        cout << d1 << endl;
        cout << d2 << endl;
        // d0.out(); d1.out(); d2.out();
    }
    if (op == 2 || op == 0)
    {
        cin >> yy >> mm >> dd;
        Date d0(yy, mm, dd);
        for (int i = 0; i < 5; ++i)
            cout << ++d0 << endl; //(++d0).out();
        for (int i = 0; i < 5; ++i)
            cout << d0++ << endl; //(d0++).out();
        for (int i = 0; i < 5; ++i)
            cout << d0-- << endl; //(d0--).out();
        for (int i = 0; i < 2; ++i)
            cout << --d0 << endl; //(--d0).out();
        cout << d0 << endl;
        // d0.out();
    }
    if (op == 3 || op == 0)
    {
        cin >> yy >> mm >> dd;
        Date d0(yy, mm, dd);
        cout << d0 + 100 << endl;
        // (d0+100).out();
        cout << d0 - 1000 << endl;
        // (d0-1000).out();
    }
    if (op == 4 || op == 0)
    {
        cin >> yy >> mm >> dd;
        Date d0(yy, mm, dd);
        Date d1(2020, 12, 21);
        cout << (d0 < d1) << endl;
    }
    if (op == 5 || op == 0)
    {
        cin >> yy >> mm >> dd;
        Date d0(yy, mm, dd);
        Date d1(1912, 6, 23);
        cout << d0 - d1 << endl;
    }
}

int main()
{
    Test();
    return 0;
}

#endif