#include <iostream>
#include <cstddef> // for offsetof

// Base class with virtual function
class Base {
public:
    char a;
    char b;

    virtual void print() { std::cout << "Base::print\n"; }
};

// Derived class with an extra char 'e'
class Derived : public Base {
public:
    char c;
    char d;
    char e;
    void print() override {std::cout << "Derived::print\n";}
};

// Multiple inheritance
class Other {
public:
    char x;
};

class MultiDerived : public Base, public Other {
public:
    char y;
    void print() override {std::cout << "MultiDerived::print\n";}
};

int main() {
    std::cout << "==== Sizes ====\n";
    std::cout << "sizeof(Base)        = " << sizeof(Base) << "\n";
    std::cout << "sizeof(Derived)     = " << sizeof(Derived) << "\n";
    std::cout << "sizeof(Other)       = " << sizeof(Other) << "\n";
    std::cout << "sizeof(MultiDerived)= " << sizeof(MultiDerived) << "\n\n";

    std::cout << "==== Offsets in Base ====\n";
    std::cout << "offset of a = " << offsetof(Base, a) << "\n";
    std::cout << "offset of b = " << offsetof(Base, b) << "\n\n";

    std::cout << "==== Offsets in Derived ====\n";
    std::cout << "offset of a (Base::a) = " << offsetof(Derived, a) << "\n";
    std::cout << "offset of b (Base::b) = " << offsetof(Derived, b) << "\n";
    std::cout << "offset of c           = " << offsetof(Derived, c) << "\n";
    std::cout << "offset of d           = " << offsetof(Derived, d) << "\n";
    std::cout << "offset of e           = " << offsetof(Derived, e) << "\n\n";

    std::cout << "==== Offsets in Other ====\n";
    std::cout << "offset of x = " << offsetof(Other, x) << "\n\n";

    std::cout << "==== Offsets in MultiDerived ====\n";
    std::cout << "offset of Base::a = " << offsetof(MultiDerived, a) << "\n";
    std::cout << "offset of Base::b = " << offsetof(MultiDerived, b) << "\n";
    std::cout << "offset of Other::x= " << offsetof(MultiDerived, x) << "\n";
    std::cout << "offset of y       = " << offsetof(MultiDerived, y) << "\n";

    Base * ptr_b;
    Base x;
    Derived y;
    MultiDerived z;
    
    ptr_b = &x;
    ptr_b->print();
    
    ptr_b = &y;
    ptr_b->print();
    
    ptr_b = &z;
    ptr_b->print();

    return 0;
}
