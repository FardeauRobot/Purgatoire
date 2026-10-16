# 📖 GLOSSAIRE — The C++ Vocabulary, Decoded

> A guided dictionary of every technical **concept** you'll meet in the 42 CPP modules — RAII, lvalue, vtable, deep copy… Each entry: a **one-line definition**, a **sharp example**, a **mental picture**, and a **link** to the deep-dive file.
>
> Looking for a language **keyword** (`const`, `switch`, `virtual`…)? That's [`INDEX.md`](INDEX.md) — one page each, all 63 covered.
> Read this page linearly to build a map. Hop into any link when a term gets fuzzy.

```
                ┌──────────────────────────────────────┐
                │              C++  =                  │
                │  C  +  Classes  +  Templates  +  STL │
                │  +  RAII   +  Exceptions   +  RTTI   │
                └──────────────────────────────────────┘
                          ▲       ▲       ▲
                          │       │       │
                       SAFETY   REUSE  ABSTRACTION
                       (no UB) (generic) (objects)
```

**The library, at a glance:**

| Section | What lives there |
|---|---|
| [`lexique/`](INDEX.md) | You are here — all 63 keywords + this concept glossary |
| [`notions/`](../notions/INDEX.md) | Concepts by theme: fundamentals, OOP, STL, I/O, tooling |
| [`projets/`](../projets/INDEX.md) | The 42 CPP00→CPP04 walkthroughs + webserv |

---

## 🗺️ Master map

```
                       ┌───────────────────────┐
                       │   YOUR SOURCE FILE    │
                       └──────────┬────────────┘
                                  │
           ┌──────────────────────┼──────────────────────┐
           │                      │                      │
       PREPROCESSOR           COMPILER                 LINKER
       #include / #define     .cpp → .o               .o → executable
                                  │
                ┌─────────────────┼─────────────────┐
                │                 │                 │
              TYPES            OBJECTS           TEMPLATES
              int, bool,       class, struct,    template<T>
              char, ...        vtable, ...       monomorphized
                │                 │                 │
                └─────► RAII ◄────┴────► STL ◄──────┘
                       (lifetime)    (reusable code)
```

---

## 🔤 Alphabetical quick-jump

