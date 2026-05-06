# C++ Basics — From C to C++

Written for someone who knows C and is starting the 42 CPP modules.

---

## 1. What is C++ and why does it exist?

C++ was created by Bjarne Stroustrup in the early 1980s. His original name for it was **"C with Classes"** — which is still the most honest description of what it is.

The core motivation: C gives you full control over memory and hardware, but it gives you **no tools to organize large programs**. As programs grow, raw structs + function pointers become unmanageable. C++ adds a structured way to bundle **data + behavior** together, enforce **invariants**, and reuse code through **inheritance and polymorphism** — without giving up C's performance.

The guiding principle of C++ is:
> **You don't pay for what you don't use.**

If you don't use virtual functions, there is no vtable overhead. If you don't use exceptions, there is no exception handling cost. This is what separates C++ from Java or Python.

---

## 2. The mental model shift: from C to C++

In C you think in terms of:
- Functions that manipulate data
- Structs that hold data
- Pointers that connect them

In C++ you think in terms of:
- **Objects** that *are* something and *can do* things
- **Classes** that define what those objects are
- **Interfaces** that define what different objects have in common

The shift is subtle but fundamental. In C you write:

```c
// C: functions that act on data
t_phonebook *pb = phonebook_create();
phonebook_add(pb, contact);
phonebook_destroy(pb);
```

In C++ you write:

```cpp
// C++: objects that own data and manage their own lifetime
PhoneBook pb;       // constructor runs automatically
pb.add(contact);
// destructor runs automatically when pb goes out of scope
```

This is not just syntax sugar. The object is **responsible for itself**.

---

## 3. Strengths of C++

| Strength | What it means in practice |
|---|---|
| **Performance** | As fast as C. No runtime, no GC, no VM. |
| **Zero-cost abstractions** | Classes, templates, inline functions compile away. |
| **RAII** | Resources (memory, files, locks) are tied to object lifetime — they clean up automatically. |
| **Compatibility with C** | You can call any C library directly. |
| **Expressiveness** | Operator overloading, references, templates let you write very natural code. |
| **Control** | You decide exactly where memory lives (stack vs heap). |

---

## 4. Weaknesses of C++

| Weakness | What it means in practice |
|---|---|
| **Complexity** | The language has enormous surface area. Many ways to do the same thing. |
| **No memory safety** | You still get dangling pointers, use-after-free, buffer overflows. Unlike Rust. |
| **Slow compilation** | Headers are included and re-parsed everywhere. Templates make this worse. |
| **Legacy baggage** | C++ must remain backward-compatible with C. Old bad patterns still compile. |
| **Undefined behavior** | Many mistakes (overflow, null deref, uninitialized reads) are UB — the compiler may do anything. |
| **Hard error messages** | Template errors are notoriously unreadable. |

---

## 5. C++ vs C — what stays the same

Everything you know from C still works:

- All basic types: `int`, `char`, `float`, `double`, `size_t`, ...
- All operators: `+`, `-`, `*`, `/`, `%`, `&`, `|`, `>>`, `<<`, ...
- Pointers: same syntax, same rules, same dangers
- Arrays: same stack/heap rules
- Control flow: `if`, `for`, `while`, `switch`, `return`
- The preprocessor: `#include`, `#define`, `#ifndef`, `#pragma once`
- `malloc` / `free` still exist and still work
- All C standard library headers (`<cstdio>`, `<cstring>`, `<cmath>`, ...)
- `main(int argc, char **argv)` is identical

**Rule of thumb:** if it compiles in C89, it almost certainly compiles in C++.

---

## 6. C++ vs C — what is different

### 6.1 `new` / `delete` instead of `malloc` / `free`

```c
// C
int *arr = malloc(10 * sizeof(int));
free(arr);
```

```cpp
// C++
int *arr = new int[10];
delete[] arr;

// Single object
MyClass *obj = new MyClass();
delete obj;  // calls the destructor first, then frees memory
```

Key differences:
- `new` calls the **constructor**. `malloc` does not.
- `delete` calls the **destructor**. `free` does not.
- `new` throws `std::bad_alloc` on failure (no need to check for NULL, unless you use `new (std::nothrow)`).
- In the 42 CPP modules, **always use `new`/`delete`**, never `malloc`/`free`.

---

### 6.2 References — an alias, not a pointer

A reference is an alias for an existing variable. It cannot be NULL and cannot be reseated.

```cpp
int x = 42;
int &ref = x;    // ref IS x — same memory address
ref = 100;       // x is now 100

// Pointers vs references
void doubleVal_ptr(int *p) { *p *= 2; }   // C style, caller passes &x
void doubleVal_ref(int &r) { r  *= 2; }   // C++ style, caller passes x directly

int n = 5;
doubleVal_ptr(&n);   // n = 10
doubleVal_ref(n);    // n = 20  — no & at the call site
```

