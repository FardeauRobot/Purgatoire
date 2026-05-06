# `template` — Code That Writes Code

> **TL;DR.** A template is a **recipe** for a function or class. The compiler stamps out a concrete version (an *instantiation*) per type or value the program actually uses. The keyword `template` introduces a parameter list of types or constants that the recipe is parameterized over.

Related: [`TYPENAME.md`](TYPENAME.md) · [`TEMPLATES.md`](../advanced/TEMPLATES.md) · [`STL.md`](../advanced/STL.md)

---

## 1. The shape

```cpp
template <typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}
```

`template <typename T>` says: *"this function has one type parameter named `T`; please instantiate as needed."*

Use it:

```cpp
max<int>(3, 5);          // explicit:   T = int
max(3, 5);               // deduced:    T = int
max(3.14, 2.71);         // deduced:    T = double
max(std::string("a"), std::string("b"));  // T = std::string
```

The compiler **emits a separate function body** for each `T` actually used. Calling `max<int>` and `max<double>` results in two distinct function symbols in the binary.

```
   source:                              binary:
   template <typename T> max(T,T)       max<int>(int,int)
                                        max<double>(double,double)
                                        max<std::string>(...)   ← only the ones called
```

This is **monomorphization** — generic source, specific code.

---

## 2. Two kinds of template parameters

### 2.1 Type parameter

```cpp
template <typename T>      // 'typename' or 'class' — same thing for type params
template <class T>         // identical
```

`T` is a placeholder for any type the caller supplies.

### 2.2 Non-type parameter (a value, must be a compile-time constant)

```cpp
template <int N>
class FixedArray {
    int _data[N];
};

FixedArray<10> a;          // N = 10
FixedArray<20> b;          // N = 20 — DIFFERENT type from FixedArray<10>
```

Allowed non-type kinds in C++98: integral (int, char, etc.), enums, pointers/references to objects with external linkage, pointers to members. **Not** floats or arbitrary objects.

### 2.3 Mix and match

```cpp
template <typename T, int N>
class Buffer {
    T _data[N];
};

Buffer<int, 100>    bi;
Buffer<double, 50>  bd;
```

---

## 3. Function templates

```cpp
template <typename T>
void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}
```

Three things are happening when you call `swap(x, y)`:

```
   1. argument deduction:  T = decltype(x) (with array-to-pointer, etc., decay rules)
   2. instantiation:       compiler emits swap<T>'s body
   3. type checking:       does T support copy and assignment? if not → compile error
```

Compile error from a template instantiation looks like:

```
   error: no match for 'operator>' (operand types are 'X' and 'X')
   note:  in instantiation of 'T max(T, T) [with T = X]'
```

Read these from the **bottom** — find the user code that triggered the instantiation, then look up to see what the compiler actually wanted.

### 3.1 Explicit instantiation

```cpp
max<int>(3.14, 2.71);    // forces T = int; argument 3.14 converted to int (truncated to 3)
```

Use this when deduction would pick a wrong type, or when arguments differ.

### 3.2 Deduction tricks

```cpp
template <typename T>
void f(T value);

f(42);          // T = int
f("hi");        // T = const char*
f<int>("hi");   // T = int explicitly; "hi" doesn't convert → ERROR

template <typename T>
void g(T &ref);

int x;
g(x);           // T = int; ref is int&
const int y = 5;
g(y);           // T = const int; ref is const int&
```

Reference deduction preserves cv-qualifiers; value deduction strips top-level `const`.

---

## 4. Class templates

```cpp
template <typename T>
class Stack {
    T   _data[100];
    int _top;
public:
    Stack() : _top(0) {}
    void push(const T& x) { _data[_top++] = x; }
    T    pop()            { return _data[--_top]; }
    bool empty() const    { return _top == 0; }
};

Stack<int>          si;
Stack<std::string>  ss;
```

`Stack<int>` and `Stack<std::string>` are entirely separate types. Same for any difference in template arguments.

### 4.1 Member functions are templates too

When you define them outside the class, you must repeat the `template` clause:

```cpp
template <typename T>
void Stack<T>::push(const T& x) {
    _data[_top++] = x;
}
```

`Stack<T>::` makes the member belong to the template; the leading `template <typename T>` re-introduces `T` for this scope.

### 4.2 Header-only

Templates are **almost always defined entirely in headers**. The reason: instantiation happens in the translation unit that *uses* the template, and that needs the full body, not just a declaration. Putting the body in a `.cpp` works for one TU but breaks for others.

42 norm pattern:

```cpp
// Stack.hpp
template <typename T>
class Stack {
public:
    void push(const T& x);
};

template <typename T>
void Stack<T>::push(const T& x) {
    // ...
}

// (no Stack.cpp)
```

Or define everything inside the class body. Either is fine.

---

## 5. Specialization

You can write a different implementation for a specific type:

```cpp
template <typename T>
struct Printer {
    static void print(const T& x) { std::cout << x; }
};

template <>                                  // explicit specialization
struct Printer<bool> {
    static void print(const bool& x) {
        std::cout << (x ? "true" : "false");
    }
};

Printer<int>::print(42);            // 42
Printer<bool>::print(true);         // true (not "1")
```

The compiler picks the most specialized matching version.

