# `<iostream>`

> *"The C++ answer to printf — type-safe, extensible, and tied to the object model."*

Provides the standard C++ stream objects for console I/O. In a webserv context, used exclusively for logging and debugging — never for sending HTTP data (that goes through `write()`/`send()`).

---

## Objects

```cpp
std::cin    // standard input  (fd 0)
std::cout   // standard output (fd 1)
std::cerr   // standard error  (fd 2) — unbuffered
std::clog   // standard error  (fd 2) — buffered
```

### `cout` vs `cerr`

`cerr` is **unbuffered** — each write goes to the terminal immediately. Use it for error messages and signal notifications so they appear even if the process crashes before flushing `cout`.

`cout` is **line-buffered** when connected to a terminal, **fully buffered** when redirected to a file or pipe. In a server, this matters: debug output to `cout` may not appear immediately if the output is piped.

---

## Operators

```cpp
std::cout << "text" << variable << std::endl;   // output
std::cin  >> variable;                           // input
```

`<<` and `>>` are overloaded for all built-in types and `std::string`. They return a reference to the stream, enabling chaining.

---

## Manipulators

```cpp
std::endl    // insert '\n' and flush the buffer
std::flush   // flush the buffer without inserting '\n'
std::dec     // integers in decimal (default)
std::hex     // integers in hexadecimal
std::oct     // integers in octal
std::boolalpha  // print booleans as "true"/"false" instead of 1/0
```

**`std::endl` vs `'\n'`:** `std::endl` flushes, `'\n'` does not. In a tight loop, flushing on every line is expensive. Use `'\n'` for performance, `std::endl` only when you need the flush (e.g. before a blocking call or crash-prone section).

---

## In webserv

```cpp
// Good — logging server events
std::cout << "Client connected on fd " << client_fd << '\n';
std::cerr << "SIGNAL RECEIVED" << std::endl;  // endl: flush before process state changes

// Wrong — never use cout/cerr to send HTTP data
std::cout << "HTTP/1.1 200 OK\r\n";  // this goes to your terminal, not the client
// Use write(client_fd, ...) instead
```

---

## Related

- [`STRING.md`](STRING.md) — `std::string`, which streams accept and produce
- [`SSTREAM.md`](SSTREAM.md) — build strings with stream syntax before sending them
