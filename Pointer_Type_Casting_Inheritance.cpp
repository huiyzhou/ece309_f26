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
    void describe() const { std::cout << name_ << " is an animal\n"; }  // NOT virtual
    const std::string& name() const { return name_; }

private:
    std::string name_;
};

class Dog : public Animal {
public:
    using Animal::Animal;

    void speak() const override { std::cout << name() << " says Woof!\n"; }
    void fetch() const { std::cout << name() << " fetches the ball\n"; }   // Dog-only

    // Same signature as Animal::describe(), but Animal's isn't virtual, so this
    // HIDES it rather than overriding it. (Adding `override` here would not compile.)
    void describe() const { std::cout << name() << " is a dog\n"; }
};

class Cat : public Animal {
public:
    using Animal::Animal;

    void speak() const override { std::cout << name() << " says Meow!\n"; }
};

// Takes ANY Animal by reference. Which functions run depends on whether they're virtual.
void demonstrateOverride(const Animal& a) {
    a.speak();      // virtual    -> chosen at RUNTIME by the object's real type (override)
    a.describe();   // non-virtual -> chosen at COMPILE TIME by the reference type (Animal)
}

int main() {
    Dog rex("Rex");
    Cat tom("Tom");

    // ── 0. Overriding vs. hiding ─────────────────────────────────────
    std::cout << "== Overrides ==\n";
    Animal generic("Generic");
    demonstrateOverride(generic);   // Animal::speak,  Animal::describe
    demonstrateOverride(rex);       // Dog::speak,     Animal::describe  (!)
    demonstrateOverride(tom);       // Cat::speak,     Animal::describe
    rex.describe();                 // Dog::describe: called directly on a Dog
    std::cout << "\n";

    // ── 1. Static upcasting (Dog* -> Animal*) ─────────────────────────
    std::cout << "== Upcasting ==\n";
    Animal* a1 = &rex;                          // implicit, always safe
    Animal* a2 = static_cast<Animal*>(&rex);    // same thing, written explicitly
    a1->speak();                                // "Rex says Woof!"  (virtual dispatch)
    a2->speak();
    // a1->fetch();                             // ❌ compile error: Animal has no fetch()
    demonstrateOverride(*a2);

    // ── 2. Static downcasting (Animal* -> Dog*), NO runtime check ────
    std::cout << "\n== Static downcasting (safe) ==\n";
    Dog* d1 = static_cast<Dog*>(a1);            // OK: a1 really points to a Dog
    d1->fetch();                                // "Rex fetches the ball"
    d1->describe();
    demonstrateOverride(*d1);

    // ── 3. Static downcasting (Animal* -> Dog*), UB ──────────────────
    // a3 points to a plain Animal — NOT a Dog. static_cast has no runtime
    // check, so it blindly reinterprets the Animal's memory as a Dog.
    // Calling fetch() is undefined behavior: the vtable and data layout of
    // a plain Animal object do not match what Dog::fetch() expects.
    std::cout << "\n== Static downcasting (undefined behavior) ==\n";
    Animal base("BaseOnly");
    Animal* a3 = &base;                         // points to a plain Animal, not a Dog
    Dog* bad = static_cast<Dog*>(a3);           // compiles: downcast is syntactically valid,
                                                // but the object is not a Dog
    bad->fetch();                               // ⚠️ UNDEFINED BEHAVIOR

    // ── 4. dynamic_cast: checked at runtime ──────────────────────────
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
        Dog& dref = dynamic_cast<Dog&>(*a3);    // a3 is a plain Animal
        dref.fetch();
    } catch (const std::bad_cast& e) {
        std::cout << "Reference cast failed: " << e.what() << "\n";
    }

    return 0;
}
