# `<cstring>`

> *"The C memory and string functions, wrapped in the C++ standard library. Most of what you need is memset and memcpy."*

Provides C-style memory and string manipulation functions. In webserv, the most used are `memset` (zeroing structs) and `memcpy` (copying raw bytes). The string functions (`strlen`, `strcmp`...) are largely replaced by `std::string` methods, but remain useful at C API boundaries.

---

## Memory functions

### `memset` — fill a block with a byte value

```c
void *memset(void *s, int c, size_t n);
// Returns: s
```

Set `n` bytes of memory at `s` to the value `c`. Almost always called with `c = 0` to zero-initialize a struct:

```cpp
struct sockaddr_in addr;
std::memset(&addr, 0, sizeof(addr));  // zero all fields including sin_zero padding
```

Must be called before filling a `sockaddr_in` — the `sin_zero[8]` padding field must be zero.

---

### `memcpy` — copy bytes between non-overlapping regions

```c
void *memcpy(void *dest, const void *src, size_t n);
// Returns: dest
```

Copies `n` bytes from `src` to `dest`. The regions must not overlap. For overlapping regions, use `memmove`.

```cpp
char buf[4096];
std::memcpy(client_buf, buf, bytes_read);
```

---

### `memmove` — copy bytes, handles overlap

```c
void *memmove(void *dest, const void *src, size_t n);
// Returns: dest
```

Safe version of `memcpy` when source and destination may overlap. Slower than `memcpy` in principle, but compilers often optimize them identically.

---

### `memcmp` — compare two byte regions

```c
int memcmp(const void *s1, const void *s2, size_t n);
// Returns: 0 if equal, <0 if s1 < s2, >0 if s1 > s2
```

Byte-level comparison, unlike `strcmp` which stops at `\0`. Use when comparing binary data or fixed-size protocol fields.

---

## String functions

Less common in C++ webserv code since `std::string` handles most use cases, but appear at C API boundaries (`execve` env arrays, CGI, config parsing).

```c
size_t strlen(const char *s);              // length up to first \0
char  *strcpy(char *dst, const char *src); // copy (unsafe — no bounds check)
char  *strncpy(char *dst, const char *src, size_t n); // bounded copy
int    strcmp(const char *s1, const char *s2);         // compare
int    strncmp(const char *s1, const char *s2, size_t n); // bounded compare
char  *strstr(const char *haystack, const char *needle);  // find substring
char  *strchr(const char *s, int c);        // find first occurrence of char
char  *strrchar(const char *s, int c);      // find last occurrence of char
```

**Prefer `std::string` methods** (`find`, `compare`, `substr`) over these wherever possible — they're bounds-safe and work with binary data. Use the C functions when you must pass to a C API or process null-terminated data from the environment.

---

## `strerror` — turn errno into text

```c
char *strerror(int errnum);
// Returns: pointer to a static string describing the error
```

Often seen paired with `errno` for error messages:

```cpp
#include <cstring>
#include <cerrno>

if (bind(fd, ...) == -1)
    throw std::runtime_error(std::string("bind failed: ") + std::strerror(errno));
```

Note: `strerror` returns a pointer to static storage — don't `free()` it, and don't call it twice in the same expression.

---

## In webserv

```cpp
// zeroing a struct before use
std::memset(&addr, 0, sizeof(addr));

// copying raw bytes into a string buffer
client_buffer.append(read_buf, bytes_read);  // std::string::append is safer

// building a CGI environment variable
char env_buf[256];
std::memset(env_buf, 0, sizeof(env_buf));
std::strncpy(env_buf, "REQUEST_METHOD=GET", sizeof(env_buf) - 1);
```

---

## Related

- [`STRING.md`](STRING.md) — `std::string`, the C++ alternative for most string operations
- [`UNISTD_H.md`](UNISTD_H.md) — `read`/`write` produce raw bytes that often go through `memcpy`
