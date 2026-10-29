// ECE 309 — AVL Tree with RAW pointers (C++20)
// Build: g++ -std=c++20 -Wall -Wextra -O2 avl_raw.cpp -o avl_raw
//
// Same algorithm and interface as avl.cpp, but each Node owns its
// children through raw Node* pointers. Nothing is freed automatically,
// so this class follows the Rule of Five: destructor, copy constructor,
// copy assignment, move constructor and move assignment are all written
// by hand.

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <utility>

template <typename T>
class AVLTree {
private:
    struct Node {
        T key;
        int height = 0;  // leaf has height 0
        Node* left = nullptr;
        Node* right = nullptr;
        explicit Node(T k) : key(std::move(k)) {}
    };

    Node* root_ = nullptr;
    std::size_t size_ = 0;

    // ── Height / balance helpers ─────────────────────────────────────
    static int h(const Node* n) { return n ? n->height : -1; }

    static void update(Node* n) {
        n->height = 1 + std::max(h(n->left), h(n->right));
    }

    // > 0: left-heavy, < 0: right-heavy
    static int balanceFactor(const Node* n) {
        return n ? h(n->left) - h(n->right) : 0;
    }

    // ── Rotations ────────────────────────────────────────────────────
    /*
       rotateRight(y):        y                x
                             / \              / \
                            x   C    ==>     A   y
                           / \                  / \
                          A   B                B   C
    */
    // Each returns the new subtree root. Only pointers are re-linked:
    // no node is allocated or freed.
    static Node* rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;  // B moves under y
        x->right = y;
        update(y);           // y is now lower: update it first
        update(x);
        return x;
    }

    static Node* rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        update(x);
        update(y);
        return y;
    }

    // Restore the AVL property at n (assumes both subtrees are valid AVL).
    static Node* rebalance(Node* n) {
        update(n);
        int bf = balanceFactor(n);

        if (bf > 1) {                           // left-heavy
            if (balanceFactor(n->left) < 0)     // Left-Right case
                n->left = rotateLeft(n->left);
            return rotateRight(n);              // Left-Left case
        }
        if (bf < -1) {                          // right-heavy
            if (balanceFactor(n->right) > 0)    // Right-Left case
                n->right = rotateRight(n->right);
            return rotateLeft(n);               // Right-Right case
        }
        return n;                               // already balanced
    }

    // ── Recursive insert / remove ────────────────────────────────────
    // Each call returns the pointer that should now occupy the slot.
    static Node* insert(Node* n, const T& key, bool& inserted) {
        if (!n) {
            inserted = true;
            return new Node(key);
        }
        if (key < n->key)
            n->left = insert(n->left, key, inserted);
        else if (n->key < key)
            n->right = insert(n->right, key, inserted);
        else
            return n;  // duplicate
        return rebalance(n);
    }

    static Node* remove(Node* n, const T& key, bool& removed) {
        if (!n) return nullptr;  // not found

        if (key < n->key) {
            n->left = remove(n->left, key, removed);
        } else if (n->key < key) {
            n->right = remove(n->right, key, removed);
        } else {
            removed = true;
            // 0 or 1 child: splice the child in and free n.
            // (With unique_ptr, returning the child freed n for us.)
            if (!n->left || !n->right) {
                Node* child = n->left ? n->left : n->right;
                delete n;
                return child;  // may be nullptr; nothing to rebalance here
            }
            // 2 children: take the in-order successor's key,
            // then remove the successor node from the right subtree.
            const Node* s = n->right;
            while (s->left) s = s->left;
            n->key = s->key;
            bool dummy = false;
            n->right = remove(n->right, n->key, dummy);
        }
        return rebalance(n);
    }

    // Deep copy. If `new` throws partway through, free the part already
    // built so the copy constructor doesn't leak.
    static Node* clone(const Node* n) {
        if (!n) return nullptr;
        Node* c = new Node(n->key);
        c->height = n->height;
        try {
            c->left = clone(n->left);
            c->right = clone(n->right);
        } catch (...) {
            destroy(c);
            throw;
        }
        return c;
    }

    // Post-order delete. Recursion is safe: depth is O(log n) for AVL.
    static void destroy(Node* n) noexcept {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    template <typename F>
    static void inorder(const Node* n, F& visit) {
        if (!n) return;
        inorder(n->left, visit);
        visit(n->key);
        inorder(n->right, visit);
    }

    // Checks BST ordering, stored heights and the AVL balance rule.
    // Returns the subtree height, or -2 if anything is wrong.
    static int check(const Node* n, const T* lo, const T* hi) {
        if (!n) return -1;
        if ((lo && !(*lo < n->key)) || (hi && !(n->key < *hi))) return -2;
        int lh = check(n->left, lo, &n->key);
        int rh = check(n->right, &n->key, hi);
        if (lh == -2 || rh == -2) return -2;
        if (std::abs(lh - rh) > 1) return -2;
        if (n->height != 1 + std::max(lh, rh)) return -2;
        return n->height;
    }

public:
    // ── Special member functions (Rule of Five) ──────────────────────
    AVLTree() = default;

    // A compiler-generated copy would be shallow (shared nodes, then a
    // double free), so we deep-copy.
    AVLTree(const AVLTree& o) : root_(clone(o.root_)), size_(o.size_) {}

    // Copy-and-swap: self-assignment safe and exception safe.
    AVLTree& operator=(const AVLTree& o) {
        if (this != &o) {
            AVLTree tmp(o);
            std::swap(root_, tmp.root_);
            std::swap(size_, tmp.size_);
        }  // tmp's destructor frees our OLD nodes
        return *this;
    }

    // Steal the root and null out the source so its destructor
    // doesn't free nodes we now own.
    AVLTree(AVLTree&& o) noexcept
        : root_(std::exchange(o.root_, nullptr)), size_(std::exchange(o.size_, 0)) {}

    AVLTree& operator=(AVLTree&& o) noexcept {
        if (this != &o) {
            destroy(root_);  // free what we currently own
            root_ = std::exchange(o.root_, nullptr);
            size_ = std::exchange(o.size_, 0);
        }
        return *this;
    }

    ~AVLTree() { destroy(root_); }

    // ── Public interface ─────────────────────────────────────────────
    bool insert(const T& key) {
        bool inserted = false;
        root_ = insert(root_, key, inserted);
        if (inserted) ++size_;
        return inserted;
    }

    bool remove(const T& key) {
        bool removed = false;
        root_ = remove(root_, key, removed);
        if (removed) --size_;
        return removed;
    }

    [[nodiscard]] bool contains(const T& key) const {
        const Node* cur = root_;
        while (cur) {
            if (key < cur->key)      cur = cur->left;
            else if (cur->key < key) cur = cur->right;
            else                     return true;
        }
        return false;
    }

    [[nodiscard]] std::optional<T> min() const {
        const Node* cur = root_;
        if (!cur) return std::nullopt;
        while (cur->left) cur = cur->left;
        return cur->key;
    }

    [[nodiscard]] std::optional<T> max() const {
        const Node* cur = root_;
        if (!cur) return std::nullopt;
        while (cur->right) cur = cur->right;
        return cur->key;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] int height() const noexcept { return h(root_); }
    [[nodiscard]] bool isValid() const { return check(root_, nullptr, nullptr) != -2; }

    void clear() noexcept {
        destroy(root_);
        root_ = nullptr;  // avoid a dangling pointer
        size_ = 0;
    }

    template <typename F> void inorder(F visit) const { inorder(root_, visit); }
};

