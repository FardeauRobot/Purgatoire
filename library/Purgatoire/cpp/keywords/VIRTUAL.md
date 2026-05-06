# `virtual` — The Keyword That Makes Polymorphism Work

> **TL;DR.** `virtual` enables **dynamic dispatch**: the function called depends on the object's runtime type, not the variable's static type. The mechanism is the **vtable** — a per-class array of function pointers, with each polymorphic object carrying a hidden vptr.

Related: [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) · [`INHERITANCE.md`](../oop/INHERITANCE.md) · [`CASTS.md`](../advanced/CASTS.md) · [`CPP04.md`](../modules/CPP04.md)

---

## 1. The problem `virtual` solves

```cpp
class Animal {
public:
    void sound() const { std::cout << "generic noise\n"; }
};

class Dog : public Animal {
public:
    void sound() const { std::cout << "woof\n"; }      // hides Animal::sound
};

void make(const Animal& a) { a.sound(); }              // what does this call?

Dog d;
make(d);            // prints "generic noise" — WRONG
```

Without `virtual`, the call `a.sound()` is dispatched **based on the static type of `a`** — which is `Animal&`. The compiler emits a direct call to `Animal::sound`. Even though the actual object is a `Dog`, the override is ignored.

Add one keyword:

```cpp
class Animal {
public:
    virtual void sound() const { std::cout << "generic noise\n"; }
};

Dog d;
make(d);            // prints "woof" — CORRECT
```

Now the call dispatches **at runtime** based on the dynamic type of `*a`.

---

## 2. The mechanism — vtable + vptr

### 2.1 Per-class vtable

For every class with at least one virtual function, the compiler builds a **vtable** — an array of function pointers, one per virtual function:

```
   Animal's vtable:
   ┌────────────────────────────────────┐
   │  [0] &Animal::sound                │
   │  [1] &Animal::~Animal              │
   └────────────────────────────────────┘

   Dog's vtable (inherits from Animal):
   ┌────────────────────────────────────┐
   │  [0] &Dog::sound          ← overrides
   │  [1] &Dog::~Dog           ← overrides
   └────────────────────────────────────┘
```

The slot order is the same in derived and base — that's how dispatch can be O(1).

### 2.2 Per-object vptr

Each object of a polymorphic class carries a hidden pointer to its class's vtable. Usually it's the **first 8 bytes** (on 64-bit) of the object:

```
   Animal a;                  Dog d;
   ┌──────────────────┐       ┌──────────────────────────┐
   │ vptr → Animal vt │       │ vptr → Dog vt            │
   ├──────────────────┤       ├──────────────────────────┤
   │ (Animal members) │       │ (Animal members)         │
   └──────────────────┘       │ (Dog members)            │
                              └──────────────────────────┘
```

```cpp
sizeof(Animal);        // 8 (vptr) + members
sizeof(Dog);           // 8 (vptr) + Animal members + Dog members
```

A class without any virtuals has **no vptr** — `sizeof` reflects only the data members.

### 2.3 The dispatch sequence

```cpp
Animal *p = new Dog();
p->sound();
```

What the compiler emits, conceptually:

```asm
   mov rax, [p]            ; load object pointer
   mov rbx, [rax]          ; load vptr (first 8 bytes of object)
   mov rcx, [rbx + 0]      ; load slot 0 of vtable (sound)
   call rcx
```

```
   p              object       vtable           function
   ───            ──────       ──────           ────────
   ptr ──► [vptr | ...] ──► [sound, ~Dog] ──► Dog::sound  ── runs
```

Three pointer loads + one indirect call. In micro-benchmarks this is a few cycles slower than a direct call. In real code, the difference is usually drowned out by everything else (cache misses, branch prediction, the actual work).

---

## 3. Constructor sets the vptr — that's why virtuals don't dispatch in ctors

When you `new Dog()`:

```
   1. Memory is allocated.
   2. Animal's constructor runs. The vptr is set to Animal's vtable.
      ← if you call a virtual now, it dispatches to Animal's version
   3. Dog's constructor runs. The vptr is updated to Dog's vtable.
      ← from this point on, virtual calls dispatch to Dog's overrides
   4. Construction complete.
```

So inside `Animal`'s constructor, `this->sound()` goes to `Animal::sound`, **not** `Dog::sound`. The Dog isn't constructed yet — calling its method would access uninitialized Dog state.

Same in destructors, in reverse order: as `~Dog` finishes and `~Animal` starts, the vptr is reset to `Animal`'s — virtual calls go to `Animal`.

Rule: **never call virtuals from constructors or destructors.**

---

## 4. The pure virtual — abstract classes

```cpp
class Shape {
public:
    virtual double area() const = 0;     // pure virtual: no implementation here
    virtual ~Shape() {}                  // virtual dtor — important!
};
```

