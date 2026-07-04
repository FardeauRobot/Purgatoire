# `this` — The Hidden Pointer Every Member Function Receives

> **TL;DR.** `this` is a pointer to the current object, available inside every non-static member function. The compiler passes it as an invisible first argument. It's typed `T*` in normal members, `const T*` in `const` members, and doesn't exist in `static` members.

Related: [`CLASS.md`](CLASS.md) · [`CONST.md`](CONST.md) · [`OPERATOR_OVERLOADING.md`](../notions/oop/OPERATOR_OVERLOADING.md) · [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. The model

A member function looks like a method, but under the hood it's a regular function that takes the object by pointer:

```cpp
class Point {
    int _x, _y;
public:
    void translate(int dx, int dy) { _x += dx; _y += dy; }
};

Point p(3, 4);
p.translate(1, 1);
```

The compiler effectively rewrites the call as:

```cpp
Point__translate(&p, 1, 1);
```

Inside `translate`, `this` is the name of that hidden first parameter:

```cpp
void Point::translate(int dx, int dy) {
    this->_x += dx;        // explicit — same as below
    _y += dy;              // implicit — compiler inserts this->
}
```

```
                caller                          callee
              ┌────────────┐               ┌──────────────────────┐
              │ Point p    │               │ translate(this, dx, dy)
              │  _x = 3    │ ── &p ──►    │   this points at p
              │  _y = 4    │               │   dx = 1, dy = 1
              └────────────┘               │   *this._x += dx
                                            │   *this._y += dy
                                            └──────────────────────┘
```

After the call: `p._x == 4`, `p._y == 5`.

---

## 2. The type of `this`

Inside `class T`:

| Member function declared as… | Type of `this` |
|---|---|
| `void f()`                  | `T *` |
| `void f() const`            | `const T *` |
| `void f() volatile`         | `volatile T *` |
| `void f() const volatile`   | `const volatile T *` |
| `static void f()`           | `this` does not exist |

The trailing `const` after a member function's parameter list applies to `*this`, not to anything else.

```cpp
class Box {
    int _value;
public:
    int get() const { return _value; }   // this is const Box*
    void set(int v) { _value = v; }      // this is Box*
};
```

```cpp
const Box b;
b.get();          // OK — get is const, this is const Box*
b.set(1);         // ERROR — set's this is Box*, can't bind to a const Box
```

---

## 3. When `this` is essential

### 3.1 Self-assignment guard

```cpp
MyClass& MyClass::operator=(const MyClass& other) {
    if (this != &other) {        // are we assigning to ourselves?
        delete _data;
        _data = new int[other._size];
        std::memcpy(_data, other._data, other._size * sizeof(int));
    }
    return *this;
}
```

Without the guard, `a = a;` would `delete _data` and then try to copy from already-freed memory — undefined behavior.

### 3.2 Returning `*this` for chaining

```cpp
class Builder {
    std::string _s;
public:
    Builder& add(const std::string& part) { _s += part; return *this; }
};

Builder b;
b.add("hello").add(", ").add("world");
```

`*this` dereferences the pointer and gives back a reference to the actual object. Returning by reference (not value!) is essential for chaining.

### 3.3 Disambiguating member from parameter

```cpp
class C {
    int x;
public:
    C(int x) : x(x) {}              // OK in initializer list
    void set(int x) {
        x = x;                      // BUG! both are the parameter
        this->x = x;                // OK
    }
};
```

The 42 underscore convention (`_x` for members) sidesteps this entirely:

```cpp
void set(int x) { _x = x; }         // unambiguous
```

### 3.4 Passing the object to a free function

```cpp
class Logger {
    void log() const {
        write(std::cout, *this);    // pass yourself by reference
    }
};
```

---

## 4. Hardware view — `this` at the assembly level

x86_64 System V ABI passes the first integer/pointer argument in `rdi`. For a non-static member function, that's `this`:

```asm
Point::translate(int, int):
        ; rdi = this (pointer to Point)
        ; esi = dx
        ; edx = dy
        mov     eax, dword ptr [rdi]        ; load this->_x
        add     eax, esi                    ; += dx
        mov     dword ptr [rdi], eax        ; store back
        mov     eax, dword ptr [rdi + 4]    ; load this->_y
        add     eax, edx                    ; += dy
        mov     dword ptr [rdi + 4], eax    ; store back
        ret
```

The pointer is just a register. There is **no extra cost** to having member functions vs. free functions taking a `T*` first argument.

Microsoft's x64 ABI does the same with `rcx`. ARM64's ABI uses `x0`. The pattern — first argument is `this` — is universal.

---

## 5. Member access through `this` — implicit vs explicit

```cpp
class C {
    int _x;
    int _y;
public:
    int sum() const {
        return _x + _y;             // implicit
        // identical to:
        // return this->_x + this->_y;
    }
};
```

Use `this->` explicitly when:

- You need to disambiguate from a parameter (`this->x = x`).
- You want to make member access stand out in a long function (style choice).
- You're inside a template that inherits from another template, and the dependent name needs `this->` to be found (advanced — see [`TEMPLATES.md`](../notions/advanced/TEMPLATES.md)).

Otherwise, drop it for brevity.

---

## 6. `this` inside a constructor and destructor

```cpp
class C {
    int _x;
public:
    C() : _x(0) {
        // this points at the partially-constructed object
        // member initialization has happened (via the init list);
        // we're now in the body
    }
    ~C() {
        // this points at the still-fully-constructed object
        // members are alive; destruction begins after the body
    }
};
```

A subtle hazard: in a base class constructor, `this` *is* of type `Base*`, even if the eventual object is a `Derived*`. Calling a virtual function from a constructor or destructor will dispatch to the **base** version — because the derived part isn't yet (or no longer) constructed. See [`POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md).

```cpp
class Animal {
public:
    Animal() { sound(); }                  // calls Animal::sound, even if 'new Dog()'
    virtual void sound() = 0;
};

class Dog : public Animal {
public:
    void sound() { /* bark */ }
};

new Dog();      // calls Animal() → Animal::sound() → undefined behavior (pure virtual)
```

The fix is to never call virtuals from constructors/destructors.

---

## 7. `*this` returns the object; `this` returns the pointer

```cpp
class C {
public:
    C&        ref()     { return *this; }   // by reference
    C*        ptr()     { return  this; }   // by pointer
    C         copy()    { return *this; }   // by VALUE — a copy is made!
};
```

The third one is a subtle bug-magnet: it returns a copy, not the original. If you wanted "give me back this object," you almost always want a reference (`return *this;` and `C&` return type).

---

## 8. `this` cannot be reassigned

```cpp
void C::f() {
    this = something;       // ERROR — this is not an lvalue
}
```

`this` is a **pure rvalue** — a value, not a variable. You can use it (`this->x`, `*this`, `&*this`), but you can't change what it points at.

---

## 9. `this` does not exist in static members

```cpp
class C {
public:
    static void s() {
        // this here     — ERROR: 'this' is unavailable for static member functions
    }
};
```

Static member functions don't take an implicit `this`. They operate on the class, not on an instance.

---

## 10. Tips & tricks

### 10.1 Always return `*this` (not `this`) from compound operators

```cpp
class C {
public:
    C& operator+=(int n) { _x += n; return *this; }
};
```

`return this` would compile only if the return type were `C*` — and that breaks the `(a += 1) += 2` chaining idiom.

### 10.2 The "delete this" pattern — exists, almost never use

```cpp
class Refcounted {
    int _refs;
public:
    void release() {
        if (--_refs == 0) delete this;     // legal — but careful
    }
};
```

After `delete this;`, the function must not touch any member or call any virtual. Used in COM, in some smart-pointer internals — never in 42 modules.

### 10.3 Don't store `this` in a long-lived data structure

```cpp
class Logger {
public:
    Logger() { register_logger(this); }  // dangerous if Logger is on the stack —
                                          // the registry now points at a soon-to-die object
};

void f() {
    Logger l;
    do_work();    // do_work logs through 'l'
}                  // l destructed — registry now has a dangling pointer
```

### 10.4 Comparing `this` to parameters

```cpp
operator==(const C& other) const {
    if (this == &other) return true;       // same object → trivially equal
    return _x == other._x && _y == other._y;
}
```

Identity check (`this == &other`) is cheap and skips a deep comparison.

### 10.5 In templates, `this->` may be required

```cpp
template <typename T>
class Base {
protected:
    int _value;
};

template <typename T>
class Derived : public Base<T> {
public:
    int get() {
        return _value;          // ERROR — _value is a "dependent name"
        return this->_value;    // OK
    }
};
```

This is one of the few times explicit `this->` is mandatory in C++98. See [`TEMPLATES.md`](../notions/advanced/TEMPLATES.md).

---

## 11. Common errors

| Error | Cause | Fix |
|---|---|---|
| `'this' was not declared in this scope` | Used in a static member or free function | Pass the object explicitly |
| `passing 'const C' as 'this' argument discards qualifiers` | Calling a non-const member fn on a const object | Mark the member fn `const` |
| `cannot bind 'C' lvalue to 'C&&' rvalue reference` | Returning by value from `operator=` (C++11+) | Return `*this` by reference, return type `C&` |
| Infinite recursion in `operator=` | Wrote `*this = other` inside `operator=` | Do member-by-member copy, then return `*this` |

---

## 12. Visual summary

```
              ┌────────────────────────────────────────────────────┐
              │   p.translate(1, 2);                                │
              │                                                     │
              │      ▼  compiler rewrites as                        │
              │                                                     │
              │   Point::translate(&p, 1, 2);                       │
              │                                                     │
              │           ▲                                          │
              │           │ this                                     │
              │           │                                          │
              │       ┌───┴───┐                                      │
              │       │ Point │                                      │
              │       │ _x _y │ ← regular memory                     │
              │       └───────┘                                      │
              │                                                     │
              │   inside translate:                                 │
              │       this  is a regular pointer (rdi)              │
              │       *this is the object                           │
              │       this->_x is just a member load                │
              └────────────────────────────────────────────────────┘
```

---

## 13. Practice

1. What's the type of `this` inside `void C::f() const`? *(`const C*`.)*
2. Why is `return *this;` (with return type `C&`) preferred over `return this;` for chaining? *(Chaining requires expressions to be lvalues; a reference works, a pointer would force `(b.add(...))->add(...)` syntax everywhere.)*
3. Why is calling a virtual function from a constructor problematic? *(`this` has the static type of the constructor's class, so virtuals dispatch to the base, not the derived. Possibly UB if pure virtual.)*
4. Can you call a static member function via `this->`? *(Yes — `this->staticMember()` is legal. `Class::staticMember()` is preferred for clarity.)*