**When to use what:**
- Use **references** for function parameters when you want to modify the caller's variable (no NULL possible).
- Use **const references** to pass large objects cheaply without copying.
- Use **pointers** when the value can be NULL, or when you need to reseat (change what it points to).

```cpp
// Passing large object cheaply — const reference
void print(const std::string &s) { std::cout << s; }

// Will NOT compile — references must be initialized
int &bad;         // error: reference not bound
int *ok = nullptr; // pointers can be null
```

---

### 6.3 Namespaces

Namespaces prevent name collisions between libraries.

```cpp
namespace ft {
    int max(int a, int b) { return a > b ? a : b; }
}

int main() {
    ft::max(3, 5);        // explicit namespace
    using ft::max;        // bring one name in
    max(3, 5);            // works now

    using namespace ft;   // bring everything in (avoid in headers)
}
```

`std::` is the standard library namespace. Every standard library function/type lives there.

**42 rule:** Never write `using namespace std;` in a header file. It's acceptable in `.cpp` files but the 42 norm often prefers explicit `std::`.

---

### 6.4 `std::string` instead of `char *`

```c
// C
char *s = strdup("hello");
strcat(s, " world");
free(s);
```

```cpp
// C++
std::string s = "hello";
s += " world";              // automatic memory management
s.length();                 // 11
s.substr(0, 5);             // "hello"
s.find("world");            // 6
s.empty();                  // false
```

`std::string` manages its own memory. No `malloc`, no `free`, no buffer overflows.

---

### 6.5 Function overloading

In C, you cannot have two functions with the same name. In C++ you can, as long as they have different parameter types:

```cpp
int    add(int a, int b)       { return a + b; }
double add(double a, double b) { return a + b; }
std::string add(std::string a, std::string b) { return a + b; }

add(1, 2);         // calls int version
add(1.0, 2.0);     // calls double version
add("hi", "bye");  // calls string version
```

---

### 6.6 Default arguments

```cpp
void greet(std::string name, std::string title = "Mr.") {
    std::cout << "Hello, " << title << " " << name << '\n';
}

greet("Smith");           // Hello, Mr. Smith
greet("Smith", "Dr.");    // Hello, Dr. Smith
```

Default arguments must be **at the end** of the parameter list, and must be declared in the **header** (not the `.cpp`).

---

## 7. Classes — the core of C++

A class is a struct that also carries its functions (called **member functions** or **methods**), access control, and special lifecycle functions.

```cpp
// C struct
typedef struct s_point {
    int x;
    int y;
} t_point;

// C++ class
class Point {
private:
    int _x;   // 42 convention: prefix private members with _
    int _y;

public:
    // Constructor: runs when object is created
    Point(int x, int y) : _x(x), _y(y) {}

    // Member functions
    int getX() const { return _x; }
    int getY() const { return _y; }

    void move(int dx, int dy) { _x += dx; _y += dy; }
};

int main() {
    Point p(3, 4);         // stack-allocated, constructor called
    p.move(1, 2);
    std::cout << p.getX(); // 4

    Point *q = new Point(0, 0); // heap-allocated
    delete q;                   // destructor called, memory freed
}
```

### Access control

| Keyword     | Accessible from              |
|-------------|------------------------------|
| `public`    | Anywhere                     |
| `private`   | Only inside the class itself |
| `protected` | Inside the class + subclasses|

**Default access:** `struct` defaults to `public`, `class` defaults to `private`.

---

## 8. The Orthodox Canonical Form (OCF) — mandatory for 42

Every class in 42 CPP must implement these four functions unless there is an explicit reason not to:

```
1. Default constructor       MyClass();
2. Copy constructor          MyClass(const MyClass& other);
3. Copy assignment operator  MyClass& operator=(const MyClass& other);
4. Destructor                ~MyClass();
```

This is the **Rule of Three** (Rule of Five in modern C++, but 42 uses C++98).

```cpp
// Contact.hpp
class Contact {
private:
    std::string _firstName;
    std::string _lastName;
    int         _index;

public:
    Contact();                              // 1. Default constructor
    Contact(const Contact& other);          // 2. Copy constructor
    Contact& operator=(const Contact& other); // 3. Copy assignment
    ~Contact();                             // 4. Destructor

    // Getters and setters
    std::string getFirstName() const;
    void        setFirstName(const std::string& name);
};
```

```cpp
// Contact.cpp
Contact::Contact() : _firstName(""), _lastName(""), _index(0) {}

Contact::Contact(const Contact& other)
    : _firstName(other._firstName),
      _lastName(other._lastName),
      _index(other._index) {}

Contact& Contact::operator=(const Contact& other) {
    if (this != &other) {           // guard against self-assignment
        _firstName = other._firstName;
        _lastName  = other._lastName;
        _index     = other._index;
    }
    return *this;                   // always return *this
}

Contact::~Contact() {}             // string members clean up themselves
```