The `= 0` makes `Shape` an **abstract class**. You cannot instantiate it directly:

```cpp
Shape s;             // ERROR — cannot allocate an object of abstract type
Shape *p = ...;      // OK — pointer to abstract is fine
Shape& r = ...;      // OK
```

Subclasses must override the pure virtual to become concrete:

```cpp
class Circle : public Shape {
    double _r;
public:
    Circle(double r) : _r(r) {}
    double area() const { return 3.14159 * _r * _r; }    // override
};

Circle c(5);          // OK — Circle is concrete
Shape *p = new Circle(5);
p->area();            // dispatched to Circle::area
delete p;             // calls Shape::~Shape virtually → Circle::~Circle first
```

A pure virtual *can* have an implementation:

```cpp
class Shape {
public:
    virtual double area() const = 0;
};
double Shape::area() const { return 0; }    // legal — but still pure virtual

class FlatShape : public Shape {
public:
    double area() const { return Shape::area(); }    // can call the base impl
};
```

Rare but useful for shared default behavior.

---

## 5. Virtual destructor — non-negotiable for polymorphic bases

```cpp
class Base {
public:
    ~Base() {}        // NOT virtual
};

class Derived : public Base {
    int *_data;
public:
    Derived() : _data(new int[100]) {}
    ~Derived() { delete[] _data; }
};

Base *p = new Derived();
delete p;             // calls only Base::~Base — Derived::~Derived skipped
                      // _data leaks. UB strictly.
```

Always make destructors virtual in any class meant to be inherited from polymorphically:

```cpp
class Base {
public:
    virtual ~Base() {}
};

delete p;             // dispatches via vtable: ~Derived runs, then ~Base. OK.
```

If a class has any virtual function, give it a virtual destructor. The cost is the same vptr you already have.

---

## 6. The `override` and `final` keywords (C++11+, not in 42 C++98)

C++11 added these for safety. Not available in 42's C++98 mode, but worth knowing for the future:

```cpp
class Dog : public Animal {
public:
    void sound() const override;          // compiler verifies it actually overrides
    void sound() final;                   // no further overrides allowed in subclasses
};
```

In C++98 you don't get the typo-detection. Be careful with signatures (a stray `const` mismatch creates a *new* virtual instead of overriding).

---

## 7. The static-vs-dynamic dispatch decision

```cpp
Animal a;
Dog d;
Animal *p = &d;
Animal &r = d;
Animal v = d;            // SLICED — copies only the Animal part

a.sound();          // static: Animal::sound (a is Animal)
d.sound();          // static: Dog::sound (compile-time resolves to derived)
p->sound();         // dynamic: Dog::sound (vtable lookup)
r.sound();          // dynamic: Dog::sound (vtable lookup)
v.sound();          // static: Animal::sound (v is a sliced Animal)
```

**Polymorphism only happens through pointers and references.** Pass-by-value of a base type slices off the derived part — the vptr is overwritten with the base's vptr during the copy, and dispatch resolves to the base.

```
   Dog d;                 Animal v = d;
   ┌──────────────┐       ┌────────────────┐
   │ vptr → Dog   │       │ vptr → Animal  │  ← overwritten during copy
   │ animalState  │       │ animalState    │
   │ dogState     │       └────────────────┘
   └──────────────┘       (dogState dropped)
```

This is **slicing**. Almost always a bug. Pass polymorphic objects by `*` or `&`.

---

## 8. Multiple virtuals — vtable layout

```cpp
class Shape {
public:
    virtual void draw() const;
    virtual double area() const;
    virtual void scale(double f);
    virtual ~Shape();
};

class Circle : public Shape { /* overrides all three */ };
```

```
   Shape's vtable:                      Circle's vtable:
   ┌─────────────────────┐              ┌─────────────────────┐
   │ [0] Shape::draw     │              │ [0] Circle::draw    │
   │ [1] Shape::area     │              │ [1] Circle::area    │
   │ [2] Shape::scale    │              │ [2] Circle::scale   │
   │ [3] Shape::~Shape   │              │ [3] Circle::~Circle │
   └─────────────────────┘              └─────────────────────┘
```

The slot order is preserved across the hierarchy — that's how a `Shape*` can do an O(1) lookup regardless of the actual derived class.

---

## 9. Multiple inheritance and the diamond — vtables get fancy

When a class inherits from two bases each with their own vtables, it has **multiple vptrs** — one per base subobject. Dispatch becomes more complex (thunks, "this-adjustments"). The diamond problem (a class indirectly inheriting from the same base twice) brings in `virtual` inheritance — a different use of the keyword. See [`INHERITANCE.md`](../oop/INHERITANCE.md).

