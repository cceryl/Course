#include <iostream>

struct triplet
{
    int  x, y, z;
    void sort()
    {
        if (x > y)
            std::swap(x, y);
        if (x > z)
            std::swap(x, z);
        if (y > z)
            std::swap(y, z);
    }
    int ratio()
    {
        int Dxy = y - x;
        int Dyz = z - y;
        if (Dxy < Dyz)
            return (Dyz - 1) / Dxy;
        else
            return (Dxy - 1) / Dyz;
    }
    bool    AP() { return x + z == 2 * y; }
    bool    tree() { return y - x < z - y; }
    triplet fit()
    {
        int Dxy = y - x;
        int Dyz = z - y;
        if (Dxy < Dyz)
            return triplet{z - Dxy - 1 - (Dyz - 1) % Dxy, z - 1 - (Dyz - 1) % Dxy, z};
        else
            return triplet{x, x + 1 + (Dxy - 1) % Dyz, x + Dyz + 1 + (Dxy - 1) % Dyz};
    }
    bool operator!=(const triplet &rhs) { return x != rhs.x || y != rhs.y || z != rhs.z; }
};

int simpABC, simpXYZ;

bool check(triplet a, triplet b)
{
    while (!a.AP())
    {
        simpABC += a.ratio();
        a        = a.fit();
    }
    while (!b.AP())
    {
        simpXYZ += b.ratio();
        b        = b.fit();
    }
    return !(a != b);
}

int main()
{
    triplet abc, xyz;
    scanf("%d%d%d", &abc.x, &abc.y, &abc.z);
    scanf("%d%d%d", &xyz.x, &xyz.y, &xyz.z);
    abc.sort(), xyz.sort();
    if (check(abc, xyz))
    {
        printf("YES\n");
        int ans = simpABC + simpXYZ;
        while (abc != xyz)
        {
            triplet x = abc.fit();
            triplet y = xyz.fit();
            int     A = simpABC - abc.ratio();
            int     B = simpXYZ - xyz.ratio();
            if (A < B)
                xyz = y, simpXYZ = B;
            else if (A > B)
                abc = x, simpABC = A;
            else if (A == B)
            {
                if (x != y)
                {
                    abc = x, simpABC = A;
                    xyz = y, simpXYZ = B;
                }
                else
                    break;
            }
        }
        if (abc != xyz)
        {
            if (abc.tree() == xyz.tree())
                ans -= std::min(simpABC, simpXYZ) * 2;
            else
                ans -= (simpABC - abc.ratio()) * 2;
        }
        else
            ans -= simpABC * 2;
        printf("%d", ans);
    }
    else
        printf("NO");
    return 0;
}