# `typedef` — Give An Existing Type Another Name

> **TL;DR.** `typedef` introduces an alias for a type. The alias and the original are **the same type** — the compiler treats them identically, no overloading distinguishes them, no conversion is needed. It's a documentation/abbreviation tool.

Related: [`USING.md`](USING.md) · [`STRUCT.md`](STRUCT.md) · [`TEMPLATE.md`](TEMPLATE.md)

---

## 1. The basic form

```cpp
typedef <existing-type>  <new-name>;
```

Examples:

```cpp
typedef unsigned char        byte;
typedef long long            i64;
typedef std::vector<int>     IntVec;
typedef std::map<std::string, int> StringIntMap;
typedef int (*FnPtr)(int);                    // function pointer alias
typedef void (Container::*MemberFn)();        // pointer-to-member alias
```

After this:

```cpp
byte       b = 200;             // same as unsigned char b = 200;
IntVec     v;                   // same as std::vector<int> v;
FnPtr      f = &square;         // same as int (*f)(int) = &square;
```

---

## 2. Where the new name goes — read it like a declaration

The trick to reading `typedef` is to **read it as if you were declaring a variable, then replace the variable name with the type alias**:

```cpp
int (*f)(int);            // 'f' is a pointer to a function taking int and returning int.
typedef int (*FnPtr)(int);// FnPtr is the TYPE: pointer to that kind of function.

FnPtr g = &square;        // g is a FnPtr (a function pointer)
```

```cpp
char buf[10];                  // 'buf' is array of 10 chars
typedef char Buffer[10];       // 'Buffer' is the TYPE: array of 10 chars

Buffer x;                      // declares 'x' as char[10]
```

```cpp
int (Container::*p)();              // p is a pointer-to-member-function
typedef int (Container::*MemFn)();  // MemFn is that type
```

This works for *any* declaration. `typedef` literally transforms the variable name into a type alias.

---

## 3. The compiler view — pure aliasing

A `typedef` is **not a new type**. It's a synonym. The compiler treats the alias and the original interchangeably:

```cpp
typedef int Distance;
typedef int Mass;

void f(Distance d);
void f(Mass m);             // ERROR — same signature: void f(int)
```

Both names mean `int`. You can't overload on them. They're identical from the type system's point of view.

