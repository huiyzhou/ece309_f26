#include <iostream>
using namespace std;

class ComplexNum {
private:
    float re;
    float im;

public:
    // Constructor
    ComplexNum(float a = 0, float b = 0) : re(a), im(b) { 
        cout << re << "+" << im << "i " << "constructor called" << endl;
    }

    // Copy constructor
    ComplexNum(const ComplexNum& x) : re(x.re), im(x.im) {
        cout << re << "+" << im << "i " << "Copy constructor called" << endl;
    }

    // Assignment operator
    ComplexNum& operator=(const ComplexNum& other) {
        cout << other.re << "+" << other.im << "i " << "Assignment operator called" << endl;
        if (this != &other) {   // protect against self-assignment
            re = other.re;
            im = other.im;
        }
        return *this;           // return reference to current object
    }

    // Destructor
    ~ComplexNum() {
        cout << "Destructor called for (" << re << ", " << im << ")" << endl;
    }

    //When  overload an operator as a member function, the compiler
    // intepret x + y is x.operator+(y)
    //Or lhs operator rhs is interpreted as lhs.operator(rhs)
    // Addition operator
    ComplexNum operator+(const ComplexNum& a) const {
        return ComplexNum(re + a.re, im + a.im);   // return by value
    }
    
    // Compound assignment operator (+=)
    ComplexNum& operator+=(const ComplexNum& a) {
        re += a.re;
        im += a.im;
        return *this;    // return by reference to allow chaining
    }
    
    // Output operator
    friend ostream& operator<<(ostream& stream, const ComplexNum& a) {
        stream << a.re << "+" << a.im << "i ";
        return stream;
    }

    // Example addition function (pass by value -> copy is made)
    ComplexNum Add(const ComplexNum a) const {
        return ComplexNum(re + a.re, im + a.im);
    }

    // ---------- Prefix Increment (++c) ----------
    ComplexNum& operator++() {
        // First increment, then return *this
        ++re;
        ++im;
        return *this;
    }

    // ---------- Postfix Increment (c++) ----------
    ComplexNum operator++(int) {
        // Create a copy of current object
        ComplexNum temp(*this);
        // Increment current object
        re++;
        im++;
        // Return the old value (before increment)
        return temp;
    }
};

int main() {
    ComplexNum c1(1, 2);
    ComplexNum c2;
    c2 = c1 + c2;               // uses operator+ and assignment operator
    //ComplexNum c2 = c1 + c2    //only use operator + (not assignment operator due to return value optimization and Mandatory Copy Elision since C++17) 
    cout << "c2 " << c2 << endl;         // uses operator<<
 

    ComplexNum c3(c1);
    c3 += c2;                   // uses operator+=
    cout << "c3 " << c3 << endl;

    c3.Add(c2);                 // creates a temporary sum (not used here)

    // ---------- Test prefix and postfix ----------
    cout << "Testing prefix ++c3: " << ++c3 << endl; // increments first, then prints
    cout << "Testing postfix c3++: " << c3++ << endl; // prints old value, then increments
    cout << "After postfix, c3 = " << c3 << endl;

    return 0;
}
