# `namespace` — Logical Boxes For Names

> **TL;DR.** A namespace is a named scope. Anything declared inside it must be qualified with `Name::` from outside, preventing collisions between libraries that happen to use the same identifiers. Namespaces nest, alias, and merge across files.

Related: [`USING.md`](USING.md) · [`BASICS.md`](../notions/fundamentals/BASICS.md#6.3%20Namespaces) · [`EXTERN.md`](EXTERN.md)

---

## 1. The problem `namespace` solves

```cpp
// libA.hpp
int log(int x);

// libB.hpp
double log(double x);

// main.cpp
#include "libA.hpp"
#include "libB.hpp"
log(2.0);     // ambiguous? collision? → headache
```

Two libraries that share a name conflict. Namespaces give each library its own scope:

```cpp
namespace mathA { int    log(int);    }
namespace mathB { double log(double); }

mathA::log(2);
mathB::log(2.0);
```

No collision. The full names are `mathA::log` and `mathB::log` — different symbols.

---

## 2. Declaring and using

```cpp
// declaration
namespace ft {
    int max(int a, int b) { return a > b ? a : b; }

    class Container {
        // ...
    };
}

// use — three ways
ft::max(1, 2);             // explicit qualification — preferred in headers

using ft::max;             // bring one name into scope
max(1, 2);

using namespace ft;        // bring everything into scope (avoid in headers)
max(1, 2);
Container c;
```

The standard library lives in `std::`. `std::cout`, `std::string`, `std::cin`, `std::vector<int>`.

---

## 3. Compiler/linker view

A namespace adds its name to the **mangled symbol** of everything inside it.

```cpp
namespace ft { void f(); }
namespace gh { void f(); }
```

```
   mangled (Itanium ABI):
   ft::f()  →  _ZN2ft1fEv
   gh::f()  →  _ZN2gh1fEv
```

Two distinct symbols, no collision at link time. Inspect:

```bash
$ nm prog | grep ' f'
0000000000401120 T _ZN2ft1fEv
0000000000401140 T _ZN2gh1fEv
```

Without namespaces, both would mangle to `_Z1fv` and the linker would refuse.

---

## 4. Nested namespaces

```cpp
namespace lib {
    namespace internal {
        int helper();
    }
    int  api();
}

lib::internal::helper();      // explicit
lib::api();
```

C++98 syntax forces the explicit nesting (one `namespace` per level). C++17 allows `namespace lib::internal { ... }`. Not in 42.

---

## 5. The `using` directives

### 5.1 Targeted — bring one name in

```cpp
using std::cout;
cout << "hello\n";        // unqualified
```

This is fine in `.cpp` files when used judiciously.

### 5.2 Bulk — bring all names in

```cpp
using namespace std;       // dump everything from std into the current scope
cout << "hello\n";
string s;
vector<int> v;
```

**Never put `using namespace std;` in a header.** Every TU that includes the header inherits the dump → collisions, name-shadowing surprises, breakage when the standard library adds new names.

42 norm preference: write `std::` explicitly. It's three extra characters and saves debugging hours.

```cpp
// 42-style:
std::cout << contact.getName() << '\n';
```

### 5.3 Inside functions, `using` is fine

```cpp
void process() {
    using std::cout;
    cout << "...";        // local scope; no global pollution
}
```

---

## 6. Namespace alias

```cpp
namespace longLibraryName {
    int doStuff();
}

namespace ll = longLibraryName;
ll::doStuff();
```

A short alias keeps qualifications readable without dragging everything into scope.

---

## 7. The anonymous namespace — file-local scope

```cpp
namespace {
    int helper() { return 42; }      // visible only in this TU
    int counter = 0;
}
```

Equivalent to `static` for free functions and variables — internal linkage. The C++ idiomatic alternative to file-scope `static`.

```
   namespace ___unnamed_<TU-id> { … }
                ↑
       compiler invents a unique name per TU,
       then implicitly does 'using namespace ___unnamed_<TU-id>;'
```

Unlike `static`, anonymous namespaces also work for **types** (you can put a class inside one to make the class TU-private).

---

## 8. Namespaces are open — you can add to them

```cpp
// in math.hpp
namespace ft {
    int abs(int);
}

// in another header, math_extra.hpp
namespace ft {
    int gcd(int, int);
}
```

Both contribute to `ft`. The compiler merges them. This lets a library spread its symbols across many files without forcing you to put everything in one `namespace { ... }` block.

```
   header1: namespace ft { abs }
   header2: namespace ft { gcd }
   
   final ft = { abs, gcd }
```

---

## 9. Argument-dependent lookup (ADL)

A subtle but useful feature: if you call a function with arguments of namespace-N types, the compiler also searches namespace N for matching overloads.

```cpp
namespace ft {
    struct Vec { int x, y; };
    std::ostream& operator<<(std::ostream&, const Vec&);
}

int main() {
    ft::Vec v;
    std::cout << v;       // works — ADL finds ft::operator<<
}
```

ADL is why `std::cout << "string"` works — `operator<<(std::ostream&, const char*)` lives in `std`, and `std::cout`'s type triggers the lookup. Without ADL you'd write `std::operator<<(std::cout, "string")` everywhere.

---

## 10. Tips & tricks

### 10.1 Always prefix `std::` in 42 norm

```cpp
std::cout << "..." << std::endl;
std::string s;
std::vector<int> v;
```

Yes, it's verbose. Yes, it makes the code grep-friendly and unambiguous.

### 10.2 Use a project namespace

For larger projects, wrap your code in your own namespace:

```cpp
namespace mylib {
    class Foo { /* ... */ };
    void do_thing();
}
```

Gives you a clean barrier from third-party libraries.

### 10.3 Don't over-nest

```cpp
namespace company { namespace product { namespace module { namespace impl {
    /* deeply nested code */
}}}}
```

Three-level depth is plenty. Beyond that, qualifications become unreadable.

### 10.4 Anonymous namespace > file-scope static for non-trivial code

```cpp
// modern C++ way:
namespace { void helper(); }

// old C way:
static void helper();
```

Both work; anon namespace is preferred and also works for types.

### 10.5 Aliases for long template instantiations

```cpp
namespace utils {
    typedef std::map<std::string, std::vector<int> > NamedSeries;
}

utils::NamedSeries data;
```

Note the space in `> >` — required in C++98 (otherwise `>>` parses as the right-shift operator). C++11+ removes that.

---

## 11. Common errors

| Error | Cause | Fix |
|---|---|---|
| `'foo' was not declared in this scope` | Forgot `namespace::` or `using` | Qualify: `ns::foo()` or add `using ns::foo;` |
| Ambiguous overload | Multiple `using namespace`s pulled in same name | Use targeted `using` instead |
| Header pollution | `using namespace std;` in a header | Move it out; qualify explicitly |
| ADL surprise — wrong `operator<<` picked | Multiple namespaces define the same operator | Anchor types properly; consider explicit qualification |

---

## 12. Visual summary

```
                ┌────────────────────────────────────────┐
                │  namespace ns { … }                     │
                │                                          │
                │  named scope. names inside need ns::    │
                │  qualification from outside.            │
                └────────────────────┬─────────────────────┘
                                     │
            ┌────────────────────────┼────────────────────────┐
            ▼                        ▼                        ▼
       explicit use            using ns::name           using namespace ns
       ns::foo()                bring one name in        bring all names in
       (preferred,              (ok in .cpp,             (NEVER in headers;
        especially in           targeted)                 sparingly even
        headers)                                          in .cpp)

       open: many files contribute to one namespace.
       nested: namespace a::b — write 'a::b::name'.
       alias: namespace short = long;
       anonymous: file-local — replaces 'static' for symbols.

       std::  is the standard library namespace.
       ADL:   compiler also searches an argument's namespace
              for matching overloads.
```

---

## 13. Practice

1. Why is `using namespace std;` in a header a bad idea? *(It pollutes every TU that includes the header; can collide with user names; can break when std adds new names.)*
2. What's the difference between a file-scope `static` function and one in an anonymous namespace? *(Functionally equivalent; anon namespace is the C++ idiom and also works for types.)*
3. What is ADL? *(Argument-dependent lookup — when you call `f(x)` with `x` of namespace-N type, the compiler also searches N for `f`.)*
4. Why is `std::vector<std::vector<int>>` invalid in C++98 but `std::vector<std::vector<int> >` (with the space) valid? *(C++98 parses `>>` as the right-shift operator; the space disambiguates. C++11 fixes the parser.)*
