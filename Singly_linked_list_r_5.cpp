#pragma once

#include <algorithm>    // std::swap
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <utility>      // std::move, std::exchange

// ─────────────────────────────────────────────────────────────────────────────
//  Node
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
struct Node {
    T     data;
    Node* next;

    explicit Node(const T& value, Node* next_node = nullptr)
        : data(value), next(next_node) {}

    explicit Node(T&& value, Node* next_node = nullptr)
        : data(std::move(value)), next(next_node) {}
};

// ─────────────────────────────────────────────────────────────────────────────
//  SinglyLinkedList
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
class SinglyLinkedList {
public:
    // ── Constructors / Rule of Five ──────────────────────────────────────────

    // 1. Default constructor
    SinglyLinkedList() noexcept : head_(nullptr), tail_(nullptr) {}

    // Convenience: construct from initializer list
    SinglyLinkedList(std::initializer_list<T> il) : SinglyLinkedList() {
        for (const T& val : il)
            push_back(val);
    }

    // 2. Destructor
    ~SinglyLinkedList() { clear(); }

    // 3. Copy constructor  (deep copy)
    SinglyLinkedList(const SinglyLinkedList& other) : SinglyLinkedList() {
        copy_from(other);
    }

    // 4. Copy assignment operator  (copy-and-swap idiom)
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            SinglyLinkedList tmp(other);   // deep copy
            swap(tmp);                     // steal resources
        }
        return *this;                      // old data destroyed with tmp
    }

    // 5. Move constructor
    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head_(std::exchange(other.head_, nullptr)),
          tail_(std::exchange(other.tail_, nullptr)) {}

    // 6. Move assignment operator
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = std::exchange(other.head_, nullptr);
            tail_ = std::exchange(other.tail_, nullptr);
        }
        return *this;
    }

    // ── Capacity ─────────────────────────────────────────────────────────────

    [[nodiscard]] bool empty() const noexcept { return head_ == nullptr; }

    // O(n) — no size member; traverse to count
    [[nodiscard]] size_t size() const noexcept {
        size_t count = 0;
        for (Node<T>* cur = head_; cur; cur = cur->next) ++count;
        return count;
    }

    // ── Element access ───────────────────────────────────────────────────────

    T& front() {
        if (!head_) throw std::out_of_range("front() called on empty list");
        return head_->data;
    }
    const T& front() const {
        if (!head_) throw std::out_of_range("front() called on empty list");
        return head_->data;
    }

    T& back() {
        if (!tail_) throw std::out_of_range("back() called on empty list");
        return tail_->data;
    }
    const T& back() const {
        if (!tail_) throw std::out_of_range("back() called on empty list");
        return tail_->data;
    }

    T& at(size_t index) {
        return node_at(index)->data;   // throws if out of range
    }
    const T& at(size_t index) const {
        return node_at(index)->data;
    }

    // ── Modifiers ────────────────────────────────────────────────────────────

    // O(1) – insert at front
    void push_front(const T& value) {
        head_ = new Node<T>(value, head_);
        if (!tail_) tail_ = head_;
    }
    void push_front(T&& value) {
        head_ = new Node<T>(std::move(value), head_);
        if (!tail_) tail_ = head_;
    }

    // O(1) – insert at back  (tail pointer makes this O(1))
    void push_back(const T& value) {
        Node<T>* node = new Node<T>(value);
        link_tail(node);
    }
    void push_back(T&& value) {
        Node<T>* node = new Node<T>(std::move(value));
        link_tail(node);
    }

    // O(n) – insert before the node currently at 'index'
    void insert(size_t index, const T& value) {
        if (index == 0) { push_front(value); return; }
        Node<T>* prev     = node_at(index - 1);    // throws if out of range
        Node<T>* new_node = new Node<T>(value, prev->next);
        prev->next = new_node;
        if (!new_node->next) tail_ = new_node;      // inserted at the very end
    }

    // O(1) – remove front
    void pop_front() {
        if (!head_) throw std::out_of_range("pop_front() called on empty list");
        Node<T>* old = head_;
        head_ = head_->next;
        if (!head_) tail_ = nullptr;                // list became empty
        delete old;
    }

    // O(n) – remove node at 'index'
    void erase(size_t index) {
        if (!head_) throw std::out_of_range("erase() called on empty list");
        if (index == 0) { pop_front(); return; }
        Node<T>* prev   = node_at(index - 1);       // throws if out of range
        Node<T>* target = prev->next;
        if (!target) throw std::out_of_range("erase(): index out of range");
        prev->next = target->next;
        if (!prev->next) tail_ = prev;              // erased the tail
        delete target;
    }

    // Destroy all nodes
    void clear() noexcept {
        while (head_) {
            Node<T>* tmp = head_->next;
            delete head_;
            head_ = tmp;
        }
        tail_ = nullptr;
    }

    // Reverse the list in-place, O(n)
    void reverse() noexcept {
        tail_ = head_;                              // old head becomes new tail
        Node<T>* prev    = nullptr;
        Node<T>* current = head_;
        while (current) {
            Node<T>* next = current->next;
            current->next = prev;
            prev    = current;
            current = next;
        }
        head_ = prev;
    }

    // ── Utilities ────────────────────────────────────────────────────────────

    void swap(SinglyLinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
    }

    void print(std::ostream& os = std::cout) const {
        for (Node<T>* cur = head_; cur; cur = cur->next) {
            os << cur->data;
            if (cur->next) os << " -> ";
        }
        os << '\n';
    }

    // ── Forward iterator ─────────────────────────────────────────────────────
    class iterator {
    public:
        explicit iterator(Node<T>* node) : current_(node) {}
        T&         operator*()  { return current_->data; }
        T*         operator->() { return &current_->data; }
        iterator&  operator++() { current_ = current_->next; return *this; }
        iterator   operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        bool operator==(const iterator& rhs) const { return current_ == rhs.current_; }
        bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
    private:
        Node<T>* current_;
    };

    iterator begin() { return iterator(head_); }
    iterator end()   { return iterator(nullptr); }

