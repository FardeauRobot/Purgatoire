# `operator` — Make Your Type Behave Like A Built-In

> **TL;DR.** `operator` is a keyword used to declare a function that overloads a built-in operator (`+`, `-`, `<<`, `[]`, `()`, conversions, etc.). It looks like syntactic sugar but it's a regular function — same calling convention, same overload resolution, same compile-time decisions.

Related: [`OPERATOR_OVERLOADING.md`](../notions/oop/OPERATOR_OVERLOADING.md) · [`FRIEND.md`](FRIEND.md) · [`EXPLICIT.md`](EXPLICIT.md) · [`THIS.md`](THIS.md)

---

## 1. The syntax

```cpp
ReturnType operator<symbol>(parameters);
```

The keyword `operator` followed by the operator's symbol becomes the function's name. The function can be a member or a free function (depending on the operator).

```cpp
class Fixed {
    int _raw;
public:
    Fixed operator+(const Fixed& other) const;          // member
    Fixed& operator++();                                 // member, prefix ++
    Fixed  operator++(int);                              // member, postfix ++ (dummy int param)
    bool operator<(const Fixed& other) const;           // member, comparison
};

std::ostream& operator<<(std::ostream& os, const Fixed& f);   // free, stream insertion
```

That's the whole game. Now `a + b`, `a < b`, `++a`, `std::cout << a` all work for your type.

---

## 2. The full list of overloadable operators (C++98)

```
   arithmetic:      +    -    *    /    %
   compound assign: +=   -=   *=   /=   %=
   bitwise:         &    |    ^    ~    <<   >>
   compound assign: &=   |=   ^=   <<=  >>=
   logical:         !    &&   ||
   comparison:      ==   !=   <    >    <=   >=
   increment/dec:   ++   --
   special:         =    []   ()   ->   ->*   ,
   memory:          new  new[]  delete  delete[]
   conversion:      operator T()
   address:         &    *
```

You **cannot** overload:

- `.` (member access — too special)
- `.*` (pointer-to-member access)
- `::` (scope resolution)
- `?:` (ternary)
- `sizeof`
- `typeid`
- preprocessor things like `#`, `##`

You **cannot** invent new operators (`**`, `<>`, etc.). Only the existing ones.

---

## 3. Member vs free function — which is which?

```
    member          when the operator's left-hand operand is YOUR type
                    and you want privileged access to its state.

    free function   when the LHS may not be your type
                    (e.g., 2 * MyVec, std::cout << MyType).
```

These operators **must** be members:

| Why | Operators |
|---|---|
| Access semantics tied to `this` | `=`, `[]`, `()`, `->`, `->*` |

These operators **must** be free functions:

| Why | Operators |
|---|---|
| Symmetric, may need to convert LHS | (No formal requirement, but `<<` / `>>` for streams are practically always free) |

The rest can be either. As a rule:

- **Compound-assignment first** (`+=`, `-=`, `*=`, etc.) as members.
- **Then symmetric versions** (`+`, `-`, `*`) as free functions defined in terms of compound assignment.
- **Stream operators** as free functions.

```cpp
class Vec {
    float _x, _y;
public:
    Vec& operator+=(const Vec& other) {
        _x += other._x; _y += other._y;
        return *this;
    }
};

Vec operator+(Vec a, const Vec& b) {     // pass-by-value of LHS, returns a copy
    a += b;
    return a;
}
```

Why pass `Vec a` by value? You're going to copy it anyway to produce the result; let the compiler do RVO/move.

---

## 4. Implementation patterns by operator

### 4.1 `operator=` (copy assignment) — see OCF

```cpp
MyClass& MyClass::operator=(const MyClass& other) {
    if (this != &other) {
        // copy each member
        _data = other._data;
    }
    return *this;
}
```

Three rules:
1. Take by `const&`.
2. Self-assignment guard (`if (this != &other)`).
3. Return `*this` by reference.

### 4.2 `operator==` and `operator!=`

Define `==` and derive `!=`:

```cpp
bool operator==(const Fixed& a, const Fixed& b) { return a.raw() == b.raw(); }
bool operator!=(const Fixed& a, const Fixed& b) { return !(a == b); }
```

In C++20+ you'd use `<=>` (the spaceship operator) but that's not in 42.

### 4.3 `operator<` etc. — define one, derive others

