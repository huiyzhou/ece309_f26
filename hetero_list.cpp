#include <iostream>
#include <string>
using namespace std;

// ==================== Base Node ====================
class BaseNode {
public:
    BaseNode* next;
    BaseNode(BaseNode* n = nullptr) : next(n) {}
    virtual ~BaseNode() {}
    virtual void print() const = 0;        // must override
    virtual BaseNode* clone() const = 0;   // must override
};

// ==================== Templated Node ====================
template <typename T>
class Node : public BaseNode {
public:
    T data;
    Node(const T& value, BaseNode* n = nullptr)
        : BaseNode(n), data(value) {}

    void print() const override {
        cout << data;
    }

    BaseNode* clone() const override {
        return new Node<T>(data); // copy only the data, not the next pointer
    }
};

// ==================== Singly Linked List ====================
class SinglyLinkedList {
private:
    BaseNode* head;
    BaseNode* tail;

    void clear() {
        while (head) {
            BaseNode* tmp = head;
            head = head->next;
            delete tmp;
        }
        tail = nullptr;
    }

    void copy_from(const SinglyLinkedList& other) {
        for (BaseNode* curr = other.head; curr; curr = curr->next) {
            push_back_clone(curr);
        }
    }

    void push_back_clone(BaseNode* node) {
        if (!node) return;
        BaseNode* newNode = node->clone();
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

public:
    // ============= Rule of 5 =============
    SinglyLinkedList() : head(nullptr), tail(nullptr) {}
    ~SinglyLinkedList() { clear(); }

    // Copy constructor
    SinglyLinkedList(const SinglyLinkedList& other) : head(nullptr), tail(nullptr) {
        copy_from(other);
    }

    // Copy assignment
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            clear();
            copy_from(other);
        }
        return *this;
    }

    // Move constructor
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head(other.head), tail(other.tail) {
        other.head = other.tail = nullptr;
    }

    // Move assignment
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            other.head = other.tail = nullptr;
        }
        return *this;
    }

    // ============= Public API =============
    template <typename T>
    void push_back(const T& value) {
        BaseNode* newNode = new Node<T>(value);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    void pop_front() {
        if (!head) return;
        BaseNode* tmp = head;
        head = head->next;
        delete tmp;
        if (!head) tail = nullptr;
    }

    bool empty() const { return head == nullptr; }

    size_t size() const {
        size_t count = 0;
        for (BaseNode* curr = head; curr; curr = curr->next) {
            ++count;
        }
        return count;
    }

    void print() const {
        for (BaseNode* curr = head; curr; curr = curr->next) {
            curr->print();
            if (curr->next) cout << " -> ";
        }
        cout << "\n";
    }
};

// ==================== Example Usage ====================
int main() {
    SinglyLinkedList list1;
    list1.push_back(42);
    list1.push_back(3.14);
    list1.push_back(string("hello"));

    cout << "Original list1: ";
    list1.print();

    // Copy constructor
    SinglyLinkedList list2 = list1;
    cout << "Copied list2:   ";
    list2.print();

    // Copy assignment
    SinglyLinkedList list3;
    list3 = list1;
    cout << "Assigned list3: ";
    list3.print();

    // Move constructor
    SinglyLinkedList list4 = std::move(list1);
    cout << "Moved list4:    ";
    list4.print();
    cout << "After move, list1 empty? " << boolalpha << list1.empty() << "\n";

    // Move assignment
    SinglyLinkedList list5;
    list5 = std::move(list2);
    cout << "Moved list5:    ";
    list5.print();
    cout << "After move, list2 empty? " << boolalpha << list2.empty() << "\n";

    return 0;
}
