# C++ Module 00 — TOPICS

## Theme
Namespaces, classes, member functions, stdio streams, initialisation
lists, `static`, `const`. First real contact with C++: the goal is to
stop writing C and start thinking in **classes** and **`std::`
streams**.

## Global rules (shared across all modules)
- `c++ -Wall -Wextra -Werror -std=c++98`
- `UpperCamelCase` class names, files named after the class
- No `using namespace`, no `friend`, no STL containers/algorithms
  (until Mod 08)
- Forbidden: `*printf`, `*alloc`, `free`, C++11+, external libs
- Include guards mandatory; function bodies in headers = 0
  (templates aside)
- No memory leaks

---

## Cross-cutting concepts (read before starting)

If one of these feels thin, go read the linked library doc first. The
per-exercise sections below assume you've already seen them.

| Concept | Where to read |
|---|---|
| C → C++ mindset, namespaces, `std::`, `new`/`delete` | [library/cpp/fundamentals/BASICS.md](../../../library/cpp/fundamentals/BASICS.md) |
| Streams (`std::cin`, `std::cout`, formatting) | [library/cpp/tooling/LIBRARIES.md](../../../library/cpp/tooling/LIBRARIES.md) |
| `<iomanip>` manipulators (`setw`, `setfill`, `setprecision`) | [library/cpp/tooling/LIBRARIES.md §2](../../../library/cpp/tooling/LIBRARIES.md) |
| `std::string` API | [library/cpp/io-errors/STRING_FUNCTIONS.md](../../../library/cpp/io-errors/STRING_FUNCTIONS.md) |
| Getters/setters, encapsulation | [library/cpp/oop/GETTERS_SETTERS.md](../../../library/cpp/oop/GETTERS_SETTERS.md) |
| Per-exercise walkthrough (companion to this file) | [library/cpp/modules/CPP00.md](../../../library/cpp/modules/CPP00.md) |

### The mental shift — C to C++

Stop thinking *"data + functions that act on data"*. Start thinking
*"objects that own their data and the operations on it"*. A `Contact`
isn't a `struct` with a bag of `char*`s and a `contact_print()`
function — it's a class that **is** a contact, and calling
`contact.display()` is asking the object to do something to itself.

This matters for Mod00 because the subject is deceptively simple.
If you write ex01 in C-with-classes-syntax, you'll pass, but you'll
have learned nothing. The drills: what belongs in which class, what
stays private, why getters exist.

### Streams are not `printf`

`std::cout << x` is a **method call** on an object, not a format-string
operation. `<<` is overloaded by type — for `int`, for `double`, for
`std::string`, for anything with a custom `operator<<`. That's why
there's no format string: the type system picks the right formatter.

```cpp
std::cout << "Hello " << name << ", you are " << age << " years old\n";
```

Each `<<` returns a reference to the stream (`std::ostream&`), which
lets you chain. That's the **fluent builder pattern**, and it's how
every `<<`/`>>` works — yours included once you start overloading.

At the hardware level, `std::cout` writes to `stdout` (fd 1) via
**buffered writes**. The buffer is flushed on `std::endl` (which also
emits `\n`), or when the buffer fills, or at program exit. `"\n"`
doesn't flush, `std::endl` does — that difference matters when you're
debugging a crash and your last "I got here" line never prints.

---

## Exercise 00 — Megaphone

### Goal
A CLI that uppercases all argv arguments and prints them together.
No argument → print
`* LOUD AND UNBEARABLE FEEDBACK NOISE *`.

### What the exercise actually drills

- **`main(int argc, char** argv)`** — same as C. C++ doesn't change
  the entry point.
- **`std::cout`** instead of `printf`. No format string, just `<<`.
- **`std::toupper`** from `<cctype>` (not `<ctype.h>` — C++ naming:
  drop the `.h`, prefix with `c`).
- **`std::string`** — you'll iterate over it. It has `operator[]`,
  `size()`, and a range you can loop with an `int` index in c++98.

### Hardware-level notes

- `argv` is a `char**` in read-only memory (usually `.rodata` in the
  process image on Linux). Mutating `argv[i][j]` directly is
  technically allowed by POSIX but a footgun — copy to `std::string`
  first.
- `std::string` uses the heap for any non-empty content. Each
  `std::string s = argv[i];` is one `new char[]` under the hood.
  For ex00 that's fine; for hot paths you'd stay with the `char*`.