private:
    Node<T>* head_;
    Node<T>* tail_;     // always points to the last node, or nullptr when empty

    // Deep-copy helper — reuses push_back so tail_ is maintained automatically
    void copy_from(const SinglyLinkedList& other) {
        for (Node<T>* src = other.head_; src; src = src->next)
            push_back(src->data);
    }

    // Attach a pre-allocated node at the tail in O(1)
    void link_tail(Node<T>* node) {
        if (!tail_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
    }

    // Bounds-checked node lookup — O(n), no size_ available
    Node<T>* node_at(size_t index) const {
        Node<T>* cur = head_;
        size_t   i   = 0;
        while (cur) {
            if (i == index) return cur;
            cur = cur->next;
            ++i;
        }
        throw std::out_of_range("index " + std::to_string(index) +
                                " out of range (size=" +
                                std::to_string(i) + ")");
    }
};

// Non-member swap (lets std::swap work via ADL)
template <typename T>
void swap(SinglyLinkedList<T>& a, SinglyLinkedList<T>& b) noexcept {
    a.swap(b);
}
//#include "linked_list.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== Default constructor + push_back / push_front ===\n";
    SinglyLinkedList<int> list;
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    list.push_front(0);
    list.print();   // 0 -> 10 -> 20 -> 30

    std::cout << "\n=== Initializer-list constructor ===\n";
    SinglyLinkedList<std::string> words = {"alpha", "beta", "gamma"};
    words.print();  // alpha -> beta -> gamma

    std::cout << "\n=== Copy constructor (deep copy) ===\n";
    SinglyLinkedList<int> copy(list);
    copy.push_back(99);
    std::cout << "original: "; list.print();   // unchanged
    std::cout << "copy    : "; copy.print();   // has extra 99

    std::cout << "\n=== Copy assignment ===\n";
    SinglyLinkedList<int> assigned;
    assigned = list;
    std::cout << "assigned: "; assigned.print();

    std::cout << "\n=== Move constructor ===\n";
    SinglyLinkedList<int> moved(std::move(assigned));
    std::cout << "moved   : "; moved.print();
    std::cout << "assigned after move (empty=" << assigned.empty() << ")\n";

    std::cout << "\n=== Move assignment ===\n";
    SinglyLinkedList<int> move_assigned;
    move_assigned = std::move(moved);
    std::cout << "move_assigned: "; move_assigned.print();

    std::cout << "\n=== insert / erase ===\n";
    SinglyLinkedList<int> lst = {1, 2, 4, 5};
    lst.insert(2, 3);   // insert 3 before index 2
    lst.print();        // 1 -> 2 -> 3 -> 4 -> 5
    lst.erase(0);       // remove head
    lst.print();        // 2 -> 3 -> 4 -> 5

    std::cout << "\n=== reverse ===\n";
    lst.reverse();
    lst.print();        // 5 -> 4 -> 3 -> 2

    std::cout << "\n=== Range-for via iterator ===\n";
    for (const auto& v : lst) std::cout << v << ' ';
    std::cout << '\n';

    std::cout << "\n=== at() / front() ===\n";
    std::cout << "front=" << lst.front() << "  at(2)=" << lst.at(2) << '\n';

    std::cout << "\n=== out-of-range exception ===\n";
    try { lst.at(100); }
    catch (const std::out_of_range& e) { std::cout << "Caught: " << e.what() << '\n'; }

    std::cout << "\n=== Template with double ===\n";
    SinglyLinkedList<double> dlist = {1.1, 2.2, 3.3};
    dlist.print();

    return 0;
}
