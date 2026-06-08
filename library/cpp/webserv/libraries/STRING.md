# `<string>`

> *"std::string is a managed byte buffer with a rich interface. In a webserv, it holds headers, paths, and response bodies."*

Provides `std::string` — the C++ string class. Used throughout webserv for HTTP headers, request paths, response bodies, and configuration values.

---

## Construction

```cpp
std::string s;                  // empty string
std::string s("hello");         // from C string literal
std::string s(other);           // copy constructor
std::string s(5, 'x');          // "xxxxx" — N copies of a char
std::string s(buf, n);          // from raw bytes + length (safe for binary data)
```

The last form is important for HTTP: request bodies may contain binary data (image uploads, etc.). Constructing from a raw pointer and length handles embedded null bytes correctly, unlike `std::string(buf)` which stops at the first `\0`.

---

## Key methods

### Size and content

```cpp
s.size()        // number of bytes (same as length())
s.empty()       // true if size() == 0
s.c_str()       // const char* — null-terminated, for C API calls
s.data()        // const char* — same as c_str() in C++11, but use c_str() in C++98
```

`c_str()` is what you pass to `write()` when sending an HTTP response:

```cpp
std::string response = "HTTP/1.1 200 OK\r\n\r\n";
write(client_fd, response.c_str(), response.size());
```

Never use `strlen(response.c_str())` for the length — use `response.size()`. The string may contain `\r\n` which `strlen` would handle correctly, but if it ever contains binary data, `strlen` would stop early.

### Searching

```cpp
s.find("needle")          // returns position, or std::string::npos if not found
s.find('\n', pos)         // search from position pos
s.find("\r\n\r\n")        // find the HTTP header/body boundary
```

The header-body boundary detection:

```cpp
size_t pos = buffer.find("\r\n\r\n");
if (pos != std::string::npos) {
    // headers complete — body starts at pos + 4
}
```

### Extracting

```cpp
s.substr(pos)           // from pos to end
s.substr(pos, len)      // len bytes starting at pos
```

### Modifying

```cpp
s += " more text";       // append
s.append(buf, n);        // append raw bytes — safe for binary
s.clear();               // empty the string
```

### Comparison

```cpp
s == "GET"               // exact match
s.compare(0, 3, "GET")   // compare first 3 chars — no allocation
```

---

## `std::string::npos`

```cpp
static const size_t npos = -1;  // effectively the largest size_t value
```

Returned by `find()` when the search fails. Always check against it before using the result:

```cpp
size_t pos = header.find("Content-Length:");
if (pos == std::string::npos)
    // header not present
```

---

## In webserv

Common patterns:

```cpp
// accumulate bytes into a per-client buffer
std::string client_buf;
client_buf.append(read_buf, bytes_read);

// detect end of headers
if (client_buf.find("\r\n\r\n") != std::string::npos)
    parse_headers();

// build a response
std::string response;
response += "HTTP/1.1 200 OK\r\n";
response += "Content-Length: 5\r\n";
response += "\r\n";
response += "hello";
write(fd, response.c_str(), response.size());
```

---

## Related

- [`SSTREAM.md`](SSTREAM.md) — build strings with stream syntax (useful for integer-to-string conversion in C++98)
- [`CSTRING.md`](CSTRING.md) — `memset`, `memcpy` for raw byte operations before they enter a `std::string`
