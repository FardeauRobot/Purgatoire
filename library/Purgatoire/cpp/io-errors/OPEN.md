# File I/O in C++ — Open, Read, Write, Close

This guide is written for C++98 (what 42 uses). Everything lives in the `<fstream>` header, which pulls in `<iostream>` indirectly — but include both explicitly anyway.

---

## The three stream classes

| Class | Purpose | Think of it as |
|---|---|---|
| `std::ifstream` | **i**nput file stream | Reading a file |
| `std::ofstream` | **o**utput file stream | Writing to a file |
| `std::fstream` | bidirectional | Reading **and** writing the same file |

You pick based on what you need. For ex04 you want **one `ifstream` for the source file** and **one `ofstream` for the output file** — not `fstream`, because you're not modifying the source.

All three inherit from `std::ios` and behave like `std::cin` / `std::cout` for reading/writing. If you can read from `cin`, you can read from an `ifstream` — same operators, same idioms.

---

## Opening a file

There are two equivalent ways. **Prefer the constructor form** — it's more RAII, and closing is automatic when the stream goes out of scope.

### Constructor form (preferred)
```cpp
#include <fstream>

std::ifstream in("input.txt");
std::ofstream out("output.txt");
```

### `.open()` form
```cpp
std::ifstream in;
in.open("input.txt");     // open later, e.g. inside an if
// ...
in.close();               // optional — destructor will do it
```

### The C++98 `const char*` trap

In C++98, `ifstream` / `ofstream` constructors and `.open()` take a **`const char*`**, not a `std::string`. If `filename` is a `std::string`, you must call `.c_str()`:

```cpp
std::string filename = "input.txt";

std::ifstream in(filename);           // ❌ compile error in C++98
std::ifstream in(filename.c_str());   // ✅ correct
```

This is a common gotcha on ex04. C++11 added a `std::string` overload, but you don't have that.

---

## Open modes

Second argument to the constructor / `open()`. These are flags in `std::ios`, combined with `|`.

| Flag | Meaning |
|---|---|
| `std::ios::in` | Open for reading (default for `ifstream`) |
| `std::ios::out` | Open for writing (default for `ofstream`) |
| `std::ios::app` | **Append** — all writes go to end of file |
| `std::ios::ate` | "at end" — open and seek to end, but writes can still go anywhere |
| `std::ios::trunc` | Truncate — erase file contents on open (default for `ofstream`) |
| `std::ios::binary` | Binary mode (no text translation — relevant on Windows, noop on Linux) |

```cpp
std::ofstream log("app.log", std::ios::app);           // append to log
std::ifstream bin("data.bin", std::ios::binary);        // binary read
std::ofstream out("x.txt", std::ios::out | std::ios::trunc);  // explicit clear
```

For 42 exercises you rarely need anything beyond the defaults. For ex04, default open modes are perfect.

---

## Always check if the file opened

