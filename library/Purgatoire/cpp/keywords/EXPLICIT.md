# `explicit` — "Don't Convert Behind My Back"

> **TL;DR.** A constructor with one argument acts as an **implicit conversion operator** unless you mark it `explicit`. Almost every single-argument constructor should be `explicit`.

Related: [`CASTS.md`](../advanced/CASTS.md) · [`OPERATOR_OVERLOADING.md`](../oop/OPERATOR_OVERLOADING.md) · [`ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. The problem

```cpp
class Fixed {
    int _raw;
public:
    Fixed(int v) : _raw(v << 8) {}     // not marked explicit
};

void process(Fixed f);

process(42);                            // ← what?
```

What happened: the compiler saw `process` wants a `Fixed`, you handed it an `int`, and there's a one-argument `Fixed(int)` constructor. The compiler quietly performed `Fixed(42)` for you — an **implicit conversion**.

Sometimes that's what you want. Often it's a bug:

```cpp
std::string s(5);                       // s = ??? — actually a string of 5 NUL chars
//                                        because std::string(size_t) exists
```

---

## 2. The fix

```cpp
class Fixed {
    int _raw;
public:
    explicit Fixed(int v) : _raw(v << 8) {}
};

process(42);          // ERROR — no implicit conversion
process(Fixed(42));   // OK — explicit conversion
```

`explicit` says: *"to make a `Fixed` out of an `int`, the caller must spell it out."*

---

## 3. When does implicit conversion fire?

Anywhere the compiler can call your single-argument constructor to satisfy a type:

```cpp
class S {
public:
    S(const char *);    // not explicit
};

void f(S s);

f("hi");                 // implicit: f(S("hi"))

S a = "hi";              // implicit (copy-init) — calls S(const char*)
S b("hi");               // explicit syntax (direct-init)
S c = S("hi");           // also explicit (direct-init via temporary)
```

If the constructor were `explicit`:

```cpp
class S {
public:
    explicit S(const char *);
};

S a = "hi";              // ERROR — copy-init forbidden
S b("hi");               // OK    — direct-init still allowed
S c = S("hi");           // OK    — direct-init via temporary
f("hi");                 // ERROR
f(S("hi"));              // OK
```

Mnemonic: **`explicit` blocks the `=` form, allows the `()` form.**

---

## 4. The conversion chain

The compiler is allowed to insert **at most one** user-defined conversion in a single conversion sequence:

```cpp
class A { public: A(int); };
class B { public: B(A); };

B b = 42;        // 42 → A → B  ?
                 // NO — that's two user-defined conversions. Rejected.

B b = A(42);     // OK — one user-defined: A → B
```

Marking `A(int)` `explicit` prevents the first link of the chain. Marking `B(A)` `explicit` prevents the second.

---

## 5. The rule of thumb

> **In C++98, single-argument constructors should be `explicit` by default.** Drop the `explicit` only when implicit conversion is the *intended interface*.

Cases where implicit *is* intended:
- `std::string(const char *)` — every codebase relies on `"hello"` becoming a `std::string`.
- Wrapper / smart-pointer types whose whole point is to behave like the wrapped thing.
- Numeric types that mimic a builtin (e.g., a `Fixed` that should behave like `float`).

For your average 42 module class? **Always `explicit`.**

---

## 6. Hardware & compiler view

`explicit` is purely a **type-system hint**. No runtime cost, no metadata, no machine code difference between `explicit` and non-explicit. The compiler simply changes its overload-resolution rules:

```
   call site                    overload resolution
   ─────────                    ───────────────────
   process(42);                 candidate: process(Fixed)
                                  needs:   int → Fixed
                                  source:  Fixed(int) ctor
                                  is it explicit?
                                       ┌───── yes ──→ reject this candidate
                                       └───── no  ──→ accept (cost = 1 user-defined conv)
```

If no candidate survives → compile error. The optimizer never sees `explicit`; it's gone before code generation.

---

## 7. C++11+ note (for context, 42 uses C++98)

C++11 extended `explicit` to **conversion operators**:

```cpp
class Smart {
public:
    explicit operator bool() const { return _ptr != 0; }
};

Smart s;
if (s) {}              // OK — bool conversion in a "boolean context"
bool b = s;            // ERROR
bool b = bool(s);      // OK
```

In C++98, conversion operators (`operator T()`) cannot be marked explicit. That's why the 42 norm is to avoid them or use the safe-bool idiom.

---

## 8. Multi-argument and zero-argument constructors

```cpp
class P {
public:
    P();                     // default ctor — explicit is allowed but pointless in C++98
    P(int);                  // single-arg — should be explicit
    P(int, int);             // two-arg in C++98 — never converts implicitly
                             // (but in C++11 with brace-init it CAN: P p = {1, 2};
                             //  → marking it explicit blocks copy-list-init)
};
```

In C++98, only single-argument constructors do implicit conversion, so only they need `explicit`. A constructor with two **required** parameters can't fire from one source value.

A constructor with one required parameter and others defaulted **counts as one-argument**:

```cpp
class Q {
public:
    Q(int x, int y = 0);    // can convert from int — should be explicit
};
```

---

## 9. Tips & tricks

### 9.1 Add `explicit` first; drop it only with a reason

When you write a new ctor with one argument, slap `explicit` on it. If a caller bumps into "no implicit conversion," check whether they should be writing `T(x)` explicitly anyway — usually yes.

### 9.2 Read library code through this lens

```cpp
std::vector<int> v(10);          // 10 default-constructed ints — explicit ctor
std::vector<int> v = 10;         // ERROR — vector<T>(size_t) is explicit
```

The standard library uses `explicit` extensively. Note where, and why.

### 9.3 Combine with `const` reference parameter

```cpp
class Buffer {
public:
    explicit Buffer(const std::string& filename);
};

void load(const Buffer& b);

load("data.txt");                // ERROR — would need implicit string→Buffer conversion
load(Buffer("data.txt"));        // OK
```

### 9.4 Explicit copy constructor — almost never useful

```cpp
class X {
public:
    explicit X(const X&);    // legal but unusual
};

X a;
X b = a;                     // ERROR — copy-init forbidden
X c(a);                      // OK
```

This breaks pass-by-value and many idiomatic patterns. Don't do it without a strong reason.

### 9.5 Beware `BankAccount(double)`

```cpp
class BankAccount {
public:
    BankAccount(double initialBalance);
};

void transfer(BankAccount from, BankAccount to);

transfer(100.0, account);    // implicit creates a NEW account with $100 — wrong account!
```

Adding `explicit` would have caught this.

---

## 10. Common errors

| Error | Cause | Fix |
|---|---|---|
| `conversion from 'int' to non-scalar type 'X' requested` | You used `=` form with an `explicit` ctor | Use direct-init form: `X x(42);` |
| Surprising overload picked | Implicit conversion via a non-explicit single-arg ctor | Mark the ctor `explicit` |
| `more than one conversion required` | Tried to chain two user-defined conversions | Insert one explicit conversion in the middle |

---

## 11. Visual summary

```
              ┌───────────────────────────────────────┐
              │   class T { T(U); }     not explicit  │
              │       ▲                               │
              │       │                               │
              │   U value     ───── compiler may ──►  │
              │                       silently        │
              │                       construct T     │
              └───────────────────────────────────────┘
                              │
                              │  add 'explicit' →
                              ▼
              ┌───────────────────────────────────────┐
              │   class T { explicit T(U); }          │
              │       ▲                               │
              │       │                               │
              │   U value  ─────  compiler refuses    │
              │                   unless caller       │
              │                   writes T(value)     │
              └───────────────────────────────────────┘
```

---

## 12. Practice

1. Why does `std::string s = "hello";` work but `std::vector<int> v = 5;` fail? *(`std::string(const char*)` is implicit; `std::vector<T>(size_t)` is `explicit`.)*
2. Is `explicit` checked at compile time or runtime? *(Compile time only; `explicit` produces no code.)*
3. Why is the rule of thumb "make every single-argument ctor explicit"? *(Most accidental conversions are bugs; the cost of writing `T(x)` at the call site is tiny; the cost of debugging an implicit conversion is large.)*
