# 🧬 CPP07 — Templates: Write It Once, Let the Compiler Copy It

> **TL;DR.** A template is a **pattern**, not code. When you write `max(a, b)` with `int`s, the compiler stamps out a real `int max(int, int)` from your pattern. With `std::string`s it stamps out a second one. This module has three exercises: function templates (`swap`/`min`/`max`), a function template that takes *another* function (`iter`), and a class template that owns memory (`Array<T>`). The one trap that matters: **a template's body has to be visible wherever it's used**, so it lives in the header (or a `.tpp` the header includes).

Related: [`TEMPLATES.md`](../../notions/advanced/TEMPLATES.md) · [`TEMPLATE.md`](../../lexique/TEMPLATE.md) · [`TYPENAME.md`](../../lexique/TYPENAME.md) · [`ORTHODOX_CANONICAL_FORM.md`](../../notions/oop/ORTHODOX_CANONICAL_FORM.md) · [`TRY_CATCH_THROW.md`](../../lexique/TRY_CATCH_THROW.md) · [`MEMORY.md`](../../notions/fundamentals/MEMORY.md) · Previous: [`CPP06.md`](CPP06.md)

---

## 1. The problem, in C you already know

You want one `max` that works for every type. C gives you two bad options:

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int i = MAX(x++, y);        /* if x wins, x++ runs TWICE: a silent bug */

void *max_generic(void *a, void *b, int (*cmp)(void *, void *));
                            /* like qsort: no type checking, casts everywhere */
```

- The **macro** is plain text substitution. It has no types, no scope, and it double-evaluates its arguments.
- **`void *`** is what libft and `qsort` use. It's type-erased: the compiler can't tell you passed a `char *` where an `int *` was expected.

**The C++ answer:** write the function once with a *type parameter*. The compiler generates a fully type-checked version for each type you actually use.

```cpp
template <typename T>
T const &max(T const &a, T const &b)
{
    return (a > b) ? a : b;
}
```

Each argument is evaluated once and the result is checked by the compiler. No casts.

---

## 2. The mechanism, in one diagram

```
  YOUR HEADER (pattern)                 WHAT THE COMPILER GENERATES
 ┌──────────────────────────────┐
 │ template <typename T>        │ max(1, 2)          ┌───────────────────────────────┐
 │ T const &max(T const &a,     │ ─────────────────► │ int const &max(int const &,   │
 │              T const &b)     │   T = int          │                int const &)   │
 │ { return a > b ? a : b; }    │                    └───────────────────────────────┘
 │                              │ max(s1, s2)        ┌───────────────────────────────┐
 │                              │ ─────────────────► │ std::string const &max(...)   │
 └──────────────────────────────┘   T = std::string  └───────────────────────────────┘

  No call with double? → no double version exists in the binary.
```

This is called **instantiation**. It happens at compile time, in every `.cpp` that uses the template. Two consequences:

1. **The body must be visible** in that `.cpp`. If you put the body in `whatever.cpp`, `main.cpp` sees only a declaration. The compiler can't stamp anything out, and the linker fails with `undefined symbol: max<int>`.
2. **Type errors appear at instantiation.** `max(a, b)` on a type with no `operator>` compiles fine until someone actually calls it with that type.

| C / earlier modules | CPP07 |
|---|---|
| `#define` macro | Function template (type-checked, single evaluation) |
| `void *` + element size (libft `ft_memcpy`) | `T` known exactly |
| `int *arr = malloc(n * sizeof(int))` | `Array<int> arr(n)` |
| One class per element type | One class template, `Array<T>` |
| Function bodies in `.cpp` | Template bodies in `.hpp` / `.tpp` |

---

## 3. ex00 — `swap`, `min`, `max`

### What the subject asks
- Three function templates in `whatever.hpp`: `swap` (returns nothing), `min` and `max`.
- **If the two values are equal, `min` and `max` return the second one.**
- They must work with any type whose values can be compared (they support `<` and `>`).
- The subject provides a `main`, and your output must match:

```
a = 3, b = 2
min(a, b) = 2
max(a, b) = 3
c = chaine2, d = chaine1
min(c, d) = chaine1
max(c, d) = chaine2
```

### Core snippet

```cpp
#ifndef WHATEVER_HPP
# define WHATEVER_HPP

template <typename T>
void swap(T &a, T &b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

template <typename T>
T const &min(T const &a, T const &b)
{
    return (a < b) ? a : b;
}

template <typename T>
T const &max(T const &a, T const &b)
{
    return (a > b) ? a : b;
}

#endif
```

