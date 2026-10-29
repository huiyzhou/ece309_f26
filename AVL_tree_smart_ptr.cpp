// ECE 309 — AVL Tree with smart pointers (C++20)
// Build: g++ -std=c++20 -Wall -Wextra -O2 avl.cpp -o avl
//
// An AVL tree is a self-balancing BST: for every node, the heights of
// its left and right subtrees differ by at most 1. After each insert or
// remove we walk back up the path and apply rotations where needed,
// which keeps height = O(log n). Every operation is therefore O(log n),
// even for sorted input that would turn a plain BST into a linked list.
//
// Ownership: each Node owns its children via std::unique_ptr, exactly
// like the plain BST. Rotations are just moves of those unique_ptrs.

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

template <typename T>
class AVLTree {
private:
    struct Node {
        T key;
        int height = 0;  // leaf has height 0
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(T k) : key(std::move(k)) {}
    };
    using Ptr = std::unique_ptr<Node>;

    Ptr root_;
    std::size_t size_ = 0;

    // ── Height / balance helpers ─────────────────────────────────────
    static int h(const Ptr& n) { return n ? n->height : -1; }

    static void update(Node& n) {
        n.height = 1 + std::max(h(n.left), h(n.right));
    }

    // > 0: left-heavy, < 0: right-heavy
    static int balanceFactor(const Ptr& n) {
        return n ? h(n->left) - h(n->right) : 0;
    }

    // ── Rotations ────────────────────────────────────────────────────
    //
    /*
       rotateRight(y):        y                x
                             / \              / \
                            x   C    ==>     A   y
                           / \                  / \
                          A   B                B   C
    
    */
    // Each takes ownership of the subtree root and returns the new root.
    static Ptr rotateRight(Ptr y) {
        Ptr x = std::move(y->left);
        y->left = std::move(x->right);  // B moves under y
        update(*y);                     // y is now lower: update it first
        x->right = std::move(y);
        update(*x);
        return x;
    }

    static Ptr rotateLeft(Ptr x) {
        Ptr y = std::move(x->right);
        x->right = std::move(y->left);
        update(*x);
        y->left = std::move(x);
        update(*y);
        return y;
    }

    // Restore the AVL property at n (assumes both subtrees are valid AVL).
    static Ptr rebalance(Ptr n) {
        update(*n);
        int bf = balanceFactor(n);

        if (bf > 1) {                              // left-heavy
            if (balanceFactor(n->left) < 0)        // Left-Right case
                n->left = rotateLeft(std::move(n->left));
            return rotateRight(std::move(n));      // Left-Left case
        }
        if (bf < -1) {                             // right-heavy
            if (balanceFactor(n->right) > 0)       // Right-Left case
                n->right = rotateRight(std::move(n->right));
            return rotateLeft(std::move(n));       // Right-Right case
        }
        return n;                                  // already balanced
    }

    // ── Recursive insert / remove ────────────────────────────────────
    // Recursion is safe here: depth is the tree height, which AVL keeps
    // at about 1.44 * log2(n) (≈ 29 levels for a million keys).
    static Ptr insert(Ptr n, const T& key, bool& inserted) {
        if (!n) {
            inserted = true;
            return std::make_unique<Node>(key);
        }
        if (key < n->key)
            n->left = insert(std::move(n->left), key, inserted);
        else if (n->key < key)
            n->right = insert(std::move(n->right), key, inserted);
        else
            return n;  // duplicate: no change, no rebalance needed
        return rebalance(std::move(n));
    }

    static Ptr remove(Ptr n, const T& key, bool& removed) {
        if (!n) return nullptr;  // not found

        if (key < n->key) {
            n->left = remove(std::move(n->left), key, removed);
        } else if (n->key < key) {
            n->right = remove(std::move(n->right), key, removed);
        } else {
            removed = true;
            // 0 or 1 child: splice the child into our place.
            // Returning it destroys n (its other child slot is empty).
            if (!n->left)  return std::move(n->right);
            if (!n->right) return std::move(n->left);

            // 2 children: take the in-order successor's key,
            // then remove the successor from the right subtree.
            const Node* s = n->right.get();
            while (s->left) s = s->left.get();
            n->key = s->key;
            bool dummy = false;
            n->right = remove(std::move(n->right), n->key, dummy);
        }
        return rebalance(std::move(n));
    }