```cpp
bool operator<(const F& a, const F& b)  { return a.raw() <  b.raw(); }
bool operator>(const F& a, const F& b)  { return  b < a; }
bool operator<=(const F& a, const F& b) { return !(b < a); }
bool operator>=(const F& a, const F& b) { return !(a < b); }
```

This is the **strict weak ordering** template.

### 4.4 `operator++` and `operator--` — pre vs post

```cpp
class C {
public:
    C& operator++();         // prefix:  ++c → returns *this by reference
    C  operator++(int);      // postfix: c++ → returns the OLD value by value
};
```

The dummy `int` parameter is the only way to disambiguate prefix and postfix at the syntax level. Don't pass a value through it; it's a tag.

```cpp
C& C::operator++()        { _v += 1; return *this; }
C  C::operator++(int)     { C tmp = *this; ++(*this); return tmp; }
```

Postfix is more expensive (it makes a copy). Prefer prefix when you don't need the old value.

### 4.5 `operator[]` — array indexing

```cpp
class Buffer {
    char _data[1024];
public:
    char&       operator[](std::size_t i)       { return _data[i]; }
    const char& operator[](std::size_t i) const { return _data[i]; }
};
```

Two overloads — one for mutable access, one for read-only. The compiler picks the right one based on whether the buffer is `const`.

### 4.6 `operator()` — function call (functor)

```cpp
class Adder {
    int _n;
public:
    Adder(int n) : _n(n) {}
    int operator()(int x) const { return x + _n; }      // a "callable"
};

Adder add5(5);
add5(10);          // 15
```

Functors are huge in the STL — `std::sort(v.begin(), v.end(), Adder(...))` and similar.

### 4.7 `operator<<` for output

```cpp
std::ostream& operator<<(std::ostream& os, const Fixed& f) {
    return os << (f.raw() / 256.0f);
}
```

Always:
- Take `os` by reference (non-const — streams mutate).
- Take your type by const reference.
- Return `os` to enable chaining (`std::cout << a << b << '\n';`).
- Free function, often `friend` if you need private access (or use a public accessor).

### 4.8 Conversion operator

```cpp
class Fixed {
public:
    operator float() const { return _raw / 256.0f; }
};

Fixed f(2);
float x = f;       // implicit conversion via operator float
```

In C++98 you can't make conversion operators `explicit`. Be very careful — they fire in many contexts and are a top source of "what just happened" bugs.

---

## 5. Hardware view — same as any function

`operator+` is just a function. There is no "operator stub" or special path. After overload resolution:

```cpp
Vec a, b, c;
c = a + b;
```

becomes:

```cpp
c.operator=(operator+(a, b));      // free + and member =
```

Compiles to two function calls. The optimizer can inline both, especially for small types.

```
   source: c = a + b;

   compiler: which 'operator+' fits (Vec, Vec)? → free fn version.
             which 'operator=' fits Vec on Vec? → member version.

   IR: t = operator+(a, b);
       c.operator=(t);

   asm (after inlining):
   addss  xmm0, xmm1            ; vector add (if compiler vectorized)
   ...
```

There's no runtime distinction between `a + b` and `operator+(a, b)`. They're synonyms.

---

## 6. Overload resolution and `operator`

The same rules as for any overloaded function:

1. The compiler collects all candidate overloads (member, free, in scope, ADL).
2. Each candidate's parameters are matched against the actual argument types.
3. Best viable match wins. Ties cause an ambiguity error.

That's why `friend` declarations sometimes matter — to make the operator visible at the call site.

---

## 7. The 42 norm operator pattern

For a `Fixed` class with one private `int _raw`:

```cpp
// Fixed.hpp
class Fixed {
    int _raw;

public:
    // OCF
    Fixed();
    Fixed(int);
    Fixed(float);
    Fixed(const Fixed& other);
    Fixed& operator=(const Fixed& other);
    ~Fixed();

    // arithmetic (members because we need _raw)
    Fixed operator+(const Fixed& other) const;
    Fixed operator-(const Fixed& other) const;
    Fixed operator*(const Fixed& other) const;
    Fixed operator/(const Fixed& other) const;

    // comparison
    bool operator< (const Fixed& other) const;
    bool operator> (const Fixed& other) const;
    bool operator<=(const Fixed& other) const;
    bool operator>=(const Fixed& other) const;
    bool operator==(const Fixed& other) const;
    bool operator!=(const Fixed& other) const;

    // increment / decrement
    Fixed& operator++();
    Fixed  operator++(int);
    Fixed& operator--();
    Fixed  operator--(int);

    // accessors
    int   getRawBits() const;
    void  setRawBits(int raw);
    float toFloat() const;
    int   toInt() const;
};

// stream insertion (free function)
std::ostream& operator<<(std::ostream& os, const Fixed& f);
```

