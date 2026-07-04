# `class` — A `struct` That Hides Its Insides By Default

> **TL;DR.** `class` and `struct` are *almost* the same in C++. The only differences: default access (`private` vs `public`) and default inheritance (`private` vs `public`). Everything else — members, methods, virtuals, layout, sizeof — is identical.

Related: [`STRUCT.md`](STRUCT.md) · [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) · [`THIS.md`](THIS.md) · [`BASICS.md`](../notions/fundamentals/BASICS.md#7.%20Classes%20%E2%80%94%20the%20core%20of%20C%2B%2B) · [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. The C-to-C++ leap

In C, `struct` glues data together; functions are separate.

```c
typedef struct s_point { int x, y; } t_point;
void   point_translate(t_point *p, int dx, int dy);
```

In C++, a `class` glues data **and** the operations that act on it into a single type:

```cpp
class Point {
    int _x, _y;
public:
    Point(int x, int y) : _x(x), _y(y) {}
    void translate(int dx, int dy) { _x += dx; _y += dy; }
    int  x() const { return _x; }
    int  y() const { return _y; }
};
```

The point is now **responsible for itself**. It carries its operations with it; access control prevents outsiders from poking at `_x` directly.

---

## 2. The anatomy of a class

```cpp
class MyClass {
private:
    int _data;                                          // member variable

public:
    MyClass();                                          // default ctor
    MyClass(int d);                                     // converting ctor
    MyClass(const MyClass& other);                      // copy ctor
    MyClass& operator=(const MyClass& other);           // copy assignment
    ~MyClass();                                         // destructor

    int  getData() const;                               // const member fn (getter)
    void setData(int d);                                // non-const member fn (setter)

    static int instances();                             // static member fn

private:
    static int _instanceCount;                          // static member variable
};
```

The C++ compiler will **synthesize** any of the four OCF members you don't declare (default ctor, copy ctor, copy assignment, dtor) — but only if it can. See [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md).

---

## 3. Memory layout — what `class` actually compiles to

A class has the **same layout as a C struct** in the simplest case. The compiler lays out members **in declaration order**, possibly inserting padding for alignment.

```cpp
class A {
    char  _c;       // 1 byte
    int   _i;       // 4 bytes, must align to 4 → 3 bytes of padding before
    char  _d;       // 1 byte
};                   // total: 12 bytes (1 + 3 pad + 4 + 1 + 3 tail pad)
```

```
   address →    0    1    2    3    4    5    6    7    8    9   10   11
              ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
              │_c  │ pd │ pd │ pd │_i.0│_i.1│_i.2│_i.3│_d  │ pd │ pd │ pd │
              └────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
              ↑                 ↑                       ↑
              │                 │                       └ tail padding so the
              │                 └ aligned to 4              size is a multiple
              └ start of object                             of the strictest
                                                            alignment (4)
```

Reorder for compactness:

```cpp
class B {
    int  _i;       // 4
    char _c;       // 1
    char _d;       // 1
};                  // total: 8 (4 + 1 + 1 + 2 tail pad)
```

`sizeof(class)` is whatever the layout adds up to — predictable and inspectable:

```cpp
std::cout << sizeof(A) << " " << sizeof(B);   // 12 8
```

### What about member functions?

Member functions are **not** stored in the object. They're plain functions in `.text` that take a hidden first parameter `this`:

```cpp
class Point {
    int _x, _y;
public:
    void translate(int dx, int dy);
};

// the compiler effectively emits:
// void Point__translate(Point *this, int dx, int dy);
```

So:

```
   sizeof(Point) == sizeof(int) * 2 == 8

   memory:                            code (.text):
   ┌─────────────────┐                Point::translate(this, dx, dy)
   │ _x   │   _y     │                Point::x(this)
   └─────────────────┘                Point::y(this)
   one Point object,                  one copy in the binary
   8 bytes total                      shared by all Point instances
```

This is part of why C++ can claim "zero-cost abstractions" — methods don't add per-instance bytes (unless they're [`virtual`](VIRTUAL.md) — see that file).

---

## 4. The hidden `this` pointer

Inside any non-static member function, `this` is a pointer to the object the function was called on:

```cpp
void Point::translate(int dx, int dy) {
    this->_x += dx;          // explicit
    _y += dy;                // implicit — same as this->_y
}
```

See [`THIS.md`](THIS.md) for the full breakdown.

---

## 5. Access control: `private`, `protected`, `public`

```cpp
class Point {
private:    int _x, _y;        // accessible only inside Point's own member fns
protected:                     // accessible in Point + subclasses
public:                        // accessible everywhere
    Point(int x, int y);
};
```

`class` defaults to `private`. `struct` defaults to `public`. That is the **only** real difference between them. See [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md).

Access control is enforced **at compile time** — there's no runtime check, and `_x`'s memory is just as writable as `x`'s. A determined caller with a pointer cast can poke at it; access control is for friendly enforcement of design, not security.

---

## 6. Constructor / destructor — the lifecycle hooks

Every class implicitly has a lifecycle:

```
   Point p(3, 4);   ─────►  constructor runs
                                _x ← 3
                                _y ← 4
   p.translate(1, 1);
   p.translate(2, 2);
                            ... use ...
   }                ─────►  destructor runs as p goes out of scope
```

```cpp
Point::Point(int x, int y) : _x(x), _y(y) {       // initializer list
    // body — runs after members initialized
}

Point::~Point() {
    // cleanup — close files, release memory, etc.
    // for trivial members like int there's nothing to do.
}
```

For RAII, see [`MEMORY.md`](../notions/fundamentals/MEMORY.md). For OCF, see [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md).

---

## 7. Forward declaration

You can tell the compiler "this class exists, details to follow" without including its full definition:

```cpp
class Point;             // forward declaration

void f(Point *p);        // OK — pointers/refs only need a forward decl
void g(Point  p);        // ERROR — full type needed for value parameters
```

This is gold for breaking circular includes and reducing compile time. Use it whenever a header only needs `T*` or `T&`.

---

## 8. Nested types

A class can contain another class, an enum, a typedef:

```cpp
class List {
public:
    class Iterator {              // nested class
        // ...
    };

    typedef int size_type;        // nested typedef
    enum { MAX = 1024 };          // nested enum
};

List::Iterator it;                // outside, qualify with List::
```

Nested types share the access control of where they're declared (public here).

---

## 9. Forward-declaring with the wrong key (`class` vs `struct`)

Either of these works:

```cpp
struct Foo;
class  Foo;                     // identical at the type-system level
```

But pick one and stick to it for clarity (linkers don't care; readers do).

---

## 10. The size of an empty class

```cpp
class Empty {};
std::cout << sizeof(Empty);     // 1 — NOT zero
```

Why? An object must have a unique address. If `sizeof(Empty)` were zero, two `Empty` objects could share an address — confusing things like array indexing. The standard mandates *at least* 1 byte.

```
   Empty array[3];
   address: 0x100   0x101   0x102
            ┌─────┬─────┬─────┐
            │  ?  │  ?  │  ?  │   ← 1 byte each, no overlap
            └─────┴─────┴─────┘
```

When an empty class is a **base class** of a non-empty derived class, the compiler may apply the **empty base optimization** — no extra byte for the base.

---

## 11. Tips & tricks

### 11.1 The 42 underscore convention

42 norm prefixes private members with `_`:

```cpp
class C {
private:
    int _value;             // not 'value', not 'm_value', not 'value_'
};
```

The point: when you read code like `_value = x;`, you immediately know you're touching a private member.

### 11.2 Header layout

Standard 42 layout:

```cpp
#ifndef MYCLASS_HPP
# define MYCLASS_HPP

# include <string>           // include what you use

class MyClass {
private:
    std::string _name;
    int         _value;

public:
    MyClass();
    MyClass(const std::string& name, int value);
    MyClass(const MyClass& other);
    MyClass& operator=(const MyClass& other);
    ~MyClass();

    const std::string& getName() const;
    int                getValue() const;
    void               setName(const std::string& name);
    void               setValue(int value);
};

#endif
```

### 11.3 Don't mix declaration and definition for non-trivial functions

```cpp
// good
class C {
public:
    void heavy_function() const;     // declaration only
};
// ... in C.cpp: void C::heavy_function() const { ... }

// avoid
class C {
public:
    void heavy_function() const {    // body in header — implicitly inline
        // 50 lines of logic
    }
};
```

Implicit inline means every TU including the header recompiles the body.

### 11.4 Order members by alignment for size

For pure data classes, sorting members from largest alignment to smallest minimizes padding:

```cpp
struct Bad  { char a; double d; char b; };       // 24 bytes
struct Good { double d; char a; char b; };       // 16 bytes
```

For 42-level code this rarely matters. For tight loops over millions of objects, it matters a lot.

### 11.5 Pimpl idiom — hide the implementation

If you want the header to expose only the public interface and hide the data members entirely:

```cpp
// MyClass.hpp
class MyClass {
public:
    MyClass();
    ~MyClass();
    void doStuff();
private:
    class Impl;                   // forward decl
    Impl *_impl;                  // pointer to hidden type
};
```

```cpp
// MyClass.cpp
class MyClass::Impl {
    int _bigInternalState;
    // ...
};

MyClass::MyClass() : _impl(new Impl()) {}
MyClass::~MyClass() { delete _impl; }
```

Pros: the header doesn't include heavy implementation headers; ABI-stable. Cons: extra heap allocation per object. Not 42-norm-mandated, but a great pattern to know.

---

## 12. `class` vs `typedef struct` (C → C++)

```c
// C
typedef struct s_node {
    int data;
    struct s_node *next;
} t_node;
```

```cpp
// C++
class Node {
public:
    int   data;
    Node *next;
};
```

In C++ you don't need `typedef`; the class name is automatically a type. You also don't need to write `struct Node` — just `Node`.

---

## 13. Common errors

| Error | Cause | Fix |
|---|---|---|
| `private member 'X::_y' is inaccessible` | Touching a private member from outside | Add a getter/setter, or make the accessor a `friend` |
| `class X has no member named 'foo'` | Typo or member declared in wrong section | Check spelling and access level |
| `field has incomplete type` | Used a forward-declared type by value | Include the full definition, or store a pointer/reference |
| `unresolved external symbol Class::method` | Declared but never defined | Provide the body in the `.cpp` |

---

## 14. Visual summary

```
                ┌────────────────────────────────────┐
                │   class C { … };                   │
                └─────────────────┬──────────────────┘
                                  │
              ┌───────────────────┼───────────────────┐
              ▼                   ▼                   ▼
         data members         member functions    access control
         ─────────────        ────────────────    ──────────────
         live in each         live in .text       enforced at compile
         object's memory      shared by all       time. private by
         (with padding        instances; receive  default. private/
          for alignment)      'this' as hidden    protected/public
                              first parameter.
              │
              ▼
         sizeof(C) =
         sum of members
         + padding
         + (8 bytes for vtable
            ptr if any virtuals)
```

---

## 15. Practice

1. What's `sizeof(C)` for `class C { char x; };`? *(1 byte. No padding needed.)*
2. What changes if you add `int y;` after `x`? *(8 bytes — 1 for x, 3 padding, 4 for y.)*
3. What happens if you forward-declare a class and pass it by value? *(Compile error — the compiler needs the size.)*
4. Why does `sizeof(empty class) >= 1`? *(So distinct objects have distinct addresses.)*