**Every file open can fail** (file doesn't exist, no permission, path wrong, etc.). Streams convert to `bool` — `false` means "bad state." Check it **every time**.

Three patterns, from simplest to most informative. Pick based on how much error detail your program needs.

### Pattern A — Minimum viable (just check and bail)

```cpp
std::ifstream in(filename.c_str());
if (!in) {
    std::cerr << "Error: could not open " << filename << std::endl;
    return 1;
}
```

`!in` returns `true` if the stream is in a bad state — which covers "file doesn't exist", "no permission", "path is a directory", etc. Good enough for 42.

### Pattern B — Slightly more explicit (`is_open()`)

```cpp
std::ifstream in(filename.c_str());
if (!in.is_open()) {
    std::cerr << "Error: could not open " << filename << std::endl;
    return 1;
}
```

Same effect in practice. Some readers / evaluators prefer `is_open()` because the intent is clearer: *"is the file open?"* rather than the more cryptic `!in`.

### Pattern C — With the OS-level reason (`errno` + `strerror`)

```cpp
#include <cerrno>
#include <cstring>

std::ifstream in(filename.c_str());
if (!in) {
    std::cerr << "Error opening " << filename << ": " << std::strerror(errno) << std::endl;
    return 1;
}
```

Produces messages like:
```
Error opening foo.txt: No such file or directory
Error opening /root/secret: Permission denied
```

Useful for real programs. Catch: C++98 doesn't *guarantee* that `ifstream` sets `errno` — it's implementation-defined. On Linux with g++/clang it does, so for 42 it works fine.

### `!in` vs `!in.is_open()` — what's the difference?

- `!in` → `true` if the stream is in **any** bad state (not open, or open but EOF, or read error, etc.)
- `!in.is_open()` → strictly whether the file handle is open

For **just after opening**, either works. For **mid-read error checks**, prefer the `!in` idiom (or check `fail()`, `eof()`, `bad()` explicitly — see below).

### Writing — same pattern

```cpp
std::ofstream out(output.c_str());
if (!out) {
    std::cerr << "Error: could not create " << output << std::endl;
    return 1;
}
```

`ofstream` can fail when the directory doesn't exist, the disk is full, or you don't have write permission.

### Defending against *post-open* write failures

Opening can succeed, but individual writes (or the final flush) can fail later:

```cpp
std::ofstream out(filename.c_str());
if (!out) { /* handle */ return 1; }

out << content;
out.close();         // force flush

if (!out) {
    std::cerr << "Error: write failed for " << filename << std::endl;
    return 1;
}
```

This catches disk-full errors that only surface on flush. Overkill for ex04, worth knowing it exists.

### The antipattern to avoid

```cpp
std::ifstream in(filename.c_str());
std::string line;
while (std::getline(in, line)) { /* ... */ }   // silently does nothing if file didn't open
```

No explicit check → no error message → user gets an empty output file and no clue why. **Always check immediately after the open.**

---

## Reading a file — the main patterns

Pick the one that matches your task. These are in order from most common to least.

### Pattern 1 — Read line by line (`std::getline`)

Best when the file is line-oriented and you process each line independently.

```cpp
std::ifstream in("input.txt");
std::string line;
while (std::getline(in, line)) {
    std::cout << line << std::endl;   // process `line`
}
```

- The `while` loop condition is `std::getline(...)` → returns the stream, which converts to `bool`. Loop ends on EOF or error.
- The trailing `\n` is **stripped** from `line`. If you write it back out, add it yourself.
- Works with any delimiter: `std::getline(in, line, ',')` reads CSV-style.

### Pattern 2 — Read word by word (`operator>>`)

Best when the file is whitespace-delimited (numbers, tokens, etc.).

```cpp
std::ifstream in("numbers.txt");
int n;
while (in >> n) {
    total += n;
}
```

- `operator>>` skips leading whitespace by default.
- Works for `int`, `double`, `std::string`, etc.
- Fails (loop exits) on EOF **or** on a format mismatch (e.g. reading `int` but seeing `"abc"`).

### Pattern 3 — Read the whole file into one string (`rdbuf()`)

Best when you need to process the file as a single blob — like ex04, where a match could span newlines.

```cpp
#include <fstream>
#include <sstream>

std::ifstream in(filename.c_str());
std::stringstream ss;
ss << in.rdbuf();              // dump entire file buffer into the stringstream
std::string content = ss.str();
```

**Why this works:** `rdbuf()` returns a pointer to the stream's underlying buffer. `<<` on a stream accepts a `streambuf*` and reads it to completion.

This is idiomatic in C++98 and is **the** way to slurp a file. Don't reinvent it with a byte-by-byte loop.

### Pattern 4 — Character by character (`get()`, `peek()`)

Rare at your level, but useful for parsers.

```cpp
std::ifstream in("input.txt");
char c;
while (in.get(c)) {
    // process c
}
```

- `in.get()` without args returns `int` (to distinguish EOF, which is -1).
- `in.peek()` returns the next char without consuming it.

### Pattern 5 — Fixed-size binary read (`read()`)

For binary files.

```cpp
std::ifstream in("data.bin", std::ios::binary);
char buffer[1024];
in.read(buffer, sizeof(buffer));
std::streamsize bytesRead = in.gcount();   // how many bytes were actually read
```

---

## Writing a file

Mirror of reading. Use `operator<<` like `std::cout`.

```cpp
std::ofstream out("output.txt");
if (!out) { /* error */ }

out << "Hello, world" << std::endl;
out << 42 << " " << 3.14 << std::endl;
```

- `std::endl` flushes the buffer **and** writes `\n`. For pure speed, use `"\n"` — it doesn't flush, so writes stay buffered.
- `operator<<` handles strings, numbers, everything `std::cout` handles.

### The ex04 copy loop
```cpp
std::ofstream out(output_filename.c_str());
out << transformed_content;   // assuming you built `transformed_content` as std::string
```

---

## Closing a file

### You usually don't need to call `close()`

When an `ifstream` or `ofstream` goes out of scope, its destructor closes the file automatically. This is **RAII** — resource acquisition is initialization. Rely on it.

```cpp
{
    std::ifstream in("input.txt");
    // ... use in ...
}   // `in` destroyed here, file closed automatically
```

### When to call `close()` explicitly

- You want to **reopen** the same stream with a different file.
- You need to **flush and verify** writes succeeded before the scope ends:
  ```cpp
  out.close();
  if (!out) {
      std::cerr << "Write failed" << std::endl;
  }
  ```
- You're writing and want to **release the handle ASAP** (another process is waiting to read it).

### What `close()` does
- Flushes any buffered writes to disk.
- Releases the OS file handle.
- Puts the stream in a "not open" state (but doesn't clear error flags — call `in.clear()` if you want to reuse the stream object).

---

## Stream state flags

Every stream tracks its own state. Four flags, four checks:

| State | `goodbit` | `eofbit` | `failbit` | `badbit` |
|---|---|---|---|---|
| Check with | `good()` | `eof()` | `fail()` | `bad()` |
| Meaning | All clear | End of file reached | Read failed (format mismatch, partial read) | Catastrophic error (hardware, OS) |

```cpp
if (in.eof())   std::cout << "reached EOF";
if (in.fail())  std::cout << "read failed";
if (in.bad())   std::cout << "irrecoverable error";
if (in.good())  std::cout << "all fine";
```

**`operator!()` returns `fail() || bad()`.** So `if (!in)` is "something went wrong" — typically what you want.

### Clearing flags
Once a stream goes bad (e.g. you tried to read `int` but got `"abc"`), **it stays bad** until you `clear()`:

```cpp
in.clear();              // reset all flags to goodbit
in.clear(std::ios::eofbit);   // clear everything except eofbit
```

This matters if you want to `seekg` back and re-read after hitting EOF.

---

## Seeking (positioning inside a file)

| Function | Purpose |
|---|---|
| `in.tellg()` | Get current read position (returns `streampos`) |
| `in.seekg(pos)` | Seek to absolute position |
| `in.seekg(off, std::ios::beg)` | Seek relative to beginning |
| `in.seekg(off, std::ios::cur)` | Seek relative to current position |
| `in.seekg(off, std::ios::end)` | Seek relative to end |
| `out.tellp()` / `out.seekp(...)` | Same, for output streams ("put" position) |

Common idiom — get file size:
```cpp
in.seekg(0, std::ios::end);
std::streamsize size = in.tellg();
in.seekg(0, std::ios::beg);   // back to start to actually read
```

You usually don't need this in ex04 — `rdbuf()` handles it for you.

---

## The 42-exercise 04 template

Putting it together for "*read file, transform, write to file.replace*":

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <filename> <s1> <s2>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string s1 = argv[2];
    std::string s2 = argv[3];

    if (s1.empty()) {
        std::cerr << "Error: s1 must not be empty" << std::endl;
        return 1;
    }

    // --- Read whole file ---
    std::ifstream in(filename.c_str());
    if (!in) {
        std::cerr << "Error: cannot open " << filename << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();   // optional but explicit

    // --- Transform (manual replace since std::string::replace is forbidden) ---
    std::string result;
    size_t i = 0;
    while (i < content.size()) {
        size_t found = content.find(s1, i);
        if (found == std::string::npos) {
            result += content.substr(i);
            break;
        }
        result += content.substr(i, found - i);
        result += s2;
        i = found + s1.size();
    }

    // --- Write ---
    std::ofstream out((filename + ".replace").c_str());
    if (!out) {
        std::cerr << "Error: cannot create output file" << std::endl;
        return 1;
    }
    out << result;
    // out closed automatically here

    return 0;
}
```

---

## Gotchas to remember

1. **C++98 needs `.c_str()`** on `std::string` when passing to `ifstream`/`ofstream`.
2. **Always check the stream after open** — don't assume the file exists.
3. **`getline` strips the `\n`** — if you re-emit lines, add it back.
4. **Mixing `>>` and `getline`** is dangerous: `>>` leaves trailing whitespace in the buffer, and the next `getline` sees an empty line. After `cin >> n`, call `cin.ignore()` before `getline`.
5. **`std::endl` flushes**, `"\n"` doesn't. In tight loops writing to a file, prefer `"\n"`.
6. **`eof()` isn't set until you try to read past the end** — so `while (!in.eof())` as a loop condition is a classic bug. It reads once past the end before checking. Always use `while (in >> x)` or `while (std::getline(in, line))` instead.
7. **Streams don't throw by default.** On failure, they set `failbit` and silently refuse further operations. Check `!in`.
8. **A failed `open()` leaves the stream in a bad state.** You can call `open()` again on the same object after `clear()` + `close()`, but starting fresh is cleaner.

---

## Minimum vocabulary to memorize

You should be able to reproduce these from memory:

```cpp
// Open + check
std::ifstream in(filename.c_str());
if (!in) { /* error */ }

// Read line by line
std::string line;
while (std::getline(in, line)) { /* ... */ }

// Read whole file
std::stringstream ss; ss << in.rdbuf();
std::string content = ss.str();

// Write
std::ofstream out(name.c_str());
out << "text\n";
```

If you've got those four fragments in your fingers, you can handle any 42-level file-I/O exercise.

---

## Further reading

- [`<fstream>` on cppreference](https://en.cppreference.com/w/cpp/header/fstream) — bookmark it.
- See also [STRING_FUNCTIONS.md](STRING_FUNCTIONS.md) for `std::string` operations you'll combine with file I/O.
