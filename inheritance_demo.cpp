#include <iostream>
#include <cstddef> // for offsetof

// Base class
class Base {
public:
    char a;
    char b;

    void print() { std::cout << "Base::print\n"; }
    //virtual void print() { std::cout << "Base::print\n"; }
};

// Derived class
class Derived : public Base {
public:
    char c;
    char d;
    char e;
    void print() { std::cout << "Derived::print\n"; }
};

// Multiple inheritance
class Other {
public:
    char x;
    void print() { std::cout << "Other::print\n"; }
};

class MultiDerived : public Base, public Other {
public:
    char y;
};

int main() {

    Base b;
    Derived d;
    Other o;
    MultiDerived md;
    b.print();
    d.print();
    o.print();
    Base *pb;
    pb = &d;
    pb->print();
    //md.print();

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

    return 0;
}
