#include <iostream>
#include <map>
#include <vector>
using std::vector, std::map, std::string, std::cin, std::cout, std::endl, std::min;

#define CantFetch 0
#define Private   1
#define Protected 2
#define Public    3

struct classInfo
{
    int              baseCnt, memberCnt;
    vector<int>      bases;
    vector<int>      base_inherTypes;
    map<string, int> inherType_map;
};

classInfo Class[1001];

int              n; // num of class
map<string, int> idMap;

int inherTypeConvert(string inherType)
{
    if (inherType == "public")
        return Public;
    if (inherType == "private")
        return Private;
    if (inherType == "protected")
        return Protected;
    exit(-1);
}

int find(int id, const string &memberName)
{
    auto iter = Class[id].inherType_map.find(memberName);
    if (iter == Class[id].inherType_map.end())
    {
        for (int i = 0; i < Class[id].baseCnt; ++i)
        {
            int v;
            if ((v = find(Class[id].bases[i], memberName)) >= 0)
            {
                if (v <= 1)
                    return 0;
                else
                    return min(v, Class[id].base_inherTypes[i]);
            }
        }
        return -1;
    }
    else
        return iter->second;
}

int main()
{
    cin >> n; // num of class
    for (int i = 0; i < n; ++i)
    {
        string name, inherType;
        cin >> name;
        idMap.insert(make_pair(name, i));
        cin >> Class[i].baseCnt;
        for (int j = 0; j < Class[i].baseCnt; ++j)
        {
            cin >> inherType >> name;
            int v = inherTypeConvert(inherType);
            Class[i].bases.push_back(idMap[name]);
            Class[i].base_inherTypes.push_back(v);
        }
        cin >> Class[i].memberCnt;
        for (int j = 0; j < Class[i].memberCnt; ++j)
        {
            cin >> inherType >> name;
            int v = inherTypeConvert(inherType);
            Class[i].inherType_map.insert(make_pair(name, v));
        }
    }
    int k; // num of inquiry
    cin >> k;
    while (k--)
    {
        string className, memberName;
        cin >> className >> memberName;
        int         id        = idMap[className];
        int         result    = find(id, memberName);
        const char *results[] = {"Can not Fetch", "Private", "Protected", "Public"};
        if (result == -1)
            cout << "None" << endl;
        else
            cout << results[result] << endl;
    }
}