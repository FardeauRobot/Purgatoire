# `typeid` — Ask Runtime: "What Type Is This Object?"

> **TL;DR.** `typeid(expr)` returns a `std::type_info` reference identifying the type. For polymorphic types accessed via base pointer/reference, the answer is the **dynamic type**. For everything else, it's the **static type** computed at compile time.

Related: [`VIRTUAL.md`](VIRTUAL.md) · [`CASTS.md`](CASTS.md) · [`POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md)

---

## 1. The shape

```cpp
#include <typeinfo>

int x = 0;
const std::type_info& ti = typeid(x);
std::cout << ti.name();              // implementation-defined string, e.g. "i" for int

std::cout << (typeid(x) == typeid(int));     // 1 (true)
std::cout << (typeid(x) == typeid(double));  // 0 (false)
```

Always `#include <typeinfo>`. The result is a reference to a `std::type_info` object — comparable, but not copyable, not constructible.

---

## 2. Static vs dynamic type

```cpp
class Animal { public: virtual ~Animal() {} };
class Dog : public Animal {};

Animal *p = new Dog();
typeid(*p);                  // typeid of Dog — DYNAMIC type, because Animal is polymorphic
typeid(p);                   // typeid of Animal* — STATIC type of the pointer

Animal a;
typeid(a);                   // typeid of Animal — DYNAMIC == STATIC

Animal &r = *p;
typeid(r);                   // typeid of Dog — DYNAMIC for polymorphic ref
```

Rule:

```
   if expr is a glvalue (lvalue or non-temporary) of polymorphic class type
       → dynamic type (looks at vptr at runtime)
   else
       → static type (computed at compile time)
```

**Polymorphic class** = a class with at least one virtual function. If your class has no virtuals, `typeid` always gives the static type.

---

## 3. Worked example

```cpp
class Shape {
public:
    virtual ~Shape() {}
};
class Circle    : public Shape {};
class Square    : public Shape {};

Shape *shapes[3];
shapes[0] = new Circle();
shapes[1] = new Square();
shapes[2] = new Shape();

for (int i = 0; i < 3; i++) {
    if      (typeid(*shapes[i]) == typeid(Circle))  std::cout << "circle\n";
    else if (typeid(*shapes[i]) == typeid(Square))  std::cout << "square\n";
    else                                             std::cout << "shape\n";
}
```

Output:
```
circle
square
shape
```

`typeid(*shapes[i])` reaches through the vptr to find the dynamic type. With non-polymorphic Shape, all three would print "shape."

---

## 4. Hardware/runtime view — vtable carries `type_info`

For polymorphic classes, the compiler attaches a pointer to the type's `std::type_info` *to the vtable*:

```
   Circle's vtable:
   ┌──────────────────────────┐
   │  &Circle's type_info     │  ← typeid(circle) reads this
   │  &Circle::~Circle        │
   │  &Circle::draw           │
   │  ...                     │
   └──────────────────────────┘

   Square's vtable:
   ┌──────────────────────────┐
   │  &Square's type_info     │
   │  &Square::~Square        │
   │  ...                     │
   └──────────────────────────┘
```

So `typeid(*p)` for polymorphic `p` does:

```
   1. Load vptr from *p (one memory read).
   2. Read the type_info pointer from the vtable (one memory read).
   3. Return reference to the type_info.
```

For non-polymorphic types, `typeid(x)` is purely compile-time — the compiler knows the static type and inserts a reference to a fixed `type_info` object.

The `type_info` object itself is a global, constructed at program startup, with a unique-per-type address. That's why `==` between two `type_info` references is just a pointer comparison — fast.

---

## 5. `name()` — implementation-defined

```cpp
typeid(int).name();              // "i" (gcc/clang Itanium ABI), "int" (MSVC)
typeid(std::string).name();      // a long mangled string on gcc
```

`name()` returns a `const char*` that the implementation chooses — typically the mangled name. For human-readable form on gcc/clang, use `c++filt`:

```bash
$ ./prog | c++filt -t
```

Or `abi::__cxa_demangle` programmatically. Not portable, not standardized.

Use `name()` for debugging only. For type comparison, use `operator==`:

```cpp
if (typeid(x) == typeid(int)) { ... }
```

---

## 6. `typeid` and exceptions

`typeid` can throw two exceptions:

### 6.1 `std::bad_typeid` — null pointer dereference

```cpp
Shape *p = 0;
typeid(*p);              // throws std::bad_typeid (only when *p is polymorphic)
```

If the operand is a polymorphic deref of a null pointer, you get `std::bad_typeid`. For non-polymorphic types, `typeid(*p)` doesn't actually deref (it's a static lookup) — but the standard still says behavior is undefined; in practice most compilers don't throw.

