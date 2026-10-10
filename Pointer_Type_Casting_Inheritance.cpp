// animal_casting.cpp
// Demonstrates static upcasting, static downcasting, and dynamic_cast.
// Compile: g++ -std=c++20 -Wall animal_casting.cpp -o animal_casting

#include <iostream>
#include <string>

class Animal {
public:
    explicit Animal(std::string name) : name_(std::move(name)) {}
    virtual ~Animal() = default;                    // virtual dtor: required for a base class

    virtual void speak() const { std::cout << name_ << " makes a sound\n"; }
    const std::string& name() const { return name_; }

private:
    std::string name_;
};

class Dog : public Animal {
public:
    using Animal::Animal;

    void speak() const override { std::cout << name() << " says Woof!\n"; }
    void fetch() const { std::cout << name() << " fetches the ball\n"; }   // Dog-only
};

class Cat : public Animal {
public:
    using Animal::Animal;

    void speak() const override { std::cout << name() << " says Meow!\n"; }
};

int main() {
    Dog rex("Rex");
    Cat tom("Tom");

    // ── 1. Static upcasting (Dog* -> Animal*) ─────────────────────────
    std::cout << "== Upcasting ==\n";
    Animal* a1 = &rex;                          // implicit, always safe
    Animal* a2 = static_cast<Animal*>(&rex);    // same thing, written explicitly
    a1->speak();                                // "Rex says Woof!"  (virtual dispatch)
    a2->speak();
    // a1->fetch();                             // ❌ compile error: Animal has no fetch()

    // ── 2. Static downcasting (Animal* -> Dog*), NO runtime check ────
    std::cout << "\n== Static downcasting ==\n";
    Dog* d1 = static_cast<Dog*>(a1);            // OK: a1 really points to a Dog
    d1->fetch();                                // "Rex fetches the ball"

    Animal* a3 = &tom;
    // Dog* bad = static_cast<Dog*>(a3);        // ⚠️ compiles, but a3 is a Cat!
    // bad->fetch();                            //    undefined behavior

    // ── 3. dynamic_cast: checked at runtime ──────────────────────────
    std::cout << "\n== dynamic_cast ==\n";
    Animal* zoo[] = { &rex, &tom };

    for (Animal* a : zoo) {
        if (Dog* d = dynamic_cast<Dog*>(a)) {   // returns nullptr if not a Dog
            std::cout << a->name() << " is a Dog -> ";
            d->fetch();
        } else {
            std::cout << a->name() << " is NOT a Dog (dynamic_cast returned nullptr)\n";
        }
    }

    // Reference version: failure throws std::bad_cast instead of returning null
    try {
        Dog& dref = dynamic_cast<Dog&>(*a3);    // a3 is a Cat
        dref.fetch();
    } catch (const std::bad_cast& e) {
        std::cout << "Reference cast failed: " << e.what() << "\n";
    }

    return 0;
}