This is sometimes annoying (you'd want `Distance` and `Mass` to be distinguishable). Solutions: a wrapper struct, or — in C++11+ — `enum class`. In C++98 it's a real limitation.

---

## 4. Why use `typedef`?

### 4.1 Shorten long template types

```cpp
typedef std::map<std::string, std::vector<int> > NamedSeries;

NamedSeries data;                                // not std::map<std::string, std::vector<int> > data;
NamedSeries::iterator it = data.begin();         // also way shorter
```

### 4.2 Document intent

```cpp
typedef int FileDescriptor;
typedef int Bytes;
typedef int Seconds;

FileDescriptor fd = open(...);
Bytes          n  = read(fd, buf, BUF_SIZE);
Seconds        t  = time(0);
```

The compiler still treats them all as `int`, but the reader sees the meaning.

### 4.3 Hide implementation details

```cpp
// MyContainer.hpp
class MyContainer {
public:
    typedef std::map<std::string, int>::iterator iterator;
    typedef std::size_t                           size_type;

    iterator begin();
    size_type size() const;
};
```

If you change the underlying container later, only the typedef updates; client code doesn't break.

### 4.4 Function-pointer types

```cpp
typedef int (*ComparatorFn)(const void*, const void*);

void mySort(void *base, std::size_t n, std::size_t sz, ComparatorFn cmp);
```

Function-pointer syntax is famously ugly; an alias makes signatures readable.

### 4.5 Iterator types in templates

```cpp
template <typename T>
class Stack {
public:
    typedef T*       iterator;
    typedef const T* const_iterator;
    typedef std::size_t size_type;
    typedef T        value_type;
};
```

This is the standard STL pattern. Code can then write `Stack<int>::value_type` and stay generic.

---

## 5. `typedef` with structs/classes — C-style trick

In C, `struct s_node` requires the `struct` keyword. Combining with `typedef` was the convention:

```c
typedef struct s_node {
    int data;
    struct s_node *next;
} t_node;

t_node n;             // shorter than 'struct s_node n;'
```

In C++, the class/struct name *is* the type — no `struct` prefix needed:

```cpp
struct Node {
    int data;
    Node *next;
};

Node n;               // works directly
```

So **you don't need `typedef struct` in C++**. Just `struct Name { … };`.

---

## 6. C++11 alternative: `using` (not in 42 C++98)

C++11 added a cleaner alias syntax:

```cpp
using FnPtr = int (*)(int);                       // C++11 — same as typedef
using IntVec = std::vector<int>;
```

Reads left-to-right: "FnPtr is an alias for `int(*)(int)`." Handles templates better (alias templates, see below).

In 42 C++98, stick to `typedef`.

---

## 7. Alias templates (C++11+, not 42)

```cpp
template <typename T>
using Vec = std::vector<T>;

Vec<int>    v;        // same as std::vector<int>
```

`typedef` cannot be parameterized like this in C++98. You'd need a struct wrapper:

```cpp
template <typename T>
struct Vec {
    typedef std::vector<T> type;
};

Vec<int>::type v;     // clunky but works in C++98
```

---

## 8. Hardware/codegen view

`typedef` has zero runtime impact. There's no separate type, no conversion, no metadata. The compiler resolves the alias before any codegen:

```
   typedef int  Distance;
   Distance d = 5;     →   int d = 5;     // same code as if you wrote int directly
```

Same alignment, same layout, same calling convention.

---

## 9. Tips & tricks

### 9.1 Use `typedef` inside classes for STL-style interfaces

```cpp
template <typename T>
class List {
public:
    typedef T              value_type;
    typedef T&             reference;
    typedef const T&       const_reference;
    typedef T*             pointer;
    typedef const T*       const_pointer;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;
};
```

This lets generic algorithms ask the container "what's your value type?" via `Container::value_type`.

### 9.2 Don't rename built-in types pointlessly

```cpp
typedef int   Integer;          // no value
typedef char  Character;        // no value
```

Rename built-ins only when adding **semantic** info (`FileDescriptor`, `Bytes`).

### 9.3 Spell function pointers with typedefs

```cpp
// painful:
void register_callback(void (*fn)(int, const char *), void *user);

// readable:
typedef void (*Callback)(int, const char *);
void register_callback(Callback fn, void *user);
```

### 9.4 Typedef-name vs class name — both are types

```cpp
struct S { int x; };
typedef S Synonym;

S        a;
Synonym  b;             // identical type — these vars are the same kind of thing
```

You can pass `a` to a function taking `Synonym` and vice versa.

### 9.5 The C++ "elaborated type specifier" interaction

```cpp
typedef int X;
class X { ... };          // this declares a *class* X, separate from the typedef

X x;                       // ambiguity? — looks up the class first
typedef X y;               // typedef-name lookup
```

Don't shadow typedefs with class names. It's legal but confusing.

---

## 10. Common errors

| Error | Cause | Fix |
|---|---|---|
| `redefinition of 'X'` when typedef'ing the same name twice | Two typedefs in the same scope | Either remove the duplicate or guarantee they're in different scopes |
| Function pointer typedef syntax errors | Misplaced parens | `typedef Ret (*Name)(Args);` — the parens around `*Name` are required |
| `'X' has not been declared` | Used a typedef before declaring it | Move the `typedef` above its first use |
| Overload conflict between two typedefs | Aliases of the same underlying type | `typedef` doesn't introduce new types — use a wrapper struct |

---

## 11. Visual summary

```
              ┌────────────────────────────────────────────┐
              │   typedef <type> <alias>;                   │
              │                                              │
              │   the alias and <type> are the SAME TYPE.   │
              │   no new type. no overload distinction.     │
              │   resolved by the compiler before codegen.  │
              └────────────────────┬───────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────┐
              ▼                    ▼                     ▼
        readability            implementation        function /
        ─────────────          hiding                pointer aliases
        long template          ─────────────         ───────────────
        types                  client uses           int (*F)(int);
        intent                 typedef'd name        typedef int (*F)(int);
        documenting            internal type can     ──────────────────
        units                  change without
                              breaking client.

       reads like a declaration: replace the variable name with
       the alias name and that's the type.
       
       in C++, struct/class names are already types — no need
       for 'typedef struct s_X {…} t_X;'.
```

---

## 12. Practice

1. Why are `typedef int Distance;` and `typedef int Mass;` not distinguishable for overloading? *(`typedef` aliases an existing type; both are still `int`. Type identity is what overloading uses.)*
2. How would you alias a function pointer type? *(`typedef Ret (*Name)(Args);` — note parens around `*Name`.)*
3. Why is `typedef struct s_node { … } t_node;` unnecessary in C++? *(In C++, `struct Name` already declares `Name` as a type; the `typedef` adds nothing.)*
4. Why does the C++11 `using` syntax (not 42) feel cleaner? *(It reads left-to-right and supports alias templates: `template <typename T> using Vec = std::vector<T>;`.)*