**Why does this matter?**
If you allocate memory with `new` inside a class and you don't write a copy constructor, C++ will do a **shallow copy** — copying the pointer, not the data. Two objects now point to the same memory. When one destructs, it frees it. The other now has a dangling pointer. This is a classic C++ bug.

---

## 9. The initialization list — always use it

Prefer the initialization list over assignment in the constructor body:

```cpp
// Bad (default-constructs then assigns)
Contact::Contact(std::string name) {
    _name = name;   // _name was already default-constructed, now re-assigned
}

// Good (initializes directly)
Contact::Contact(std::string name) : _name(name) {}
```

The initialization list is **mandatory** for:
- `const` member variables
- Reference member variables
- Member objects with no default constructor

---

## 10. `const` — much more powerful than in C

In C++, `const` can be applied to member functions to mean "this function does not modify the object":

```cpp
class Circle {
private:
    double _radius;

public:
    Circle(double r) : _radius(r) {}

    // const member function: can be called on const objects
    double area() const { return 3.14159 * _radius * _radius; }

    // non-const: modifies the object
    void setRadius(double r) { _radius = r; }
};

const Circle c(5.0);
c.area();       // OK — const function
c.setRadius(3); // ERROR — non-const function on const object
```

**Rule:** Any getter or function that does not modify the object should be `const`.

---

## 11. `static` members

A `static` member belongs to the **class**, not to any instance.

```cpp
class PhoneBook {
private:
    static int _count; // shared by all PhoneBook objects

public:
    PhoneBook()  { _count++; }
    ~PhoneBook() { _count--; }

    static int getCount() { return _count; }
};

int PhoneBook::_count = 0; // must be defined in the .cpp

PhoneBook a, b, c;
std::cout << PhoneBook::getCount(); // 3
```

---

## 12. Header guards — same as C but stricter

```cpp
// Contact.hpp
#ifndef CONTACT_HPP
# define CONTACT_HPP

# include <string>  // include what you use

class Contact {
    // ...
};

#endif
```

**Important:** Include `<string>` in the header if `std::string` is used there. Do not rely on it being transitively included.

---

## 13. Compilation and the 42 Makefile

C++ files use the `.cpp` extension. The compiler is `c++` or `g++`:

```makefile
NAME    = phonebook
CXX     = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS    = phonebook.cpp Contact.cpp PhoneBook.cpp
OBJS    = $(SRCS:.cpp=.o)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)
```

The `-std=c++98` flag is required by 42. It locks you to C++98 features (no `auto`, no range-based for, no lambdas, no smart pointers).

---

## 14. What each CPP module focuses on

| Module | Topics |
|--------|--------|
| **CPP00** | Namespaces, classes, member functions, `iostream`, `std::string`, `const`, `static` |
| **CPP01** | `new`/`delete`, pointers to members, references, `switch` |
| **CPP02** | Operator overloading, Orthodox Canonical Form, fixed-point numbers |
| **CPP03** | Inheritance, base/derived classes, constructor chaining |
| **CPP04** | Virtual functions, pure virtual (abstract classes), interfaces |
| **CPP05** | Exceptions: `try`/`catch`/`throw` |
| **CPP06** | Casts: `static_cast`, `dynamic_cast`, `reinterpret_cast`, `const_cast` |
| **CPP07** | Templates (function and class templates) |
| **CPP08** | STL containers and iterators |
| **CPP09** | STL algorithms, more containers |

---

## 15. Practical advice for 42 CPP modules

### Always split into `.hpp` and `.cpp`

Never write function bodies in the header (except for short getters or templates).

```
PhoneBook.hpp  — class declaration
PhoneBook.cpp  — method definitions
Contact.hpp    — class declaration
Contact.cpp    — method definitions
main.cpp       — main()
```

### Write OCF first, then fill in logic

Every time you create a new class:
1. Write the four OCF functions as empty shells
2. Make it compile
3. Then add actual functionality

This prevents the "I forgot the copy constructor" bug at the end.

### Read the error from the top

```
error: no matching function for call to 'PhoneBook::PhoneBook()'
```
This means you defined a non-default constructor and C++ is looking for `PhoneBook()` with no arguments. Either add one, or fix the code that constructs a PhoneBook without arguments.

### `this` — pointer to the current object

Inside any non-static member function, `this` is a pointer to the object being called on:

```cpp
Contact& Contact::operator=(const Contact& other) {
    if (this != &other)   // self-assignment guard: am I trying to assign to myself?
        _name = other._name;
    return *this;         // return a reference to myself
}
```

