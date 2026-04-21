# C++ References — Full Guide

## 1. What is a reference?

A **reference** is an *alias* for an existing variable. Once bound, the reference and the original refer to the same memory location — they are literally the same object under two different names.

```cpp
int x = 42;
int &r = x;   // r is now another name for x

r = 100;
std::cout << x;  // prints 100
std::cout << &x << " == " << &r;  // same address
```

Syntax: the `&` goes **after the type** in a declaration. It means "reference to".

---

## 2. References vs Pointers (coming from C)

In C, indirection = pointers. C++ adds references as a safer, cleaner alternative for most use cases.

| Feature | Pointer (`int *p`) | Reference (`int &r`) |
|---|---|---|
| Can be null | Yes (`NULL` / `nullptr`) | **No** — must bind at creation |
| Must be initialized | No | **Yes** |
| Can be reassigned to another object | Yes (`p = &y;`) | **No** — bound for life |
| Syntax to access value | `*p` | `r` (direct) |
| Syntax to get address | `p` | `&r` |
| Can have array of them | Yes | **No** (`int &arr[10];` illegal) |
| Can be `const` | Yes (`int *const p`) | Implicitly — refs are always "const-bound" |
| Memory cost | Takes space (8 bytes on 64-bit) | Usually optimized away by compiler |

### Mental model
- Pointer = "a variable that holds an address"
- Reference = "a nickname for an existing variable"

---

## 3. The three hard rules

1. **Must be initialized on declaration.**
   ```cpp
   int &r;       // ERROR: references must be bound
   int &r = x;   // OK
   ```

2. **Cannot be rebound.** Once `r` refers to `x`, it always refers to `x`.
   ```cpp
   int x = 1, y = 2;
   int &r = x;
   r = y;        // This does NOT rebind r to y.
                 // It assigns the value of y to x (r is still x).
   // After this: x == 2, y == 2, r refers to x
   ```

3. **Cannot be null.**
   ```cpp
   int &r = *(int*)NULL;  // Undefined behavior — do not do this
   ```
   A valid reference always refers to a valid object. No null checks needed.

---

## 4. Why references exist — use cases

### 4.1 Pass-by-reference (avoid copies)

In C, to modify a variable inside a function, you pass its address:
```c
void increment(int *n) { (*n)++; }
increment(&x);
```

In C++, with references, it's cleaner:
```cpp
void increment(int &n) { n++; }
increment(x);  // no &, no *, just works
```

The function **modifies the caller's variable directly**, no copying.

### 4.2 `const` reference — read-only, no copy

The idiomatic way to pass "large" objects (strings, classes) without copying them *and* without allowing mutation:

```cpp
void print(const std::string &s) {
    std::cout << s;
    // s = "oops"; // ERROR: s is const
}
```

This is the standard form for:
- Copy constructors: `ClassName(const ClassName &src);`
- Copy assignment: `ClassName &operator=(const ClassName &rhs);`
- Any function parameter where you don't need to modify the argument.

### 4.3 Return by reference — allow chaining / lvalue access

```cpp
ClassName &operator=(const ClassName &rhs) {
    // ...
    return *this;   // return reference to the current object
}

a = b = c;   // works because operator= returns a reference
```

Also lets a function return something that can appear on the **left side** of an assignment:
```cpp
int &getElement(int idx) { return arr[idx]; }
getElement(3) = 99;   // modifies arr[3]
```

---

## 5. `const` references — the most common form

```cpp
const int &r = x;   // r is a read-only alias for x
```

Special property: a `const` reference can bind to a **temporary (rvalue)**:
```cpp
const int &r = 42;           // OK — lifetime of 42 is extended
const std::string &s = "hi"; // OK — temporary string is kept alive
```

A non-const reference **cannot**:
```cpp
int &r = 42;   // ERROR
```

That's why function parameters are nearly always `const T &` — they accept both variables and literals/temporaries.

---

## 6. References to pointers, pointers to references

### Reference to pointer — legal
```cpp
int *p = &x;
int *&rp = p;   // rp is a reference to the pointer p
rp = &y;        // modifies p, now p points to y
```

