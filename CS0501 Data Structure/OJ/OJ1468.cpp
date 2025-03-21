#include <iostream>

// singly circular linked list with head node
template <typename T>
class LinkedList
{
    class node
    {
    public:
        T     data;
        node *next;
        node() = default;
        node(T _data, node *_next) : data(_data), next(_next) {}
        node(const node &obj) = delete;
        node(node &&obj)      = delete;
        ~node()               = default;
    };

protected:
    node *head;
    node *rear;

public:
    LinkedList()
    {
        head       = new node;
        head->next = head;
        rear       = head;
    }
    LinkedList(const LinkedList &obj)
    {
        head       = new node;
        head->next = head;
        rear       = head;
        for (node *p = obj.head->next; p != obj.head; p = p->next)
            rear = rear->next = new node(p->data, head);
    }
    LinkedList(LinkedList &&obj) = delete;
    virtual ~LinkedList()
    {
        node *p = head->next, *q;
        while (p != head)
        {
            q = p;
            p = p->next;
            delete q;
        }
        delete p;
    }
    void pushFront(T _data)
    {
        head->next = new node(_data, head->next);
        rear       = rear == head ? head->next : rear;
    }
    void pushBack(T _data) { rear = rear->next = new node(_data, head); }
    T    popFront()
    {
        if (head == rear)
            return head->data;
        node *first = head->next;
        head->next  = first->next;
        T _data     = first->data;
        rear        = rear == first ? head : rear;
        delete first;
        return _data;
    }
    T popBack()
    {
        if (head == rear)
            return head->data;
        node *last  = rear;
        T     _data = last->data;
        delete last;
        rear = head;
        while (rear->next != last)
            rear = rear->next;
        rear->next = head;
        return _data;
    }
    int size() const
    {
        int nodeCnt = 0;
        for (node *p = head->next; p != head; p = p->next)
            nodeCnt++;
        return nodeCnt;
    }
    void print() const
    {
        for (node *p = head->next; p != head; p = p->next)
            std::cout << p->data << ' ';
        std::cout << std::endl;
    }
    virtual const char *name() { return "LinkedList"; }
    virtual T           peak() { return rear->data; }
    virtual T           pop() { return popBack(); }
    virtual void        push(T val) { pushBack(val); }
};

template <typename T>
class Stack : public LinkedList<T>
{
public:
    Stack() : LinkedList<T>::LinkedList() {}
    Stack(const Stack &obj) : LinkedList<T>::LinkedList(obj) {}
    Stack(Stack &&obj) = delete;
    ~Stack() {}
    const char *name() { return "Stack"; }
    T           peak() { return LinkedList<T>::rear->data; }
    T           pop() { return LinkedList<T>::popBack(); }
    void        push(T val) { LinkedList<T>::pushBack(val); }
};

template <typename T>
class Queue : public LinkedList<T>
{
public:
    Queue() : LinkedList<T>::LinkedList() {}
    Queue(const Queue &obj) : LinkedList<T>::LinkedList(obj) {}
    Queue(Queue &&obj) = delete;
    ~Queue() {}
    const char *name() { return "Queue"; }
    T           peak() { return LinkedList<T>::head->next->data; }
    T           pop() { return LinkedList<T>::popFront(); }
    void        push(T val) { LinkedList<T>::pushBack(val); }
};