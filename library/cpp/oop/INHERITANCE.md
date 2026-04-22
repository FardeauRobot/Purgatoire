# Inheritance in C++ — Complete Guide

Inheritance lets you express an **"is-a"** relationship between types. A `Dog` is-a `Animal`. A `ScavTrap` is-a `ClapTrap`. The derived class automatically gets the members of the base — plus the ability to add or override its own.

```cpp
class Animal {
public:
    void breathe() const { std::cout << "..." << '\n'; }
};

class Dog : public Animal {
public:
    void bark() const { std::cout << "Woof!" << '\n'; }
};

Dog d;
d.breathe();   // inherited from Animal
d.bark();      // Dog's own
```

---

## Table of Contents

1. [Why inheritance exists](#1-why-inheritance-exists)
2. [Syntax and the three access modes](#2-syntax-and-the-three-access-modes)
3. [`protected` — the middle ground](#3-protected--the-middle-ground)
4. [Construction and destruction order](#4-construction-and-destruction-order)
5. [Calling the parent's constructor](#5-calling-the-parents-constructor)
6. [Memory layout](#6-memory-layout)
7. [Upcasting and downcasting](#7-upcasting-and-downcasting)
8. [Name hiding](#8-name-hiding)
9. [Multiple inheritance](#9-multiple-inheritance)
10. [The diamond problem and virtual inheritance](#10-the-diamond-problem-and-virtual-inheritance)
11. [Composition vs inheritance](#11-composition-vs-inheritance)
12. [In CPP03 — ClapTrap family](#12-in-cpp03--claptrap-family)
13. [In CPP04 — polymorphic hierarchies](#13-in-cpp04--polymorphic-hierarchies)
14. [Rules of thumb](#14-rules-of-thumb)
15. [Gotchas](#15-gotchas)

---

## 1. Why inheritance exists

Two reasons:

1. **Code reuse.** The `Dog` doesn't re-implement `breathe()` — it inherits it.
2. **Polymorphism.** A `Dog*` can be treated as an `Animal*` anywhere the code wants an `Animal`. This is what lets you write code that handles "any kind of animal" without knowing about dogs and cats specifically. See [POLYMORPHISM.md](POLYMORPHISM.md).

The first is useful but *not enough* by itself (composition is often cleaner — see §11). The second is the real power.

---

## 2. Syntax and the three access modes

```cpp
class Base { /* ... */ };

class Derived : public    Base { /* ... */ };   // is-a  — 99% of the time
class Derived : protected Base { /* ... */ };   // rare
class Derived : private   Base { /* ... */ };   // "implemented-in-terms-of"
```

The access specifier on the inheritance line controls **what the outside world can see** through the derived class.

| Inheritance type | `public` in Base becomes… | `protected` in Base becomes… | `private` in Base becomes… |
|---|---|---|---|
| `public`    | `public`    | `protected` | not accessible |
| `protected` | `protected` | `protected` | not accessible |
| `private`   | `private`   | `private`   | not accessible |

**`private` members of the base are never accessible to the derived**, no matter the inheritance type. Only `protected` and `public` members come through.

**At 42**, you use `public` inheritance almost exclusively. The other two exist but rarely earn their keep.

---

## 3. `protected` — the middle ground

- **`private`** — only the class itself.
- **`public`** — anyone.
- **`protected`** — the class itself **and any derived class**.

```cpp
class Animal {
protected:
    std::string _type;     // derived classes can touch this
};

class Dog : public Animal {
public:
    Dog() { _type = "Dog"; }   // OK — _type is protected
};

Dog d;
d._type = "Cat";               // ERROR — outside the class, _type is hidden
```

`protected` is useful but leaky: any subclass can reach in and mess with state the base class was trying to protect. Prefer `private` + protected accessors unless the member is *designed* to be shared with subclasses.

---

## 4. Construction and destruction order

**Parent first up, child first down.** Always.

```
Dog d;  // construction

  1. Animal() runs       — the base sub-object is built
  2. Dog()    runs       — then Dog's own code

~Dog();  // destruction (whether explicit or automatic)

  1. ~Dog()    runs      — Dog's own code first
  2. ~Animal() runs      — then the base destructor
```

### Why this order?

Because the `Dog` constructor may rely on the `Animal` part being ready. (You can't use fields that aren't initialized yet.) And the `Animal` destructor can rely on `Dog`-specific state already being cleaned up.

### Multi-level chain

```cpp
class A { public: A() { std::cout << "A\n"; } ~A() { std::cout << "~A\n"; } };
class B : public A { public: B() { std::cout << "B\n"; } ~B() { std::cout << "~B\n"; } };
class C : public B { public: C() { std::cout << "C\n"; } ~C() { std::cout << "~C\n"; } };

C obj;
// Prints:  A  B  C                 — construction (top-down)
// Then:    ~C  ~B  ~A              — destruction (bottom-up)
```

---

## 5. Calling the parent's constructor

Base's default constructor runs automatically. To pass arguments to the base, use the initialization list:

```cpp
class Animal {
public:
    Animal(const std::string& t) : _type(t) {}
protected:
    std::string _type;
};

class Dog : public Animal {
public:
    Dog() : Animal("Dog") {}           // explicit base construction
    Dog(const std::string& name)
        : Animal("Dog"), _name(name) {}
private:
    std::string _name;
};
```

**If the base has no default constructor, the derived's init list MUST construct it explicitly.** Forgetting this is a compile error that says "no matching function for call to `Animal::Animal()`".

### Copy constructor and `operator=`

These are **not** inherited as-is — the compiler generates them for the derived class, but if you write your own, you must forward to the base explicitly:

```cpp
Dog::Dog(const Dog& other) : Animal(other) { /* copy Dog-specific fields */ }

Dog& Dog::operator=(const Dog& other) {
    if (this != &other) {
        Animal::operator=(other);        // call base's operator=
        _name = other._name;
    }
    return *this;
}
```

Forgetting to forward to the base means the base sub-object doesn't get copied — a classic bug.

---

## 6. Memory layout

```
Dog d;

Memory:
┌──────────────────────────────┐  ◄── starts here
│  Animal sub-object:          │
│    _type (std::string)       │
│    [vptr if virtual]         │
├──────────────────────────────┤
│  Dog-specific members:       │
│    _name (std::string)       │
└──────────────────────────────┘
```

Key property: a `Dog*` and an `Animal*` both point to the **same address** (the base sub-object is at offset 0). That's why upcasting is free — no pointer adjustment needed.

(Multiple inheritance breaks this slightly; the second base lives at a non-zero offset, and the cast has to shift the pointer.)

---

## 7. Upcasting and downcasting

### Upcasting — implicit and safe

From derived to base. Always works, no cast needed:

```cpp
Dog     d;
Animal* a = &d;       // implicit upcast
a->breathe();         // OK
```

This is the cornerstone of polymorphism: a function that takes `Animal*` works for any subclass.

### Downcasting — needs a cast, may fail

From base to derived. The compiler can't verify the actual type, so you must be explicit:

```cpp
Animal* a = ...;
Dog*    d = static_cast<Dog*>(a);        // you promise it's really a Dog — no check
Dog*    d = dynamic_cast<Dog*>(a);       // checked at runtime — NULL if not a Dog
```

Use `dynamic_cast` whenever you can't prove at compile time that `a` is a `Dog`. See [CASTS.md](CASTS.md).

### Object slicing — the silent bug

Upcasting through a **value** (not pointer/reference) **slices** the derived part off:

```cpp
Dog    d;
Animal a = d;         // copies ONLY the Animal sub-object; Dog-specific fields are lost
```

`a` is now a bare `Animal` — no Dog-ness. This is legal and silent. Always pass polymorphic hierarchies **by pointer or reference**, never by value.

---

## 8. Name hiding

If a derived class declares a name that also exists in the base, the **entire** base name is hidden — even overloads that take different arguments:

```cpp
class Base {
public:
    void f(int);
    void f(double);
};

class Derived : public Base {
public:
    void f(int);        // hides BOTH Base::f(int) AND Base::f(double)
};

Derived d;
d.f(3.14);              // ERROR — Base::f(double) is hidden
d.Base::f(3.14);        // OK — explicit
```

Fix: pull the name back in with `using`:
```cpp
class Derived : public Base {
public:
    using Base::f;      // bring all Base::f overloads into scope
    void f(int);        // add your own
};
```

---

## 9. Multiple inheritance

A class may inherit from more than one base:

```cpp
class Flier    { public: virtual void fly() = 0;  };
class Swimmer  { public: virtual void swim() = 0; };

class Duck : public Flier, public Swimmer {
public:
    void fly()  { /* ... */ }
    void swim() { /* ... */ }
};
```

Memory layout:

```
Duck d;
┌──────────────┐  ◄── &d as Duck* or Flier*
│ Flier part   │
├──────────────┤  ◄── &d as Swimmer*     (non-zero offset)
│ Swimmer part │
├──────────────┤
│ Duck part    │
└──────────────┘
```

Upcasting to `Swimmer*` **adjusts the pointer** — it now points at the middle of the object. `static_cast` and `dynamic_cast` handle this for you.

---

## 10. The diamond problem and virtual inheritance

```
     A
    / \
   B   C
    \ /
     D
```

If `B` and `C` both inherit from `A`, and `D` inherits from both, `D` has **two copies of `A`**. Accessing any `A` member from `D` is ambiguous.

```cpp
class A { public: int n; };
class B : public A {};
class C : public A {};
class D : public B, public C {};

D d;
d.n = 5;          // ERROR — ambiguous: B::A::n or C::A::n?
d.B::n = 5;       // works, but ugly
```

### Fix: virtual inheritance

Mark `B` and `C`'s inheritance from `A` as `virtual`:

```cpp
class A { public: int n; };
class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {};

D d;
d.n = 5;          // OK — single shared A sub-object
```

Now `D` contains **one** shared `A`. At 42, CPP04 ex03 ("Interesting idea") is a controlled taste of virtual inheritance. Outside that, you'll rarely write it.

---

## 11. Composition vs inheritance

Inheritance is not the default reuse mechanism — it's the mechanism for polymorphism. For "class X uses class Y", usually use **composition**:

```cpp
// Inheritance (is-a) — for polymorphism
class Dog : public Animal { /* ... */ };

// Composition (has-a) — for reuse
class Car {
    Engine _engine;             // Car HAS an Engine
    Wheel  _wheels[4];
public:
    void start() { _engine.ignite(); }
};
```

### When to pick which

| Question | Inheritance | Composition |
|---|---|---|
| Is X an Y? | ✅ | ❌ |
| Does X have an Y? | ❌ | ✅ |
| Will callers hold `Y*` and want virtual dispatch? | ✅ | ❌ |
| Do you just want to reuse Y's methods? | ❌ | ✅ |
| Can Y change over the object's life? | ❌ | ✅ (swap the member) |

Composition is looser, easier to test, and doesn't leak base internals into the derived class. Use inheritance only when you need is-a semantics and polymorphism.

---

## 12. In CPP03 — ClapTrap family

```
ClapTrap
   │
   ├── ScavTrap  (virtual inheritance of ClapTrap in ex03)
   │
   └── FragTrap  (virtual inheritance of ClapTrap in ex03)
         │
       DiamondTrap : ScavTrap, FragTrap   ← the diamond
```

- **ex00** — plain class, no inheritance. Builds `ClapTrap`.
- **ex01** — single inheritance. `ScavTrap : public ClapTrap`. Key lesson: construction and destruction chain, calling the base constructor with the right argument.
- **ex02** — `FragTrap : public ClapTrap`. Same, another sibling.
- **ex03** — `DiamondTrap : public ScavTrap, public FragTrap`. This is where you hit the diamond problem and need **virtual inheritance** on `ScavTrap`'s and `FragTrap`'s inheritance lines. Expected output traces the construction order precisely.

Watch out for the subtle rule: in virtual inheritance, the **most-derived class** (`DiamondTrap`) is responsible for initializing the virtual base (`ClapTrap`). Intermediate constructors (`ScavTrap`, `FragTrap`) can't do it.

---

## 13. In CPP04 — polymorphic hierarchies

- **ex00** — `Animal` base with `Dog`, `Cat` subclasses. Focus: virtual destructor, virtual `makeSound`.
- **ex01** — add `Brain` as a **composed** member (not inherited!). Deep-copy discipline (`Dog` owns a `Brain*`; copy ctor and `operator=` must clone).
- **ex02** — make `Animal` **abstract** (pure virtual `makeSound`). Cannot instantiate `Animal` directly — only `Dog` and `Cat`.
- **ex03** — the `AMateria` / `Character` / `MateriaSource` interface exercise. Interfaces = abstract classes with only pure-virtual methods.

This is where inheritance pays off: you write code that manipulates `Animal*` and the right methods run for `Dog` or `Cat` automatically. That's polymorphism. See [POLYMORPHISM.md](POLYMORPHISM.md).

---

## 14. Rules of thumb

- **Default to `public` inheritance.** `private` and `protected` inheritance are edge cases.
- **Every polymorphic base class has a `virtual` destructor.** Without it, `delete base_ptr` skips the derived destructor → leaks.
- **Pass polymorphic objects by pointer or reference**, never by value. (Prevents slicing.)
- **Prefer composition** unless you specifically need polymorphism.
- **Forward to the base in copy ctor and `operator=`** when you write them.
- **Multiple inheritance for interfaces only.** Inheriting from multiple concrete classes is where bugs live.
- **Don't use `protected` data members** unless the class is specifically designed to be extended. Use `protected` methods instead.

---

## 15. Gotchas

- **No virtual destructor on a polymorphic base** → `delete ptr` calls only the base destructor → derived resources leak. 42 checks this.
- **Object slicing** on pass-by-value. `void f(Animal a)` + `f(dog)` silently drops the Dog part.
- **Forgetting to forward** in copy ctor / `operator=` leaves the base sub-object untouched.
- **Name hiding** — one `f(int)` in the derived hides all overloads of `f` in the base.
- **Diamond inheritance without `virtual`** gives you two copies of the grandparent and ambiguous member access.
- **Calling a virtual function from a constructor or destructor** does NOT dispatch to the derived — during those, the object isn't fully a `Derived` yet (or isn't anymore). You get the base's version.
- **Private inheritance is not "is-a"** — it's "implemented-in-terms-of". Outside code cannot upcast `Derived*` to `Base*`.
- **Inherited default constructors are implicitly generated** but not inherited-with-arguments. You have to write each overload explicitly in the derived class.