    static Ptr clone(const Node* n) {
        if (!n) return nullptr;
        auto c = std::make_unique<Node>(n->key);
        c->height = n->height;
        c->left = clone(n->left.get());
        c->right = clone(n->right.get());
        return c;
    }

    template <typename F>
    static void inorder(const Node* n, F& visit) {
        if (!n) return;
        inorder(n->left.get(), visit);
        visit(n->key);
        inorder(n->right.get(), visit);
    }

    // Checks BST ordering, stored heights and the AVL balance rule.
    // Returns the subtree height, or -2 if anything is wrong.
    static int check(const Node* n, const T* lo, const T* hi) {
        if (!n) return -1;
        if ((lo && !(*lo < n->key)) || (hi && !(n->key < *hi))) return -2;
        int lh = check(n->left.get(), lo, &n->key);
        int rh = check(n->right.get(), &n->key, hi);
        if (lh == -2 || rh == -2) return -2;
        if (std::abs(lh - rh) > 1) return -2;
        if (n->height != 1 + std::max(lh, rh)) return -2;
        return n->height;
    }

public:
    // ── Special member functions ─────────────────────────────────────
    // The tree is always balanced, so the default (recursive) destructor
    // only recurses O(log n) deep. No iterative clear() is needed, and
    // we don't have to declare a destructor at all.
    //
    // Copying still needs to be written by hand, because unique_ptr is
    // move-only. Since we never declare a destructor, the compiler still
    // generates the move operations for us; we default them explicitly
    // anyway for clarity.
    AVLTree() = default;
    AVLTree(const AVLTree& o) : root_(clone(o.root_.get())), size_(o.size_) {}
    AVLTree& operator=(const AVLTree& o) {
        if (this != &o) {
            AVLTree tmp(o);
            std::swap(root_, tmp.root_);
            std::swap(size_, tmp.size_);
        }
        return *this;
    }
    AVLTree(AVLTree&& o) noexcept
        : root_(std::move(o.root_)), size_(std::exchange(o.size_, 0)) {}
    AVLTree& operator=(AVLTree&& o) noexcept {
        root_ = std::move(o.root_);
        size_ = std::exchange(o.size_, 0);
        return *this;
    }

    // ── Public interface ─────────────────────────────────────────────
    bool insert(const T& key) {
        bool inserted = false;
        root_ = insert(std::move(root_), key, inserted);
        if (inserted) ++size_;
        return inserted;
    }

    bool remove(const T& key) {
        bool removed = false;
        root_ = remove(std::move(root_), key, removed);
        if (removed) --size_;
        return removed;
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

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] int height() const noexcept { return h(root_); }
    [[nodiscard]] bool isValid() const { return check(root_.get(), nullptr, nullptr) != -2; }
    void clear() noexcept { root_.reset(); size_ = 0; }

    template <typename F> void inorder(F visit) const { inorder(root_.get(), visit); }
};

// ── Demo ─────────────────────────────────────────────────────────────
template <typename T>
void print(const char* label, const AVLTree<T>& t) {
    std::cout << label << " [size=" << t.size() << ", height=" << t.height()
              << ", valid=" << std::boolalpha << t.isValid() << "]: ";
    t.inorder([](const T& k) { std::cout << k << ' '; });
    std::cout << '\n';
}

int main() {
    // Sorted input: a plain BST would reach height 9 here. AVL stays at 3.
    AVLTree<int> t;
    for (int k = 1; k <= 10; ++k) t.insert(k);
    print("sorted 1..10 ", t);

    t.remove(4);   // root-area node with two children
    t.remove(1);
    t.remove(2);   // forces a rotation on the way back up
    print("after removes", t);

    AVLTree<int> copy = t;
    copy.insert(100);
    print("original     ", t);
    print("copy         ", copy);

    // Stress test: 1,000,000 sorted keys.
    AVLTree<int> big;
    const int N = 1'000'000;
    for (int i = 0; i < N; ++i) big.insert(i);
    std::cout << "\n1M sorted inserts: height=" << big.height()
              << " valid=" << big.isValid() << '\n';

    for (int i = 0; i < N; i += 2) big.remove(i);  // remove every even key
    std::cout << "after removing evens: size=" << big.size()
              << " height=" << big.height()
              << " valid=" << big.isValid()
              << " contains(7)=" << big.contains(7)
              << " contains(8)=" << big.contains(8) << '\n';
}