Line by line:
- `template <typename T>` introduces a type parameter named `T`. `typename` and `class` mean the same thing here.
- `swap` takes **references** because it has to modify the caller's variables. This is the C++ version of `void ft_swap(int *a, int *b)`.
- `min` takes **const references**, so `std::string` values aren't copied. It returns a const reference to whichever argument won.
- `(a < b) ? a : b` returns `b` when the values are equal. That's exactly the rule the subject asks for. `max` uses `>` the same way.

### Why the subject's `main` writes `::swap(a, b)`
`<string>` brings in `std::swap`, `std::min` and `std::max`. You're not allowed `using namespace std`, so you might expect no clash. But when an argument is a `std::string`, **argument-dependent lookup (ADL)** also searches namespace `std`, the namespace the argument's type lives in. Tested with Apple clang in `-std=c++98`:

| Unqualified call on two `std::string`s | Result |
|---|---|
| `min(c, d)` | ❌ `error: call to 'min' is ambiguous`. Yours and `std::min` have the same signature |
| `swap(c, d)` | ⚠️ Compiles, but **silently calls `std::swap`**, because the string overload is more specialised. Your template never runs |

`::swap` means "the one in the global namespace", which is yours. It turns both problems off.

### ⚠️ Gotchas
- **Returning `a` for equal values** is the classic mistake with `(a <= b) ? a : b`. Test with `min(x, x)` on two different variables that hold the same value, and compare their addresses.
- Don't give your templates a namespace. The subject's `main` calls `::min`.
- A **header-only file** is fine here. The grading sheet's "no implementation in headers" rule exempts templates.

---

## 4. ex01 — `iter`

### What the subject asks
`iter(address, length, function)` calls `function` on every element of the array. It returns nothing. It must work with **any element type**, and the function may be a **function template instantiation**. Elements can be `const` or not, so the function may take `T &` or `T const &`.

### The shape of the solution

```cpp
template <typename T, typename F>
void iter(T *array, size_t const length, F func)
{
    if (array == NULL)
        return;
    for (size_t i = 0; i < length; i++)
        func(array[i]);
}
```

Line by line:
- **`template <typename T, typename F>`**: two unknowns. `T` is the element type and `F` is the callback's type. The compiler deduces both from the call, so you never write them by hand.
- **`void iter(T *array, size_t const length, F func)`**: the same three things `ft_striteri` takes (an address, a length, a function). `F func` is passed **by value**, which is cheap: it's a function pointer or a small functor.
- **`if (array == NULL) return;`**: the one real boundary. Called with `NULL` and a non-zero length, `array[i]` would dereference NULL.
- **`for (size_t i = 0; i < length; i++)`**: `size_t` so it's the same type as `length`. An `int i` compared with a `size_t` triggers `-Wsign-compare`, which is an error under `-Werror`.
- **`func(array[i]);`**: `array[i]` is an lvalue of type `T`, so it can bind to a `T &` or a `T const &` parameter. When `T` is `int const`, only a `T const &` callback fits. Passing `inc<int>` (which takes `int &`) on a const array fails with `binding reference of type 'int' to value of type 'const int' drops 'const' qualifier` (tested). That's the compiler protecting the const array for you.

Why two type parameters?

```
iter(tab, 5, print<int>)
     │    │  └── F = void (*)(int const &)   ← whatever callable you pass
     │    └───── length
     └────────── T = int                      ← deduced from int tab[5]

iter(ctab, 3, print<int>)       with  int const ctab[3]
     └── T = int const → array[i] is int const& → OK for a `T const &` callback
```

- `T` comes from the array. Pass a `const` array and `T` itself becomes `int const`, so the const case works on its own.
- `F` accepts *anything you can call*: a plain function pointer, an instantiated function template, and even a functor object (C++98 has no lambdas).
- `F` is also more flexible than hard-coding `void (*)(T &)`. That signature would **reject** a `void f(T const &)` callback on a non-const array.

If you prefer to write the pointer types explicitly, provide two overloads:

```cpp
template <typename T>
void iter(T *array, size_t const length, void (*func)(T &));

template <typename T>
void iter(T const *array, size_t const length, void (*func)(T const &));
```