// ── Demo (identical to avl.cpp, plus a few assignment checks) ────────
template <typename T>
void print(const char* label, const AVLTree<T>& t) {
    std::cout << label << " [size=" << t.size() << ", height=" << t.height()
              << ", valid=" << std::boolalpha << t.isValid() << "]: ";
    t.inorder([](const T& k) { std::cout << k << ' '; });
    std::cout << '\n';
}

int main() {
    AVLTree<int> t;
    for (int k = 1; k <= 10; ++k) t.insert(k);
    print("sorted 1..10 ", t);

    t.remove(4);
    t.remove(1);
    t.remove(2);
    print("after removes", t);

    AVLTree<int> copy = t;
    copy.insert(100);
    print("original     ", t);
    print("copy         ", copy);

    AVLTree<int> moved = std::move(copy);
    print("moved        ", moved);
    print("moved-from   ", copy);

    copy = t;   // copy-assign into a moved-from tree
    moved = t;  // copy-assign over an existing tree (old nodes freed)
    print("reassigned   ", moved);

    AVLTree<int> big;
    const int N = 1'000'000;
    for (int i = 0; i < N; ++i) big.insert(i);
    std::cout << "\n1M sorted inserts: height=" << big.height()
              << " valid=" << big.isValid() << '\n';

    for (int i = 0; i < N; i += 2) big.remove(i);
    std::cout << "after removing evens: size=" << big.size()
              << " height=" << big.height()
              << " valid=" << big.isValid()
              << " contains(7)=" << big.contains(7)
              << " contains(8)=" << big.contains(8) << '\n';
}
