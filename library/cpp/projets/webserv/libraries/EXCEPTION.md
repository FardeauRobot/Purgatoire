# `<exception>`

> *"The root of the C++ exception tree. You rarely use it directly — but you always catch it."*

Provides `std::exception`, the base class for all standard C++ exceptions. Also provides `std::bad_exception` and the `terminate`/`unexpected` machinery — mostly irrelevant for webserv.

---

## `std::exception`

```cpp
class exception {
public:
    exception() throw();
    exception(const exception&) throw();
    exception& operator=(const exception&) throw();
    virtual ~exception() throw();
    virtual const char* what() const throw();
};
```

`what()` returns a human-readable description of the error. The default implementation returns `"std::exception"` — not very useful. Every subclass overrides it.

---

## Usage

You almost never instantiate `std::exception` directly. Its value is in catch blocks:

```cpp
#include <exception>

try {
    // anything that might throw
} catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
}
```

This single catch handles every exception from `<stdexcept>`, `<new>` (`std::bad_alloc`), and any of your own classes that inherit from `std::exception`.

---

## Inheritance pattern

If you define custom exception types, inherit from `std::runtime_error` (via `<stdexcept>`), not directly from `std::exception` — you get the string message constructor for free:

```cpp
#include <stdexcept>

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string &msg)
        : std::runtime_error("Config error: " + msg) {}
};
```

---

## Related

- [`STDEXCEPT.md`](STDEXCEPT.md) — `std::runtime_error`, `std::logic_error` and their subtypes