This is the canonical Fixed-class shape from CPP02.

---

## 8. Tips & tricks

### 8.1 Don't go wild — overload only when it makes sense

`operator+` for matrices, vectors, complex numbers — yes.
`operator+` to mean "concatenate logs" or "add a child node" — no. Use a named method (`addChild`, `appendLog`).

The criterion: would a reader who sees `a + b` immediately know what it does? If not, give it a name.

### 8.2 Symmetric operators should commute (when math demands it)

If `Vec(1) + Vec(2) == Vec(2) + Vec(1)`, your `+` should reflect that. If a non-Vec operand can be involved, free functions and possibly two non-member overloads:

```cpp
Vec operator*(float scalar, const Vec& v);
Vec operator*(const Vec& v, float scalar);
```

### 8.3 Don't overload `&&`, `||`, `,`

Built-in `&&` and `||` short-circuit. **Overloaded** versions don't — both arguments are evaluated. This is surprising and error-prone. Same for `operator,` (the comma operator).

### 8.4 The "rule of zero" — let the compiler do the work

If your class wraps types that already implement the operations correctly (like `std::string`), let the compiler-generated copy ctor / `=` do their thing. Don't write a `=` you don't need to.

### 8.5 Stream operators always return the stream

```cpp
std::ostream& operator<<(std::ostream& os, const T& t) {
    // ...
    return os;            // mandatory for chaining
}
```

Any other return type breaks `cout << a << b << '\n';`.

### 8.6 Function-call operator is the most flexible

A functor (`operator()`) can carry state and be passed to algorithms. In C++11+ lambdas largely replace them, but in C++98 functors are the way to do generic "code as data."

---

## 9. Common errors

| Error | Cause | Fix |
|---|---|---|
| `operator+ must take exactly N arguments` | Wrong arity (e.g., binary as 1-arg member) | Member binary takes 1 explicit param + implicit `this`; free binary takes 2 |
| `no match for operator+` | Overload not visible / wrong types | Check namespace, ADL, signatures |
| `operator= must be a non-static member function` | Defined as a free function | Move it inside the class |
| `recursive call to operator=` | Wrote `*this = other` inside `operator=` | Do member-by-member copies; never call yourself |
| Surprising silent conversions | Non-`explicit` conversion operator | Make conversions explicit or remove them |

---

## 10. Visual summary

```
                  ┌────────────────────────────────────────────┐
                  │   keyword: operator                         │
                  │                                              │
                  │   names a function that overloads a         │
                  │   built-in symbol. compile-time dispatch    │
                  │   via normal overload resolution.            │
                  └─────────────────────┬──────────────────────┘
                                        │
            ┌───────────────────────────┼───────────────────────────┐
            ▼                           ▼                           ▼
     member operator             free operator              conversion operator
     ───────────────             ─────────────              ──────────────────
     when LHS is your type       when LHS may not be        operator T() — converts
     and you need this access.   yours (streams, scalar     your type to T. fires
     mandatory for: =, [], (),   * vector, etc.) or you     implicitly in many
     ->, ->*                     prefer symmetry.           contexts; use sparingly.

   pattern:
     1. compound-assign (+=, -=) as members → mutate this
     2. binary symmetric (+, -) as free functions in terms of compound
     3. comparison: implement < and ==, derive the rest
     4. << / >> as free functions (often friends)
```

---

## 11. Practice

1. Why must `operator=` be a non-static member? *(Symmetry/convention; the standard requires it. The LHS is the object; the operator is conceptually "assign to me".)*
2. Why is the postfix `operator++(int)` form less efficient than the prefix `operator++()`? *(Postfix returns the old value, so it must save a copy before mutating.)*
3. Why are `&&`, `||`, and `,` rarely overloaded? *(Overloading them turns off short-circuit / sequencing semantics, which is surprising.)*
4. Why can `operator<<` not be a member of your custom class? *(Its left operand is `std::ostream`. A member operator's `this` would be the stream — and you can't add member functions to `std::ostream`.)*