### Separate concerns: getters/setters

Private data, public accessors:

```cpp
// Header
std::string getFirstName() const;       // getter
void        setFirstName(const std::string& name); // setter

// Implementation
std::string Contact::getFirstName() const { return _firstName; }
void Contact::setFirstName(const std::string& name) { _firstName = name; }
```

### When something does not compile

1. Read the **first** error only — later errors are often cascades from the first.
2. Check for: missing `#include`, missing `;` after `}` of a class, type mismatch.
3. `const` correctness is the most common source of errors: if you call a non-const method on a `const` object, or miss a `const` on a getter.

### Memory: one `new` = one `delete`

Track every `new` manually in your destructor. If a class allocates with `new`, it must `delete` in `~MyClass()`. If it allocates an array with `new[]`, it must `delete[]`.

```cpp
// BAD
MyClass::~MyClass() { delete _arr; }    // _arr was new int[10] — UB

// GOOD
MyClass::~MyClass() { delete[] _arr; }
```

---

## 16. `switch` statement

`switch` compares one integer (or `char`) against a list of constant values. Same as in C — nothing changes in C++.

```cpp
int cmd = 2;

switch (cmd) {
    case 1:
        std::cout << "one\n";
        break;
    case 2:
        std::cout << "two\n";
        break;
    case 3:
        std::cout << "three\n";
        break;
    default:
        std::cout << "unknown\n";
        break;
}
// prints: two
```

### `break` is mandatory — or you get fallthrough

Without `break`, execution **falls through** into the next case:

```cpp
switch (cmd) {
    case 1:
        std::cout << "one\n";   // no break!
    case 2:
        std::cout << "two\n";   // runs even if cmd == 1
        break;
    case 3:
        std::cout << "three\n";
        break;
}
// if cmd == 1: prints "one" then "two"
// if cmd == 2: prints "two"
```

Fallthrough is sometimes intentional (grouping cases), but always write it explicitly:

```cpp
switch (cmd) {
    case 1:
    case 2:
        std::cout << "one or two\n"; // both cases do the same thing
        break;
    case 3:
        std::cout << "three\n";
        break;
}
```

### `switch` only works on integers and chars

```cpp
switch (42)    { ... }  // int — ok
switch ('A')   { ... }  // char — ok
switch (3.14)  { ... }  // double — ERROR
switch ("ADD") { ... }  // std::string — ERROR
```

For `std::string` (like your phonebook commands), you must use `if`/`else if`:

```cpp
std::string input;
std::cin >> input;

if (input == "ADD")
    addContact();
else if (input == "SEARCH")
    searchContact();
else if (input == "EXIT")
    return 0;
```

### `default` is the fallback

Runs when no `case` matches. Always add it — it's the `else` of a `switch`.

---

## 17. Common mistakes coming from C

| C habit | C++ equivalent |
|---|---|
| `char *str = malloc(...)` | `std::string str` or `new char[n]` + `delete[]` |
| `printf("%d", n)` | `std::cout << n` |
| `strcmp(a, b) == 0` | `a == b` (std::string comparison) |
| `struct` with function pointers | `class` with virtual functions |
| `NULL` | `NULL` still works, prefer `0` in C++98 or `nullptr` in C++11 |
| `typedef struct {...} t_name` | Just `class Name` or `struct Name` |
| `void *` for generics | Templates |
| `#include <string.h>` | `#include <cstring>` (C++ versions of C headers) |

---

## 17. Quick reference card

```cpp
// Class skeleton (copy-paste for every new class)
#ifndef MYCLASS_HPP
# define MYCLASS_HPP

class MyClass {
private:
    int _value;

public:
    MyClass();                               // default constructor
    MyClass(const MyClass& other);           // copy constructor
    MyClass& operator=(const MyClass& other); // copy assignment
    ~MyClass();                              // destructor

    int  getValue() const;
    void setValue(int value);
};

#endif
```

```cpp
// .cpp skeleton
#include "MyClass.hpp"
#include <iostream>

MyClass::MyClass() : _value(0) {
    std::cout << "MyClass default constructor called\n";
}

MyClass::MyClass(const MyClass& other) : _value(other._value) {
    std::cout << "MyClass copy constructor called\n";
}

MyClass& MyClass::operator=(const MyClass& other) {
    std::cout << "MyClass copy assignment operator called\n";
    if (this != &other)
        _value = other._value;
    return *this;
}

MyClass::~MyClass() {
    std::cout << "MyClass destructor called\n";
}

int  MyClass::getValue() const  { return _value; }
void MyClass::setValue(int v)   { _value = v; }
```

The `std::cout` calls in constructor/destructor are standard 42 practice to show the object lifecycle during evaluation.
