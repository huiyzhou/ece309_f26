#include <iostream>

using namespace std;

template <typename T>

class SinglyLinkedList {
private:
    // ====================== Node (nested) ======================
    struct Node {
        T data;
        Node* next;
        Node(const T& value, Node* n = nullptr)
            : data(value), next(n) {}
    };

    Node* head;
    Node* tail;

    void clear() {
        while (head) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
        tail = nullptr;
    }

    void copyFrom(const SinglyLinkedList& other) {
        if (!other.head) {
            head = tail = nullptr;
            return;
        }
        head = new Node(other.head->data);
        Node* curr = head;
        Node* otherCurr = other.head->next;
        while (otherCurr) {
            curr->next = new Node(otherCurr->data);
            curr = curr->next;
            otherCurr = otherCurr->next;
        }
        tail = curr;
    }

public:
    // ====================== Iterator ======================
    class Iterator {
    private:
        Node* node;
    public:
        Iterator(Node* n) : node(n) {}
        T& operator*() { return node->data; }
        Iterator& operator++() { node = node->next; return *this; }
        bool operator!=(const Iterator& other) const { return node != other.node; }
    };

    // Rule of Five
    SinglyLinkedList() : head(nullptr), tail(nullptr) {}
    SinglyLinkedList(const SinglyLinkedList& other) : head(nullptr), tail(nullptr) { copyFrom(other); }
    SinglyLinkedList(SinglyLinkedList&& other) noexcept : head(other.head), tail(other.tail) {
        other.head = other.tail = nullptr;
    }
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            clear();
            copyFrom(other);
        }
        return *this;
    }
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            other.head = other.tail = nullptr;
        }
        return *this;
    }
    ~SinglyLinkedList() { clear(); }

    // Member functions
    void push_front(const T& value) {
        Node* newNode = new Node(value, head);
        head = newNode;
        if (!tail) tail = head;
    }

    void push_back(const T& value) {
        Node* newNode = new Node(value, nullptr);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    void pop_front() {
        if (!head) return;
        Node* tmp = head;
        head = head->next;
        delete tmp;
        if (!head) tail = nullptr;
    }

    bool empty() const { return head == nullptr; }

    size_t size() const {
        size_t count = 0;
        Node* curr = head;
        while (curr) {
            ++count;
            curr = curr->next;
        }
        return count;
    }

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
};


template <typename T>
class Queue {
private:
    SinglyLinkedList<T> list;   //composition

public:
    void enqueue(const T& value) {
        list.push_back(value); // queue enqueue → back of list
    }

    void dequeue() {
        list.pop_front(); // queue dequeue → front of list
    }

    T& front() {
        return *list.begin();
    }

    bool empty() const {
        return list.empty();
    }

    size_t size() const {
        return list.size();
    }
};

template <typename T>
class Stack : public SinglyLinkedList<T> {   //inheritance
public:
    void push(const T& value) {
        this->push_front(value);
    }

    void pop() {
        this->pop_front();
    }

    T& top() {
        return *this->begin();
    }
};

int main() {
    Stack<int> s;
    s.push(10);
    s.push(20);
    cout << "Stack top: " << s.top() << "\n";
    s.pop();
    cout << "Stack top after pop: " << s.top() << "\n";

    Queue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    cout << "Queue front: " << q.front() << "\n";
    q.dequeue();
    cout << "Queue front after dequeue: " << q.front() << "\n";
}