For 42 single-inheritance hierarchies, you don't have to worry about this. But know that "virtual" has a *second* meaning when applied to a base class:

```cpp
class A {};
class B : virtual public A {};       // 'virtual' inheritance: A is shared in diamonds
class C : virtual public A {};
class D : public B, public C {};     // only ONE A in D, not two
```

This is a different feature from virtual functions, despite the shared keyword.

---

## 10. Tips & tricks

### 10.1 The `virtual` keyword is needed once — at the base

```cpp
class A {
public: virtual void f();
};
class B : public A {
public: void f();              // automatically virtual — override
};
class C : public B {
public: void f();              // automatically virtual — override
};
```

You can repeat `virtual` on overrides for clarity, and it's harmless. In C++11+, `override` makes intent obvious.

### 10.2 Don't make every function virtual

`virtual` has costs:
- The vtable + vptr.
- Indirect call (cannot be inlined unless the compiler proves the dynamic type).
- Cache miss on first vtable load.

Make a function virtual when subclasses are *expected* to override it. Otherwise, leave it concrete.

### 10.3 Calling the base version

```cpp
class Dog : public Animal {
public:
    void sound() const {
        Animal::sound();             // explicit base call
        std::cout << " — and woof!\n";
    }
};
```

Useful for layering behavior (constructor chains, augmenting a base implementation).

### 10.4 Pure virtual destructor — the unusual case

```cpp
class Base {
public:
    virtual ~Base() = 0;          // pure virtual destructor
};
Base::~Base() {}                  // STILL needs a body!
```

Why a body? Because the derived destructor will call the base destructor, which must exist. `= 0` only marks Base as abstract; the body is still needed.

This is the standard trick to make a class abstract without giving it any abstract member functions.

### 10.5 Watch out for signature mismatches

```cpp
class Animal {
public:
    virtual void sound() const;
};

class Dog : public Animal {
public:
    void sound();                 // NOT an override — missing const!
};                                 // creates a new virtual function in Dog,
                                   // doesn't override Animal::sound
```

In C++11 use `override`; in C++98 just be careful. A typo'd signature is a silent bug.

### 10.6 Inspect a vtable

You can't *standardly*, but tools help:

```bash
$ g++ -fdump-class-hierarchy a.cpp
```

This dumps the layout including vtable contents. Use it to learn what your compiler actually does.

---

## 11. Common errors

| Error | Cause | Fix |
|---|---|---|
| Slicing — derived methods not called | Passed/copied by value | Pass by reference or pointer |
| Memory leak — derived dtor skipped | Base destructor not virtual | Make `~Base()` virtual |
| `cannot allocate object of abstract class type` | Tried to instantiate a class with unimplemented pure virtuals | Override every pure virtual, or use a concrete subclass |
| `pure virtual method called` (runtime) | Called a virtual from a constructor/destructor | Don't call virtuals from ctor/dtor |
| Subtle no-override: derived "override" doesn't fire | Signature mismatch (const, parameters, return type) | Compare base/derived declarations carefully |

---

## 12. Visual summary

```
                  ┌──────────────────────────────────────┐
                  │   keyword: virtual                    │
                  └─────────────────┬────────────────────┘
                                    │
            ┌───────────────────────┼───────────────────────┐
            ▼                       ▼                       ▼
       virtual function        pure virtual           virtual base
       (dynamic dispatch)      (= 0, abstract)        (diamond resolution
                                                       in multiple inheritance)
            │                       │                       │
            ▼                       ▼                       ▼
       vtable + vptr           class is abstract:      shared base subobject
       per-class table         cannot instantiate;     in multi-inheritance
       per-object pointer      subclasses must         hierarchies. one of A,
                              implement to be          not two.
                              concrete.

    Object memory:
       ┌──────────────────────────────┐
       │  vptr  ──► class vtable      │  ← 8 bytes overhead per polymorphic object
       │  data members (in order)     │
       └──────────────────────────────┘

    Always:
       virtual base destructor when designing for inheritance.
       no virtual calls from ctor/dtor.
       pass polymorphic objects by ref/ptr (no slicing).
```

---

## 13. Practice

1. Why does `Animal a = dog;` print "generic noise" even when `Dog::sound` is virtual? *(Slicing — `a` is a fresh Animal object with Animal's vptr.)*
2. What's the cost of one virtual function call vs. a non-virtual one? *(One extra memory load (vtable lookup) + an indirect call. Usually negligible; can prevent inlining.)*
3. Why must a polymorphic base class have a virtual destructor? *(`delete base_ptr` would otherwise skip the derived destructor → leak/UB.)*
4. Why doesn't a virtual call from a constructor dispatch to the derived class? *(The vptr is set to the base class's table during base construction; the derived part hasn't been built yet.)*