- The first overload takes a **mutable** array and a callback that may modify: `void (*func)(T &)` reads "pointer to a function taking a `T &` and returning `void`". It's C's function-pointer syntax, as in `qsort`'s comparator.
- The second takes a `T const *` array and a callback that only reads. When you pass a const array, only this overload matches.
- A *read-only* callback on a *mutable* array still works. Deduction may add `const` (`int *` → `int const *`), so `::iter(tab, 5, print<int>)` picks the second overload (tested: all three cases compile and run). What you lose compared to `F` is functors: an overload that asks for a function **pointer** won't take a callable object.

Both approaches pass. Be ready to explain the one you picked.

### Passing a function template as the callback

```cpp
template <typename T>
void print(T const &x) { std::cout << x << std::endl; }

int tab[] = { 0, 1, 2, 3, 4 };
::iter(tab, 5, print<int>);     // explicit template argument: names one real function
```

- **`template <typename T> void print(T const &x)`**: a second function template. It takes `T const &`, so it works on const and non-const arrays alike, and it doesn't copy `std::string`s.
- **`int tab[] = { 0, 1, 2, 3, 4 };`**: a plain C array. Its name decays to `int *` when passed, so `T = int`.
- **`::iter(tab, 5, print<int>)`**: `::` picks your global `iter` (same reason as `::swap` in ex00). `print<int>` names one real function, `void print(int const &)`, and its address becomes `F`.

The explicit `<int>` matters. A bare `print` is a *family* of functions, and with a generic `F` the compiler can't deduce which one you mean: `::iter(c, 1, p)` fails with `no matching function for call to 'iter'` (tested).

### What the evaluator runs
The grading sheet compiles its own test file against your `iter`. It must print:

```
0
1
2
3
4
42
42
42
42
42
```

The exact test file isn't reproduced here, so don't guess its callbacks. Make sure your `iter` accepts a function template instantiation (`print<int>`), works on a `const` array, and works with a callback that takes a non-const reference and modifies the element. A generic `F func` parameter handles all three.

### ⚠️ Gotchas
- `length` is `const` in the subject's description. Match it.
- Don't allocate or copy the array. `iter` only borrows it, just like `ft_striteri` in libft.

---

## 5. ex02 — `Array<T>`

### What the subject asks
A class template `Array<T>` with:

| Member | Behaviour |
|---|---|
| `Array()` | Empty array |
| `Array(unsigned int n)` | `n` elements, **default-initialised** (tip from the subject: try `int *a = new int();`) |
| Copy ctor / `operator=` | **Deep copy**: changing the copy must never change the original |
| `operator[]` | Access by index. **Throws a `std::exception`** when out of bounds |
| `size() const` | Number of elements. Takes no parameters and doesn't modify the instance |

Other rules:
- **Memory must come from `new[]`.** No preventive allocation (no "reserve 1000 just in case"), and no STL containers.
- Your own `main` must test it.
- You may put the implementation in a `.tpp` file.

### The class shape

```cpp
#ifndef ARRAY_HPP
# define ARRAY_HPP

# include <exception>
# include <cstddef>

template <typename T>
class Array
{
private:
    T               *_data;
    unsigned int    _size;

public:
    Array();
    Array(unsigned int n);
    Array(Array const &other);
    Array &operator=(Array const &other);
    ~Array();

    T               &operator[](unsigned int i);
    T const         &operator[](unsigned int i) const;
    unsigned int    size() const;

    class OutOfBoundsException : public std::exception
    {
    public:
        virtual const char *what() const throw();
    };
};

# include "Array.tpp"

#endif
```

Line by line:
- **`# include <exception>`** for `std::exception`, the base of your out-of-bounds exception. **`<cstddef>`** for `NULL`.
- **`template <typename T>` right before `class Array`**: the whole class is a pattern. `Array` alone isn't a type. `Array<int>` and `Array<std::string>` are.
- **`T *_data;`**: the heap block, typed. It's `int *arr` from C, except `T` is filled in per instantiation.
- **`unsigned int _size;`**: `unsigned` because the subject's constructor takes an `unsigned int`. A negative size is impossible by construction.
- **The four OCF lines.** Inside the class, `Array` is shorthand for `Array<T>`, so `Array const &other` means "another array of the same `T`".
- **Two `operator[]`**: one returns `T &` (read and write), the other is a `const` member returning `T const &` (read only). See "Const access" below.
- **`unsigned int size() const;`**: `const` because it only reads, which the subject asks for ("doesn't modify the instance").
- **`class OutOfBoundsException : public std::exception`**: a nested exception class, the CPP05 pattern. `what() const throw()` matches the C++98 signature of `std::exception::what`.

The `.tpp` goes at the **bottom** of the header, after the class is declared. Anything that includes `Array.hpp` gets the bodies too, which solves the visibility problem from §2.

