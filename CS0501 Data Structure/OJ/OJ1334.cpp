// friend judge

#ifndef FRIEND_JUDGE
#define FRIEND_JUDGE

#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

class friend_t
{
public:
    string         name;
    vector<string> dec;
    vector<string> arg;
    friend_t(const char *_name) { name = _name; }
};

class template_t
{
public:
    string           name;
    vector<string>   dec;
    vector<friend_t> fri;
    template_t(const char *_name) { name = _name; }
};

int main()
{
    int tempCnt, inquiryCnt;
    cin >> tempCnt >> inquiryCnt;
    vector<template_t> templates;
    string             input;
    while (tempCnt--)
    {
        templates.push_back("");
        auto iter = templates.rbegin();
        cin >> input >> input;
        while (input[0] != '>')
        {
            iter->dec.push_back(input);
            cin >> input;
        }
        cin >> iter->name;
        int friCnt;
        cin >> friCnt;
        while (friCnt--)
        {
            iter->fri.push_back("");
            auto iterF = iter->fri.rbegin();
            cin >> input >> input;
            while (input[0] != '>')
            {
                iterF->dec.push_back(input);
                cin >> input;
            }
            cin >> iterF->name;
            cin >> input >> input;
            while (input[0] != '>')
            {
                iterF->arg.push_back(input);
                cin >> input;
            }
            ++iterF;
        }
        ++iter;
    }
    while (inquiryCnt--)
    {
        bool           res = 0;
        string         name, friname, input;
        vector<string> arg, friarg;
        cin >> friname >> input >> input;
        while (input[0] != '>')
        {
            friarg.push_back(input);
            cin >> input;
        }
        cin >> name >> input >> input;
        while (input[0] != '>')
        {
            arg.push_back(input);
            cin >> input;
        }
        auto iter = templates.begin();
        while (iter->name != name)
            ++iter;
        for (auto iterF = iter->fri.begin(); iterF < iter->fri.end(); ++iterF)
        {
            if (iterF->name != friname)
                continue;
            bool check[iterF->arg.size()];
            for (unsigned int i = 0; i < iterF->arg.size(); ++i)
                check[i] = 0;
            for (unsigned int i = 0; i < iterF->dec.size(); ++i)
            {
                for (unsigned int j = 0; j < iterF->arg.size(); ++j)
                    if (iterF->arg[j] == iterF->dec[i])
                        check[j] = 1;
                for (unsigned int j = 0; j < iterF->arg.size(); ++j)
                    for (unsigned int k = j + 1; k < iterF->arg.size(); ++k)
                        if (iterF->arg[j] == iterF->arg[k] && friarg[j] != friarg[k])
                            goto result;
            }
            int tmpres = 1;
            for (unsigned i = 0; i < iterF->arg.size(); ++i)
            {
                if (check[i])
                    continue;
                for (unsigned int j = 0; j < iter->dec.size(); ++j)
                    if (iter->dec[j] == iterF->arg[i] && arg[j] != friarg[i])
                        tmpres = 0;
            }
            if (tmpres)
            {
                res = 1;
                goto result;
            }
        }
result:
        cout << (res ? "Yes" : "No") << endl;
    }
    return 0;
}

#endif