# The Orthodox Canonical Form (OCF) — Complete Guide

## Table of Contents
1. [What is the Orthodox Canonical Form?](#what-is-the-orthodox-canonical-form)
2. [Why does it exist?](#why-does-it-exist)
3. [The Four Mandatory Members](#the-four-mandatory-members)
   - [1. Default Constructor](#1-default-constructor)
   - [2. Copy Constructor](#2-copy-constructor)
   - [3. Copy Assignment Operator](#3-copy-assignment-operator)
   - [4. Destructor](#4-destructor)
4. [The Rule of Three (and Five, and Zero)](#the-rule-of-three-and-five-and-zero)
5. [Deep Copy vs Shallow Copy](#deep-copy-vs-shallow-copy)
6. [Common Pitfalls](#common-pitfalls)
7. [Full Examples](#full-examples)

---

## What is the Orthodox Canonical Form?

The **Orthodox Canonical Form** (OCF), sometimes called the **Coplien Form** (after James Coplien who described it in *Advanced C++ Programming Styles and Idioms*, 1992), is a coding convention in C++ that requires every class to **explicitly define four specific member functions**:

1. A **default constructor**
2. A **copy constructor**
3. A **copy assignment operator** (`operator=`)
4. A **destructor**

This convention is **strictly enforced at 42 School** (and in many professional C++98 codebases) for all non-trivial classes. It ensures that you, the programmer, are **always in control** of how your objects are created, copied, assigned, and destroyed.

> **In short**: the OCF is the contract you sign with the compiler — "I know exactly how my object behaves during its entire lifecycle."

---

## Why does it exist?

When you write a class in C++ without defining these functions, the **compiler generates them implicitly**. That sounds convenient, but it's dangerous:

- The compiler-generated copy constructor performs a **shallow copy** (member-by-member). If your class manages a resource (heap memory, file descriptor, socket, mutex...), this leads to **double frees**, **dangling pointers**, and **undefined behavior**.
- Implicit functions are silent — you can't tell at a glance what your class does when copied or destroyed.
- Without an explicit destructor, resources you allocated (e.g., with `new`) will **leak**.

Defining all four functions explicitly makes your class:
- **Predictable**: behavior is documented in code.
- **Safe**: no silent shallow copies of owned resources.
- **Maintainable**: future readers immediately see how the lifecycle is handled.

---

## The Four Mandatory Members

Let's take a class `Foo` as our running example.

### 1. Default Constructor

**Signature:**
```cpp
Foo();
```

**What it does:**
The default constructor is called when you create an object **without arguments**. It initializes the object's members to a valid, defined state.

**When is it called?**
- `Foo a;` — explicit declaration without arguments.
- `Foo arr[10];` — when an array of objects is created.
- When the class is used as a member of another class that itself uses its default constructor.
- When you write `Foo()` to create a temporary.

**Why define it explicitly?**
- If you define **any other constructor** (e.g., `Foo(int x)`), the compiler **stops generating** the default one. So `Foo a;` would fail to compile unless you define `Foo()` yourself.
- It guarantees your members start in a **known state** (no garbage values for primitives like `int`, which are not zero-initialized by default in C++98).

---

### 2. Copy Constructor

**Signature:**
```cpp
Foo(const Foo& other);
```

**What it does:**
Constructs a **new object** as a copy of an existing one. It is called when an object is being **created from another object of the same type**.

**When is it called?**
- `Foo a; Foo b(a);` — explicit copy construction.
- `Foo b = a;` — copy initialization (note: this is **not** assignment, it's construction!).
- When an object is **passed by value** to a function: `void func(Foo f);` called with `func(a);`.
- When an object is **returned by value** from a function (though the compiler may elide this with RVO/NRVO).
- When inserting an object into an STL container (in C++98).

**Why define it explicitly?**
- The compiler-generated version copies each member **bitwise**. If a member is a pointer, both the original and the copy will point to the **same memory** — when one of them is destroyed and frees that memory, the other becomes a **dangling pointer**.
- You may want to **log** copies, or **forbid** them (by declaring the copy constructor `private` and not defining it).

**Important:**
- The parameter must be a **`const` reference** (`const Foo&`). If it were passed by value, calling the copy constructor would itself require... a copy constructor → infinite recursion.

---

### 3. Copy Assignment Operator

**Signature:**
```cpp
Foo& operator=(const Foo& other);
```

**What it does:**
Assigns the contents of one **already-constructed** object to another **already-constructed** object. It does **not** create a new object — it modifies an existing one.

**When is it called?**
- `Foo a; Foo b; b = a;` — explicit assignment between existing objects.
- **Not** called by `Foo b = a;` (that's the copy constructor!).

**Why define it explicitly?**
- Same reason as the copy constructor: the compiler generates a shallow, bitwise copy.
- If your object owns resources, you must:
  1. **Release** any resource currently held by `*this`.
  2. **Allocate new resources** and copy the data from `other`.
  3. **Return `*this`** by reference to allow chaining: `a = b = c;`.

**Best-practice checklist:**
- ✅ Check for **self-assignment**: `if (this != &other)` — assigning an object to itself can lead to freeing memory and then trying to copy from it.
- ✅ Return `*this` by reference (`Foo&`).
- ✅ Take the parameter as `const Foo&`.

---

### 4. Destructor

**Signature:**
```cpp
~Foo();
```

**What it does:**
Cleans up an object before its memory is reclaimed. It's the **last function** called on an object's lifetime.

**When is it called?**
- When a stack-allocated object goes **out of scope**: `{ Foo a; } // ~Foo() called here`.
- When `delete` is called on a heap-allocated object: `Foo* a = new Foo(); delete a;`.
- When a temporary object is destroyed at the end of the full expression.
- When a containing object is destroyed (member destructors called automatically, in reverse order of declaration).

**Why define it explicitly?**
- If your class allocates resources (`new`, `open`, `malloc`, etc.), you **must** release them in the destructor or you'll leak.
- A destructor is the **only place** where you can guarantee cleanup happens — even if exceptions are thrown.
- For polymorphic base classes, the destructor should be **`virtual`** to ensure derived destructors run when deleting through a base pointer.

---

## The Rule of Three (and Five, and Zero)

The **Rule of Three** (C++98) states:

> If a class needs an explicit **destructor**, **copy constructor**, or **copy assignment operator**, it almost certainly needs all three.

Why? Because all three deal with **resource management**. If your destructor frees memory, your copy constructor must duplicate that memory (otherwise both copies free the same block), and your assignment operator must do the same (and also clean up the existing resource first).

The **Orthodox Canonical Form** is a **stricter** version: it adds the **default constructor** to that list, making it the **Rule of Four**.

> *(Modern C++ adds the Rule of Five — including move constructor and move assignment — and the Rule of Zero — design classes that don't need any of them by leveraging RAII types like `std::vector`, `std::unique_ptr`. But these are post-C++98 and outside the scope of the 42 curriculum for CPP00–CPP04.)*

---

## Deep Copy vs Shallow Copy

This is the **core reason** OCF exists.

### Shallow copy (default, dangerous when owning resources)
```cpp
class Bad {
    int* data;
public:
    Bad() { data = new int(42); }
    ~Bad() { delete data; }
    // No copy constructor → compiler generates one that copies the POINTER, not the data
};

Bad a;
Bad b(a);   // b.data == a.data (same memory!)
// When a and b are destroyed → delete called twice on the same pointer → undefined behavior 💥
```

### Deep copy (correct)
```cpp
class Good {
    int* data;
public:
    Good() : data(new int(42)) {}
    Good(const Good& other) : data(new int(*other.data)) {} // allocate new memory, copy value
    Good& operator=(const Good& other) {
        if (this != &other) {
            delete data;
            data = new int(*other.data);
        }
        return *this;
    }
    ~Good() { delete data; }
};
```

---

## Common Pitfalls

| Pitfall | Consequence |
|---|---|
| Forgetting `const` on the copy constructor parameter | Cannot copy `const` objects, cannot copy temporaries |
| Forgetting to return `*this` from `operator=` | Cannot chain assignments (`a = b = c;`) |
| Forgetting the self-assignment check | Resource freed before being copied — UB |
| Forgetting `virtual` on a base-class destructor | Derived destructor won't run when deleting through base pointer — leaks |
| Defining a non-default constructor without redefining the default one | `Foo a;` no longer compiles |
| Implementing copy as shallow copy when class owns memory | Double-free, dangling pointers |
| Returning `operator=` by value instead of by reference | Unnecessary copy, breaks chaining semantics |

---

## Full Examples

### Example 1: A trivial class (no resources owned)

```cpp
// Animal.hpp
#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <string>

class Animal {
private:
    std::string _name;
    int         _age;

public:
    Animal();                                    // default constructor
    Animal(const std::string& name, int age);    // parameterized constructor
    Animal(const Animal& other);                 // copy constructor
    Animal& operator=(const Animal& other);      // copy assignment operator
    ~Animal();                                   // destructor

    const std::string& getName() const;
    int                getAge() const;
};

#endif
```

```cpp
// Animal.cpp
#include "Animal.hpp"
#include <iostream>

Animal::Animal() : _name("Unnamed"), _age(0) {
    std::cout << "Default constructor called" << std::endl;
}

Animal::Animal(const std::string& name, int age) : _name(name), _age(age) {
    std::cout << "Parameterized constructor called for " << _name << std::endl;
}

Animal::Animal(const Animal& other) : _name(other._name), _age(other._age) {
    std::cout << "Copy constructor called for " << _name << std::endl;
}

Animal& Animal::operator=(const Animal& other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other) {
        _name = other._name;
        _age  = other._age;
    }
    return *this;
}

Animal::~Animal() {
    std::cout << "Destructor called for " << _name << std::endl;
}

const std::string& Animal::getName() const { return _name; }
int                Animal::getAge()  const { return _age; }
```

```cpp
// main.cpp
#include "Animal.hpp"

int main() {
    Animal a;                       // default constructor
    Animal b("Rex", 5);             // parameterized constructor
    Animal c(b);                    // copy constructor
    Animal d = b;                   // ALSO copy constructor (initialization)
    a = b;                          // copy assignment operator

    return 0;
    // destructors called for d, c, b, a (reverse order of construction)
}
```

**Expected output:**
```
Default constructor called
Parameterized constructor called for Rex
Copy constructor called for Rex
Copy constructor called for Rex
Copy assignment operator called
Destructor called for Rex
Destructor called for Rex
Destructor called for Rex
Destructor called for Rex
```

---

### Example 2: A class managing a heap-allocated resource (deep copy required)

```cpp
// Buffer.hpp
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstddef>

class Buffer {
private:
    char*  _data;
    size_t _size;

public:
    Buffer();                                    // default constructor
    Buffer(size_t size);                         // parameterized constructor
    Buffer(const Buffer& other);                 // copy constructor (deep copy)
    Buffer& operator=(const Buffer& other);      // copy assignment operator (deep copy)
    ~Buffer();                                   // destructor (frees memory)

    size_t getSize() const;
    char*  getData() const;
};

#endif
```

```cpp
// Buffer.cpp
#include "Buffer.hpp"
#include <cstring>
#include <iostream>

Buffer::Buffer() : _data(NULL), _size(0) {
    std::cout << "Default Buffer constructed (empty)" << std::endl;
}

Buffer::Buffer(size_t size) : _data(new char[size]), _size(size) {
    std::memset(_data, 0, size);
    std::cout << "Buffer of size " << size << " constructed" << std::endl;
}

Buffer::Buffer(const Buffer& other) : _data(NULL), _size(other._size) {
    if (_size > 0) {
        _data = new char[_size];
        std::memcpy(_data, other._data, _size);
    }
    std::cout << "Buffer copy-constructed (deep copy of " << _size << " bytes)" << std::endl;
}

Buffer& Buffer::operator=(const Buffer& other) {
    std::cout << "Buffer assignment operator called" << std::endl;
    if (this != &other) {            // self-assignment guard
        delete[] _data;              // release old resource
        _size = other._size;
        if (_size > 0) {
            _data = new char[_size]; // allocate new
            std::memcpy(_data, other._data, _size);
        } else {
            _data = NULL;
        }
    }
    return *this;
}

Buffer::~Buffer() {
    std::cout << "Buffer destructed (freeing " << _size << " bytes)" << std::endl;
    delete[] _data;
}

size_t Buffer::getSize() const { return _size; }
char*  Buffer::getData() const { return _data; }
```

```cpp
// main.cpp
#include "Buffer.hpp"

int main() {
    Buffer a(128);     // allocates 128 bytes
    Buffer b(a);       // deep copy → b has its OWN 128 bytes
    Buffer c;          // empty
    c = a;             // deep copy via assignment

    return 0;
    // each destructor cleans up its OWN memory — no double-free
}
```

---

### Example 3: Forbidding copy (singleton-like / non-copyable)

Sometimes you **don't want** a class to be copyable (e.g., a class managing a unique handle). The C++98 idiom is to declare copy constructor and copy assignment operator **`private`** and **not implement** them:

```cpp
// Logger.hpp
class Logger {
public:
    Logger();
    ~Logger();
    void log(const std::string& msg);

private:
    Logger(const Logger& other);              // declared but not defined
    Logger& operator=(const Logger& other);   // declared but not defined
};
```

Now any attempt to copy a `Logger` will fail at **compile time** (or link time if called from a friend/member). This is still considered respecting the OCF — you've **explicitly handled** all four cases.

---

### Example 4: A class with a `virtual` destructor (polymorphism)

```cpp
class Base {
public:
    Base();
    Base(const Base& other);
    Base& operator=(const Base& other);
    virtual ~Base();              // VIRTUAL — crucial for safe polymorphic deletion
    virtual void speak() const;
};

class Derived : public Base {
private:
    int* _resource;
public:
    Derived();
    Derived(const Derived& other);
    Derived& operator=(const Derived& other);
    ~Derived();                   // automatically virtual because parent's is
    virtual void speak() const;
};
```

Without `virtual ~Base()`, this code:
```cpp
Base* b = new Derived();
delete b;   // calls only ~Base() — ~Derived() never runs → leaks _resource!
```
...would silently leak. With `virtual`, both destructors run in the correct order (derived first, then base).

---

## Quick Reference Cheat Sheet

```cpp
class ClassName {
public:
    ClassName();                                       // 1. Default constructor
    ClassName(const ClassName& other);                 // 2. Copy constructor
    ClassName& operator=(const ClassName& other);      // 3. Copy assignment operator
    ~ClassName();                                      // 4. Destructor
};
```

| Function | When it runs | Returns | Parameter |
|---|---|---|---|
| Default constructor | `ClassName a;` | nothing | none |
| Copy constructor | `ClassName b(a);` or `ClassName b = a;` | nothing | `const ClassName&` |
| Copy assignment | `b = a;` (both already exist) | `ClassName&` | `const ClassName&` |
| Destructor | scope exit / `delete` | nothing | none |

---

**TL;DR**: The Orthodox Canonical Form is the C++98 discipline of always defining the four lifecycle functions explicitly so that object creation, copying, assignment, and destruction are never left to the compiler's silent defaults. It prevents resource leaks, double frees, and dangling pointers in any class that owns resources — and makes intent crystal clear in classes that don't.