### Core snippets from `Array.tpp`

```cpp
template <typename T>
Array<T>::Array() : _data(NULL), _size(0) {}

template <typename T>
Array<T>::Array(unsigned int n) : _data(new T[n]()), _size(n) {}

template <typename T>
Array<T>::Array(Array const &other) : _data(NULL), _size(0)
{
    *this = other;
}

template <typename T>
Array<T> &Array<T>::operator=(Array const &other)
{
    if (this != &other)
    {
        T *fresh = new T[other._size]();
        for (unsigned int i = 0; i < other._size; i++)
            fresh[i] = other._data[i];
        delete[] _data;
        _data = fresh;
        _size = other._size;
    }
    return *this;
}

template <typename T>
Array<T>::~Array() { delete[] _data; }

template <typename T>
T &Array<T>::operator[](unsigned int i)
{
    if (i >= _size)
        throw OutOfBoundsException();
    return _data[i];
}

template <typename T>
T const &Array<T>::operator[](unsigned int i) const
{
    if (i >= _size)
        throw OutOfBoundsException();
    return _data[i];
}

template <typename T>
unsigned int Array<T>::size() const
{
    return _size;
}

template <typename T>
const char *Array<T>::OutOfBoundsException::what() const throw()
{
    return "Array: index out of bounds";
}
```

Line by line, in the order of the file:
- **`Array<T>::Array() : _data(NULL), _size(0) {}`**: the empty array owns nothing. The initialisation list sets both members before the body runs, which is the CPP02 habit.
- **`Array(unsigned int n) : _data(new T[n]()), _size(n) {}`**: one allocation of exactly `n` elements. That's the "no preventive allocation" rule.
- **Copy ctor:** first make `*this` a valid empty array, then let `operator=` do the deep copy. One copy routine instead of two.
- **`if (this != &other)`**: self-assignment guard. Without it, `a = a` would copy from a block it's about to free.
- **`T *fresh = new T[other._size]();`** then the `for` loop: a new block, filled element by element. `fresh[i] = other._data[i]` calls `T`'s own `operator=`, so a `std::string` gets a real copy of its characters.
- **`delete[] _data; _data = fresh; _size = other._size;`**: swap the old block for the new one, and only now.
- **`return *this;`** allows `a = b = c`, the usual `operator=` contract.
- **`~Array() { delete[] _data; }`**: `new[]` in, `delete[]` out.
- **`operator[]`, both versions:** the same bounds check and the same return. Only the `const` after `)` and the `const` in the return type differ. `throw OutOfBoundsException();` builds a temporary exception object and throws it. `main` catches it as `std::exception &`.
- **`size()`** returns the stored count. Unlike C's `strlen`, nothing is recounted.
- **`Array<T>::OutOfBoundsException::what()`**: three levels of name: the template, the nested class, then the method. The string is a literal, so it lives for the whole program and returning its address is safe.

Walkthrough:
- **`template <typename T>` goes before *every* member definition.** Each one is its own little template, and you qualify it with `Array<T>::`.
- **`new T[n]()`**: the `()` at the end value-initialises the elements, so an `int` array starts as `0`s instead of garbage. Without it, `Array<int> a(5); a[0]` reads uninitialised memory, which is what valgrind warns about as "conditional jump depends on uninitialised value".
- **`new T[0]` is legal**, and `delete[]` on it is fine. `delete[] NULL` is also a no-op. That's why the default ctor can simply use `NULL`.
- **`operator=` allocates first and deletes second.** If `new` throws (`std::bad_alloc`), the object still holds its old valid data. Deleting first would leave a dangling `_data`.
- **`i >= _size` is the whole bounds check.** The index is `unsigned`, so `a[-1]` wraps to 4294967295 and gets caught by the same test.
- **The copy ctor sets `_data(NULL)` first**, then reuses `operator=`. Otherwise `operator=` would `delete[]` an uninitialised pointer.

### Deep copy versus shallow copy, in memory

```
 SHALLOW (compiler-generated copy)          DEEP (what you write)
   stack              heap                    stack              heap
 ┌──────────────┐                           ┌──────────────┐
 │ a._data ─────┼──┐                        │ a._data ─────┼──► [1][2][3]
 │ a._size 3    │  ├──► [1][2][3]           │ a._size 3    │
 ├──────────────┤  │                        ├──────────────┤
 │ b._data ─────┼──┘                        │ b._data ─────┼──► [1][2][3]  (a new block)
 │ b._size 3    │                           │ b._size 3    │
 └──────────────┘                           └──────────────┘
 b[0] = 42 changes a too.                  b[0] = 42 leaves a alone.
 ~b then ~a → double delete[].             ~b and ~a each free their own block.
```

