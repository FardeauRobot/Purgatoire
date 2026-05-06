# `using` — Three Different Jobs Sharing One Keyword

> **TL;DR.** `using` does **three** unrelated things: (1) bring a single name from a namespace into the current scope; (2) bring an *entire* namespace's contents in (`using namespace`); (3) re-expose an inherited base member in a derived class. The C++11 `using`-as-typedef is a fourth job (not available in 42 C++98).

Related: [`NAMESPACE.md`](NAMESPACE.md) · [`TYPEDEF.md`](TYPEDEF.md) · [`INHERITANCE.md`](../oop/INHERITANCE.md)

---

## 1. Job #1 — `using`-declaration (single name)

```cpp
using std::cout;
using std::endl;

cout << "hello" << endl;     // unqualified now
```

Brings `std::cout` into the current scope. After the `using` line, `cout` and `std::cout` refer to the same thing.

```
  before:                     after:
  ─────────                   ──────
  cout         → not found    cout       → std::cout
  std::cout    → found
```

### When to use it

- Inside a `.cpp` for one or two heavily-used names (`cout`, `endl`, `string`).
- Inside a function body for local convenience.

### When NOT to use it

- In a header. Every `.cpp` that includes the header inherits the bring-in, polluting their scopes.
- For names that conflict with local code (`std::min`, `std::distance` — common pitfalls).

---

## 2. Job #2 — `using`-directive (whole namespace)

```cpp
using namespace std;

cout << "hi";
string s;
vector<int> v;
```

Brings **everything** from `std` into the current scope. Quick to type, slow to debug. Names are now drawn from a much larger pool, and ambiguities are easy.

### Strict 42 advice

**Never `using namespace std;` in a header.** Every TU that includes the header gets the dump.

In `.cpp` files it's tolerated by some norms, frowned on by others. The safest habit: write `std::` explicitly.

```cpp
// 42-clean
std::cout << "..." << std::endl;
std::string s;
std::vector<int> v;
```

---

## 3. Job #3 — re-expose an inherited member

```cpp
class Base {
public:
    void f(int);
};

class Derived : public Base {
public:
    void f(double);    // hides Base::f(int) by name
};

Derived d;
d.f(1.0);              // OK
d.f(42);               // calls f(double), with int → double conversion
                        // does NOT call Base::f(int) anymore — it's hidden by name
```

Adding `using` brings `Base::f(int)` back into Derived's scope:

```cpp
class Derived : public Base {
public:
    using Base::f;     // pull Base::f into Derived's namespace
    void f(double);
};

Derived d;
d.f(42);               // now calls Base::f(int)
d.f(1.0);              // calls Derived::f(double)
```

This is **name lookup**, not access. The member function would still be inaccessible if it were `private` — `using` doesn't grant access, it brings the name into scope so name resolution finds it.

### Use case: changing access of an inherited member

```cpp
class Base {
public:
    void publicFn();
};

class Derived : private Base {
public:
    using Base::publicFn;     // re-promote to public in Derived
};

Derived d;
d.publicFn();                  // OK — publicFn is public in Derived now
```

This is a textbook trick for selectively exposing parts of a privately-inherited base.

---

## 4. Job #4 — `using` as `typedef` alternative (C++11+, NOT in 42 C++98)

```cpp
using FnPtr  = int (*)(int);                  // C++11 — same as typedef
using IntVec = std::vector<int>;

template <typename T>                         // alias templates — typedef can't do this
using Vec = std::vector<T>;
```

Cleaner than `typedef` for function pointers, supports templates. In 42 C++98 you don't have this; use `typedef`.

---

## 5. Compiler view — `using` is name lookup only

`using` declarations and directives **do not generate code**. They modify the compiler's name lookup tables for the current scope:

```
   before 'using std::cout;':
       lookup 'cout'  → not found
       lookup 'std::cout' → found

   after 'using std::cout;':
       lookup 'cout'  → resolves via the using-decl to std::cout
       lookup 'std::cout' → still found
```

The actual symbol in the binary is unchanged. There's no aliasing at the linker level — both names point at the same `_ZSt4cout` symbol.

---

## 6. Tips & tricks

### 6.1 Header hygiene

Never put `using` declarations or directives at namespace scope in a header. Always put them inside functions or classes — or just qualify the names explicitly.

### 6.2 Targeted > directive

```cpp
// preferred
using std::cout;
using std::endl;

// avoid
using namespace std;
```

You opt in to the names you actually use; surprises are minimized.

### 6.3 In templates, prefer qualification

```cpp
template <typename T>
void f(T value) {
    std::cout << value;          // explicit — no surprise from a using somewhere upstream
}
```

Templates are instantiated in the user's TU; whatever `using` declarations are around at instantiation time can affect lookup. Be explicit.

### 6.4 Re-exposing operators

```cpp
class Base {
public:
    Base& operator=(const Base&);
};

class Derived : public Base {
public:
    using Base::operator=;
    Derived& operator=(const Derived&);
};
```

Without `using Base::operator=`, the derived class's `operator=` would shadow the inherited one entirely. Sometimes you want both — `using` re-exposes them.

### 6.5 Multi-level inheritance

You can `using` a base member through multiple levels:

```cpp
class A { public: void f(); };
class B : public A {};
class C : public B {
public:
    using A::f;          // legal, even though f comes from grandparent
};
```

### 6.6 Don't `using namespace` inside a class

```cpp
class C {
    using namespace std;     // ERROR — directives are not allowed at class scope
};
```

`using namespace` only goes at namespace scope or function scope. `using std::cout;` (a using-*declaration*, not a directive) is allowed at class scope.

---

## 7. Common errors

| Error | Cause | Fix |
|---|---|---|
| `'cout' was not declared in this scope` | No `using` and no `std::` prefix | Either `using std::cout;` or `std::cout` |
| Ambiguous overload after `using namespace std;` | std functions colliding with your code | Use targeted `using` or qualify explicitly |
| Inherited base function not found in Derived | Hidden by a same-named override | `using Base::name;` to re-expose |
| `'using namespace' allowed only at namespace and function scope` | Tried inside a class | Switch to `using std::name;` form |

---

## 8. Visual summary

```
              ┌──────────────────────────────────────────────┐
              │   keyword: using                              │
              └────────────────────┬─────────────────────────┘
                                   │
        ┌─────────────────┬────────┴────────┬────────────────┐
        ▼                 ▼                 ▼                ▼
   using ns::name     using namespace ns   using Base::fn    using A = T;
   (declaration)      (directive)          (re-expose        (C++11 alias)
                                            inherited)
   ─────────────────  ─────────────────    ─────────────     ──────────────
   bring one name     bring all names      undo name         alternative to
   from a namespace   from a namespace     hiding when a     typedef. supports
   into scope.        into scope.          derived class     template aliases.
   targeted —         broad — avoid in     overrides only    not in C++98.
   safer.             headers.             one overload.

   using is name-lookup only. zero runtime cost. zero new symbols.
```

---

## 9. Practice

1. Why is `using namespace std;` in a header dangerous? *(Every TU that includes the header inherits the dump → name collisions, surprising overloads.)*
2. What does `using Base::f;` do in a `Derived` class that defines its own `f`? *(Re-exposes the base overloads; without it, the derived `f` shadows the base `f` by name.)*
3. Is `using` resolved at runtime? *(No — it's purely compile-time name lookup.)*
4. In C++98, can you alias `std::vector<int>` with `using`? *(No — that's the C++11 alias syntax. Use `typedef std::vector<int> IntVec;`.)*
