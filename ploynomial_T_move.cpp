#include <iostream>
#include <algorithm>
using namespace std;

// ======================== ComplexNum ===========================
class ComplexNum {
private:
    double re;
    double im;

public:
    ComplexNum(double a = 0, double b = 0) : re(a), im(b) {}
    ComplexNum(const ComplexNum& other) : re(other.re), im(other.im) {}

    ComplexNum& operator=(const ComplexNum& other) {
        if (this != &other) {
            re = other.re;
            im = other.im;
        }
        return *this;
    }

    ComplexNum operator+(const ComplexNum& other) const {
        return ComplexNum(re + other.re, im + other.im);
    }

    ComplexNum operator*(const ComplexNum& other) const {
        return ComplexNum(re * other.re - im * other.im,
                          re * other.im + im * other.re);
    }

    ComplexNum& operator+=(const ComplexNum& other) {
        re += other.re;
        im += other.im;
        return *this;
    }

    friend ostream& operator<<(ostream& os, const ComplexNum& c) {
        os << c.re;
        if (c.im >= 0) os << "+";
        os << c.im << "i";
        return os;
    }
};

// ======================== Polynomial Template ===========================
template <typename T>
class Polynomial {
private:
    int degree;
    T* coeffs;

public:
    // Constructor
    Polynomial(int deg = 0) : degree(deg) {
        coeffs = new T[degree + 1]();  // value-initialize coefficients
    }

    // Copy constructor
    Polynomial(const Polynomial& other) : degree(other.degree) {
        coeffs = new T[degree + 1];
        for (int i = 0; i <= degree; i++)
            coeffs[i] = other.coeffs[i];
    }

    // Move constructor
    Polynomial(Polynomial&& other) noexcept : degree(other.degree), coeffs(other.coeffs) {
        other.coeffs = nullptr;
        other.degree = 0;
    }

    // Copy assignment
    Polynomial& operator=(const Polynomial& other) {
        if (this != &other) {
            delete[] coeffs;
            degree = other.degree;
            coeffs = new T[degree + 1];
            for (int i = 0; i <= degree; i++)
                coeffs[i] = other.coeffs[i];
        }
        return *this;
    }

    // Move assignment
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

    // Indexing
    T& operator[](int i) { return coeffs[i]; }
    const T& operator[](int i) const { return coeffs[i]; }

    int getDegree() const { return degree; }

    // Evaluate at x
    T operator()(const T& x) const {
        T result = T(0);
        T power = T(1);
        for (int i = 0; i <= degree; i++) {
            result = result + coeffs[i] * power;
            power = power * x;
        }
        return result;
    }

    // Addition
    Polynomial operator+(const Polynomial& other) const {
        int maxDeg = max(degree, other.degree);
        Polynomial result(maxDeg);
        for (int i = 0; i <= maxDeg; i++) {
            T c1 = (i <= degree ? coeffs[i] : T(0));
            T c2 = (i <= other.degree ? other.coeffs[i] : T(0));
            result[i] = c1 + c2;
        }
        return result;
    }

    // Multiplication
    Polynomial operator*(const Polynomial& other) const {
        int newDeg = degree + other.degree;
        Polynomial result(newDeg);
        for (int i = 0; i <= newDeg; i++) {
            result[i] = T(0);
        }
        for (int i = 0; i <= degree; i++) {
            for (int j = 0; j <= other.degree; j++) {
                result[i+j] += coeffs[i] * other.coeffs[j];
            }
        }
        return result;
    }

    // Print
    friend ostream& operator<<(ostream& os, const Polynomial& p) {
        for (int i = p.degree; i >= 0; i--) {
            os << "(" << p.coeffs[i] << ")";
            if (i > 0) os << "x^" << i << " + ";
        }
        return os;
    }
};

// ======================== Main ===========================
int main() {
    // Polynomial with ComplexNum coefficients
    Polynomial<ComplexNum> p(2);
    p[2] = ComplexNum(1, 2);
    p[1] = ComplexNum(3, 4);
    p[0] = ComplexNum(5, 6);

    cout << "Polynomial p(x): " << p << endl;

    Polynomial<ComplexNum> q(1);
    q[1] = ComplexNum(2, -1);
    q[0] = ComplexNum(1, 0);

    cout << "Polynomial q(x): " << q << endl;

    auto sum = p + q;
    cout << "p(x) + q(x) = " << sum << endl;

    auto prod = p * q;
    cout << "p(x) * q(x) = " << prod << endl;

    ComplexNum x(1,2);
    cout << "p(1+2i) = " << p(x) << endl; 

    // Polynomial with int coefficients: r(x) = 2x^2 + 3x + 4
    Polynomial<int> r(2);
    r[2] = 2; r[1] = 3; r[0] = 4;
    cout << "Polynomial r(x): " << r << endl;
    cout << "r(2) = " << r(2) << endl;

    // ---------------- Move constructor demo ----------------
    cout << "\n--- Move constructor demo ---\n";
    Polynomial<double> temp1(2);
    temp1[0] = 10.0; temp1[1] = 20.0; temp1[2] = 30.0;

    cout << "Creating rt = std::move(temp1)\n";
    Polynomial<double> rt = std::move(temp1); // invokes move constructor
    cout << "Polynomial r(x): " << rt << endl;
    cout << "Polynomial temp1(x) after move: degree = " << temp1.getDegree() << endl;

    // ---------------- Move assignment demo ----------------
    cout << "\n--- Move assignment demo ---\n";
    Polynomial<double> temp2(3);
    temp2[0] = 1.0; temp2[1] = 1.0; temp2[2] = 1.0; temp2[3] = 1.0;

    Polynomial<double> s(1);
    s[0] = 42.0; s[1] = 99.0;

    cout << "Before move assignment, s(x): " << s << endl;

    cout << "Assigning s = std::move(temp2)\n";
    s = std::move(temp2); // invokes move assignment
    cout << "After move assignment, s(x): " << s << endl;
    cout << "Polynomial temp2(x) after move: degree = " << temp2.getDegree() << endl;


    return 0;
}
