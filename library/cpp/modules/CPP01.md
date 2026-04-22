# C++ Module 01 — Learning Guide

**Module theme:** *Memory allocation, pointers to members, references, and switch statements.*

This module is the first time you're asked to think about **where** your objects live (stack vs heap), **how** you refer to them (value, pointer, reference), and **how** to dispatch behavior without a chain of `if/else`. Each exercise isolates one of these ideas.

---

## Exercise 00 — Braiiiiiinnnzzz

### What you're learning
**Stack allocation vs heap allocation, and when to pick each.**

- `Zombie foo("bar");` — **stack**. The object dies when it leaves scope. Destructor runs automatically.
- `Zombie* foo = new Zombie("bar");` — **heap**. The object lives until you `delete` it. You are responsible for that `delete`.

### Practical rules of thumb
| Situation | Use |
|---|---|
| Object is only needed inside this function | Stack |
| Object must outlive the function it was created in | Heap (`new`) |
| You want a fixed, known count at compile time | Stack |
| You need to decide the count at runtime | Heap |

### Tips
- `newZombie` returns a pointer → caller must `delete` it. Comment this in your code or document it — memory ownership is a *contract*.
- `randomChump` uses a **local stack object**; no `new`, no `delete`. The destructor fires automatically at the end of the function.
- Make the destructor print something (e.g. `"<name> destroyed"`). You'll *see* when each zombie dies — it's the whole point of the exercise.
- If you see the destructor message fire twice for the same zombie, you have a double-free. If you never see it, you have a leak.

### Gotcha
C++98 has no `nullptr`. Use `NULL` or just `0` when a pointer might not be set.

---

## Exercise 01 — Moar brainz!

### What you're learning
**Array heap allocation in a single `new[]`, and matching `delete[]`.**

```cpp
Zombie* horde = new Zombie[N];     // single allocation for N objects
// ...
delete[] horde;                    // MUST use delete[], not delete
```

### Practical details
- `new Zombie[N]` calls the **default constructor** N times. So your `Zombie` class **must** have a default constructor — which means you need a separate way to set the name later (e.g. a setter, or assigning to the public attribute).
- You can't use `new Zombie[N]("name")` — that syntax doesn't exist. You must loop and name them after allocation.
- The usual pattern:
  ```cpp
  Zombie* horde = new Zombie[N];
  for (int i = 0; i < N; i++)
      horde[i].setName(name);
  ```

### Tips
- `delete horde;` (without `[]`) on an array = **undefined behavior**. On most compilers it only destroys the first element and leaks the rest. Always pair `new[]` with `delete[]`.
- Run with `valgrind ./moar_brainz` to confirm zero leaks / zero errors.
- The ex01 subject says "allocate N Zombies in a **single** allocation." So do NOT do `N` individual `new Zombie(...)` calls in a loop — that would be `N` allocations.

### Gotcha
If your `Zombie` from ex00 has no default constructor, you must add one for ex01. Each exercise is evaluated independently, so that's fine.

---

## Exercise 02 — HI THIS IS BRAIN

### What you're learning
**References are just another syntax for "address manipulation."** This exercise strips away the mystery: a reference holds the same address as a pointer; it just dereferences implicitly.

### What to print
```cpp
std::string   str = "HI THIS IS BRAIN";
std::string*  stringPTR = &str;
std::string&  stringREF = str;

std::cout << &str       << std::endl;  // address of str
std::cout << stringPTR  << std::endl;  // same address
std::cout << &stringREF << std::endl;  // same address

std::cout << str        << std::endl;  // value
std::cout << *stringPTR << std::endl;  // same value (dereference)
std::cout << stringREF  << std::endl;  // same value (implicit)
```

### Key takeaways (memorize these)
| | Pointer | Reference |
|---|---|---|
| Can be null | Yes (`NULL`) | **Never** |
| Can be reassigned | Yes | **Never** (bound at init) |
| Must be initialized | No (but UB if used uninit) | **Yes, at declaration** |
| Syntax to access value | `*p` | `r` (implicit) |
| Syntax to get address | `p` (already an address) | `&r` |

### Tip
When you see `stringREF` *behave like* `str`, you've understood references. They're syntactic sugar over "a pointer that can't be null and can't be changed."

---

## Exercise 03 — Unnecessary violence

### What you're learning
**When to use a reference member vs a pointer member in a class.** This is the natural follow-up to ex02.

