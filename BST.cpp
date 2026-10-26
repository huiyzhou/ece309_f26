#include <iostream>
using namespace std;

template <typename T>
class BST {
private:
    struct Node {
        T data;
        Node* left;
        Node* right;
        Node(const T& val) : data(val), left(nullptr), right(nullptr) {}
    };

    Node* root;

    // --- Helper functions ---
    Node* insert(Node* node, const T& val) {
        if (!node)
            return new Node(val);
        if (val < node->data)
            node->left = insert(node->left, val);
        else if (val > node->data)
            node->right = insert(node->right, val);
        return node; // no duplicates
    }

    Node* search(Node* node, const T& val) const {
        if (!node || node->data == val)
            return node;
        if (val < node->data)
            return search(node->left, val);
        else
            return search(node->right, val);
    }

    Node* findMin(Node* node) const {
        while (node && node->left)
            node = node->left;
        return node;
    }

    Node* remove(Node* node, const T& val) {
        if (!node)
            return nullptr;
        if (val < node->data)
            node->left = remove(node->left, val);
        else if (val > node->data)
            node->right = remove(node->right, val);
        else {
            // Node found
            if (!node->left && !node->right) {
                delete node;
                return nullptr;
            } else if (!node->left) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                Node* temp = node->left;
                delete node;
                return temp;
            } else {
                Node* succ = findMin(node->right);
                node->data = succ->data;
                node->right = remove(node->right, succ->data);
            }
        }
        return node;
    }

    void inorder(Node* node) const {
        if (!node) return;
        inorder(node->left);
        cout << node->data << " ";
        inorder(node->right);
    }
    void preorder(Node* node) const {
        if (!node) return;
        cout << node->data << " ";
        preorder(node->left);
        preorder(node->right);
    }
    void postorder(Node* node) const {
        if (!node) return;
        postorder(node->left);
        postorder(node->right);
        cout << node->data << " ";
    }

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    Node* clone(Node* node) const {
        if (!node) return nullptr;
        Node* newNode = new Node(node->data);
        newNode->left = clone(node->left);
        newNode->right = clone(node->right);
        return newNode;
    }

public:
    // --- Constructors and destructor (Rule of 5) ---
    BST() : root(nullptr) {}

    ~BST() { destroy(root); }

    // Copy constructor
    BST(const BST& other) : root(nullptr) {
        if (other.root)
            root = clone(other.root);
    }

    // Copy assignment operator
    BST& operator=(const BST& other) {
        if (this == &other)
            return *this;
        destroy(root);
        root = other.root ? clone(other.root) : nullptr;
        return *this;
    }

    // Move constructor
    BST(BST&& other) noexcept : root(other.root) {
        other.root = nullptr;
    }

    // Move assignment operator
    BST& operator=(BST&& other) noexcept {
        if (this == &other)
            return *this;
        destroy(root);
        root = other.root;
        other.root = nullptr;
        return *this;
    }

    // --- Public API ---
    void insert(const T& val) { root = insert(root, val); }

    bool search(const T& val) const { return search(root, val) != nullptr; }

    void remove(const T& val) { root = remove(root, val); }

    void inorder() const { inorder(root); cout << "\n"; }

    void preorder() const { preorder(root); cout << "\n"; }

    void postorder() const { postorder(root); cout << "\n"; }
    
    T& get_root_data() const {return root->data;}
};

// --- Example usage ---
int main() {
    BST<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);

    cout << "Original tree inorder: ";
    tree.inorder();

    tree.insert(15);
    tree.insert(8);
    tree.insert(6);
    tree.insert(1);
    tree.remove(5);
    
    cout << "After insert & remove tree inorder: ";
    cout << tree.get_root_data() << endl;
    tree.inorder();

    tree.preorder();

    tree.postorder();

    // Copy constructor
    BST<int> copyTree = tree;
    cout << "Copied tree inorder: ";
    copyTree.inorder();

    // Move constructor
    BST<int> movedTree = std::move(tree);
    cout << "Moved tree inorder: ";
    movedTree.inorder();

    // Copy assignment
    BST<int> assignTree;
    assignTree = movedTree;
    cout << "Assigned tree inorder: ";
    assignTree.inorder();

    // Move assignment
    BST<int> moveAssignTree;
    moveAssignTree = std::move(assignTree);
    cout << "Move-assigned tree inorder: ";
    moveAssignTree.inorder();
}
