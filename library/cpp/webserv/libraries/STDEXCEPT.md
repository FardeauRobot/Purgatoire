# `<stdexcept>`

> *"Named exception types for situations that are actually exceptional — not for control flow."*

Provides the standard C++ exception hierarchy for runtime and logic errors. Used in webserv for startup failures (bind failed, listen failed) where the only sensible response is to abort.

---

## The hierarchy

```
std::exception          (<exception>)
├── std::logic_error    (<stdexcept>)
│   ├── std::invalid_argument
│   ├── std::domain_error
│   ├── std::length_error
│   └── std::out_of_range
└── std::runtime_error  (<stdexcept>)
    ├── std::range_error
    ├── std::overflow_error
    └── std::underflow_error
```

---

## The two root types

### `std::logic_error` — bugs in your code

A condition that should never happen if the program is correct. Violating a precondition, accessing out of bounds, passing an invalid argument. The distinction: a logic error could theoretically have been caught at compile time or by an assert.

```cpp
throw std::invalid_argument("port must be between 1024 and 65535");
throw std::out_of_range("fd index out of poll array bounds");
```

### `std::runtime_error` — failures in the environment

Conditions your code is correct but the environment is wrong: the port is already in use, a file doesn't exist, the network is down. These are the ones you'll throw most often in webserv.

```cpp
throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
throw std::runtime_error("listen() failed");
```

---

## Construction

All standard exceptions take a `std::string` message:

```cpp
std::runtime_error e("something went wrong");
e.what();   // returns "something went wrong" as const char*
```

---

## Catching

```cpp
try {
    // startup sequence
} catch (const std::exception &e) {
    std::cerr << "Fatal: " << e.what() << std::endl;
    return 1;
} catch (...) {
    std::cerr << "Unknown exception" << std::endl;
    return 1;
}
```

Catch by `const std::exception &` to cover all standard exceptions. The `catch (...)` at the bottom is a safety net — something non-standard was thrown. In a webserv, this usually means a corrupted state; aborting is the right call.

---

## In webserv — what to throw vs not throw

**Throw for:**
- Server startup failures (`socket`, `bind`, `listen`, `setsockopt`)
- Configuration parse failures (bad port, missing required field)

**Don't throw for:**
- Individual client errors (bad request, file not found) — return an HTTP 4xx/5xx response instead
- `read()`/`write()` failures on a client fd — log it, close the fd, remove from poll array

The rule: exceptions should abort the server. Client-level errors should be handled gracefully by sending an error response.

---

## Related

- [`EXCEPTION.md`](EXCEPTION.md) — `std::exception`, the base class
- [`../07_ERRORS.md`](../functions/07_ERRORS.md) — `errno`, `strerror` for building error messages