### The design question
- `HumanA` **always** has a weapon → member is `Weapon&` (reference can't be null).
- `HumanB` **may or may not** have a weapon → member is `Weapon*` (pointer can be `NULL`, and can be reassigned with `setWeapon`).

### Practical details
- `Weapon::getType()` must return `const std::string&`, not `std::string`. Returning by value copies the string; returning by const-ref avoids the copy and matches the subject.
- `HumanA` takes the `Weapon` in its **constructor** — the reference is bound once and forever.
- `HumanB` has **no weapon argument** in its constructor. It uses `setWeapon()` separately. Default-initialize the pointer to `NULL` to avoid garbage.
- **Ownership:** neither Human *owns* the Weapon. `main` owns it. HumanB's destructor must NOT `delete` the pointer.

### The Orthodox Canonical Form trap
- `Weapon`: full OCF works fine.
- `HumanA`: **impossible** to provide a full OCF. A class with a reference member:
  - Cannot have a default constructor (reference must be bound at construction).
  - Cannot have a meaningful `operator=` (references can't be rebound).
  
  The honest answer: provide parameterized ctor + copy ctor + destructor, and either skip `operator=` or stub it to copy only the non-reference members (like `name`). Be ready to explain this at defense — evaluators test whether you understand the *why*.
- `HumanB`: full OCF works. Destructor must NOT delete the pointer (shared ownership, `main` owns the Weapon).

### Expected output
```
Bob attacks with their crude spiked club
Bob attacks with their some other type of club
Jim attacks with their crude spiked club
Jim attacks with their some other type of club
```
The fact that Bob's second attack shows the *updated* weapon type is the demonstration: reference/pointer members share the same underlying object — changing `club` is visible through Bob.

### Tip
The subject hint asks "*when is a pointer better? when is a reference better?*" — answer: **reference when the relation is mandatory and permanent; pointer when it's optional or can change.**

---

## Exercise 04 — Sed is for losers

### What you're learning
**`std::string` manipulation and C++ file I/O (`ifstream`/`ofstream`).** Also: re-implementing something you'd normally do with `sed`, without using `std::string::replace`.

### The algorithm (since `replace` is forbidden)
```cpp
std::string result;
size_t i = 0;
while (i < content.size()) {
    size_t found = content.find(s1, i);
    if (found == std::string::npos) {
        result += content.substr(i);  // append the rest
        break;
    }
    result += content.substr(i, found - i);  // text before the match
    result += s2;                             // the replacement
    i = found + s1.size();                    // skip past the match
}
```

### Practical details
- Read the whole file into one `std::string` first (easier than line-by-line for cross-line matches):
  ```cpp
  std::ifstream in(filename.c_str());
  std::stringstream buffer;
  buffer << in.rdbuf();
  std::string content = buffer.str();
  ```
- Output file: `<filename>.replace` — i.e. `argv[1] + ".replace"`.
- Handle errors: missing file, empty `s1` (infinite loop if you're not careful!), wrong argc.

### Tips
- **If `s1` is empty**, your loop will never advance — guard against this explicitly and either error out or treat it as "no replacement."
- **Do not use `getline` + line-by-line replacement** if `s1` could span a newline. Read the whole file at once.
- `std::ifstream` takes `const char*` in C++98, so pass `filename.c_str()`, not the `std::string` directly (that's C++11+).
- Close your streams (or let RAII do it — they close on destruction).

### Gotcha
Forbidden: `std::string::replace`, all C file functions (`fopen`, `fread`, `fwrite`, `fclose`, etc.). You're expected to use C++ streams.

---

## Exercise 05 — Harl 2.0

### What you're learning
**Pointers to member functions** — the C++-correct way to dispatch to one of several methods without a chain of `if/else`.

### Syntax (this is the point of the exercise)
```cpp
// Declaration of a pointer to a member function of Harl:
void (Harl::*fn)(void);

// Assigning it:
void (Harl::*fn)(void) = &Harl::debug;

// Calling it on an instance:
(this->*fn)();
// or on an object:
(harlInstance.*fn)();
```

### The canonical structure
```cpp
void Harl::complain(std::string level) {
    std::string levels[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    void (Harl::*fns[4])(void) = {
        &Harl::debug, &Harl::info, &Harl::warning, &Harl::error
    };
    for (int i = 0; i < 4; i++) {
        if (level == levels[i]) {
            (this->*fns[i])();
            return;
        }
    }
}
```

### Tips
- The subject **forbids** the if/else-forest approach. Using `switch` here also misses the point — ex06 is where `switch` belongs. Use the array-of-member-function-pointers approach.
- `debug`, `info`, `warning`, `error` are **private**. Only `complain` is public. That's deliberate — the client of `Harl` interacts through levels, not method names.
- Member-function pointer syntax is *genuinely* ugly. Take the time to type it out a few times until it stops looking foreign: `return_type (ClassName::*varname)(params)`.

### Gotcha
`Harl::debug` alone is a function name; you must write `&Harl::debug` to get the pointer. Unlike free functions (where `foo` and `&foo` are equivalent), member functions *require* the `&`.

---

## Exercise 06 — Harl filter

### What you're learning
**The `switch` statement with intentional fall-through.** This exercise reuses ex05's `Harl` class.

### The trick
Filter "show this level **and everything more severe**." That's tailor-made for fall-through:

```cpp
switch (levelIndex) {
    case 0:  // DEBUG
        harl.complain("DEBUG");
        // fall through
    case 1:  // INFO
        harl.complain("INFO");
        // fall through
    case 2:  // WARNING
        harl.complain("WARNING");
        // fall through
    case 3:  // ERROR
        harl.complain("ERROR");
        break;
    default:
        std::cout << "[ Probably complaining about insignificant problems ]" << std::endl;
}
```

### Practical details
- First translate the string arg (`"WARNING"`) into an index 0–3.
- Pass unknown levels to the `default` branch — the expected output is exactly `[ Probably complaining about insignificant problems ]`.
- Executable name must be **`harlFilter`** (subject requirement — check your Makefile).

### Tips
- Add comments like `// fall through` above each implicit fall-through — modern compilers warn about it (`-Wimplicit-fallthrough`) and readers will thank you.
- The output spec shows a blank line between sections (e.g. between `WARNING` and `ERROR`). Make sure your `Harl::warning()` etc. end with a newline, and consider whether you need an extra newline between levels.

### Gotcha
Ex06 is **optional** — you can pass the module without it. But it's the only exercise that teaches `switch` in this module, so skip it only if you're truly out of time.

---

## Module-wide checklist before defense

- [ ] Compiles with `c++ -Wall -Wextra -Werror -std=c++98` — no warnings.
- [ ] Zero memory leaks under `valgrind` (especially ex00, ex01, ex03).
- [ ] No `using namespace std;` anywhere.
- [ ] No `friend` keyword.
- [ ] No STL containers (`vector`, `map`, etc.) — forbidden until Module 08.
- [ ] No `*printf`, `*alloc`, `free` — that's -42 instantly.
- [ ] Every class is in its own `.hpp` / `.cpp` pair, named `ClassName.hpp` / `ClassName.cpp`.
- [ ] Every `.hpp` has include guards.
- [ ] No function implementation in headers (except templates).
- [ ] Each header is **self-sufficient** — it includes everything it needs, doesn't rely on include order.
- [ ] From Mod02 onward (and already here when it makes sense), classes follow the **Orthodox Canonical Form** — with the HumanA-style exception explained.

## Concept-to-exercise map (quick reference)

| Concept | Where it shows up |
|---|---|
| Stack vs heap allocation | ex00 |
| `new[]` / `delete[]` | ex01 |
| Memory leaks & `valgrind` | ex00, ex01, ex03 |
| References vs pointers (syntax) | ex02 |
| References vs pointers (as class members) | ex03 |
| Orthodox Canonical Form with references | ex03 |
| File I/O with `ifstream`/`ofstream` | ex04 |
| `std::string` manipulation | ex04 |
| Pointers to member functions | ex05 |
| `switch` statement (with fall-through) | ex06 |

## Things to read / review separately

- **References** — cppreference's page on "Reference declaration" (skim, not memorize).
- **Member access operators** — specifically `.*` and `->*` (used in ex05).
- **File streams** — `<fstream>`, `<sstream>`, `<iostream>` interactions.
- **`std::string`** member functions — see [STRING_FUNCTIONS.md](STRING_FUNCTIONS.md) in this directory.
- **Orthodox Canonical Form** — see [ORTHODOX_CANONICAL_FORM.md](ORTHODOX_CANONICAL_FORM.md).
