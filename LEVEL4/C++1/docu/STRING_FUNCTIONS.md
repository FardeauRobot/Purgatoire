# `std::string` — Reference

`std::string` lives in `<string>` and is a `typedef` for `std::basic_string<char>`. Everything below is C++98 unless marked **(C++11)**.

## Quick mental model

A `std::string` owns a heap-allocated `char` buffer and three numbers: **size** (chars currently stored), **capacity** (bytes available before reallocation), and the pointer. It ensures the buffer stays null-terminated so `c_str()` works in O(1).

---

## Construction & Assignment

| Function | Purpose |
|---|---|
| `string()` | Empty string |
| `string(const string& s)` | Copy |
| `string(const string& s, size_t pos, size_t len = npos)` | Substring copy from `s[pos..pos+len]` |
| `string(const char* s)` | From C-string (reads until `\0`) |
| `string(const char* s, size_t n)` | First `n` chars of `s` (no `\0` required) |
| `string(size_t n, char c)` | `n` copies of `c` |
| `string(InputIt first, InputIt last)` | From iterator range |
| `operator=(const string&)` / `(const char*)` / `(char)` | Assignment |
| `assign(...)` | Same overloads as constructors; replaces content |

```cpp
std::string a;                  // ""
std::string b("hello");         // "hello"
std::string c(5, 'x');          // "xxxxx"
std::string d(b, 1, 3);         // "ell"
a.assign("world");              // a = "world"
```

---

## Element Access

| Function | Returns | Notes |
|---|---|---|
| `operator[](size_t i)` | `char&` | **No bounds check**; UB if `i > size()` |
| `at(size_t i)` | `char&` | Throws `std::out_of_range` |
| `front()` **(C++11)** | `char&` | First char; UB if empty |
| `back()` **(C++11)** | `char&` | Last char; UB if empty |
| `data()` | `const char*` | Pointer to internal buffer (null-terminated since C++11) |
| `c_str()` | `const char*` | Null-terminated C-string view |

```cpp
std::string s = "abc";
s[0];          // 'a'
s.at(10);      // throws
s.c_str();     // "abc\0..."
```

---

## Iterators

| Function | Returns |
|---|---|
| `begin()` / `end()` | `iterator` |
| `rbegin()` / `rend()` | `reverse_iterator` |
| `cbegin()` / `cend()` **(C++11)** | `const_iterator` |
| `crbegin()` / `crend()` **(C++11)** | `const_reverse_iterator` |

```cpp
for (std::string::iterator it = s.begin(); it != s.end(); ++it)
    std::cout << *it;
```

---

## Capacity

