# `<sstream>`

> *"A string that behaves like a stream — useful whenever you need to build or parse text without printf."*

Provides in-memory stream classes that read from and write to `std::string` buffers. The main use in webserv is **integer-to-string conversion** in C++98, where `std::to_string` doesn't exist.

---

## Classes

```cpp
std::ostringstream   // output stream — write into a string
std::istringstream   // input stream — read from a string
std::stringstream    // bidirectional — both read and write
```

---

## `std::ostringstream` — building strings

```cpp
#include <sstream>

std::ostringstream oss;
oss << "Content-Length: " << body.size() << "\r\n";
std::string header = oss.str();
```

In C++98, this is the standard way to convert an integer to a string:

```cpp
std::ostringstream oss;
oss << 42;
std::string s = oss.str();  // "42"
```

### Key methods

```cpp
oss.str()          // returns the accumulated string
oss.str("")        // clears the buffer (reset for reuse)
oss.clear()        // clears error flags (not the buffer — confusingly named)
```

To reuse a stringstream, call both:

```cpp
oss.str("");    // clear content
oss.clear();    // clear flags
```

---

## `std::istringstream` — parsing strings

```cpp
std::string line = "Content-Length: 137";
std::istringstream iss(line);

std::string key, value;
iss >> key;    // "Content-Length:"
iss >> value;  // "137"
```

Useful for parsing HTTP header fields once you've split them from the raw request.

---

## In webserv

The most common use is building HTTP response headers:

```cpp
std::ostringstream response;
response << "HTTP/1.1 200 OK\r\n";
response << "Content-Type: text/html\r\n";
response << "Content-Length: " << body.size() << "\r\n";
response << "\r\n";
response << body;

std::string resp_str = response.str();
write(fd, resp_str.c_str(), resp_str.size());
```

And error responses:

```cpp
std::ostringstream oss;
oss << "HTTP/1.1 " << status_code << " " << status_text << "\r\n\r\n";
```

---

## Related

- [`STRING.md`](STRING.md) — `std::string`, the type that `str()` returns
- [`IOSTREAM.md`](IOSTREAM.md) — the same `<<` operators, but to the terminal
