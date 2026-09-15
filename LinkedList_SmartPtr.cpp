#include <iostream>
#include <memory>

using namespace std;

template <typename T>
class SinglyLinkedList {
private:
    struct Node {
        T data;
        unique_ptr<Node> next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    unique_ptr<Node> head;
    Node* tail;   // non-owning raw pointer for O(1) push_back

    void copyFrom(const SinglyLinkedList& other) {
        if (!other.head) {
            head.reset();
            tail = nullptr;
            return;
        }
        head = make_unique<Node>(other.head->data);
        Node* curr = head.get();
        Node* otherCurr = other.head->next.get();
        while (otherCurr) {
            curr->next = make_unique<Node>(otherCurr->data);
            curr = curr->next.get();
            otherCurr = otherCurr->next.get();
        }
        tail = curr;
    }

public:
    // ====================== Iterator ======================
    class Iterator {
        Node* node;
    public:
        Iterator(Node* n) : node(n) {}
        T& operator*() { return node->data; }
        Iterator& operator++() { node = node->next.get(); return *this; }
        bool operator!=(const Iterator& other) const { return node != other.node; }
    };

    // ====================== Rule of Five ======================
    SinglyLinkedList() : head(nullptr), tail(nullptr) {}

    // ✅ Needed: deep copy (unique_ptr is not copyable)
    SinglyLinkedList(const SinglyLinkedList& other) { copyFrom(other); }

    // ✅ Needed: deep copy assignment
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            head.reset();
            tail = nullptr;
            copyFrom(other);
        }
        return *this;
    }

    // ✅ Needed: move to transfer ownership and fix tail
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head(std::move(other.head)), tail(other.tail) {
        other.tail = nullptr;
    }

    // ✅ Needed: move assignment to fix tail
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            head = std::move(other.head);
            tail = other.tail;
            other.tail = nullptr;
        }
        return *this;
    }

    // ❌ Not needed: destructor (unique_ptr handles cleanup)
    ~SinglyLinkedList() = default;

    // ====================== Member functions ======================
    void push_front(const T& value) {
        auto newNode = make_unique<Node>(value);
        if (!head) {
            tail = newNode.get();
        } else {
            newNode->next = std::move(head);
        }
        head = std::move(newNode);
    }

    void push_back(const T& value) {
        auto newNode = make_unique<Node>(value);
        Node* newTail = newNode.get();
        if (!head) {
            head = std::move(newNode);
        } else {
            tail->next = std::move(newNode);
        }
        tail = newTail;
    }

    void pop_front() {
        if (!head) return;
        head = std::move(head->next);
        if (!head) tail = nullptr;
    }

    bool empty() const { return !head; }

    size_t size() const {
        size_t count = 0;
        Node* curr = head.get();
        while (curr) {
            ++count;
            curr = curr->next.get();
        }
        return count;
    }

    Iterator begin() { return Iterator(head.get()); }
    Iterator end() { return Iterator(nullptr); }
};

// ====================== Example Usage ======================
int main() {
    SinglyLinkedList<int> list;
    list.push_back(10);
    list.push_back(20);
    list.push_front(5);

    cout << "List: ";
    for (auto it = list.begin(); it != list.end(); ++it)
        cout << *it << " ";
    cout << "\n";

    cout << "Size = " << list.size() << "\n";

    SinglyLinkedList<int> copyList = list;        // copy constructor
    SinglyLinkedList<int> movedList = std::move(list);  // move constructor

    cout << "Copied list size = " << copyList.size() << "\n";
    cout << "Moved list size = " << movedList.size() << "\n";
}
