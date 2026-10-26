// ECE 309 — Binary Search Tree with smart pointers (C++20)
// Build: g++ -std=c++20 -Wall -Wextra -O2 bst.cpp -o bst
//
// Ownership model: each Node OWNS its children via std::unique_ptr.
// The tree owns the root. No raw `new`/`delete` anywhere, so no leaks
// and no double-frees. Raw pointers (Node*) are used only for
// non-owning traversal.

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

template <typename T>
class BST {
private:
    struct Node {
        T key;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(T k) : key(std::move(k)) {}
    };

    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;

    // Deep copy of a subtree (used by the copy constructor).
    static std::unique_ptr<Node> clone(const Node* n) {
        if (!n) return nullptr;
        auto copy = std::make_unique<Node>(n->key);
        copy->left = clone(n->left.get());
        copy->right = clone(n->right.get());
        return copy;
    }

    static int height(const Node* n) {
        if (!n) return -1;  // empty tree has height -1, single node has 0
        return 1 + std::max(height(n->left.get()), height(n->right.get()));
    }

    template <typename F>
    static void inorder(const Node* n, F& visit) {
        if (!n) return;
        inorder(n->left.get(), visit);
        visit(n->key);
        inorder(n->right.get(), visit);
    }

    template <typename F>
    static void preorder(const Node* n, F& visit) {
        if (!n) return;
        visit(n->key);
        preorder(n->left.get(), visit);
        preorder(n->right.get(), visit);
    }

    template <typename F>
    static void postorder(const Node* n, F& visit) {
        if (!n) return;
        postorder(n->left.get(), visit);
        postorder(n->right.get(), visit);
        visit(n->key);
    }

public:
    // ── Special member functions (Rule of Five) ──────────────────────
    BST() = default;

    // unique_ptr is move-only, so the compiler CANNOT generate a copy
    // constructor for us. We write a deep copy explicitly.
    BST(const BST& other) : root_(clone(other.root_.get())), size_(other.size_) {}

    // Copy-and-swap: strong exception safety, self-assignment safe.
    BST& operator=(const BST& other) {
        if (this != &other) {
            BST tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // Moves just transfer the root pointer: O(1).
    BST(BST&& other) noexcept
        : root_(std::move(other.root_)), size_(std::exchange(other.size_, 0)) {}

    BST& operator=(BST&& other) noexcept {
        root_ = std::move(other.root_);
        size_ = std::exchange(other.size_, 0);
        return *this;
    }

    // Iterative teardown. The default destructor would recurse once per
    // level, which can overflow the stack on a degenerate (list-shaped)
    // tree. Here we detach children before each node is destroyed.
    ~BST() { clear(); }

    void swap(BST& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
    }

    // ── Capacity ─────────────────────────────────────────────────────
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] int height() const { return height(root_.get()); }

    void clear() noexcept {
        std::vector<std::unique_ptr<Node>> stack;
        if (root_) stack.push_back(std::move(root_));
        while (!stack.empty()) {
            std::unique_ptr<Node> n = std::move(stack.back());
            stack.pop_back();
            if (n->left) stack.push_back(std::move(n->left));
            if (n->right) stack.push_back(std::move(n->right));
            // n is destroyed here with no children attached → no recursion
        }
        size_ = 0;
    }

    // ── Core operations ──────────────────────────────────────────────

    // Returns false if key already exists (no duplicates).
    // We walk a pointer-to-unique_ptr so we can install the new node
    // directly into the empty slot we find.
    bool insert(const T& key) {
        std::unique_ptr<Node>* slot = &root_;
        while (*slot) {
            if (key < (*slot)->key)      slot = &(*slot)->left;
            else if ((*slot)->key < key) slot = &(*slot)->right;
            else                         return false;
        }
        *slot = std::make_unique<Node>(key);
        ++size_;
        return true;
    }

    [[nodiscard]] bool contains(const T& key) const {
        const Node* cur = root_.get();
        while (cur) {
            if (key < cur->key)      cur = cur->left.get();
            else if (cur->key < key) cur = cur->right.get();
            else                     return true;
        }
        return false;
    }

    // Returns false if key was not found.
    bool remove(const T& key) {
        std::unique_ptr<Node>* slot = &root_;
        while (*slot) {
            if (key < (*slot)->key)      slot = &(*slot)->left;
            else if ((*slot)->key < key) slot = &(*slot)->right;
            else                         break;
        }
        if (!*slot) return false;

        Node* n = slot->get();
        if (!n->left) {
            // 0 or 1 child (right): splice right child into our place.
            // The old node is freed automatically by the assignment.
            *slot = std::move(n->right);
        } else if (!n->right) {
            // 1 child (left)
            *slot = std::move(n->left);
        } else {
            // 2 children: replace key with in-order successor
            // (minimum of the right subtree), then unlink successor.
            std::unique_ptr<Node>* succ = &n->right;
            while ((*succ)->left) succ = &(*succ)->left;
            n->key = std::move((*succ)->key);
            *succ = std::move((*succ)->right);
        }
        --size_;
        return true;
    }

    [[nodiscard]] std::optional<T> min() const {
        const Node* cur = root_.get();
        if (!cur) return std::nullopt;
        while (cur->left) cur = cur->left.get();
        return cur->key;
    }

    [[nodiscard]] std::optional<T> max() const {
        const Node* cur = root_.get();
        if (!cur) return std::nullopt;
        while (cur->right) cur = cur->right.get();
        return cur->key;
    }

    // ── Traversals (visitor receives const T&) ───────────────────────
    template <typename F> void inorder(F visit) const   { inorder(root_.get(), visit); }
    template <typename F> void preorder(F visit) const  { preorder(root_.get(), visit); }
    template <typename F> void postorder(F visit) const { postorder(root_.get(), visit); }
};

// ── Demo ─────────────────────────────────────────────────────────────
template <typename T>
void print(const char* label, const BST<T>& t) {
    std::cout << label << " [size=" << t.size() << ", height=" << t.height() << "]: ";
    t.inorder([](const T& k) { std::cout << k << ' '; });
    std::cout << '\n';
}

int main() {
    BST<int> tree;
    for (int k : {50, 30, 70, 20, 40, 60, 80, 35, 45, 65}) tree.insert(k);
    print("initial  ", tree);

    std::cout << "contains(40)? " << std::boolalpha << tree.contains(40) << '\n';
    std::cout << "contains(99)? " << tree.contains(99) << '\n';
    std::cout << "insert dup 50? " << tree.insert(50) << '\n';
    std::cout << "min=" << *tree.min() << " max=" << *tree.max() << '\n';

    std::cout << "preorder : ";
    tree.preorder([](int k) { std::cout << k << ' '; });
    std::cout << "\npostorder: ";
    tree.postorder([](int k) { std::cout << k << ' '; });
    std::cout << '\n';

    tree.remove(20);  // leaf
    tree.remove(70);  // two children
    tree.remove(30);  // two children
    print("after rm ", tree);

    // Deep copy: modifying the copy must not affect the original.
    BST<int> copy = tree;
    copy.insert(999);
    copy.remove(50);
    print("original ", tree);
    print("copy     ", copy);

    // Move: O(1) pointer transfer; source left empty but valid.
    BST<int> moved = std::move(copy);
    print("moved    ", moved);
    print("moved-from", copy);

    // Degenerate tree: iterative clear() avoids stack overflow on destruction.
    BST<int> chain;
    for (int i = 0; i < 100000; ++i) chain.insert(i);
    std::cout << "chain size=" << chain.size() << " (destroyed safely at scope exit)\n";
}
