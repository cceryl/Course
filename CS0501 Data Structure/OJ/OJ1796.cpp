#include <iostream>

using std::cin, std::cout;

#define maxStackDepth 100

enum status { saveData, recursion };

class RecursionStack
{
    struct node
    {
        int                arg;
        unsigned long long returnValue;
        status             stat;
    };

private:
    node stack[maxStackDepth];
    int  top;

public:
    RecursionStack() : top(0) {}
    void push(int _arg, unsigned long long _returnValue, status _stat)
    {
        stack[top].arg         = _arg;
        stack[top].returnValue = _returnValue;
        stack[top].stat        = _stat;
        ++top;
    }
    RecursionStack &pop()
    {
        --top;
        return *this;
    }
    node &end() { return stack[top - 1]; }
    node &subend() { return stack[top - 2]; }
    node &begin() { return stack[0]; }
    int   len() { return top; }
    void  swap()
    {
        node tmp       = stack[top - 1];
        stack[top - 1] = stack[top - 2];
        stack[top - 2] = tmp;
    }
};

unsigned long long fibonacci(int n)
{
    RecursionStack reStack;
    reStack.push(n, 0, recursion);
    while (reStack.len() != 1 || reStack.end().stat == recursion)
    {
        if (reStack.end().stat == recursion)
        {
            if (reStack.end().arg == 1 || reStack.end().arg == 2)
            {
                reStack.pop();
                reStack.push(0, 1, saveData);
            }
            else
            {
                reStack.push(reStack.end().arg - 1, 0, recursion);
                reStack.push(reStack.subend().arg - 2, 0, recursion);
            }
        }
        else if (reStack.end().stat == saveData)
        {
            if (reStack.subend().stat == saveData)
            {
                unsigned long long tmpRes = reStack.end().returnValue + reStack.subend().returnValue;
                reStack.pop().pop().pop();
                reStack.push(0, tmpRes, saveData);
            }
            else if (reStack.subend().stat == recursion)
                reStack.swap();
        }
    }
    return reStack.begin().returnValue;
}

int main()
{
    int n;
    cin >> n;
    cout << fibonacci(n);
}