- `std::toupper(c)` is a table lookup on the current locale. In the
  default "C" locale it's equivalent to `(c >= 'a' && c <= 'z') ?
  c - 32 : c`. The bit trick is subtraction by `0x20` because ASCII
  lower/upper differ only in bit 5.

### Tips & gotchas

- **Only uppercase if there's at least one argument.** `argc == 1`
  (the program name counts) means print the feedback line. Don't
  forget the trailing `\n`.
- **Watch `-Wall -Wextra -Werror`.** `unsigned int i` vs `int i` loop
  counters will bite you when comparing to `argc` or `size()`.
- **One-liner trap:** it's tempting to squeeze this into 10 lines
  with nested ternaries. Don't. The moulinette doesn't reward
  cleverness, and readable code scores the same.

### Search terms
`"std::toupper cctype example"`, `"c++ iterate std::string"`,
`"std::cout vs printf"`.

---

## Exercise 01 — My Awesome PhoneBook

### Goal
A two-class REPL: `PhoneBook` holds up to 8 `Contact`s, supports
`ADD` / `SEARCH` / `EXIT`. A 9th `ADD` overwrites the oldest. The
`SEARCH` table is rigidly formatted: 4 columns, 10 chars each,
right-aligned, truncated with `.` if too long.

### What the exercise actually drills

This is the first real "design a class" exercise. The non-obvious
parts:

- **What goes public vs private.** `Contact`'s fields are private;
  the outside world asks for them via getters. `PhoneBook::m_contacts`
  is private too — nobody outside can reach into it.
- **Getters are `const`.** `std::string Contact::getFirstName() const`
  promises not to mutate the contact. Required so you can print a
  `const Contact&`. See
  [library/cpp/oop/GETTERS_SETTERS.md](../../../library/cpp/oop/GETTERS_SETTERS.md)
  for why and how.
- **Column formatting with `<iomanip>`.** `std::setw(10)` sets the
  next field's width; `std::right` sets alignment; `std::setfill(' ')`
  sets the padding char. These are manipulators — they modify the
  stream's state, not a value.
- **Input validation.** `std::getline(std::cin, s)` reads a line
  including spaces. After a `std::cin >> x`, the newline is still in
  the buffer — `getline` right after will read an empty line. The
  standard trick: `std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');`.
- **Empty-field rejection.** The subject says no field may be empty.
  Re-prompt in a loop until it is.
- **EOF handling.** If `std::cin` hits EOF (the user presses Ctrl-D),
  `getline` sets `failbit` and returns. Check `std::cin.eof()` and
  exit cleanly — don't loop forever.

### Column formatting, explained

```cpp
std::cout << std::setw(10) << std::right << index << "|"
          << std::setw(10) << std::right << firstName << "|"
          << std::setw(10) << std::right << lastName  << "|"
          << std::setw(10) << std::right << nickname  << "\n";
```

**Truncation rule:** if the string is longer than 10 chars, print the
first 9 chars followed by `.`. So `"Bartholomew"` becomes `"Bartholo."`.
Implement this as a helper that takes a string and returns either the
original (if short) or `s.substr(0, 9) + "."`.

**Why `setw` is single-shot:** manipulators like `setw` affect *only
the next insertion*. Manipulators like `std::right` or `setfill` are
**sticky** — they stay until you change them back. That's a common
`<iomanip>` gotcha.

### Hardware-level notes

- `PhoneBook m_contacts[8]` is **8 `Contact` objects inline in the
  `PhoneBook`**, not 8 pointers. They're constructed when the
  `PhoneBook` is, destroyed with it. No heap. `sizeof(PhoneBook)` is
  `8 * sizeof(Contact)` plus bookkeeping (current count, next slot).
- Each `Contact` holds 5 `std::string`s. Each `std::string` is
  typically 24-32 bytes of stack footprint (pointer + size + capacity
  or SSO buffer). The string *contents* live on the heap if long
  enough — SSO threshold is ~15 chars on typical libstdc++.
- `std::cin >> x` is line-buffered by default (terminal is in "cooked"
  mode). The program doesn't see input until the user hits Enter.
  That's why `getline` feels natural here — it matches the line
  granularity you're already getting.

### Tips & gotchas

- **Circular buffer for the 9th insertion.** Track a
  `size_t nextSlot` that wraps modulo 8. Don't maintain a separate
  "front" and "back" — one cursor is enough.
- **`m_count` vs `m_nextSlot`.** Once you've filled 8 slots,
  `m_count` stays at 8 while `m_nextSlot` keeps rotating. You need
  both.
- **Don't define getters inline in `Contact.hpp`.** The project rule
  (and your memory pin): headers declare, `.cpp` defines.
