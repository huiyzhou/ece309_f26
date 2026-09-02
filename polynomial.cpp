#include <iostream>
using namespace std;

class ComplexNum {
private:
    double re;
    double im;

public:
    // Constructor
    ComplexNum(double a = 0, double b = 0) : re(a), im(b) {}

    // Copy constructor
    ComplexNum(const ComplexNum& other) : re(other.re), im(other.im) {}

    // Assignment
    ComplexNum& operator=(const ComplexNum& other) {
        if (this != &other) {
            re = other.re;
            im = other.im;
        }
        return *this;
    }

    // Addition
    ComplexNum operator+(const ComplexNum& other) const {
        return ComplexNum(re + other.re, im + other.im);
    }

    // Multiplication
    ComplexNum operator*(const ComplexNum& other) const {
        return ComplexNum(re * other.re - im * other.im,
                          re * other.im + im * other.re);
    }

    // Compound addition
    ComplexNum& operator+=(const ComplexNum& other) {
        re += other.re;
        im += other.im;
        return *this;
    }

    // Output
    friend ostream& operator<<(ostream& os, const ComplexNum& c) {
        os << c.re;
        if (c.im >= 0) os << "+";
        os << c.im << "i";
        return os;
    }
};

// ---------------------------------------------------

class Polynomial {
private:
    int degree;             // degree of polynomial
    ComplexNum* coeffs;     // dynamic array of coefficients

public:
    // Constructor
    Polynomial(int deg = 0) : degree(deg) {
        coeffs = new ComplexNum[degree + 1];
    }

    // Copy constructor (deep copy)
    Polynomial(const Polynomial& other) : degree(other.degree) {
        coeffs = new ComplexNum[degree + 1];
        for (int i = 0; i <= degree; i++)
            coeffs[i] = other.coeffs[i];
    }

    // Move constructor
    Polynomial(Polynomial&& other) noexcept : degree(other.degree), coeffs(other.coeffs) {
        other.coeffs = nullptr;
        other.degree = 0;
    }

    // Assignment operator (deep copy)
    Polynomial& operator=(const Polynomial& other) {
        if (this != &other) {
            delete[] coeffs;
            degree = other.degree;
            coeffs = new ComplexNum[degree + 1];
            for (int i = 0; i <= degree; i++)
                coeffs[i] = other.coeffs[i];
        }
        return *this;
    }

    // Move assignment operator
    Polynomial& operator=(Polynomial&& other) noexcept {
        if (this != &other) {
            delete[] coeffs;
            degree = other.degree;
            coeffs = other.coeffs;
            other.coeffs = nullptr;
            other.degree = 0;
        }
        return *this;
    }

    // Destructor
    ~Polynomial() {
        delete[] coeffs;
    }

    // Access coefficient
    ComplexNum& operator[](int i) { return coeffs[i]; }  //potential out of bound access!!!
    const ComplexNum& operator[](int i) const { return coeffs[i]; } //potential out of bound access!!!

    // Degree getter
    int getDegree() const { return degree; }

    // Evaluate polynomial at a given complex number x
    ComplexNum evaluate(const ComplexNum& x) const {
        ComplexNum result(0, 0);
        ComplexNum power(1, 0);   // x^0 = 1
        for (int i = 0; i <= degree; i++) {
            result = result + coeffs[i] * power;
            power = power * x;
        }
        return result;
    }
    
    //overload () as evaluate
    ComplexNum operator()(const ComplexNum& x) const {
        //return evaluate(x);
        ComplexNum result(0, 0);
        ComplexNum power(1, 0);   // x^0 = 1
        for (int i = 0; i <= degree; i++) {
            result = result + coeffs[i] * power;
            power = power * x;
        }
        return result;
    }
    

    // Polynomial addition
    Polynomial operator+(const Polynomial& other) const {
        int maxDeg = max(degree, other.degree);
        Polynomial result(maxDeg);

        for (int i = 0; i <= maxDeg; i++) {
            ComplexNum c1 = (i <= degree ? coeffs[i] : ComplexNum(0,0));
            ComplexNum c2 = (i <= other.degree ? other.coeffs[i] : ComplexNum(0,0));
            result[i] = c1 + c2;
        }
        return result;
    }

    // Polynomial multiplication
    Polynomial operator*(const Polynomial& other) const {
        int newDeg = degree + other.degree;
        Polynomial result(newDeg);

        // Initialize coefficients
        for (int i = 0; i <= newDeg; i++) {
            result[i] = ComplexNum(0,0);
        }

        for (int i = 0; i <= degree; i++) {
            for (int j = 0; j <= other.degree; j++) {
                ComplexNum prod = coeffs[i] * other.coeffs[j];
                result[i+j] += prod;
            }
        }
        return result;
    }

    // Print polynomial
    friend ostream& operator<<(ostream& os, const Polynomial& p) {
        for (int i = p.degree; i >= 0; i--) {
            os << "(" << p.coeffs[i] << ")";
            if (i > 0) os << "x^" << i << " + ";
        }
        return os;
    }
};

// ---------------------------------------------------

int main() {
    // Create polynomial p(x) = (1+2i)x^2 + (3+4i)x + (5+6i)
    Polynomial p(2);
    p[2] = ComplexNum(1, 2);
    p[1] = ComplexNum(3, 4);
    p[0] = ComplexNum(5, 6);

    cout << "Polynomial p(x): " << p << endl;

    // Create polynomial q(x) = (2-1i)x + (1+0i)
    Polynomial q(1);
    q[1] = ComplexNum(2, -1);
    q[0] = ComplexNum(1, 0);

    cout << "Polynomial q(x): " << q << endl;

    // Polynomial addition
    Polynomial sum = p + q;
    cout << "p(x) + q(x) = " << sum << endl;

    // Polynomial multiplication
    Polynomial prod = p * q;
    cout << "p(x) * q(x) = " << prod << endl;

    ComplexNum x(1,2);
    cout << "p(1+2i) = " << p(x) << endl; 
    return 0;
}