This is the same story as CPP04's `Brain` deep copy. The template doesn't change it; it just applies it to every `T`.

### Const access, and why you need two `operator[]`

```cpp
Array<int> const frozen(3);
std::cout << frozen[0];     // calls   T const &operator[](unsigned int) const
frozen[0] = 1;              // ❌ compile error: can't assign through a const reference
```

- **`Array<int> const frozen(3);`**: three zeros, locked. Only `const` member functions can be called on it.
- **`frozen[0]`** so the compiler picks the `const` overload, the only one allowed on a const object. It returns `int const &`, and printing it is fine (prints `0`, tested).
- **`frozen[0] = 1;`**: assigning through an `int const &` is refused at compile time. The protection costs nothing at runtime.

The grading sheet checks exactly this: "reading and writing through `operator[]`, or reading only if the instance is const". With only the non-const version, `frozen[0]` doesn't compile at all.

### ⚠️ Gotchas
- **`typename` for dependent types.** Inside a template, a name like `Array<T>::Something` depends on `T`, so it might be a type or a value, and the compiler assumes *value*. When you use it as a type there, write `typename Array<T>::Something`. Outside any template the name isn't dependent: in `main`, `catch (Array<int>::OutOfBoundsException &e)` needs no `typename`, and adding one is an error in C++98 (`'typename' outside of a template is a C++11 extension`). See [`TYPENAME.md`](../../lexique/TYPENAME.md).
- **Test with a complex `T`.** The sheet asks you to prove it works with *simple and complex types*. `Array<std::string>` is the minimum. A small class with a user-defined OCF is better, because it proves `fresh[i] = other._data[i]` calls *its* `operator=`.
- **`delete` instead of `delete[]`** is undefined behaviour and valgrind reports it as a mismatch. Memory from `new[]` must be released with `delete[]`.
- Don't add a `resize` or `push_back`. The subject didn't ask for them, and extra allocation logic is extra defense risk.

---

## 6. What the evaluator checks

From the official 42evalhub grading sheet:

**Prerequisites** (fail any of these and the exercise, or the whole project, isn't graded)
- [ ] Compiles with `c++ -Wall -Wextra -Werror`. The project is C++98 and uses no C++11 features.
- [ ] No function implemented in a header, **except templates**.
- [ ] No `*alloc`, `*printf` or `free`. No `using namespace`. No `friend`. No external library.
- [ ] No leaks (valgrind / `leaks`).

**ex00**
- [ ] Output matches the subject for simple types (`int`).
- [ ] Works with **complex types** too. The evaluator uses their own test file.

**ex01**
- [ ] The evaluator's test file compiles against your `iter` and prints `0 1 2 3 4` then `42` five times.

**ex02**
- [ ] **Memory comes from `new[]`.** Otherwise the exercise isn't graded.
- [ ] You can show on the spot that it works with **both simple and complex types**.
- [ ] Both an empty array and an array of size `n` can be constructed.
- [ ] `operator[]` reads and writes, reads only on a const instance, and **throws a `std::exception`** when out of range.

**Questions to prepare for the defense**
- "Why is the template body in the header?" → Instantiation happens in each translation unit that uses it (§2).
- "What does `new T[n]()` do differently from `new T[n]`?" → Value initialisation: built-in types start at zero.
- "What happens if I copy then modify the copy?" → Show it in your `main`.

---

## 7. Cheat sheet

| Need | C++98 spelling |
|---|---|
| Function template | `template <typename T> T const &f(T const &a)` |
| Two type parameters | `template <typename T, typename F>` |
| Instantiate explicitly | `print<int>` |
| Class template member outside the class | `template <typename T> void Array<T>::f() { … }` |
| Put bodies somewhere else | `Array.tpp`, `#include`d at the bottom of `Array.hpp` |
| Default-initialised array | `new T[n]()` |
| Dependent type name | `typename Container<T>::iterator` |
| Const and non-const access | Two overloads of `operator[]`, one of them `const` |

**What this module leaves you with:** you can read `std::vector<T>` in an error message without panicking, because you've written a small one yourself. CPP08 hands you the real STL containers, all built exactly this way.

➡️ Next: [`CPP08.md`](CPP08.md). The STL's containers are templates too. Now you learn to *use* them, through iterators and algorithms.