| | |
|:---:|---|
| **A** | [Abstract class](#Abstract%20class) · [Abstraction](#Abstraction) · [Aggregate](#Aggregate) · [Allocator](#Allocator) · [API](#API) · [Argument](#Argument%20vs%20Parameter) · [Assignment op](#Assignment%20operator) · [Attribute](#Attribute) |
| **B** | [Base class](#Base%20class) · [Block](#Block) · [Bool](#Bool) · [Build](#Build) |
| **C** | [Cast](#Cast) · [Class](#Class) · [Composition](#Composition) · [Const](#Const) · [Const-correctness](#Const-correctness) · [Constructor](#Constructor) · [**Contract**](#Contract) · [Copy ctor](#Copy%20constructor) |
| **D** | [Dangling pointer](#Dangling%20pointer) · [Data member](#Attribute) · [Declaration](#Declaration%20vs%20Definition) · [Deep copy](#Deep%20vs%20Shallow%20copy) · [Definition](#Declaration%20vs%20Definition) · [Delete](#Delete) · [Destructor](#Destructor) · [Diamond problem](#Diamond%20problem) |
| **E** | [Encapsulation](#Encapsulation) · [Enum](#Enum) · [Exception](#Exception) · [Explicit](#Explicit) · [Expression](#Statement%20vs%20Expression) · [Extern](#Extern) |
| **F** | [Field](#Attribute) · [Forward declaration](#Forward%20declaration) · [Friend](#Friend) · [Functor](#Functor) |
| **G** | [Getters/Setters](#Getters%20and%20setters) |
| **H** | [Header](#Header%20file) · [Header guard](#Header%20guard) · [Heap](#Heap) |
| **I** | [Identifier](#Identifier) · [Implementation](#Interface) · [Inheritance](#Inheritance) · [Initialization](#Initialization%20vs%20Assignment) · [Initializer list](#Initializer%20list) · [Inline](#Inline) · [Instance](#Instance) · [Interface](#Interface) · [Iterator](#Iterator) |
| **L** | [Lifetime](#Lifetime) · [Linkage](#Linkage) · [Literal](#Literal) · [Lvalue](#Lvalue) |
| **M** | [Macro](#Preprocessor) · [Member function](#Method) · [Memory leak](#Memory%20leak) · [**Method**](#Method) · [Mutable](#Mutable) |
| **N** | [Name mangling](#Name%20mangling) · [Namespace](#Namespace) · [New](#New) |
| **O** | [Object](#Object) · [OCF](#Orthodox%20Canonical%20Form%20%28OCF%29) · [ODR](#One%20Definition%20Rule%20%28ODR%29) · [Operator overload](#Operator%20overloading) · [Overload](#Overload) · [Override](#Override) |
| **P** | [Parent class](#Subclass) · [POD](#Aggregate) · [Pointer](#Pointer) · [Polymorphism](#Polymorphism) · [Preprocessor](#Preprocessor) · [Private](#Access%20specifiers) · [Pure virtual](#Pure%20virtual) |
| **R** | [RAII](#RAII) · [Reference](#Reference) · [RTTI](#RTTI) · [Rvalue](#Lvalue) |
| **S** | [Scope](#Scope) · [Shallow copy](#Deep%20vs%20Shallow%20copy) · [Signature](#Signature) · [Sizeof](#Sizeof) · [Slicing](#Object%20slicing) · [Stack](#Stack) · [Statement](#Statement%20vs%20Expression) · [Static](#Static) · [STL](#STL) · [Stream](#Stream) · [String](#String) · [Struct](#Struct) · [Subclass](#Subclass) · [Superclass](#Subclass) |
| **T** | [Template](#Template) · [This](#This) · [Throw](#Throw) · [Translation unit](#Translation%20Unit) · [Try/catch](#Try%20and%20catch) · [Type](#Type) · [Typedef](#Typedef) · [Typeid](#Typeid) · [Typename](#Typename) |
| **U** | [Undefined behavior](#Undefined%20behavior%20%28UB%29) · [Using](#Using) |
| **V** | [Virtual](#Virtual) · [Void](#Void) · [Volatile](#Volatile) · [Vptr](#Vtable) · [Vtable](#Vtable) |

---

## 🌉 Synonyms — bridges between casual and C++ vocabulary

You'll hear these everyday words from teachers, blog posts, and other languages. Here's what each one is *actually called* in C++.

| You may say… | C++ calls it… | One-liner |
|---|---|---|
| **Method** | Member function | A function that belongs to a class. Same thing. |
| **Field** / **Attribute** / **Property** | Data member / Member variable | A variable that lives inside an object. |
| **Instance** | Object | A specific living example of a class. |
| **Subclass** / **Child class** | Derived class | The one that inherits. |
| **Superclass** / **Parent class** | Base class | The one being inherited from. |
| **Interface** | Pure virtual / Abstract class | A type that says **what** but not **how**. |
| **Implementation** | Definition (of a class/function) | The **how** part. |
| **API** | Public interface | The set of public methods/types you publish. |
| **Header** | Declaration file (`.hpp`) | What other files `#include`. |
| **Cpp file** | Translation unit (after preprocessing) | One `.cpp` + everything it includes. |
| **Build** | Preprocess + compile + link | The pipeline from `.cpp` → executable. |
| **Library function** | Function from a library / STL | E.g. `std::sort` is *from the STL*. |
| **Variable** | Object (in standard-speak) | Yes — even `int x;` is technically an "object". |
| **Type-cast** / **Conversion** | Cast | The four flavors live in [`CASTS.md`](CASTS.md). |

> **Mental rule:** when a 42 evaluator (or a textbook) says *method*, they mean what C++ calls a **member function**. They're interchangeable.

---

## 🧱 1. Foundations & syntax

> The bricks. If these are wobbly, nothing else stacks.
> 📚 Deep dive: [`fundamentals/BASICS.md`](../notions/fundamentals/BASICS.md) · [`fundamentals/CONTRACTS.md`](../notions/fundamentals/CONTRACTS.md)

### Contract
A **promise between two pieces of code** — caller↔callee, class↔user, you↔compiler. Most C++ syntax (`const`, `private`, `virtual`, `explicit`, `noexcept`…) is the language for writing contracts the compiler enforces. 📚 [`fundamentals/CONTRACTS.md`](../notions/fundamentals/CONTRACTS.md)

```cpp
int sum(const std::vector<int>& v);
//      ─────────────────────── 3 promises in one signature:
//                              don't copy · don't modify · return an int
```

```
   CALLER  ─── preconditions ─►  CALLEE
   CALLER  ◄── postconditions ── CALLEE
            break it ⇒ UB / compile error
```

### Type
A label that tells the compiler **(a) how many bytes** to reserve and **(b) what operations** are legal. Types are checked at compile time — that's why C++ catches mistakes C never sees.

```cpp
int    i = 42;       // 4 bytes,  arithmetic
double d = 3.14;     // 8 bytes,  floating ops
bool   b = true;     // 1 byte,   logical ops
```

### Object
Any region of memory with a type and a lifetime. Not just "instance of a class" — `int x;` is also an object.

```
┌──────────┐       memory:  [.... 42 ....]   ← 4 bytes
│  int x   │       lifetime: from `int x;` to `}`
└──────────┘
```

### Scope
The textual region where a name is visible. Three flavors: **block** (`{}`), **class**, **namespace**.

```cpp
int g = 1;           // namespace scope (global)
void f() {
    int x = 2;       // block scope — dies at `}`
    {
        int x = 3;   // shadows outer x
    }
}
```

### Argument vs Parameter
**Parameter** = the placeholder in the function signature. **Argument** = the actual value at the call site.

```cpp
void greet(std::string name);   // `name` is the parameter
greet("Alice");                  // "Alice" is the argument
```

### Header file
A `.hpp` (or `.h`) file containing **declarations** that other files `#include`. Its job is to teach the compiler what exists, not how it's implemented.

```
       header (.hpp)              source (.cpp)
       ┌──────────────┐          ┌──────────────┐
       │ class Foo {  │          │ #include     │
       │   void bar();│  ←──┐    │ "Foo.hpp"    │
       │ };           │     └────┤              │
       └──────────────┘          │ void Foo::   │
                                 │ bar() {...}  │
                                 └──────────────┘
```
Header guards (`#ifndef FOO_HPP`) prevent multiple-inclusion explosions.

### Namespace
A logical box for names. Solves "two libraries both have a `print`" collisions. 📚 [`lexique/NAMESPACE.md`](NAMESPACE.md)

```cpp
namespace ft { int abs(int x) { return x < 0 ? -x : x; } }
ft::abs(-5);            // explicit
using ft::abs;          // bring one in
```

### Using
Three jobs: bring a single name in, bring a whole namespace in, or re-expose a base member. 📚 [`lexique/USING.md`](USING.md)

### Typedef
Alias an existing type. Pure compile-time — no new type is created. 📚 [`lexique/TYPEDEF.md`](TYPEDEF.md)

```cpp
typedef unsigned long size_t;   // size_t IS unsigned long
```

### Bool
A real boolean type. `true == 1`, `false == 0`. Use `std::boolalpha` to print words. 📚 [`lexique/BOOL.md`](BOOL.md)

```cpp
std::cout << std::boolalpha << (1 < 2);   // "true"
```

### Void
"No value." Three uses: function return type, generic pointer (`void*`), empty parameter list. 📚 [`lexique/VOID.md`](VOID.md)

### Enum
Named integer constants. Becomes type-safe with `enum class` (C++11). 📚 [`lexique/ENUM.md`](ENUM.md)

```cpp
enum Level { DEBUG, INFO, WARN, ERROR };  // 0,1,2,3
```

### Identifier
Any name you give to a variable, function, type, etc. Letters/digits/underscore, no leading digit, case-sensitive.

### Block
A `{ … }` region. Defines a scope. Variables born inside die at `}`.

### Statement vs Expression
**Expression** = produces a value (`a + b`, `f(x)`). **Statement** = does something (`int x = 1;`, `if (...) {}`). Most expressions become statements when followed by `;`.

```cpp
a + b;          // expression statement (value discarded)
int x = a + b;  // declaration statement
```

### Literal
A value typed directly in source: `42`, `3.14`, `"hello"`, `'a'`, `true`. Hardcoded literals scattered in code are called **magic numbers** — name them with `const` instead.

```cpp
const int MAX_RETRIES = 3;   // ✅ named
for (int i = 0; i < 3; ++i)  // ⚠️ what's 3?
```

### Initialization vs Assignment
**Initialization** = giving a value to an object **at birth**. **Assignment** = changing an object's value **after** it exists. Different operations, different operators in your class (copy constructor vs `operator=`).

```cpp
int x = 5;      // initialization
x = 10;         // assignment

Dog a("Rex");   // initialization (calls constructor)
Dog b = a;      // initialization (copy constructor — surprise!)
b = a;          // assignment (operator=)
```

### Lifetime
The window during which an object exists in memory. Determined by where you declare it:

| Storage | Born at | Dies at |
|---|---|---|
| Automatic (stack) | declaration `{` | matching `}` |
| Static / global | program start | program end |
| Dynamic (heap) | `new` | `delete` |
| Member sub-object | enclosing object's birth | enclosing object's death |

> RAII = "tie a resource to a lifetime so cleanup is automatic."

---

## 🔧 2. Compilation model — what your code goes through

> Before your program runs, four tools have already touched it.
> 📚 Deep dive: [`tooling/MAKEFILE_CPP.md`](../notions/tooling/MAKEFILE_CPP.md) · [`tooling/LIBRARIES.md`](../notions/tooling/LIBRARIES.md)

```
   .cpp + .hpp ──► [PREPROCESSOR] ──► [COMPILER] ──► .o ──► [LINKER] ──► executable
                       ▲                  ▲                    ▲
                  expand #includes,    parse, type-check,    resolve names
                  expand macros        emit assembly         across .o files
```

### Build
**Build = preprocess → compile → link.** Three phases people often blur:
1. **Preprocess** — text substitution (`#include`, `#define`, `#ifdef`).
2. **Compile** — turn each `.cpp` into a `.o` object file (machine code with unresolved refs).
3. **Link** — stitch all `.o` files (and libraries) into one executable.

### Preprocessor
The text-substitution stage that runs before compilation. **Macros** (`#define`) are its blind text replacements — they don't know about types or scopes. Use sparingly.

```cpp
#define MAX(a,b) ((a)>(b)?(a):(b))   // works… mostly
MAX(i++, j++);                        // 💥 i or j incremented twice
const int LIMIT = 100;                // ✅ prefer typed const
```

### Header guard
`#include "foo.hpp"` literally pastes the file's contents in place. To avoid pasting twice (and breaking the One Definition Rule), every header gets a guard:

```cpp
#ifndef FOO_HPP
#define FOO_HPP
// declarations…
#endif
```

### Translation Unit
One `.cpp` file **plus** every file it transitively `#include`s, after the preprocessor finishes. The compiler works one TU at a time. Linker stitches them together.

```
   foo.cpp  +  iostream  +  Foo.hpp  +  string  →  one big TU
                          ↓
                     compiler
                          ↓
                       foo.o
```

### Declaration vs Definition
The single most clarifying distinction in the language.

- **Declaration** = "this name exists with this type." Doesn't allocate.
- **Definition** = "this is the thing." Allocates / provides the body.

```cpp
extern int g;            // declaration — promise
int g = 42;              // definition — actual storage

void f();                // declaration — function exists
void f() { /* … */ }     // definition — function body

class Foo;               // forward declaration
class Foo { int x; };    // definition
```

You can declare many times. You can **define exactly once** (per program for objects/funcs, per TU for types). That's the…

### One Definition Rule (ODR)
Every entity must have exactly **one definition** across the whole program. Violating it = ill-formed program (linker errors or, worse, silently miscompiled). Headers contain *declarations*; `.cpp` files contain *definitions*. `inline` and templates are special — they may live in headers because the compiler treats their definitions as ODR-compatible.

### Forward declaration
A *declaration without the body*, used to break circular dependencies or to avoid pulling a heavy header.

```cpp
class Window;                  // forward declaration
void render(Window* w);        // pointer/ref is enough — no body needed
```

### Signature
A function's identity to the compiler: **name + parameter types** (return type doesn't count, except for templates). Two functions can coexist if their signatures differ — that's overloading.

```cpp
int  f(int);          // signature: f(int)
int  f(double);       // different signature — OK to overload
double f(int);        // 💥 same signature, conflicting return — error
```

### Linkage
"Can the linker see this name from another translation unit?"

| Linkage | What it means | Examples |
|---|---|---|
| **External** | Name visible across TUs (default for globals/functions) | `int g;` at file scope |
| **Internal** | Name visible only in its own TU | `static int g;`, anonymous namespace |
| **No linkage** | Local — not a linker concern | block-scope locals |

### Name mangling
The compiler **encodes** function signatures into unique linker symbols so overloading and namespaces work. C doesn't do this — that's what `extern "C"` disables.

```cpp
int f(int)        // mangled symbol like  _Z1fi
int f(double)     // mangled symbol like  _Z1fd
extern "C" int f();   // raw symbol         f
```

```
   C++ source        →  mangled symbol      →  linker
   "ft::Vec::push"   →  "_ZN2ft3Vec4pushEi" →  unique
```

This is also why mixing different compiler versions can cause linker hell — they may mangle differently.

---

## 🧠 3. Memory & lifetime

> Where your bytes live. Get this picture wrong → segfaults, leaks, undefined behavior.
> 📚 Deep dive: [`fundamentals/MEMORY.md`](../notions/fundamentals/MEMORY.md)

### Stack
Automatic, fast, LIFO memory. Variables born on the stack die at the closing `}` — for free.

```
   ┌─────────────────┐  ← high addresses
   │   main's frame  │
   ├─────────────────┤
   │   f()'s  frame  │
   ├─────────────────┤
   │   g()'s  frame  │  ← stack pointer
   ├ ─ ─ ─ ─ ─ ─ ─ ─ ┤
   │     (free)      │
   ├─────────────────┤
   │      heap       │
   └─────────────────┘  ← low addresses
```

### Heap
Dynamic memory you ask for and must give back. Slower, but flexible.

```cpp
int *p = new int(42);   // heap
delete p;               // give it back, or you leak
```

### New
Allocate **plus** construct. Returns a typed pointer. 📚 [`lexique/NEW.md`](NEW.md)

```cpp
T *p   = new T;          // single object
T *arr = new T[10];      // array
```

### Delete
Destruct **plus** deallocate. Form must match `new`. 📚 [`lexique/DELETE.md`](DELETE.md)

```cpp
delete p;        // matches new
delete[] arr;    // matches new[]
```

```
   new T   ↔  delete       ✅
   new T[] ↔  delete[]     ✅
   new T   ↔  delete[]     💥 UB
   new T[] ↔  delete       💥 UB
```

### Pointer
A variable that holds an address. Cheap, nullable, reseatable. C-style.

```cpp
int  x = 5;
int *p = &x;    // p stores the address of x
*p = 10;        // x is now 10
```

### Reference
An alias — another name for an existing object. Non-null, non-reseatable. 📚 [`fundamentals/REFERENCE.md`](../notions/fundamentals/REFERENCE.md)

```cpp
int  x = 5;
int &r = x;     // r IS x
r = 10;         // x is now 10
```

```
        Pointer                 Reference
   ┌─────────────────┐    ┌─────────────────┐
   │  p ──► [ 42 ]   │    │  r ═══ [ 42 ]   │
   │  can be null    │    │  cannot be null │
   │  can re-aim     │    │  bound for life │
   └─────────────────┘    └─────────────────┘
```

### Memory leak
Heap allocation that's never `delete`d. The bytes are unreachable until process exit. RAII (below) is the cure.

### Dangling pointer
A pointer whose target was already deleted/destroyed. Reading it = undefined behavior.

```cpp
int *p = new int(7);
delete p;
*p = 42;        // 💥 dangling — p still holds the old address
```

### RAII
**Resource Acquisition Is Initialization.** The single most important C++ idiom: tie any resource (memory, file, lock, socket) to an object's lifetime. The destructor releases it automatically.

```cpp
{
    std::ifstream f("data.txt");  // open
    // use f
}                                  // f.close() called by destructor
```

### Lvalue
**lvalue** = has a name and an address (you can put it on the left of `=`). **rvalue** = a temporary, no persistent address.

```cpp
int x = 42;     // x is an lvalue, 42 is an rvalue
x = 10;         // ✅ assigning to lvalue
42 = x;         // ❌ can't assign to rvalue
```

### Sizeof
Compile-time byte count of a type or expression. 📚 [`lexique/SIZEOF.md`](SIZEOF.md)

```cpp
sizeof(int)   // 4 (typical)
sizeof(char)  // 1 (always)
```

### Undefined behavior (UB)
The compiler is **allowed to do anything** when your code breaks the rules: crash, work fine, format your disk. UB is not a runtime error — it's a contract violation. Examples: dereferencing null, reading uninitialized memory, signed overflow.

---

## 🛡️ 4. Type qualifiers & storage

> Decorations on a type that change the rules of the game.

### Const
"Promise: I won't modify this through this name." 📚 [`lexique/CONST.md`](CONST.md)

```cpp
const int x = 42;       // x cannot be reassigned
int const *p;           // pointer to const int  (read right-to-left)
int *const p;           // const pointer to int
const int *const p;     // both
```

### Const-correctness
The discipline of marking everything that doesn't mutate as `const`. It documents intent **and** lets the compiler catch bugs.

```cpp
class Vec {
public:
    int  size() const;   // ← const member: doesn't modify *this
    void push(int);      // ← non-const: modifies *this
};
```

### Static
Three jobs: 📚 [`lexique/STATIC.md`](STATIC.md)
1. **File-local** linkage (replaces `static` from C)
2. **Persistent** local variable (kept across calls)
3. **Class-wide** member (one copy shared by all instances)

```cpp
class Counter {
    static int count;     // shared
};
int Counter::count = 0;   // must define out-of-class
```

### Mutable
The legitimate "escape hatch" from `const`. Lets one specific member be modified from a `const` method (caches, mutexes). 📚 [`lexique/MUTABLE.md`](MUTABLE.md)

### Volatile
"Don't optimize loads/stores of this." For memory-mapped I/O and signal handlers. **Not** for thread synchronization. 📚 [`lexique/VOLATILE.md`](VOLATILE.md)

### Explicit
Block sneaky single-argument constructor conversions. 📚 [`lexique/EXPLICIT.md`](EXPLICIT.md)

```cpp
class S { public: S(int); };
S a = 5;        // ✅ implicit conversion (surprising)

class T { public: explicit T(int); };
T b = 5;        // ❌ refused
T b(5);         // ✅ direct init still fine
```

### Inline
A hint to the compiler to inline the call **and** a relaxation of the One-Definition Rule for header-defined functions. 📚 [`lexique/INLINE.md`](INLINE.md)

### Extern
"Defined elsewhere — please link, don't allocate." Plus `extern "C"` for C interop. 📚 [`lexique/EXTERN.md`](EXTERN.md)

```cpp
extern "C" int c_func(int);  // disable C++ name mangling
```

---

## 🧬 5. Object orientation

> Bundling **data + behavior** + enforcing **invariants**.
> 📚 Deep dive: [`oop/INHERITANCE.md`](../notions/oop/INHERITANCE.md) · [`oop/POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md) · [`oop/ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

### Class
A user-defined type bundling members (data) and member functions (behavior), private by default. 📚 [`lexique/CLASS.md`](CLASS.md)

```cpp
class Dog {
    std::string name_;            // private data
public:
    Dog(std::string n);           // constructor
    void bark() const;            // behavior
};
```

### Struct
The same thing as `class` — except members are **public by default**. Convention: `struct` for plain-data, `class` for "real" objects with invariants. 📚 [`lexique/STRUCT.md`](STRUCT.md)

### Instance
A specific living example of a class — also called an **object**. `Dog rex;` creates one instance of `Dog`. The class is the cookie cutter; the instance is the cookie.

```
        class Dog              instances
        ┌──────────┐          ┌──────┐ ┌──────┐ ┌──────┐
        │ blueprint│  ──────► │ rex  │ │ buddy│ │ luna │
        └──────────┘          └──────┘ └──────┘ └──────┘
```

### Method
Also called a **member function** — same thing, two names. A function that belongs to a class and gets a hidden `this` pointer to the current object. Most languages say "method"; the C++ standard says "member function." Use whichever your audience prefers.

```cpp
class Dog {
public:
    void bark() const;   // a method (a.k.a. member function)
};
rex.bark();              // method call
```

### Attribute
Also called **field** or **data member** — three names for the same thing: a variable that lives inside an object. C++ calls it a *data member*; Java/UML say *field*; OOP literature says *attribute*. Same concept.

```cpp
class Dog {
    std::string name_;   // ← data member / field / attribute
    int         age_;    // ← also a data member
};
```

### Access specifiers
Compile-time visibility control. Zero runtime cost. 📚 [`lexique/PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md)

| Specifier | Visible from… |
|---|---|
| `public` | anywhere |
| `protected` | the class **and** its derived classes |
| `private` | only the class itself |

```
              ┌────────────────┐
              │  public:  ✅   │  ← outside world
              │  protected: ⚠   │  ← children only
              │  private: 🔒   │  ← same class only
              └────────────────┘
```

### Encapsulation
Hiding internals behind a stable interface. Concretely: `private` data + `public` methods. The class becomes responsible for enforcing its own invariants.

### This
The hidden first parameter of every non-static member function: a pointer to the current object. 📚 [`lexique/THIS.md`](THIS.md)

```cpp
void Dog::rename(std::string n) {
    this->name_ = n;     // explicit
    name_ = n;           // implicit — same thing
}
```

### Constructor
A special member that **initializes** an object. No return type. Same name as the class. Runs once at object creation.

```cpp
Dog::Dog(std::string n) : name_(n) { }
                       // ↑ initializer list (preferred)
```

### Initializer list
The `: x_(arg), y_(arg)` syntax that initializes members **before** the constructor body runs. Mandatory for `const` and reference members.

```cpp
class A {
    const int id_;
    int      &ref_;
public:
    A(int i, int &r) : id_(i), ref_(r) { } // only way
};
```

### Destructor
A special member with name `~ClassName()` that runs at end of life. The cleanup hook for RAII.

```cpp
~Dog() { /* free resources, log, etc. */ }
```

### Copy constructor
Builds a new object **from** an existing one. Signature: `T(const T& other)`.

```cpp
Dog rex("Rex");
Dog clone(rex);          // ← copy ctor
Dog clone2 = rex;        // ← also copy ctor (not assignment!)
```

### Assignment operator
Replaces the contents of an **already-constructed** object. Signature: `T& operator=(const T& other)`.

```cpp
Dog a("A"), b("B");
a = b;                   // ← assignment, not copy ctor
```

### Deep vs Shallow copy
**Shallow** = copy bytes (default). **Deep** = also clone heap-owned resources.

```
   shallow copy of struct{int*p}:   deep copy:
   a → [ p* ]──┐                    a → [ p* ]──► [42]
   b → [ p* ]──┘                    b → [ p* ]──► [42]
        both alias the same int      separate ints
```

If you `delete` shallow-copied pointers from two destructors → **double-free**.

### Orthodox Canonical Form (OCF)
The 42-canonical "Rule of 4": every class manages its own lifecycle. 📚 [`oop/ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

```cpp
class T {
public:
    T();                         // 1. default constructor
    T(const T& other);           // 2. copy constructor
    T& operator=(const T& rhs);  // 3. copy assignment
    ~T();                        // 4. destructor
};
```

### Aggregate
A "plain old data" type (also called **POD**) — laid out like a C struct. Trivially copyable, memcpy-safe.

### Friend
A targeted access pass: this function/class can see my private parts. Use sparingly. 📚 [`lexique/FRIEND.md`](FRIEND.md)

```cpp
class Box {
    int secret_;
    friend std::ostream& operator<<(std::ostream&, const Box&);
};
```

### Operator overloading
Define what `+`, `<<`, `==`, etc. mean for your class. 📚 [`oop/OPERATOR_OVERLOADING.md`](../notions/oop/OPERATOR_OVERLOADING.md) · [`lexique/OPERATOR.md`](OPERATOR.md)

```cpp
Vec operator+(const Vec& a, const Vec& b);
std::cout << my_obj;     // calls operator<<(cout, my_obj)
```

### Getters and setters
Public methods to controlled-access private data. 📚 [`oop/GETTERS_SETTERS.md`](../notions/oop/GETTERS_SETTERS.md)

```cpp
int  getAge() const   { return age_; }   // getter — const
void setAge(int a)    { age_ = a;    }   // setter
```

---

## 🌳 6. Inheritance & polymorphism

> Reuse + dispatch. The "is-a" relationship.
> 📚 Deep dive: [`oop/INHERITANCE.md`](../notions/oop/INHERITANCE.md) · [`oop/POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md)

### Inheritance
Class B "**is a**" class A — B reuses A's interface and data, then adds/refines.

```cpp
class Animal {            class Dog : public Animal {
public:                   public:
    void breathe();           void bark();
};                        };

   ┌──────────┐
   │  Animal  │  ← base class
   └────┬─────┘
        │ public
   ┌────▼─────┐
   │   Dog    │  ← derived class (gets breathe() for free)
   └──────────┘
```

### Base class
The thing being inherited from. The opposite is the **derived class**, which inherits. Synonyms: parent/child, super/sub.

### Subclass
**Subclass** and **superclass** are common everyday names — same as **derived class** / **base class**. Java and most OOP textbooks default to *subclass / superclass*; C++ literature defaults to *derived / base*. Interchangeable.

```
        Animal      ←── superclass / base / parent
          │
          ▼
        Dog         ←── subclass / derived / child
```

### Overload
Multiple functions with the **same name, different parameters**. Resolved at compile time.

```cpp
void log(int);
void log(const std::string&);
void log(double);
```

### Override
A derived class re-implements a base's **virtual** function. Resolved at runtime.

```cpp
class Animal { public: virtual void speak() const; };
class Dog : public Animal { public: void speak() const; };
                          // ↑ overrides Animal::speak
```

### Virtual
Enables runtime polymorphism via vtable lookup. 📚 [`lexique/VIRTUAL.md`](VIRTUAL.md)

```cpp
Animal *a = new Dog;
a->speak();   // calls Dog::speak — only because speak is virtual
```

### Vtable
The implementation of `virtual`. Every polymorphic class gets a **vtable** (per class). Every instance carries a hidden **vptr** (per object) pointing to it.

```
   Dog object                vtable for Dog
   ┌──────────┐              ┌──────────────────┐
   │  vptr   ─┼─────────────►│ &Dog::speak      │
   ├──────────┤              │ &Animal::breathe │
   │  data... │              └──────────────────┘
   └──────────┘
```

### Pure virtual
A virtual with no implementation: `= 0`. Forces derived classes to override.

```cpp
class Shape {
public:
    virtual double area() const = 0;   // pure virtual
};
```

### Abstract class
A class with at least one pure virtual. **Cannot be instantiated** directly — only inherited.

```cpp
Shape s;             // ❌ abstract
Shape *p = new Circle(...);   // ✅
```

### Polymorphism
"Same call, different behavior depending on actual type." The base-pointer + virtual + override combo.

```cpp
std::vector<Animal*> zoo;
for (auto *a : zoo) a->speak();   // each one speaks its own way
```

### Object slicing
Copying a derived object **by value** into a base — the derived bits get sliced off.

```cpp
Dog d;
Animal a = d;     // 💥 only the Animal sub-object copied
a.speak();        // calls Animal::speak, NOT Dog::speak
```
Cure: pass/store **references** or **pointers** to the base.

### Diamond problem
Multiple inheritance where two parents share a grandparent → ambiguity.

```
        Animal
        /    \
     Dog    Cat
        \    /
       DogCat   ← which Animal does it have? virtual base solves it
```

---

## ⚙️ 7. Templates & generics

> Code that writes code. Type-parameterized programming.
> 📚 Deep dive: [`advanced/TEMPLATES.md`](../notions/advanced/TEMPLATES.md) · [`lexique/TEMPLATE.md`](TEMPLATE.md)

### Template
A blueprint where one or more types are parameters. The compiler **instantiates** a concrete version per type used.

```cpp
template <typename T>
T max(T a, T b) { return a > b ? a : b; }

max(3, 7);          // T = int
max(3.0, 7.0);      // T = double — separate compiled function
```

### Typename
Two jobs: 📚 [`lexique/TYPENAME.md`](TYPENAME.md)
1. Declare a template type parameter (interchangeable with `class`)
2. Disambiguate a dependent name as a type

```cpp
template <typename T>
void f() {
    typename T::value_type x;   // tell compiler this is a TYPE
}
```

### Function template
A template producing a function — like `max` above.

### Class template
A template producing a class — like `std::vector<T>`.

```cpp
template <typename T>
class Box { T value_; public: T get() const { return value_; } };

Box<int>    bi;
Box<std::string> bs;
```

### Specialization
A custom implementation of a template for a specific type.

```cpp
template <> class Box<bool> { /* bit-packed version */ };
```

### Monomorphization
The compiler-generated process of producing one concrete function/class per template instantiation. That's why templates live in headers.

---

## 🎭 8. Casts & RTTI

> Type conversions, explicit and runtime.
> 📚 Deep dive: [`CASTS.md`](CASTS.md)

### Cast
An explicit type conversion. C++ has four typed casts (vs C's blunt `(T)x`):

| Cast | Purpose | Sample |
|---|---|---|
| `static_cast<T>(x)` | Sane conversions checked at compile time | `static_cast<int>(3.14)` |
| `const_cast<T>(x)` | Add or strip `const`/`volatile` | `const_cast<char*>(s)` |
| `reinterpret_cast<T>(x)` | Bit-level reinterpretation. Last resort. | `reinterpret_cast<int*>(buf)` |
| `dynamic_cast<T>(x)` | Safe down-cast in a polymorphic hierarchy. Returns null/throws on failure. | `dynamic_cast<Dog*>(animal_ptr)` |

### RTTI
**Run-Time Type Information.** Lets you ask an object's actual type at runtime — implemented via the vtable. Costs apply only when you use it.

### Typeid
The RTTI operator. Returns a `std::type_info` describing the actual type. 📚 [`lexique/TYPEID.md`](TYPEID.md)

```cpp
Animal *a = new Dog;
std::cout << typeid(*a).name();   // "Dog" (mangled, but resolvable)
```

---

## 📦 9. Standard Template Library (STL)

> Pre-built, type-generic containers + algorithms.
> 📚 Deep dive: [`advanced/STL.md`](../notions/advanced/STL.md)

### STL
A library of templated **containers**, **iterators**, and **algorithms** — the toolbox you stop reinventing.

```
       ┌──────────┐    ┌─────────────┐    ┌──────────────┐
       │ Container│───►│  Iterator   │───►│  Algorithm   │
       │ vector   │    │ begin()/end │    │ find/sort/...│
       │ map list │    │             │    │              │
       └──────────┘    └─────────────┘    └──────────────┘
```

### Container
An object that owns and organizes a collection of values. Top hits:

| Container | Big-O insert | Big-O find | Use when |
|---|---|---|---|
| `std::vector<T>` | O(1) end | O(n) | Default. Cache-friendly array. |
| `std::list<T>` | O(1) anywhere | O(n) | Lots of mid-list inserts. |
| `std::map<K,V>` | O(log n) | O(log n) | Sorted key→value. |
| `std::set<T>` | O(log n) | O(log n) | Unique sorted keys. |
| `std::deque<T>` | O(1) ends | O(n) | Insert at both ends. |
| `std::stack<T>` | O(1) | — | LIFO. |
| `std::queue<T>` | O(1) | — | FIFO. |

### Iterator
A pointer-like object that walks a container. The glue between containers and algorithms.

```cpp
std::vector<int> v = {1, 2, 3};
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it;
```

### Allocator
The pluggable memory source for STL containers. 99% of the time you ignore it; the default `std::allocator` is fine.

### String
`std::string` — a heap-managed character buffer with automatic length, copy, and append. The fix for C's leaky `char*`. 📚 [`io-errors/STRING_FUNCTIONS.md`](../notions/io-errors/STRING_FUNCTIONS.md)

```cpp
std::string s = "hello";
s += " world";
std::cout << s.size();   // 11
```

---

## 🌊 10. I/O streams

> Buffered, type-safe input and output.
> 📚 Deep dive: [`io-errors/FSTREAM_GUIDE.md`](../notions/io-errors/FSTREAM_GUIDE.md) · [`io-errors/OPEN.md`](../notions/io-errors/OPEN.md)

### Stream
An abstraction over a sequence of bytes. The OS view (file/pipe/terminal) wrapped in a typed buffer.

```
                ┌───────────────────────────────────┐
                │     std::cout    →   stdout       │
                │     std::cin     ←   stdin        │
                │     std::cerr    →   stderr (raw) │
                │     std::ifstream /  std::ofstream│
                │     std::stringstream             │
                └───────────────────────────────────┘
```

```cpp
int x;
std::cin >> x;                    // formatted input
std::cout << "got " << x << '\n'; // formatted output
```

### Stream state flags
`good`, `eof`, `fail`, `bad`. Treated as a bool: `if (cin >> x)` is the idiom for "read succeeded."

---

## 🚨 11. Errors & exceptions

> When things go wrong, fail loudly, fail safely.
> 📚 Deep dive: [`io-errors/ERROR_MANAGEMENT.md`](../notions/io-errors/ERROR_MANAGEMENT.md) · [`lexique/TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md)

### Exception
A **non-local jump** + a payload object — used to report errors that the local code can't handle.

### Throw
Raise an exception. Stack starts unwinding immediately.

```cpp
throw std::runtime_error("file missing");
```

### Try and catch
Where exceptions are caught and handled. Catches are tested in order — put derived types first.

```cpp
try {
    risky();
} catch (const std::out_of_range& e) {
    /* specific */
} catch (const std::exception& e) {
    /* generic fallback */
}
```

### Stack unwinding
When an exception flies, every local object between throw and catch has its **destructor called** in reverse order. RAII guarantees cleanup even mid-error.

```
        throw
          │
   ┌──────▼─────┐
   │  ~Lock()   │   ← destructors fire on the way out
   │  ~File()   │
   │  ~Buffer() │
   └──────┬─────┘
          ▼
        catch
```

### Exception safety levels
| Level | Guarantee |
|---|---|
| **No-throw** | Operation never throws. |
| **Strong** | Either succeeds, or rolls back fully (no observable change). |
| **Basic** | No leaks, no UB, but state may have changed. |
| **None** | Avoid. |

---

## 🧪 12. 42-flavored idioms

> Patterns you'll see across CPP00–CPP04.
> 📚 Deep dive: [`modules/WALKTHROUGH.md`](../projets/modules/WALKTHROUGH.md)

| Idiom | Where | What it teaches |
|---|---|---|
| **Megaphone** | [`modules/CPP00.md`](../projets/modules/CPP00.md) | First contact with `std::string`, `std::cout` |
| **PhoneBook** | [`modules/CPP00.md`](../projets/modules/CPP00.md) | Class with private state + iostream formatting |
| **OCF / Rule of 4** | [`modules/CPP01.md`](../projets/modules/CPP01.md) | Default ctor, copy ctor, op=, dtor |
| **Fixed-point** | [`modules/CPP02.md`](../projets/modules/CPP02.md) | Operator overloading, conversions |
| **ClapTrap chain** | [`modules/CPP03.md`](../projets/modules/CPP03.md) | Inheritance, then virtual base (Diamond) |
| **Animal hierarchy** | [`modules/CPP04.md`](../projets/modules/CPP04.md) | Polymorphism, abstract classes, deep copy |

### The 42 norms reflex
- Tabs only. Files end in `.hpp` / `.cpp`.
- One class per file pair.
- `#ifndef` guards on every header.
- No `using namespace std;` in headers — full qualifier (`std::cout`).
- `-Wall -Wextra -Werror -std=c++98`.

---

## 🛠️ 13. Tooling

| Topic | File |
|---|---|
| Makefiles for C++ | [`tooling/MAKEFILE_CPP.md`](../notions/tooling/MAKEFILE_CPP.md) |
| Static vs shared libs | [`tooling/LIBRARIES.md`](../notions/tooling/LIBRARIES.md) |
| `<cmath>` | [`fundamentals/CMATH.md`](../notions/fundamentals/CMATH.md) |

---

## 🧭 14. Cheat sheet — when to reach for what

```
   ┌─────────────────────────────────────────────────────────────┐
   │ Need a container of unknown size?                           │
   │     → std::vector<T>                                        │
   │ Want type-safe input?                                       │
   │     → std::cin >> x;  if (cin.fail()) ...                   │
   │ Want automatic cleanup of a resource?                       │
   │     → wrap it in a class, free in destructor (RAII)         │
   │ Same operation on different types?                          │
   │     → template<typename T>                                  │
   │ Same call, different behavior per subtype?                  │
   │     → virtual + base pointer/reference                      │
   │ "Should this be a class or struct?"                         │
   │     → invariants? class. plain data? struct.                │
   │ "Should I use new?"                                         │
   │     → only if lifetime exceeds the current scope            │
   │ "Should I make this const?"                                 │
   │     → yes, until proven otherwise                           │
   └─────────────────────────────────────────────────────────────┘
```

---

## 🪜 15. Where to go next

1. Just starting? → [`fundamentals/BASICS.md`](../notions/fundamentals/BASICS.md)
2. Want the mental model behind it all? → [`fundamentals/CONTRACTS.md`](../notions/fundamentals/CONTRACTS.md)
3. Wrestling memory? → [`fundamentals/MEMORY.md`](../notions/fundamentals/MEMORY.md) + [`lexique/NEW.md`](NEW.md) / [`lexique/DELETE.md`](DELETE.md)
4. Building your first class? → [`oop/ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)
5. Inheritance not clicking? → [`oop/INHERITANCE.md`](../notions/oop/INHERITANCE.md) → [`oop/POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md)
6. Templates feel magical? → [`advanced/TEMPLATES.md`](../notions/advanced/TEMPLATES.md)
7. STL still unknown? → [`advanced/STL.md`](../notions/advanced/STL.md)
8. Exceptions blowing up? → [`io-errors/ERROR_MANAGEMENT.md`](../notions/io-errors/ERROR_MANAGEMENT.md)
9. All keywords, one click each → [`lexique/INDEX.md`](INDEX.md)

> **The rule of thumb:** if a term in a 42 subject confuses you, find it on this page → click → 5 minutes later you understand it. If it isn't here, the page is incomplete — add it.

---

## 🧾 16. Concept callouts you'll hear teachers use

Quick definitions for the high-level OOP vocabulary that floats around C++ talk.

### Abstraction
Hiding the *how* behind a clear *what*. A `std::string` is abstract — you don't care how characters are stored. The keywords for **building** abstractions: `class`, `private`, pure virtual, `template`.

### Composition
"**Has-a**" instead of "**is-a**." A `Car` *has* an `Engine` (composition); a `SportsCar` *is* a `Car` (inheritance).

```cpp
class Engine { /* … */ };
class Car {
    Engine engine_;        // composition: Car has-an Engine
};
class SportsCar : public Car { … };  // inheritance: SportsCar is-a Car
```

> Rule of thumb: **prefer composition**. Inherit only when behavior must be substitutable.

### Interface
The *what* — the public methods, the contract. In C++ usually expressed as an abstract class with pure virtuals. The matching **implementation** is the *how* — the concrete class with the bodies.

```cpp
class IShape {                          // interface
public:
    virtual double area() const = 0;
    virtual ~IShape() {}
};
class Circle : public IShape {          // implementation
    double r_;
public:
    double area() const { return 3.14 * r_ * r_; }
};
```

### API
"**Application Programming Interface**" — the set of public types, functions, and methods that your code or library exposes to others. In C++, "API" ≈ "the public part of a header."

### Functor
Also called a **function object**. A class with `operator()` overloaded — so you can call instances like functions. The C++98 way to pass behavior around (lambdas came in C++11).

```cpp
struct Greater {
    bool operator()(int a, int b) const { return a > b; }
};
std::sort(v.begin(), v.end(), Greater());   // pass behavior
```

```
   regular function:    int  cmp(int a, int b);     ← state-less
   functor:             Greater g; g(3, 5);         ← can carry state in members
```
