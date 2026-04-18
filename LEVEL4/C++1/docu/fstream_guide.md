# The `<fstream>` Library in C++

The `<fstream>` header provides file stream classes for reading from and writing to files. It builds on top of `<iostream>`.

## Class Hierarchy

```
ios_base
   └── ios
         ├── istream ──┐
         │             ├── iostream
         └── ostream ──┘
              │
              ├── ifstream    (input file stream)
              ├── ofstream    (output file stream)
              └── fstream     (input/output file stream)
```

## The Three Main Classes

| Class      | Purpose                        | Default mode           |
|------------|--------------------------------|------------------------|
| `ifstream` | Read from files                | `ios::in`              |
| `ofstream` | Write to files                 | `ios::out \| ios::trunc` |
| `fstream`  | Read and write to files        | `ios::in \| ios::out`    |

---

## Constructors

```cpp
std::ifstream();                                          // Default
std::ifstream(const char* filename,
              std::ios_base::openmode mode = ios::in);    // Open on construction
std::ifstream(const std::string& filename,
              std::ios_base::openmode mode = ios::in);    // C++11
```

The same pattern exists for `ofstream` and `fstream` (with their own default modes).

---

## Open Modes (`std::ios_base::openmode`)

| Flag          | Meaning                                                      |
|---------------|--------------------------------------------------------------|
| `ios::in`     | Open for reading                                             |
| `ios::out`    | Open for writing                                             |
| `ios::app`    | Append — all writes go to end of file                        |
| `ios::ate`    | Seek to end of file immediately after opening                |
| `ios::trunc`  | Discard file contents if it exists                           |
| `ios::binary` | Open in binary mode (no text translation)                    |

Combine with bitwise OR: `std::ios::in | std::ios::binary`

---

## Member Functions

### Opening / Closing

| Function                                        | Description                                |
|-------------------------------------------------|--------------------------------------------|
| `open(filename, mode)`                          | Open a file on an already-constructed stream |
| `close()`                                       | Close the file                             |
| `is_open()`                                     | Returns `true` if a file is currently open |

```cpp
std::ifstream file;
file.open("data.txt");
if (file.is_open()) { /* ... */ }
file.close();
```

### Reading (from `istream` / `ifstream`)

| Function                          | Description                                            |
|-----------------------------------|--------------------------------------------------------|
| `operator>>`                      | Formatted extraction (whitespace-delimited)            |
| `get()`                           | Read a single character                                |
| `get(char& c)`                    | Read one char into `c`                                 |
| `get(char* s, streamsize n)`      | Read up to `n-1` chars, stops at `\n`                  |
| `getline(char* s, streamsize n)`  | Like `get` but consumes the `\n`                       |
| `read(char* s, streamsize n)`     | Raw binary read of `n` bytes                           |
| `readsome(char* s, streamsize n)` | Read whatever is immediately available                 |
| `peek()`                          | Look at next char without extracting                   |
| `unget()`                         | Put back the last extracted character                  |
| `putback(char c)`                 | Put a specific character back into the stream          |
| `ignore(n, delim)`                | Discard up to `n` chars, or until `delim` is reached   |
| `gcount()`                        | Number of chars extracted by the last unformatted read |

Free function worth knowing:
```cpp
std::getline(std::istream& is, std::string& str, char delim = '\n');
```

### Writing (from `ostream` / `ofstream`)

| Function                          | Description                          |
|-----------------------------------|--------------------------------------|
| `operator<<`                      | Formatted insertion                  |
| `put(char c)`                     | Write a single character             |
| `write(const char* s, streamsize n)` | Raw binary write of `n` bytes     |
| `flush()`                         | Force buffer to flush to file        |

### Positioning (seek / tell)

| Function          | Description                                      |
|-------------------|--------------------------------------------------|
| `tellg()`         | Get current read position                        |
| `tellp()`         | Get current write position                       |
| `seekg(pos)`      | Set read position (absolute)                     |
| `seekg(off, dir)` | Set read position (relative)                     |
| `seekp(pos)`      | Set write position (absolute)                    |
| `seekp(off, dir)` | Set write position (relative)                    |

Direction values:
- `std::ios::beg` — from beginning
- `std::ios::cur` — from current position
- `std::ios::end` — from end

### State Flags

| Function       | Description                                         |
|----------------|-----------------------------------------------------|
| `good()`       | No errors                                           |
| `eof()`        | End-of-file reached                                 |
| `fail()`       | Logical error (e.g. bad format) or I/O error        |
| `bad()`        | Serious I/O error                                   |
| `rdstate()`    | Get raw state flags                                 |
| `setstate(s)`  | Set state flags                                     |
| `clear(s = goodbit)` | Clear / reset state flags                     |
| `operator!`    | Equivalent to `fail()`                              |
| `operator bool`| Equivalent to `!fail()`                             |

### Buffer / Misc

| Function          | Description                               |
|-------------------|-------------------------------------------|
| `rdbuf()`         | Get/set the underlying `filebuf`          |
| `swap(other)`     | Swap two streams (C++11)                  |
| `tie()`           | Get/set tied stream (e.g. flush `cout` before reading `cin`) |

---

## Typical Patterns

### Read a file line by line
```cpp
std::ifstream file("input.txt");
if (!file)
    return 1;

std::string line;
while (std::getline(file, line))
    std::cout << line << '\n';
```

### Write to a file
```cpp
std::ofstream out("output.txt");
if (!out)
    return 1;

out << "Hello " << 42 << '\n';
```

### Read/write (in-place edit)
```cpp
std::fstream f("data.bin", std::ios::in | std::ios::out | std::ios::binary);
```

### Binary I/O
```cpp
std::ofstream out("data.bin", std::ios::binary);
int n = 42;
out.write(reinterpret_cast<const char*>(&n), sizeof(n));

std::ifstream in("data.bin", std::ios::binary);
int m;
in.read(reinterpret_cast<char*>(&m), sizeof(m));
```

### Get file size
```cpp
std::ifstream f("file.txt", std::ios::ate | std::ios::binary);
std::streamsize size = f.tellg();
f.seekg(0, std::ios::beg);
```

---

## Gotchas

- **Always check the stream** after opening (`if (!file) ...`) — the constructor won't throw on failure.
- **`eof()` is only set after** a failed read past the end, not when the last byte is read. Don't loop on `while (!file.eof())`.
- **Destructors close the file** automatically (RAII), so explicit `close()` is usually unnecessary.
- **Mixing `>>` and `getline`** leaves a `\n` in the buffer — use `ignore()` between them.
- **`ofstream` truncates by default** — use `ios::app` to append.