| Function | Returns | Purpose |
|---|---|---|
| `size()` / `length()` | `size_t` | Char count (identical, both exist for compatibility) |
| `empty()` | `bool` | `size() == 0` |
| `max_size()` | `size_t` | Theoretical max size (usually huge) |
| `capacity()` | `size_t` | Currently allocated capacity |
| `reserve(size_t n)` | `void` | Ensure capacity ≥ `n` (avoids reallocation) |
| `resize(size_t n)` | `void` | Set size to `n`; pads with `\0` or user char |
| `resize(size_t n, char c)` | `void` | Same but pads with `c` |
| `shrink_to_fit()` **(C++11)** | `void` | Request capacity reduction |
| `clear()` | `void` | Set size to 0 (doesn't free capacity) |

---

## Modifiers

### Append

| Function | Purpose |
|---|---|
| `operator+=` | Accepts `string`, `char*`, `char` |
| `append(const string&)` | Concatenate |
| `append(const string&, size_t pos, size_t len)` | Append substring |
| `append(const char*)` / `append(const char*, size_t n)` | From C-string |
| `append(size_t n, char c)` | `n` copies of `c` |
| `append(InputIt first, InputIt last)` | Range append |
| `push_back(char c)` | Append single char |

### Insert

| Function | Purpose |
|---|---|
| `insert(size_t pos, const string& s)` | Insert `s` at `pos` |
| `insert(size_t pos, const char* s)` | From C-string |
| `insert(size_t pos, size_t n, char c)` | `n` copies of `c` |
| `insert(iterator p, char c)` | Insert at iterator |
| `insert(iterator p, size_t n, char c)` | Multiple chars at iterator |
| `insert(iterator p, InputIt first, InputIt last)` | Range at iterator |

### Erase

| Function | Purpose |
|---|---|
| `erase(size_t pos = 0, size_t len = npos)` | Remove `[pos, pos+len)` |
| `erase(iterator p)` | Remove char at `p` |
| `erase(iterator first, iterator last)` | Remove range |
| `pop_back()` **(C++11)** | Remove last char |

### Replace

| Function | Purpose |
|---|---|
| `replace(size_t pos, size_t len, const string& s)` | Replace `[pos, pos+len)` with `s` |
| `replace(size_t pos, size_t len, const char* s, size_t n)` | C-string variant |
| `replace(size_t pos, size_t len, size_t n, char c)` | Fill variant |
| `replace(iterator i1, iterator i2, ...)` | Iterator overloads for all above |

### Other

| Function | Purpose |
|---|---|
| `swap(string& other)` | O(1) swap |
| `copy(char* dst, size_t len, size_t pos = 0)` | Copy into `char*` buffer; **does not** null-terminate |

```cpp
std::string s = "hello";
s += " world";              // "hello world"
s.insert(5, ",");           // "hello, world"
s.erase(5, 1);              // "hello world"
s.replace(6, 5, "there");   // "hello there"
s.push_back('!');           // "hello there!"
```

---

## String Operations

### Search

All return `size_t` — either the match position or `std::string::npos` if not found.

| Function | Finds |
|---|---|
| `find(const string& s, size_t pos = 0)` | First occurrence of `s` at or after `pos` |
| `find(const char* s, size_t pos = 0)` | Same, C-string |
| `find(char c, size_t pos = 0)` | First occurrence of `c` |
| `rfind(...)` | Last occurrence at or before `pos` |
| `find_first_of(const string& chars, size_t pos = 0)` | First char in `chars` |
| `find_last_of(...)` | Last char in `chars` |
| `find_first_not_of(...)` | First char **not** in `chars` |
| `find_last_not_of(...)` | Last char **not** in `chars` |

```cpp
std::string s = "hello world";
s.find("world");              // 6
s.find('z');                  // std::string::npos
s.find_first_of("aeiou");     // 1 ('e')
s.find_last_not_of(" ");      // 10 ('d')

if (s.find("xyz") == std::string::npos) { /* not found */ }
```

### Extract

| Function | Returns |
|---|---|
| `substr(size_t pos = 0, size_t len = npos)` | New `string` copy of `[pos, pos+len)` |

```cpp
std::string("hello").substr(1, 3);  // "ell"
```

### Compare

`compare(...)` returns `int`: `0` if equal, negative if `*this` < argument, positive otherwise.

| Function | |
|---|---|
| `compare(const string& s)` | Compare whole strings |
| `compare(size_t pos, size_t len, const string& s)` | Compare substring with `s` |
| `compare(size_t pos, size_t len, const string& s, size_t subpos, size_t sublen)` | Substring vs substring |
| `compare(const char* s)` | vs C-string |
| `compare(size_t pos, size_t len, const char* s, size_t n)` | Substring vs first `n` of C-string |

Usually `==`, `!=`, `<`, `>`, `<=`, `>=` operators are preferred for equality/ordering.

---

## Non-member Functions

| Function | Purpose |
|---|---|
| `operator+` | Concatenation (returns new `string`) |
| `operator==`, `!=`, `<`, `>`, `<=`, `>=` | Lexicographic comparison |
| `operator<<` | Write to `ostream` |
| `operator>>` | Read one whitespace-delimited token from `istream` |
| `std::getline(istream& is, string& str, char delim = '\n')` | Read a full line (or until `delim`) |
| `std::swap(string&, string&)` | Free-function swap |

### C++11 conversions

| Function | Purpose |
|---|---|
| `std::to_string(int/long/double/...)` | Number → string |
| `std::stoi`, `std::stol`, `std::stoll` | String → integer |
| `std::stof`, `std::stod`, `std::stold` | String → float |
| `std::stoul`, `std::stoull` | String → unsigned |

```cpp
std::string line;
std::getline(std::cin, line);                // read entire line
std::string combined = "x = " + std::to_string(42);
```

---

## Constants

| Constant | Value |
|---|---|
| `std::string::npos` | `static_cast<size_t>(-1)` — the "not found" / "until end" sentinel |

---

## Gotchas

- **`operator[]` doesn't bounds-check** — use `at()` if you need safety.
- **`c_str()` is valid only until the string is modified** — don't store the pointer long-term.
- **`size()` returns `size_t` (unsigned).** Subtracting can underflow: `s.size() - 10` when `s.size() < 10` yields a huge number. Cast to signed or check first.
- **`find` returns `size_t`; `-1` is `npos`.** Always compare to `std::string::npos`, never to `-1`.
- **`substr(pos)` with `pos > size()` throws `std::out_of_range`.** `pos == size()` is allowed (returns empty).
- **String literals are `const char[]`, not `std::string`.** Concatenating two literals (`"a" + "b"`) is pointer arithmetic and fails. At least one operand must be `std::string`.
