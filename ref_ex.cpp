#include <iostream>
using namespace std;

// Pass by value
void incrementByValue(int x) {
    x = x + 1;
    cout << "Inside incrementByValue: x = " << x << endl;
}

// Pass by pointer
void incrementByPointer(int *x) {
    *x = *x + 1;
    cout << "Inside incrementByPointer: *x = " << *x << endl;
}

// Pass by reference
void incrementByReference(int &x) {
    x = x + 1;
    cout << "Inside incrementByReference: x = " << x << endl;
}

int main() {
    int a = 10, b = 10, c = 10;

    cout << "Initially: a = " << a << ", b = " << b << ", c = " << c << "\n\n";

    // Pass by value
    incrementByValue(a);
    cout << "After incrementByValue: a = " << a << "\n\n";  // unchanged

    // Pass by pointer
    incrementByPointer(&b);
    cout << "After incrementByPointer: b = " << b << "\n\n";  // changed

    // Pass by reference
    incrementByReference(c);
    cout << "After incrementByReference: c = " << c << "\n";  // changed

    return 0;
}