Used when a function needs to modify **which address** a pointer holds:
```cpp
void allocate(int *&ptr) { ptr = new int(5); }
```

### Pointer to reference — illegal
```cpp
int &*p;   // ERROR: no such thing
```
Because references aren't objects with addresses of their own — `&r` gives the address of what `r` refers to, not of the reference itself.

---

## 7. Common pitfalls

### 7.1 Dangling reference

A reference outliving the object it refers to → undefined behavior.

```cpp
int &bad() {
    int local = 5;
    return local;   // local dies here — returned reference is dangling
}

int &r = bad();
std::cout << r;     // UB: accessing dead memory
```

**Never return a reference to a local variable.** Returning a reference to a member, to a static, or to `*this` is fine.

### 7.2 Confusing assignment with rebinding

```cpp
int x = 1, y = 2;
int &r = x;
r = y;   // assigns 2 into x. r is still bound to x!
```

If you want "rebinding" behavior, use a pointer:
```cpp
int *p = &x;
p = &y;  // now p points to y — actual rebinding
```

### 7.3 Reference in class members

If a class has a reference member, it **must** be initialized in the constructor's initializer list (not in the body):

```cpp
class HumanA {
private:
    Weapon &_weapon;
public:
    HumanA(std::string name, Weapon &w) : _weapon(w) {}  // MUST use init list
};
```

Consequence: classes with reference members usually can't be reassigned (the compiler-generated `operator=` would try to rebind, which is impossible).

---

## 8. When to use reference vs pointer

| Situation | Use |
|---|---|
| Must exist, no null case | Reference |
| May be null / optional | Pointer |
| Need to rebind to different objects | Pointer |
| Storing in a container (`std::vector<T&>` is illegal) | Pointer |
| Function parameter (read-only large object) | `const T&` |
| Function parameter (modify caller's variable) | `T&` |
| Return from `operator=`, `operator<<`, etc. | Reference |
| Dynamic allocation (`new`) result | Pointer |
| C API interop | Pointer |

**42 rule of thumb:** prefer references when the object *must* exist and you don't need to rebind. Use pointers when you need null, reseating, or arrays.

---

## 9. Quick examples

### Pass by reference
```cpp
void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
}

int x = 1, y = 2;
swap(x, y);   // x == 2, y == 1
```

### Const reference parameter (idiomatic)
```cpp
void greet(const std::string &name) {
    std::cout << "Hello, " << name << "\n";
}
```

### Returning *this by reference (OCF)
```cpp
Fixed &Fixed::operator=(const Fixed &rhs) {
    if (this != &rhs)
        _raw = rhs._raw;
    return *this;
}
```

### Reference member (ex03 pattern)
```cpp
class HumanA {
    std::string _name;
    Weapon &_weapon;   // always refers to a real Weapon
public:
    HumanA(std::string name, Weapon &w) : _name(name), _weapon(w) {}
    void attack() { std::cout << _name << " attacks with " << _weapon.getType(); }
};
```

vs `HumanB` which may not always have a weapon — uses a pointer:
```cpp
class HumanB {
    std::string _name;
    Weapon *_weapon;   // can be null
public:
    HumanB(std::string name) : _name(name), _weapon(NULL) {}
    void setWeapon(Weapon &w) { _weapon = &w; }
};
```

This is exactly why `HumanA` uses a reference and `HumanB` uses a pointer in the ex03 exercise.

---

## 10. Summary cheat sheet

```cpp
int x = 10;

int  &r  = x;       // reference (alias)
const int &cr = x;  // const reference (read-only alias, accepts temporaries)

int *p = &x;        // pointer
int *&rp = p;       // reference to a pointer

void f1(int n);           // by value (copy)
void f2(int *n);          // by pointer (may be null)
void f3(int &n);          // by reference (must exist, modifiable)
void f4(const int &n);    // by const reference (must exist, read-only, no copy)

ClassName &operator=(const ClassName &rhs);  // canonical signature
```

**Golden rule:** a reference is just another name for an existing object. If that thought keeps making sense, you've got it.