Partial specialization (class templates only — function templates can't be partially specialized in C++):

```cpp
template <typename T> struct IsPointer        { static const bool value = false; };
template <typename T> struct IsPointer<T*>    { static const bool value = true;  };

IsPointer<int>::value;     // false
IsPointer<int*>::value;    // true
```

Used heavily in type traits (e.g., `std::is_pointer`).

---

## 6. Hardware/compile view — the cost of templates

### 6.1 Compile-time

Templates are slow to compile because:
- The compiler must parse the template body twice — once at definition (syntax check) and once per instantiation (semantic check).
- Every TU that uses a template instantiation re-parses and re-instantiates it.
- Heavy generic libraries (e.g., Eigen, Boost) explode into many internal instantiations.

Mitigations: precompiled headers, `extern template` (C++11), keeping templates simple.

### 6.2 Run-time

Zero cost. The instantiated code is exactly as fast as a hand-written specific version. Often *faster* than runtime polymorphism (no vtable, more inlining opportunities).

### 6.3 Binary size

Each used instantiation contributes its own machine code:

```
   Stack<int>::push    ─┐
   Stack<int>::pop      ├──── three full sets of methods in the binary
   Stack<int>::empty    │
                        │
   Stack<string>::push  ─┐
   Stack<string>::pop    ├──── another full set
   Stack<string>::empty  │
```

Binaries grow if you instantiate many times. The linker deduplicates identical instantiations across TUs (so you don't get five copies of `Stack<int>::push`), but distinct types each carry their own.

---

## 7. The `typename` keyword inside templates

When you have a *dependent name* (a name whose meaning depends on a template parameter), the compiler doesn't know whether it's a type or a value. You must tell it:

```cpp
template <typename T>
void f() {
    typename T::iterator it;        // 'typename' tells compiler: T::iterator is a type
}
```

See [`TYPENAME.md`](TYPENAME.md).

---

## 8. Tips & tricks

### 8.1 Header-only is the norm

Define template functions and class member templates in the header. Don't try to split them across `.hpp/.cpp` — instantiations from other TUs won't find the definitions.

### 8.2 SFINAE and `enable_if` (C++98 has it via `std::enable_if<bool, T>::type` — but barely useful before C++11)

Substitution Failure Is Not An Error: a template that fails to instantiate doesn't error if there's another candidate that works. Heavily used in modern generic programming. Out of scope for 42.

### 8.3 Keep the parameters minimal

A template with five type parameters and three non-type parameters is hard to instantiate correctly. Default-argument template parameters (only on class templates in C++98) help:

```cpp
template <typename T, int N = 10>
class Buffer { /* ... */ };

Buffer<int> b;        // N defaulted to 10
Buffer<int, 20> c;
```

### 8.4 Force instantiation when you need a symbol

```cpp
template class Stack<int>;       // explicit instantiation — forces the compiler
                                  // to generate all of Stack<int>'s code in this TU
```

Useful in some library designs but rare in 42.

### 8.5 Compile errors are wordy

A single bad instantiation often produces fifty lines of error. Read the **first** error and the line that says `in instantiation of …` — that's your call site.

### 8.6 Templates are duck-typed

```cpp
template <typename T>
T add(T a, T b) { return a + b; }
```

`T` doesn't need to be declared anywhere; whatever you pass must support `+`. If it doesn't, you get a compile error from inside the template body — at the `+` site.

This is "concept-implicit" programming. C++20 concepts make it explicit; in C++98 you write documentation.

### 8.7 Don't fight the type system

If `T` could be something for which `+` doesn't work, you'll get compiler errors when someone tries it with a mismatched type. Either accept that, document the requirements, or restrict the template to known types via specialization.

---

## 9. Common errors

| Error | Cause | Fix |
|---|---|---|
| `undefined reference to Stack<int>::push()` (linker) | Template body in a `.cpp` not visible to the using TU | Move definitions to the header |
| `'iterator' in 'class T' does not name a type` | Forgot `typename` for a dependent name | Prepend `typename`: `typename T::iterator it;` |
| `template argument deduction failed` | The arguments don't match the template signature uniquely | Provide explicit template arguments, or fix the call |
| `instantiation depth exceeds maximum` | Recursive template that doesn't terminate | Add a base-case specialization |
| Hour-long compile times | Heavy template metaprogramming | Reduce template depth; cache instantiations; use precompiled headers |

---

## 10. Visual summary

```
              ┌────────────────────────────────────────────┐
              │  template <typename T>                      │
              │  T max(T a, T b) { return a > b ? a : b; } │
              └──────────────────┬─────────────────────────┘
                                 │
                                 │   compiler emits one body per
                                 │   distinct T actually used:
                                 ▼
              ┌──────────────────────────────────────────┐
              │   max<int>(int, int)        ← in binary  │
              │   max<double>(double, …)    ← in binary  │
              │   max<std::string>(…)       ← in binary  │
              └──────────────────────────────────────────┘

              parameters:   typename T          (a type)
                            class T             (same)
                            int N               (a non-type — value)

              specializations:   template <> struct X<T>{} (full)
                                 template <T> struct X<T*>{} (partial, classes only)

              header-only is the rule. compile-time cost: high.
              runtime cost: zero. binary size: grows per instantiation.
```

---

## 11. Practice

1. Why are templates almost always defined in headers? *(Instantiation needs the full body in every TU that uses it; a separate `.cpp` definition isn't visible at the call site.)*
2. What's the difference between `class T` and `typename T` in the template parameter list? *(Nothing — both introduce a type parameter.)*
3. What's the difference between `Stack<int>` and `Stack<int>` instantiated in two TUs? *(Same type. The linker deduplicates the code; both TUs see one symbol per member.)*
4. Why can't function templates be partially specialized? *(C++98 design choice — overloading covers most use cases; partial specialization works only for class templates.)*
