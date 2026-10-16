# Pointers to Member Functions

> **Related:** [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) · [`MEMORY.md`](../fundamentals/MEMORY.md) · [`REFERENCE.md`](../fundamentals/REFERENCE.md)

A **pointer to a member function** stores *which method of a class* to call, rather than which free function. The syntax is hostile, but the idea is simple: it's a function pointer that also remembers the class it belongs to, so the caller can supply the `this` instance at call time.

```cpp
void           (*fp)();           // pointer to a FREE function
void (Harl::*  mp)();             // pointer to a MEMBER function of Harl
```

The reason these are separate types: calling a method needs **two things** — the function *and* the object (`this`). A regular function pointer only carries the first; a member function pointer carries the type information needed to bind the second at call time.

---

## Table of Contents

1. [Why two different pointer types?](#1.%20Why%20two%20different%20pointer%20types%3F)
2. [Declaring a member function pointer](#2.%20Declaring%20a%20member%20function%20pointer)
3. [Taking the address](#3.%20Taking%20the%20address)
4. [Calling through the pointer](#4.%20Calling%20through%20the%20pointer)
5. [The classic use case — dispatch tables](#5.%20The%20classic%20use%20case%20%E2%80%94%20dispatch%20tables)
6. [Virtual methods and member pointers](#6.%20Virtual%20methods%20and%20member%20pointers)
7. [`static` member functions are not members](#7.%20static%20member%20functions%20are%20not%20members)
8. [`typedef` to tame the syntax](#8.%20typedef%20to%20tame%20the%20syntax)
9. [Common errors](#9.%20Common%20errors)
10. [In CPP01](#10.%20In%20CPP01)

---

## 1. Why two different pointer types?

A free function lives at a single address. A method call, by contrast, expands roughly to:

```cpp
harl.debug();          // is conceptually: Harl_debug(&harl);
```

The hidden first parameter `this` is bound by the *caller* (the dot or arrow operator), not stored inside the pointer. So a member function pointer has to:

- Know **which class** it belongs to (so the compiler knows what `this` should be typed as).
- Possibly carry **vtable information** for virtual methods (so virtual dispatch still works through the pointer).

This means a member function pointer is **not the same size** as a regular function pointer, and **not convertible** to one. They're distinct types.

---

## 2. Declaring a member function pointer

Read it left-to-right, with the parens grouping the `ClassName::*` part:

```cpp
return_type (ClassName::*pointer_name)(arg_types);
```

Examples:

```cpp
class Harl {
    public:
        void        debug();
        void        info();
        int         level(std::string s) const;
};

void (Harl::*action)();                          // method, no args, returns void
int  (Harl::*query)(std::string) const;          // const method, takes string, returns int
```

The `const` at the end is part of the *function type* — a pointer declared without `const` cannot point to a `const`-qualified method, and vice versa.

---

## 3. Taking the address

Use the **qualified name** with `&`:

```cpp
void (Harl::*action)() = &Harl::debug;           // correct
```

You **cannot** write `&harl.debug` — that form is illegal in C++98 because the result wouldn't be a member pointer (it'd be ambiguous with binding `this` early). Always:

```cpp
&ClassName::methodName
```

---

## 4. Calling through the pointer

You need an instance to supply `this`. C++ gives you two special operators:

| Operator | Use when you have... | Example |
|---|---|---|
| `.*` | an object (or reference) | `(harl.*action)()` |
| `->*` | a pointer to an object | `(p->*action)()` |

```cpp
Harl  harl;
Harl *p = &harl;

(harl.*action)();        // dot-star
(p->*action)();          // arrow-star
```

**The outer parentheses around `(obj.*ptr)` are mandatory.** Precedence-wise, `()` (call) binds tighter than `.*`. Without them:

```cpp
harl.*action()           // parsed as: harl .* ( action() )   ❌ won't compile
```

Mnemonic: the `.*` operator says *"reach into this object and pull out the function the pointer names"*; then `()` calls it.

---

## 5. The classic use case — dispatch tables

If/else chains that match a value to a method are exactly what member function pointers replace. Compare:

**Without member pointers** — control flow:

```cpp
void Harl::complain(std::string level) {
    if (level == "DEBUG")        debug();
    else if (level == "INFO")    info();
    else if (level == "WARNING") warning();
    else if (level == "ERROR")   error();
}
```

**With member pointers** — data:

```cpp
void Harl::complain(std::string level) {
    std::string  levels[4] = { "DEBUG", "INFO", "WARNING", "ERROR" };
    void (Harl::*fns[4])() = { &Harl::debug, &Harl::info,
                               &Harl::warning, &Harl::error };

    for (int i = 0; i < 4; ++i) {
        if (levels[i] == level) {
            (this->*fns[i])();
            return;
        }
    }
}
```

Why the second is preferable:

- **Adding a level = adding two array entries**, not editing branches.
- The `for` loop is **generic** — it doesn't know what `debug`/`info`/`warning`/`error` do.
- The two arrays *describe* the mapping; the code that *uses* it is small and uniform.

This is the pattern the 42 subject is steering you toward in **CPP01 ex05/ex06**.

---

## 6. Virtual methods and member pointers

Member function pointers respect `virtual` dispatch. If you take the address of a virtual method, calling through the pointer will still invoke the most-derived override:

```cpp
class Animal {
    public:
        virtual void speak() { std::cout << "..." << std::endl; }
};

class Dog : public Animal {
    public:
        virtual void speak() { std::cout << "Woof" << std::endl; }
};

void (Animal::*sp)() = &Animal::speak;

Dog     d;
Animal &a = d;
(a.*sp)();                  // prints "Woof" — virtual dispatch still happens
```

Mechanism: the member pointer doesn't store a raw address. For a virtual method it stores something like *"vtable slot N"*, and the call resolves through `a`'s actual vtable.

---

## 7. `static` member functions are not members

A `static` method has **no `this`**. It's a free function that lives in the class's namespace. So you take its address with a plain function pointer:

```cpp
class Foo {
    public:
        static void hello() { std::cout << "hi" << std::endl; }
};

void (*fp)() = &Foo::hello;        // plain function pointer
fp();                              // call directly — no instance needed
```

Trying to write `void (Foo::*fp)() = &Foo::hello;` is an error: type mismatch.

---

## 8. `typedef` to tame the syntax

The declaration noise compounds quickly. `typedef` lets you name the type once:

```cpp
typedef void (Harl::*HarlAction)();        // HarlAction is now a real type

HarlAction  table[4] = { &Harl::debug, &Harl::info,
                         &Harl::warning, &Harl::error };

HarlAction  pick = table[2];
(this->*pick)();
```

This is the conventional C++98 way (no `using` aliases in C++98). It also makes function signatures readable when you pass these around.

---

## 9. Common errors

| Mistake | Result | Fix |
|---|---|---|
| `&harl.debug` | compile error | `&Harl::debug` |
| `harl.*ptr()` | parse error | `(harl.*ptr)()` |
| Mixing `const`-qualified types | "cannot convert" | match the qualifier in both declaration and target |
| Assigning member pointer to plain `void(*)()` | type mismatch | use a `(Class::*)` type or `typedef` it |
| Calling without an instance | compile error | you **always** need an object — there's no member call without `this` |

---

## 10. In CPP01

You will reach for these in:

- **ex05 — Harl 2.0**: a dispatch table mapping `"DEBUG" | "INFO" | "WARNING" | "ERROR"` to four private methods.
- **ex06 — Harl filter**: the same dispatch table, plus fall-through behavior implemented with a `switch` over the matched index.

The eval will sometimes ask you to *justify* using member pointers instead of `if/else`. The honest answer:

> *Member pointers turn control flow into data. The dispatch is one loop instead of four branches, and adding a level means appending two array entries — no edit to the matching logic.*

That's the lesson the subject is testing. The clunky syntax is the price of admission.

---

## See also

- [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) — virtual dispatch, vtables (why member pointers can encode vtable slots).
- [`MEMORY.md`](../fundamentals/MEMORY.md) — how `this` is passed under the hood.
- [`modules/CPP01.md`](../../projets/modules/CPP01.md) — Harl exercise walkthrough.