### 6.2 `std::bad_cast`

Not from `typeid` directly — from `dynamic_cast`. See [`CASTS.md`](CASTS.md).

---

## 7. `typeid` vs `dynamic_cast`

```cpp
Animal *p = ...;
if (typeid(*p) == typeid(Dog)) {
    // it IS exactly a Dog
    Dog *d = static_cast<Dog*>(p);
}

if (Dog *d = dynamic_cast<Dog*>(p)) {
    // it IS a Dog or something derived from Dog
}
```

- `typeid` checks for **exact** type equality.
- `dynamic_cast` checks for **convertibility** (works for derived classes too).

Use `dynamic_cast` to ask "is this a Dog or anything derived from Dog?" Use `typeid` to ask "is this *exactly* a Dog?"

In practice: `dynamic_cast` is the preferred tool. `typeid` for type identification is rarer.

---

## 8. Tips & tricks

### 8.1 Always include `<typeinfo>`

Forgetting the include is the #1 cause of "`type_info` is not a member of `std`" errors.

### 8.2 Use it for debugging, not for control flow

```cpp
// debugging:
std::cout << "got a: " << typeid(*ptr).name() << '\n';

// control flow:
if (typeid(*ptr) == typeid(Dog)) { ... }     // brittle — adds new types and you forget to update
```

If you're switching on type, ask whether a virtual function would do the job better.

### 8.3 RTTI can be disabled — be aware

g++ has `-fno-rtti` to skip emitting type_info. Some embedded codebases do this for size. If RTTI is off, `typeid` and `dynamic_cast` may not work or may give limited info. Standard 42 builds keep RTTI on.

### 8.4 Comparing across translation units

`type_info` objects are unique per type **within a program** — but if your program loads dynamic libraries (.so, .dll), each library may have its own copy of the same `type_info`. Comparing by pointer can yield false negatives. The standard provides `before()` for ordering and recommends comparing via `==`/`!=`, which the implementation must make work across libraries on most platforms.

For 42 modules (single binary, no dlopen), this is irrelevant.

### 8.5 type_info is not copyable

```cpp
std::type_info ti = typeid(int);    // ERROR
std::type_info &ref = typeid(int);  // OK — bind a reference
```

Always store it by reference (or pointer to const), never by value.

---

## 9. Common errors

| Error | Cause | Fix |
|---|---|---|
| `'type_info' is not a member of 'std'` | Missing `#include <typeinfo>` | Add the include |
| `typeid(*p)` returned the static type | Class has no virtual function | Add a virtual destructor (or any virtual fn) — required to make it polymorphic |
| Surprising `name()` like `"i"` instead of `"int"` | Implementation-defined; gcc/clang use mangled names | Demangle for display only; use `==` for comparison |
| `bad_typeid` thrown | Dereferenced null pointer in polymorphic typeid | Check for null first |

---

## 10. Visual summary

```
              ┌──────────────────────────────────────────────┐
              │    typeid(expr)                              │
              │      → const std::type_info&                  │
              │                                                │
              │    polymorphic glvalue → dynamic type         │
              │    everything else      → static type         │
              └────────────────────┬───────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────────┐
              ▼                    ▼                         ▼
       == comparison           .name()                   storage
       ─────────────           ────────                   ───────
       fast pointer            implementation-           type_info objects
       comparison.             defined string            live in static memory
       use to ask "is          (mangled on gcc).         per type. vtable
       this exactly type X?"   for debugging only.       carries the pointer
                                                          for polymorphic types.

       must #include <typeinfo>.
       compare pointers via reference: const std::type_info&.
       throws bad_typeid on null polymorphic deref.
```

---

## 11. Practice

1. Why is `typeid(*p)` for `Animal *p = new Dog();` Dog when Animal has a virtual destructor, but Animal otherwise? *(Polymorphic types use the vptr to find dynamic type at runtime; non-polymorphic types use the static type known at compile time.)*
2. What does `typeid(int).name()` return? *(Implementation-defined — on gcc/clang it's `"i"`; on MSVC it's `"int"`. Don't rely on the format.)*
3. When is `typeid` less useful than `dynamic_cast`? *(When you want to know "is it Dog *or any subclass of Dog*" rather than "is it exactly Dog".)*
4. Why is the result of `typeid` returned as a *reference*? *(`std::type_info` is not copyable; a single `type_info` object exists per type and is referenced from multiple places.)*
