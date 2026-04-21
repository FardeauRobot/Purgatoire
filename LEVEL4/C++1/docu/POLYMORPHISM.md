# Polymorphism & Virtual Functions — Complete Guide

Polymorphism means **"one interface, many behaviors."** You write code that manipulates `Animal*` — and at runtime it calls `Dog::makeSound()` if the actual object is a `Dog`, `Cat::makeSound()` if it's a `Cat`. The same call site produces different results depending on the real type behind the base pointer.

```cpp
Animal* zoo[3];
zoo[0] = new Dog();
zoo[1] = new Cat();
zoo[2] = new Dog();

for (int i = 0; i < 3; ++i)
    zoo[i]->makeSound();    // "Woof!" "Meow!" "Woof!"
```

This only works because `makeSound` is **virtual**.

---

## Table of Contents

1. [Two kinds of polymorphism](#1-two-kinds-of-polymorphism)
2. [The `virtual` keyword](#2-the-virtual-keyword)
3. [Static vs dynamic dispatch](#3-static-vs-dynamic-dispatch)
4. [The vtable — how it works in memory](#4-the-vtable--how-it-works-in-memory)
5. [Pure virtual functions and abstract classes](#5-pure-virtual-functions-and-abstract-classes)
6. [Interfaces in C++](#6-interfaces-in-c)
7. [The virtual destructor rule](#7-the-virtual-destructor-rule)
8. [Object slicing — the silent killer](#8-object-slicing--the-silent-killer)
9. [Calling virtuals from ctors/dtors](#9-calling-virtuals-from-ctorsdtors)
10. [The cost of virtual](#10-the-cost-of-virtual)
11. [In CPP04](#11-in-cpp04)
12. [Rules of thumb](#12-rules-of-thumb)
13. [Gotchas](#13-gotchas)

---

## 1. Two kinds of polymorphism

C++ has two:

- **Ad-hoc (compile-time) polymorphism** — different behavior selected at compile time. Function overloading, operator overloading, templates.
  ```cpp
  void print(int);
  void print(const std::string&);
  print(42);      // calls the int overload — compiler picks
  ```
- **Subtype (runtime) polymorphism** — different behavior selected at **runtime**, based on the actual object type. This requires `virtual` functions.
  ```cpp
  Animal* a = new Dog();
  a->makeSound();   // dispatched at runtime to Dog::makeSound
  ```

This guide is about the second kind. For templates (ad-hoc), see [TEMPLATES.md](TEMPLATES.md).

---

## 2. The `virtual` keyword

A method marked `virtual` **can be overridden** by a subclass, and calls through a base pointer/reference will dispatch to the derived version.

```cpp
class Animal {
public:
    virtual void makeSound() const { std::cout << "..." << '\n'; }
    virtual ~Animal() {}
};

class Dog : public Animal {
public:
    void makeSound() const { std::cout << "Woof!" << '\n'; }   // override
};
```

Rules:
- The subclass's method must have **the same signature** (same return type, same parameters, same const-ness). Otherwise you're declaring a *different* method that hides the base one.
- You can write `virtual` again on the override — optional in C++98, useful as a reminder.
- C++11 adds the `override` keyword for compiler-verified overrides. You don't have it in C++98.

### Without `virtual`

The base method is called, even through a derived pointer:

```cpp
class Animal { public: void makeSound() { std::cout << "...\n"; } };
class Dog : public Animal { public: void makeSound() { std::cout << "Woof!\n"; } };

Dog d;
Animal* a = &d;
a->makeSound();        // "..." — the base version, NOT Dog's
d.makeSound();         // "Woof!" — called directly on Dog
```

Without virtual: **static dispatch** — the pointer's declared type decides.
With virtual: **dynamic dispatch** — the actual object's type decides.

---

## 3. Static vs dynamic dispatch

**Static dispatch** is what you get in C: the compiler looks at the type on the page and hardcodes the function address.

```
Dog d;
d.bark();
  │
  └─► compiler emits direct call to Dog::bark — address known at compile time
```

**Dynamic dispatch** adds one level of indirection. The compiler looks up the actual method at runtime via a table that lives on the object.

```
Animal* a = new Dog();
a->makeSound();
  │
  ├─► load a->vptr
  ├─► load [vptr + offset_for_makeSound]    ← the vtable slot for this method
  └─► indirect call to that address         ← Dog::makeSound
```

Three loads + indirect call vs. a hardcoded call. That's the cost; it's small but not zero.

---

## 4. The vtable — how it works in memory

For every class that has any virtual function, the compiler builds a **vtable** — an array of function pointers, one slot per virtual method. Each object of that class gets a hidden **vptr** member that points at its class's vtable.

```
class Animal {
    virtual void makeSound() const;
    virtual ~Animal();
};
class Dog : public Animal { void makeSound() const; };
class Cat : public Animal { void makeSound() const; };

         Dog's vtable                 Cat's vtable
       ┌──────────────────────┐     ┌──────────────────────┐
       │ &Dog::makeSound      │     │ &Cat::makeSound      │
       │ &Dog::~Dog           │     │ &Cat::~Cat           │
       └──────────────────────┘     └──────────────────────┘

Dog d;                                Cat c;
┌─────────────────┐                  ┌─────────────────┐
│ vptr ──► Dog's  │                  │ vptr ──► Cat's  │
│ vtable          │                  │ vtable          │
├─────────────────┤                  ├─────────────────┤
│ _type ...       │                  │ _type ...       │
└─────────────────┘                  └─────────────────┘
```

When you write `a->makeSound()` where `a` is `Animal*`:
1. The compiler knows `makeSound` is slot #0 of `Animal`'s vtable.
2. It loads `a->vptr` — whichever vtable the actual object has.
3. It loads slot #0 from that vtable — gets `Dog::makeSound` if it's a Dog.
4. It calls that address.

The vptr is set by the **constructor** of each class in the hierarchy. During `Animal`'s constructor, the vptr points at `Animal`'s vtable. Then `Dog`'s constructor runs and updates the vptr to `Dog`'s vtable. That's why calling virtuals from a base constructor gets the base version (§9).

### How big is the object?

One pointer bigger than without virtual. On a 64-bit machine, that's 8 bytes of vptr overhead per object — shared across *all* virtual functions, no matter how many.

---

## 5. Pure virtual functions and abstract classes

A **pure virtual** function has no body in the base class. Syntax: `= 0` at the end of the declaration.

```cpp
class Shape {
public:
    virtual ~Shape() {}
    virtual double area() const = 0;        // pure virtual — no implementation
};
```

A class with any pure virtual is **abstract**:

- You cannot instantiate it. `Shape s;` is a compile error.
- You can still have `Shape*` and `Shape&`.
- Subclasses **must** override all pure virtuals to become concrete.

```cpp
class Circle : public Shape {
    double _r;
public:
    Circle(double r) : _r(r) {}
    double area() const { return 3.14159 * _r * _r; }   // provides the missing piece
};

Shape* s = new Circle(5);                    // OK
std::cout << s->area();                       // 78.5...
delete s;
```

### Why abstract classes exist

To say: "this category of thing *must* have this operation, but I can't provide a default that makes sense." Every `Shape` has an `area`, but there's no reasonable default — so you force every derived class to define its own.

---

## 6. Interfaces in C++

C++ doesn't have a keyword `interface`. Instead, an **interface** is just an abstract class with:

- Only pure virtual functions
- A virtual destructor
- No data members

```cpp
class IClickable {
public:
    virtual ~IClickable() {}
    virtual void onClick() = 0;
    virtual bool isEnabled() const = 0;
};
```

Any class that `public`-inherits `IClickable` and implements both methods **is a** `IClickable`. That's the C++ idiom for Java/C# interfaces.

Classes can inherit from **multiple** interfaces without hitting the usual multiple-inheritance problems, because interfaces have no data to conflict over.

---

## 7. The virtual destructor rule

**If anyone might `delete` your object through a base pointer, your base destructor MUST be virtual.**

```cpp
class Animal {
public:
    /* virtual */ ~Animal() {}
};
class Dog : public Animal {
    int* _brain;
public:
    Dog() : _brain(new int[100]) {}
    ~Dog() { delete[] _brain; }
};

Animal* a = new Dog();
delete a;
```

- **Without `virtual` on `~Animal()`:** only `~Animal()` runs. `_brain` leaks. This is Undefined Behavior per the standard.
- **With `virtual` on `~Animal()`:** `~Dog()` runs first (freeing `_brain`), then `~Animal()` runs.

The rule at 42: **every base class has `virtual ~ClassName();`**. Always. Even if you think nobody will delete through a base pointer today, someone will tomorrow, and the cost is one vptr you already have for the other virtuals.

---

## 8. Object slicing — the silent killer

Assigning a derived **by value** into a base copies only the base sub-object. The derived part is silently dropped.

```cpp
Dog    d;
Animal a = d;       // SLICE: only the Animal sub-object copies
a.makeSound();      // base version — there's no more Dog here
```

Same thing with function arguments:

```cpp
void treat(Animal a);         // by value — slices anything passed in
treat(d);                     // Dog's -ness is gone the moment we enter the function

void treat(const Animal& a);  // by reference — safe, a really is a Dog
treat(d);                     // calls Dog's virtuals correctly
```

**Rule: pass polymorphic objects by pointer or reference.** Never by value.

Containers have the same problem: `std::vector<Animal>` slices. If you need a container of mixed animals, it has to be `std::vector<Animal*>`.

---

## 9. Calling virtuals from ctors/dtors

**During a constructor, the object is not yet "Dog" — it's still being assembled.** The `Dog` base runs first, so the vptr points at `Animal`'s vtable while `Animal`'s constructor runs:

```cpp
class Animal {
public:
    Animal()          { init(); }      // calls whose init()?
    virtual void init() { std::cout << "Animal::init\n"; }
};
class Dog : public Animal {
public:
    void init() { std::cout << "Dog::init\n"; }
};

Dog d;    // prints "Animal::init" — NOT "Dog::init"
```

The same happens in reverse during destruction: when `~Animal()` runs, the `Dog` part is already gone, so virtual calls resolve to `Animal`'s version.

**Don't call virtuals from constructors or destructors** unless you understand this. The safer pattern is to construct the object fully, then call the virtual explicitly.

---

## 10. The cost of virtual

- **Memory:** one extra pointer per object (the vptr), one vtable per class.
- **Time:** two extra memory loads and an indirect call per virtual invocation. Modern CPUs predict these fairly well, but an indirect call defeats inlining.
- **Binary size:** slightly larger — all vtables are emitted into the executable.

For normal application code, the cost is irrelevant. For tight inner loops, virtual dispatch can matter. The C++ design principle — "you don't pay for what you don't use" — means non-virtual functions are as cheap as C calls; only classes with virtuals pay the vtable price.

---

## 11. In CPP04

This module is the one where polymorphism finally clicks.

- **ex00 — `Animal` base, `Dog`/`Cat` subclasses, and a `WrongAnimal`.**
  Compare behavior with and without `virtual makeSound()`. The `WrongAnimal` hierarchy exists to show what happens when you forget `virtual`: the base's method gets called through a `WrongAnimal*`, even though it's actually a `WrongCat`.

- **ex01 — `Brain` as a composed member.**
  `Dog` and `Cat` each own a `Brain*`. This is where deep-copy discipline matters: the copy constructor must clone the `Brain`; `operator=` must handle self-assignment and free the old brain. A `std::vector<Animal*>` of mixed `Dog`s and `Cat`s, then copy the vector — everything must survive.

- **ex02 — make `Animal` abstract.**
  Turn `Animal::makeSound` into `= 0`. Now `Animal a;` is a compile error, but `Animal* a = new Dog();` still works — and is the only way to manipulate one. This enforces "you can't have a *generic* Animal; pick a kind."

- **ex03 — the `IMateria` / `AMateria` / `Character` / `MateriaSource` interface exercise.**
  A proper C++ interface hierarchy. `IMateria` (or equivalent) is an abstract class with only pure virtuals; `AMateria` is a partial implementation; `Ice` and `Cure` are concrete. `Character` holds `AMateria*[4]`. You wield a polymorphic array without knowing the concrete type — which is the whole point.

By the end, you've seen: virtual functions, vtables' effect, abstract classes, interfaces, polymorphic arrays, and deep-copy under a polymorphic hierarchy.

---

## 12. Rules of thumb

- **Virtual destructor on every base class that will be deleted polymorphically.** Always.
- **If you add a virtual function, ask: do I already have a virtual destructor?** Usually yes if there's any virtual at all, but double-check.
- **Override signatures must match exactly** (name, params, const-ness). If not, you're adding a new method, not overriding.
- **Abstract classes for "must implement" contracts.** Don't provide a lame default for something every subclass should customize.
- **Interfaces = abstract classes with only pure virtuals.** Keep them tight.
- **Pass polymorphic objects by pointer or reference.** Never by value.
- **Don't call virtual functions from constructors or destructors.**

---

## 13. Gotchas

- **Forgetting `virtual` on the destructor** → `delete` through base pointer leaks resources.
- **Override signature mismatch** (e.g. `const` on the override but not the base, or vice-versa) → silently creates a new non-overriding method. The base's version still gets called.
- **Slicing** on pass-by-value or container-of-value. `std::list<Animal>` slices; `std::list<Animal*>` doesn't.
- **Virtuals in constructors/destructors don't dispatch to derived.**
- **Pure virtuals still need a definition if you want to call them via `Base::foo()` from an override.** Rare but legal.
- **Multiple inheritance of concrete classes is a minefield.** Interfaces (pure-virtual-only) are fine.
- **`dynamic_cast` requires a polymorphic base** (at least one virtual). See [CASTS.md](CASTS.md).
- **Static vs dynamic confusion.** `Animal* a = new Dog; typeid(*a) == typeid(Dog)` is true; `typeid(a) == typeid(Animal*)` is also true. `*a` is dynamic; `a` is static.
