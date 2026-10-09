#include <iostream>
#include <memory>
#include <string>
#include <utility>
using namespace std;

// ==================== Base Node ====================
class BaseNode {
public:
    unique_ptr<BaseNode> next;

    BaseNode() = default;
    virtual ~BaseNode() = default;

    // Disable copy — unique_ptr is not copyable
    BaseNode(const BaseNode&) = delete;
    BaseNode& operator=(const BaseNode&) = delete;

    virtual void print() const = 0;
    virtual unique_ptr<BaseNode> clone() const = 0;
};

// ==================== Templated Node ====================
template <typename T>
class Node : public BaseNode {
public:
    T data;

    Node(const T& value) : data(value) {}

    void print() const override {
        cout << data;
    }

    unique_ptr<BaseNode> clone() const override {
        return make_unique<Node<T>>(data);
    }
};

// ==================== Singly Linked List ====================
class SinglyLinkedList {
private:
    unique_ptr<BaseNode> head;
    BaseNode* tail = nullptr;   // non-owning observer pointer

    void clear() {
        // Iterative destruction to avoid deep-recursion stack overflow
        // (unique_ptr's default destructor is recursive via the chain)
        while (head) {
            head = std::move(head->next);
        }
        tail = nullptr;
    }

    void copy_from(const SinglyLinkedList& other) {
        for (const BaseNode* curr = other.head.get(); curr; curr = curr->next.get()) {
            push_back_clone(curr);
        }
    }

    void push_back_clone(const BaseNode* node) {
        if (!node) return;
        unique_ptr<BaseNode> newNode = node->clone();
        BaseNode* raw = newNode.get();
        if (!head) {
            head = std::move(newNode);
            tail = raw;
        } else {
            tail->next = std::move(newNode);
            tail = raw;
        }
    }

public:
    // ============= Rule of 5 =============
    SinglyLinkedList() = default;
    ~SinglyLinkedList() { clear(); }

    // Copy constructor
    SinglyLinkedList(const SinglyLinkedList& other) {
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
        : head(std::move(other.head)), tail(other.tail) {
        other.tail = nullptr;
    }

    // Move assignment
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = std::move(other.head);
            tail = other.tail;
            other.tail = nullptr;
        }
        return *this;
    }

    // ============= Public API =============
    template <typename T>
    void push_back(const T& value) {
        auto newNode = make_unique<Node<T>>(value);
        BaseNode* raw = newNode.get();
        if (!head) {
            head = std::move(newNode);
            tail = raw;
        } else {
            tail->next = std::move(newNode);
            tail = raw;
        }
    }

    void pop_front() {
        if (!head) return;
        head = std::move(head->next);
        if (!head) tail = nullptr;
    }

    bool empty() const { return head == nullptr; }

    size_t size() const {
        size_t count = 0;
        for (const BaseNode* curr = head.get(); curr; curr = curr->next.get()) {
            ++count;
        }
        return count;
    }

    void print() const {
        for (const BaseNode* curr = head.get(); curr; curr = curr->next.get()) {
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
