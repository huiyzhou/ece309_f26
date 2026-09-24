#include <iostream>
using namespace std;

class A {
public:
    A() { cout << "A constructor called\n"; }
    ~A() { cout << "A destructor called\n"; }
};

class B {
public:
    B() { cout << "B constructor called\n"; }
    ~B() { cout << "B destructor called\n"; }
};

class Base {
public:
    Base() { cout << "Base constructor called\n"; }
    ~Base() { cout << "Base destructor called\n"; }
};

class Derived : public Base {
private:
    A a_member; // composition
    B b_member; // composition
public:
    Derived() { cout << "Derived constructor called\n"; }
    ~Derived() { cout << "Derived destructor called\n"; }
};

int main() {
    cout << "Creating Derived object...\n";
    Derived obj;
    cout << "Derived object created.\n";
    return 0;
}
