#include <cstddef>   // for size_t
#include <utility>   // for std::move

// ==========================
// UniquePtr Implementation
// ==========================
template <typename T>
class UniquePtr {
private:
    T* ptr;

public:
    explicit UniquePtr(T* p = nullptr) : ptr(p) {}

    // No copy allowed
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Move semantics
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    ~UniquePtr() { delete ptr; }

    // Accessors
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }

    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }
};

// ==========================
// SharedPtr Implementation
// ==========================
template <typename T>
class SharedPtr {
private:
    T* ptr;
    size_t* ref_count;

public:
    explicit SharedPtr(T* p = nullptr) : ptr(p), ref_count(new size_t(1)) {}

    // Copy constructor
    SharedPtr(const SharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count) {
        ++(*ref_count);
    }

    // Copy assignment
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            ++(*ref_count);
        }
        return *this;
    }

    // Move constructor
    SharedPtr(SharedPtr&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    // Move assignment
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    ~SharedPtr() { release(); }

    void release() {
        if (ref_count) {
            if (--(*ref_count) == 0) {
                delete ptr;
                delete ref_count;
            }
        }
    }

    // Accessors
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
    size_t use_count() const { return ref_count ? *ref_count : 0; }
};


#include <iostream>

using namespace std;

class MyClass {
public:
    MyClass(int v) : value(v) {
        cout << "MyClass(" << value << ") constructed\n";
    }
    ~MyClass() {
        cout << "MyClass(" << value << ") destroyed\n";
    }
    void show() const {
        cout << "Value = " << value << endl;
    }
private:
    int value;
};

int main() {
    cout << "=== UniquePtr demo ===" << endl;

    UniquePtr<int> arr1(new int[10]);
    UniquePtr<int> arr2 = std::move(arr1);

    SharedPtr<int> arr3(new int[10]);
    arr3.get()[5] = 10; //arr3[5] = 10
    SharedPtr<int> arr4 = arr3;
    arr4.get()[5] = 8;  
    cout << "use count of the array: " << arr4.use_count() << " arr4[5] = " << arr3.get()[5] << endl;

    {
        UniquePtr<MyClass> uptr(new MyClass(10));
        uptr->show();

        // Transfer ownership via move
        UniquePtr<MyClass> uptr2 = std::move(uptr);
        if (!uptr.get()) {
            cout << "uptr is now empty after move." << endl;
        }
        uptr2->show();
    } // uptr2 goes out of scope, deletes MyClass

    cout << "\n=== SharedPtr demo ===" << endl;
    {
        SharedPtr<MyClass> sptr1(new MyClass(42));
        cout << "sptr1 use_count = " << sptr1.use_count() << endl;

        {
            SharedPtr<MyClass> sptr2 = sptr1; // copy, increase ref count
            cout << "After copy, use_count = " << sptr1.use_count() << endl;

            sptr2->show();
        } // sptr2 goes out of scope, ref count decreases

        cout << "After sptr2 destroyed, use_count = " << sptr1.use_count() << endl;
    } // sptr1 goes out of scope, object destroyed



    return 0;
}
