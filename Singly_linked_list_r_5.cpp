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
    SinglyLinkedList() noexcept : head_(nullptr), size_(0) {}

    // Convenience: construct from initializer list
    SinglyLinkedList(std::initializer_list<T> il) : SinglyLinkedList() {
        // Insert in reverse order to keep original sequence
        Node<T>* prev_head = nullptr;
        for (auto it = std::rbegin(il); it != std::rend(il); ++it) {
            Node<T>* node = new Node<T>(*it, prev_head);
            prev_head = node;
        }
        head_ = prev_head;
        size_ = il.size();
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
          size_(std::exchange(other.size_, 0)) {}

    // 6. Move assignment operator
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();                               // release own resources
            head_ = std::exchange(other.head_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    // ── Capacity ─────────────────────────────────────────────────────────────

    [[nodiscard]] bool   empty() const noexcept { return size_ == 0; }
    [[nodiscard]] size_t size()  const noexcept { return size_; }

    // ── Element access ───────────────────────────────────────────────────────

    T& front() {
        if (!head_) throw std::out_of_range("front() called on empty list");
        return head_->data;
    }
    const T& front() const {
        if (!head_) throw std::out_of_range("front() called on empty list");
        return head_->data;
    }

    T& at(size_t index) {
        Node<T>* node = node_at(index);   // throws if out of range
        return node->data;
    }
    const T& at(size_t index) const {
        Node<T>* node = node_at(index);
        return node->data;
    }

    // ── Modifiers ────────────────────────────────────────────────────────────

    // O(1) – insert at front
    void push_front(const T& value) {
        head_ = new Node<T>(value, head_);
        ++size_;
    }
    void push_front(T&& value) {
        head_ = new Node<T>(std::move(value), head_);
        ++size_;
    }

    // O(n) – insert at back
    void push_back(const T& value) {
        Node<T>* new_node = new Node<T>(value);
        append_node(new_node);
    }
    void push_back(T&& value) {
        Node<T>* new_node = new Node<T>(std::move(value));
        append_node(new_node);
    }

    // O(n) – insert before the node currently at 'index'
    void insert(size_t index, const T& value) {
        if (index == 0) { push_front(value); return; }
        Node<T>* prev = node_at(index - 1);
        prev->next = new Node<T>(value, prev->next);
        ++size_;
    }

    // O(1) – remove front
    void pop_front() {
        if (!head_) throw std::out_of_range("pop_front() called on empty list");
        Node<T>* old = head_;
        head_ = head_->next;
        delete old;
        --size_;
    }

    // O(n) – remove node at 'index'
    void erase(size_t index) {
        if (index == 0) { pop_front(); return; }
        Node<T>* prev = node_at(index - 1);
        if (!prev->next) throw std::out_of_range("erase(): index out of range");
        Node<T>* target = prev->next;
        prev->next = target->next;
        delete target;
        --size_;
    }

    // Destroy all nodes
    void clear() noexcept {
        while (head_) {
            Node<T>* tmp = head_->next;
            delete head_;
            head_ = tmp;
        }
        size_ = 0;
    }

    // Reverse the list in-place, O(n)
    void reverse() noexcept {
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
        std::swap(size_, other.size_);
    }

    void print(std::ostream& os = std::cout) const {
        Node<T>* cur = head_;
        while (cur) {
            os << cur->data;
            if (cur->next) os << " -> ";
            cur = cur->next;
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
    size_t   size_;

    // Deep-copy helper
    void copy_from(const SinglyLinkedList& other) {
        Node<T>* src = other.head_;
        if (!src) return;

        head_ = new Node<T>(src->data);
        Node<T>* dst = head_;
        src = src->next;

        while (src) {
            dst->next = new Node<T>(src->data);
            dst = dst->next;
            src = src->next;
        }
        size_ = other.size_;
    }

    // Append a pre-allocated node at the tail
    void append_node(Node<T>* new_node) {
        if (!head_) {
            head_ = new_node;
        } else {
            Node<T>* cur = head_;
            while (cur->next) cur = cur->next;
            cur->next = new_node;
        }
        ++size_;
    }

    // Bounds-checked node lookup
    Node<T>* node_at(size_t index) const {
        if (index >= size_)
            throw std::out_of_range("index " + std::to_string(index) +
                                    " out of range (size=" +
                                    std::to_string(size_) + ")");
        Node<T>* cur = head_;
        for (size_t i = 0; i < index; ++i) cur = cur->next;
        return cur;
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
