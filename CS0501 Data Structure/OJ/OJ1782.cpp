#include <iostream>

using namespace std;

class linknode;

class node
{
    friend class linknode;

public:
    int   data;
    node *next;
    node(int i = 0) : data(i), next(nullptr) {}
};

class linknode
{
public:
    node *head;

    int size;
    linknode() : size(0)
    {
        head       = new node(0);
        head->next = head;
    }
    ~linknode()
    {
        node *p, *q;
        p = head;
        while (p->next != head)
        {
            q = p;
            p = p->next;
            delete q;
        }
        delete p;
    }
    void push(int a)
    {
        node *p = head;
        while (p->next != head)
            p = p->next;
        p->next       = new node(a);
        p->next->next = head;
        size++;
    }
    int remove(int i)
    {
        int   pos = 1;
        node *p, *q;
        p = head->next;
        q = head;
        while (pos != i)
        {
            p = p->next;
            q = q->next;
            pos++;
        }
        q->next = p->next;
        int tmp = p->data;
        delete p;
        size--;
        return tmp;
    }
};

int main()
{
    int      m, n;
    linknode ln;
    cin >> n >> m;
    for (int i = 1; i <= n; i++)
        ln.push(i);
    int current = 0;
    while (ln.size != 0)
    {
        current += m;
        current %= ln.size;
        if (current == 0)
            current += ln.size;
        cout << ln.remove(current) << endl;
        current--;
    }
}