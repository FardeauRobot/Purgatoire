# Error Management in C++

Three main strategies, from simplest to most powerful.

---

## 1. Return Codes (simplest, C-style)

The function returns a value that tells the caller if it succeeded or failed.

```cpp
#define OK  0
#define ERR 1

int open_file(const std::string& path)
{
    std::ifstream file(path.c_str());
    if (!file)
    {
        std::cerr << "Error: cannot open " << path << std::endl;
        return (ERR);
    }
    // ...
    return (OK);
}

int main()
{
    if (open_file("data.txt") != OK)
        return (ERR);
}
```

**When to use:** small programs, argument validation, file opening.
**Downside:** easy to forget to check the return value.

---

## 2. Exceptions (the C++ way)

An error **throws** an exception. The caller **catches** it. If nobody catches it, the program crashes with a message.

### Throwing

```cpp
#include <stdexcept>

void divide(int a, int b)
{
    if (b == 0)
        throw std::invalid_argument("Division by zero");
    std::cout << a / b << std::endl;
}
```

### Catching

```cpp
int main()
{
    try
    {
        divide(10, 0);
    }
    catch (const std::exception& e)      // catches any standard exception
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }
}
```

### The exception hierarchy (what you can throw)

```
std::exception                         ← base class, has .what()
├── std::logic_error                   ← programmer mistakes
│   ├── std::invalid_argument          ← bad argument passed
│   ├── std::out_of_range              ← index too big
│   └── std::length_error              ← container too large
└── std::runtime_error                 ← things that fail at runtime
    ├── std::overflow_error            ← math overflow
    └── std::underflow_error           ← math underflow
```

**Quick rule:**
- `logic_error` = "this should never happen if the code is correct"
- `runtime_error` = "this can happen even if the code is correct" (file missing, network down, etc.)

### Custom exceptions

```cpp
#include <stdexcept>

class FileError : public std::runtime_error
{
public:
    FileError(const std::string& path)
        : std::runtime_error("Cannot open: " + path) {}
};

// Usage:
throw FileError("/tmp/missing.txt");
// Caught by: catch (const FileError& e) or catch (const std::exception& e)
```

**When to use:** when an error happens deep in the call stack and you want to handle it higher up, without threading return codes through every function.

---

## 3. Stream state checks (for I/O)

Streams (`ifstream`, `ofstream`, `cin`) have built-in error flags.

```cpp
std::ifstream file("data.txt");

// Check 1: did it open?
if (!file)                        // same as file.fail()
    return (err_msg("Cannot open file"));

// Check 2: read loop (stops on EOF or error)
std::string line;
while (std::getline(file, line))
    std::cout << line << std::endl;

// Check 3: WHY did the loop stop?
if (file.bad())                   // serious I/O error (disk failure, etc.)
    return (err_msg("Read error"));
// if file.eof() — normal end of file, not an error
```

| Flag       | Meaning                            | Serious? |
|------------|------------------------------------|----------|
| `good()`   | Everything fine                    | No       |
| `eof()`    | Reached end of file                | No       |
| `fail()`   | Logical error (bad format, etc.)   | Yes      |
| `bad()`    | Broken stream (hardware/OS error)  | Very     |

---

## Patterns to avoid

### 1. Don't loop on `eof()`

```cpp
// BAD — processes last line twice
while (!file.eof())
{
    std::getline(file, line);
    process(line);              // runs once more after the real last line
}

// GOOD — stops as soon as read fails
while (std::getline(file, line))
    process(line);
```

### 2. Don't catch everything silently

```cpp
// BAD — hides bugs
try { doStuff(); }
catch (...) { }                 // swallowed, you'll never know what broke

// GOOD — at least log it
try { doStuff(); }
catch (const std::exception& e)
{
    std::cerr << "Error: " << e.what() << std::endl;
    return (1);
}
```

### 3. Don't ignore return values

```cpp
// BAD
open_file("data.txt");          // what if it failed?

// GOOD
if (open_file("data.txt") != OK)
    return (ERR);
```

---

## Decision cheat sheet

| Situation                              | Use                          |
|----------------------------------------|------------------------------|
| Validating `argc/argv` in `main()`     | Return code                  |
| Opening a file                         | Stream check + return code   |
| Error deep in the call stack           | Exception                    |
| Constructor that can fail              | Exception (no return value)  |
| Checking read/write operations         | Stream state flags           |
| Unrecoverable error (must quit now)    | `err_exit()` or exception    |

---

## Template utilities (`include/utils/errors.hpp`)

The project template provides three helper functions:

```cpp
err_msg("something went wrong");     // prints to stderr, returns ERR (1)
warn_msg("this is suspicious");      // prints to stderr, continues
err_exit("cannot recover");          // prints to stderr, exits immediately
```

Use `err_msg` as a one-liner return:
```cpp
if (argc != 3)
    return (err_msg("Usage: ./program arg1 arg2"));
```