- **`std::getline` + mixed `std::cin >> n`.** After `std::cin >> n`,
  the newline is left in the buffer. Use `std::cin.ignore(...)` or
  only use `getline` + `stringstream` conversion.
- **Truncated string with `.` replaces the last visible char.** So
  `"Bartholomew"` (11 chars) → `"Bartholo."` (9 chars + `.` = 10).
  Re-read the spec if you're getting 10 chars of content + `.`.

### Search terms
`"std::setw setfill right align"`, `"c++ circular buffer array"`,
`"std::getline after std::cin newline"`,
`"c++ numeric_limits streamsize ignore"`.

### Cross-reference
- [library/cpp/modules/CPP00.md §ex01](../../../library/cpp/modules/CPP00.md) — full walkthrough
- [library/cpp/io-errors/STRING_FUNCTIONS.md](../../../library/cpp/io-errors/STRING_FUNCTIONS.md) — `substr`, `length`, `empty`
- [library/cpp/tooling/LIBRARIES.md §2](../../../library/cpp/tooling/LIBRARIES.md) — `<iomanip>` manipulators

---

## Exercise 02 — The Job Of Your Dreams (bonus)

### Goal
A **reverse-engineering** exercise. You're given `Account.hpp` and
`tests.cpp` and a log file showing what `tests` should output. Write
`Account.cpp` so the output matches (timestamps excluded).

### What the exercise actually drills

- **`static` members**. `Account` has class-wide counters:
  `_nbAccounts`, `_totalAmount`, `_totalNbDeposits`,
  `_totalNbWithdrawals`. They're shared across all instances —
  one storage per class, not per object.
- **Declaring vs defining a `static`.** In the header:
  `static int _nbAccounts;` — declaration only. In
  `Account.cpp`: `int Account::_nbAccounts = 0;` — definition +
  initialisation. Without the second line, you get an undefined
  reference at link time.
- **Class-wide behaviour via static methods.** `Account::getNbAccounts()`
  reads `_nbAccounts` without needing an instance. Static methods
  have **no `this` pointer** — they can't touch non-static members.
- **Logging conventions.** The log shows `[timestamp] accounts:X
  total:Y ...` — format is fixed. Your job is to match it character
  for character.
- **Timestamp format.** `[19920104_091532]` — `YYYYMMDD_HHMMSS`.
  You'll need `<ctime>`: `std::time(0)`, `std::localtime`, then
  fields of the `tm` struct printed with `setfill('0')` + `setw(2)`.

### Hardware-level notes

- A `static` member lives in **`.data`** (if initialised) or
  **`.bss`** (if zero-initialised) — the same segments as a C
  global, just scoped by a class name at the C++ level.
- `std::time(0)` is the POSIX `time()` syscall: returns seconds since
  the epoch (1970-01-01 UTC). `std::localtime` converts to a broken-
  down struct in the current timezone — it does a timezone lookup,
  which touches `/etc/localtime` via libc. That's why the first call
  is sometimes noticeably slower.
- `std::time(0)` has 1-second resolution. Two log lines in the same
  second share a timestamp — that's fine for the subject.

### Tips & gotchas

- **Read the log, then read it again.** Character for character. A
  missing space or a stray `;` fails the diff. Moulinette runs
  `diff -q` (or equivalent) and a single mismatched byte kills the
  grade.
- **Don't modify `Account.hpp` or `tests.cpp`.** The subject is
  explicit: `Account.cpp` is the only file you write.
- **Signed vs unsigned amount.** The header decides — match it.
- **`_displayTimestamp` is static.** It doesn't print any
  instance-specific data, just the current time. Reused everywhere.

### Search terms
`"c++ static member initialization .cpp"`,
`"c++ std::localtime example"`,
`"c++ class static method no this"`.

### Cross-reference
- [library/cpp/modules/CPP00.md §ex02](../../../library/cpp/modules/CPP00.md) — full walkthrough
- [library/cpp/fundamentals/BASICS.md §11](../../../library/cpp/fundamentals/BASICS.md) — `static` members
- `WALKTHROUGH.md §14` — `static`: three meanings, one keyword

---

## Module-level sanity checks before moulinette

- `-Wall -Wextra -Werror -std=c++98` — no warnings allowed.
- `valgrind ./phonebook` — no leaks. `std::string` is self-cleaning;
  your classes should be too.
- Every ex's output matches the subject to the byte. Check trailing
  newlines.
- File naming: `ClassName.hpp` / `ClassName.cpp` — case-sensitive.
- No `using namespace std;`. No function bodies in headers.

You can pass the module without ex02.
