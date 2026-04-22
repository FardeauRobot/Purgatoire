# `<iostream>` and `<iomanip>` — Library Reference

This document summarizes the main user-facing API from the C++ standard headers `<iostream>` and `<iomanip>`, with practical examples for each feature.

Note:
- The C++ standard library evolves by version. This file focuses on the commonly available API (C++98 to modern C++).
- Some functions are member functions of stream classes (`std::istream`, `std::ostream`, `std::ios_base`, etc.).
- "Manipulator" means a helper used with `<<` or `>>` to change formatting/behavior.

---

## 1. `<iostream>`

`<iostream>` provides stream classes, stream state management, and standard stream objects.

### 1.1 Standard stream objects

| Object       | Description                               |
|--------------|-------------------------------------------|
| `std::cin`   | Standard input stream (`istream`)         |
| `std::cout`  | Standard output stream (`ostream`)        |
| `std::cerr`  | Standard error stream, unbuffered         |
| `std::clog`  | Standard error/log stream, buffered       |

Wide-character variants: `std::wcin`, `std::wcout`, `std::wcerr`, `std::wclog`

```cpp
#include <iostream>

int main() {
    int n;
    std::cout << "Enter a number: ";   // print to stdout
    std::cin >> n;                     // read from stdin
    std::cerr << "Error: bad value\n"; // unbuffered, goes to stderr immediately
    std::clog << "Log: got " << n << '\n'; // buffered log to stderr
}
```

---

### 1.2 Core extraction / insertion operations

- `operator>>(...)` on `istream`: formatted extraction (reads numbers, words, etc.)
- `operator<<(...)` on `ostream`: formatted insertion (prints values)
- `std::getline(istream&, std::string&)` — reads a full line, stops at `'\n'`
- `std::getline(istream&, std::string&, char delim)` — stops at `delim`

```cpp
#include <iostream>
#include <string>

int main() {
    std::string word, line;

    // >> skips leading whitespace, reads one word
    std::cin >> word;           // input "  hello world" -> word = "hello"

    std::cin.ignore();          // discard leftover '\n' before getline

    // getline reads the entire line including spaces
    std::getline(std::cin, line);  // input "foo bar baz" -> line = "foo bar baz"

    // getline with custom delimiter: reads until ';'
    std::getline(std::cin, line, ';'); // input "abc;def" -> line = "abc"
}
```

---

### 1.3 `std::ios` stream state checks

These tell you whether a stream is in a usable state.

| Function      | Returns `true` when...                        |
|---------------|-----------------------------------------------|
| `good()`      | No error flags are set                        |
| `eof()`       | End-of-file was reached                       |
| `fail()`      | A recoverable parse/input failure occurred    |
| `bad()`       | A serious (irrecoverable) stream error occurred |
| `rdstate()`   | Returns the raw `iostate` bitmask             |

```cpp
#include <iostream>

int main() {
    int x;
    std::cin >> x;

    if (std::cin.good())  std::cout << "All fine\n";
    if (std::cin.fail())  std::cout << "Parse failed (e.g. typed 'abc' for int)\n";
    if (std::cin.eof())   std::cout << "Reached end of input\n";
    if (std::cin.bad())   std::cout << "Stream buffer is broken\n";

    // Check all bits at once
    std::ios::iostate state = std::cin.rdstate();
    if (state & std::ios::failbit) std::cout << "failbit is set\n";
}
```

---

### 1.4 `std::ios` state changes

- `clear()` — reset all error flags (stream becomes good again)
- `clear(iostate)` — set stream to a specific state
- `setstate(iostate)` — add flags without clearing others
- `exceptions(iostate)` — make the stream throw on certain flags

```cpp
#include <iostream>

int main() {
    int x;
    std::cin >> x;                   // fails if user types "abc"

    if (std::cin.fail()) {
        std::cin.clear();            // clear failbit so the stream is usable again
        std::cin.ignore(1000, '\n'); // discard the bad input line
        std::cin >> x;              // try again
    }

    // Throw an exception when failbit or badbit is triggered
    std::cin.exceptions(std::ios::failbit | std::ios::badbit);
    try {
        std::cin >> x;
    } catch (const std::ios::failure& e) {
        std::cerr << "Stream exception: " << e.what() << '\n';
    }
}
```

---

### 1.5 Formatting flags (`flags`, `setf`, `unsetf`)

- `flags()` — get all format flags as a bitmask
- `flags(fmtflags)` — replace all flags
- `setf(fmtflags)` — turn on specific flags
- `setf(fmtflags, fmtflags mask)` — set a flag within a group (e.g. adjustfield)
- `unsetf(fmtflags)` — turn off specific flags

```cpp
#include <iostream>

int main() {
    // setf / unsetf directly
    std::cout.setf(std::ios::hex, std::ios::basefield);  // switch to hex
    std::cout << 255 << '\n';   // ff

    std::cout.unsetf(std::ios::hex);
    std::cout.setf(std::ios::dec, std::ios::basefield);  // back to decimal
    std::cout << 255 << '\n';   // 255

    // flags() snapshot and restore
    std::ios::fmtflags saved = std::cout.flags();        // save
    std::cout.setf(std::ios::showpos | std::ios::uppercase);
    std::cout << 42 << '\n';    // +42
    std::cout.flags(saved);     // restore
    std::cout << 42 << '\n';    // 42
}
```

---

### 1.6 Width, precision, fill

- `width(n)` — minimum field width for the *next* formatted output (resets after use)
- `precision(n)` — significant digits (default) or digits after decimal (with `fixed`/`scientific`)
- `fill(c)` — character used to pad when content is shorter than width

```cpp
#include <iostream>

int main() {
    // width resets after one use
    std::cout.width(8);
    std::cout << 42 << '\n';        //       42  (6 spaces + 42)
    std::cout << 42 << '\n';        // 42        (width is gone)

    // fill persists until changed
    std::cout.fill('0');
    std::cout.width(6);
    std::cout << 42 << '\n';        // 000042

    // precision
    std::cout.precision(4);
    std::cout << 3.14159 << '\n';   // 3.142 (4 significant digits)

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(2);
    std::cout << 3.14159 << '\n';   // 3.14 (2 decimal places)
}
```

---

### 1.7 Locale and ties

- `getloc()` — returns the current locale
- `imbue(locale)` — change locale (affects numeric/monetary/time formatting)
- `tie()` — returns pointer to the tied stream (by default `cout` is tied to `cin`)
- `tie(ostream*)` — link a stream so it flushes before reads on this stream

```cpp
#include <iostream>
#include <locale>

int main() {
    // Imbue French locale for number formatting (e.g. thousands separator)
    std::cout.imbue(std::locale("fr_FR.UTF-8"));
    std::cout << 1000000.5 << '\n';   // 1 000 000,5 (locale-dependent)

    // tie: cout is flushed automatically before cin reads
    // This ensures prompts appear before blocking on input
    std::ostream* old_tie = std::cin.tie();  // old_tie == &std::cout
    std::cin.tie(nullptr);                   // remove the tie (less safe, faster)
    std::cin.tie(old_tie);                   // restore
}
```

---

### 1.8 Buffer and sync

- `rdbuf()` — get the underlying `streambuf`
- `rdbuf(streambuf*)` — redirect the stream to a different buffer
- `std::ios_base::sync_with_stdio(bool)` — sync C and C++ I/O (default: true)

```cpp
#include <iostream>
#include <sstream>

int main() {
    // Redirect cout to a string buffer
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    std::cout << "captured output";
    std::cout.rdbuf(old);           // restore cout
    std::cout << oss.str() << '\n'; // "captured output"

    // Disable sync for faster I/O (do this before any I/O)
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
}
```

---

### 1.9 `std::istream` — unformatted input

#### `get()` overloads

Reads characters without skipping whitespace (unlike `>>`).

```cpp
#include <iostream>

int main() {
    // Read one char as int (returns EOF on end)
    int ch = std::cin.get();            // e.g. reads 'A', returns 65

    // Read one char into a variable
    char c;
    std::cin.get(c);                    // c = next char including spaces/newlines

    // Read up to n-1 chars into buffer, stops at newline (keeps it in stream)
    char buf[20];
    std::cin.get(buf, 20);              // reads max 19 chars

    // Same but with custom delimiter
    std::cin.get(buf, 20, ';');         // stops at ';' or 19 chars
}
```

#### `getline()` on istream

```cpp
char buf[50];
std::cin.getline(buf, 50);       // reads line, discards '\n'
std::cin.getline(buf, 50, ';');  // reads until ';', discards it
```

#### `read()` and `readsome()`

```cpp
#include <iostream>
#include <fstream>

int main() {
    std::ifstream f("data.bin", std::ios::binary);

    char data[16];
    f.read(data, 16);                    // read exactly 16 bytes
    std::streamsize got = f.gcount();    // how many were actually read

    // readsome: reads only what's immediately available (no blocking)
    char tmp[8];
    std::streamsize n = std::cin.readsome(tmp, 8);
}
```

#### `gcount()`

```cpp
std::cin.read(buf, 100);
std::cout << "Read " << std::cin.gcount() << " bytes\n";
```

---

### 1.10 `std::istream` — lookahead and pushback

- `peek()` — look at next character without extracting it
- `unget()` — push back the last character read
- `putback(char)` — push a specific character back

```cpp
#include <iostream>

int main() {
    // peek: check what's coming without consuming it
    if (std::cin.peek() == '-') {
        char sign;
        std::cin >> sign;   // consume the '-'
    }

    // unget: put back whatever was last read
    char c;
    std::cin.get(c);
    if (c == '\n') std::cin.unget(); // put the newline back

    // putback: put a specific char back (may differ from what was read)
    std::cin.get(c);
    std::cin.putback('X'); // next get() will return 'X'
}
```

---

### 1.11 `std::istream` — skipping and positioning

- `ignore()` — discard one character
- `ignore(n)` — discard up to n characters
- `ignore(n, delim)` — discard up to n characters or until delim (inclusive)
- `seekg(pos)` — seek to absolute position
- `seekg(off, dir)` — seek relative to `beg`, `cur`, or `end`
- `tellg()` — current read position

```cpp
#include <iostream>
#include <sstream>

int main() {
    // ignore: skip the '\n' left after >>
    int x;
    std::cin >> x;
    std::cin.ignore(1000, '\n'); // discard rest of line

    // seekg / tellg (works on file/string streams, not cin)
    std::istringstream ss("hello world");
    std::streampos pos = ss.tellg();  // pos = 0
    ss.seekg(6);                      // jump to position 6
    std::string word;
    ss >> word;                       // word = "world"
    ss.seekg(0, std::ios::beg);       // back to start
    ss >> word;                       // word = "hello"
}
```

---

### 1.12 `std::ostream` — unformatted output and positioning

- `put(char)` — write a single character
- `write(ptr, n)` — write n raw bytes
- `flush()` — flush the output buffer
- `seekp(pos)` / `seekp(off, dir)` — seek write position
- `tellp()` — current write position

```cpp
#include <iostream>
#include <fstream>

int main() {
    // put: single character
    std::cout.put('A');          // prints A (no newline)
    std::cout.put('\n');

    // write: raw block (good for binary or fixed-length output)
    const char msg[] = "hello";
    std::cout.write(msg, 5);     // prints "hello"
    std::cout.put('\n');

    // flush: force buffered output to the terminal/file
    std::cout << "progress..." << std::flush; // immediately visible

    // seekp / tellp on file streams
    std::fstream f("out.bin", std::ios::in | std::ios::out | std::ios::binary);
    f.seekp(10);                 // move write head to byte 10
    f.write("XX", 2);           // overwrite bytes 10-11
    std::streampos p = f.tellp(); // p = 12
}
```

---

### 1.13 `ios_base` formatting flag groups

**Adjustfield** (alignment):
```cpp
std::cout << std::left  << std::setw(10) << "hi" << "|\n"; // "hi        |"
std::cout << std::right << std::setw(10) << "hi" << "|\n"; // "        hi|"
std::cout << std::internal << std::setw(10) << -42 << '\n'; // "-       42"
```

**Basefield** (integer base):
```cpp
std::cout << std::dec << 255 << '\n'; // 255
std::cout << std::oct << 255 << '\n'; // 377
std::cout << std::hex << 255 << '\n'; // ff
```

**Floatfield** (floating-point style):
```cpp
double v = 123456.789;
std::cout << std::defaultfloat << v << '\n';              // 123457
std::cout << std::fixed       << std::setprecision(2) << v << '\n'; // 123456.79
std::cout << std::scientific  << std::setprecision(2) << v << '\n'; // 1.23e+05
std::cout << std::hexfloat    << v << '\n';               // 0x1.e240cap+16
```

**Other common flags**:
```cpp
std::cout << std::boolalpha  << true  << '\n'; // true  (not 1)
std::cout << std::noboolalpha << true << '\n'; // 1
std::cout << std::showbase   << std::hex << 255 << '\n'; // 0xff
std::cout << std::showpos    << 42   << '\n';  // +42
std::cout << std::showpoint  << 1.0  << '\n';  // 1.00000
std::cout << std::uppercase  << std::hex << 255 << '\n'; // FF
```

---

### 1.14 Basic stream manipulators

- `std::endl` — insert `'\n'` and flush
- `std::ends` — insert `'\0'` (useful in `ostringstream`)
- `std::flush` — flush without newline
- `std::ws` — skip leading whitespace on input

```cpp
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::cout << "line 1" << std::endl; // newline + immediate flush
    std::cout << "buffered" << std::flush; // no newline, just flush

    std::ostringstream oss;
    oss << "hello" << std::ends;        // oss contains "hello\0"

    // std::ws: discard whitespace before reading
    std::istringstream ss("   42");
    int n;
    ss >> std::ws >> n;                 // n = 42 (spaces skipped)
}
```

---

## 2. `<iomanip>`

`<iomanip>` provides parameterized manipulators for formatting.

---

### 2.1 `std::setw(n)` — field width

Sets the minimum width for the **next** formatted operation only (width resets after each use).

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::setw(8) << 42     << '\n'; //       42
    std::cout << std::setw(8) << "hi"   << '\n'; //       hi
    std::cout << std::setw(8) << 3.14   << '\n'; //     3.14

    // Combine with setfill and alignment
    std::cout << std::setfill('-') << std::setw(10) << std::left  << "left"  << '\n'; // left------
    std::cout << std::setfill('-') << std::setw(10) << std::right << "right" << '\n'; // -----right
}
```

---

### 2.2 `std::setfill(c)` — fill character

Sets the padding character (persists until changed, unlike `setw`).

```cpp
#include <iostream>
#include <iomanip>

int main() {
    // Zero-pad numbers (classic use)
    std::cout << std::setfill('0') << std::setw(5) << 42   << '\n'; // 00042
    std::cout << std::setfill('0') << std::setw(5) << 7    << '\n'; // 00007

    // Dot separator for alignment in tables
    std::cout << std::setfill('.')
              << std::setw(20) << std::left << "Item"
              << std::setw(6)  << std::right << 9.99 << '\n';
    // Item................  9.99
}
```

---

### 2.3 `std::setprecision(n)` — floating-point precision

```cpp
#include <iostream>
#include <iomanip>

int main() {
    double pi = 3.14159265358979;

    // Default: n significant digits
    std::cout << std::setprecision(3) << pi << '\n'; // 3.14

    // With fixed: n digits after the decimal point
    std::cout << std::fixed << std::setprecision(5) << pi << '\n'; // 3.14159

    // With scientific
    std::cout << std::scientific << std::setprecision(2) << pi << '\n'; // 3.14e+00

    // Back to default
    std::cout << std::defaultfloat << std::setprecision(6) << pi << '\n'; // 3.14159
}
```

---

### 2.4 `std::setbase(n)` — integer base

```cpp
#include <iostream>
#include <iomanip>

int main() {
    int n = 255;
    std::cout << std::setbase(10) << n << '\n'; // 255
    std::cout << std::setbase(16) << n << '\n'; // ff
    std::cout << std::setbase(8)  << n << '\n'; // 377

    // setbase(0) on input: auto-detect prefix (0x = hex, 0 = octal, else decimal)
    int x;
    std::istringstream ss("0xff");
    ss >> std::setbase(0) >> x; // x = 255
}
```

---

### 2.5 `std::showbase` / `std::noshowbase` — base prefix

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::hex << std::showbase << 255 << '\n';  // 0xff
    std::cout << std::oct << std::showbase << 255 << '\n';  // 0377
    std::cout << std::hex << std::noshowbase << 255 << '\n'; // ff
}
```

---

### 2.6 `std::showpos` / `std::noshowpos` — explicit `+` sign

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::showpos << 42   << '\n'; // +42
    std::cout << std::showpos << -42  << '\n'; // -42 (sign already there)
    std::cout << std::showpos << 0    << '\n'; // +0
    std::cout << std::noshowpos << 42 << '\n'; // 42
}
```

---

### 2.7 `std::boolalpha` / `std::noboolalpha`

```cpp
#include <iostream>
#include <iomanip>
#include <sstream>

int main() {
    // Output
    std::cout << std::boolalpha  << true << " " << false << '\n'; // true false
    std::cout << std::noboolalpha << true << " " << false << '\n'; // 1 0

    // Input: also affects parsing
    bool b;
    std::istringstream ss("true");
    ss >> std::boolalpha >> b; // b = true
}
```

---

### 2.8 `std::uppercase` / `std::nouppercase`

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::uppercase << std::hex << 0xdeadbeef << '\n'; // DEADBEEF
    std::cout << std::uppercase << std::scientific << 1234.5 << '\n'; // 1.2345E+03
    std::cout << std::nouppercase << std::hex << 0xdeadbeef << '\n'; // deadbeef
}
```

---

### 2.9 `std::showpoint` / `std::noshowpoint`

Forces or suppresses the decimal point and trailing zeros for floating-point values.

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << std::showpoint  << 1.0  << '\n'; // 1.00000 (decimal + zeros shown)
    std::cout << std::noshowpoint << 1.0 << '\n'; // 1       (no trailing zeros)
    std::cout << std::showpoint  << 1.5  << '\n'; // 1.50000
}
```

---

### 2.10 `std::skipws` / `std::noskipws`

Controls whether `>>` skips leading whitespace. Default is on.

```cpp
#include <iostream>
#include <sstream>

int main() {
    std::istringstream ss("  A  B");
    char c;

    // Default (skipws on): skips spaces before each char
    ss >> c; // c = 'A'
    ss >> c; // c = 'B'

    ss.clear(); ss.str("  A  B");
    ss >> std::noskipws;
    ss >> c; // c = ' ' (space is read as-is)
    ss >> c; // c = ' '
    ss >> c; // c = 'A'
}
```

---

### 2.11 `std::unitbuf` / `std::nounitbuf`

Flush after every output operation (like `cerr`'s default behavior).

```cpp
#include <iostream>

int main() {
    // With unitbuf: every << flushes immediately (slow but safe for logs)
    std::cout << std::unitbuf;
    std::cout << "step 1\n"; // flushed instantly
    std::cout << "step 2\n"; // flushed instantly

    std::cout << std::nounitbuf; // back to normal buffered output
}
```

---

### 2.12 `std::get_money` / `std::put_money`

Parse and format monetary values using the stream's locale.

```cpp
#include <iostream>
#include <iomanip>
#include <sstream>
#include <locale>

int main() {
    // put_money: format a monetary value (in minor units, e.g. cents)
    std::cout.imbue(std::locale("en_US.UTF-8"));
    std::cout << std::showbase << std::put_money(123456) << '\n'; // $1,234.56

    // get_money: parse a monetary value from input
    long double amount;
    std::istringstream ss("$1,234.56");
    ss.imbue(std::locale("en_US.UTF-8"));
    ss >> std::get_money(amount); // amount = 123456 (minor units)
}
```

---

### 2.13 `std::get_time` / `std::put_time`

Parse and format date/time values using `std::tm`.

```cpp
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

int main() {
    // put_time: format a std::tm struct
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::cout << std::put_time(now, "%Y-%m-%d %H:%M:%S") << '\n'; // 2026-04-10 14:30:00

    // get_time: parse a date string into std::tm
    std::tm parsed = {};
    std::istringstream ss("2026-04-10 14:30:00");
    ss >> std::get_time(&parsed, "%Y-%m-%d %H:%M:%S");
    if (!ss.fail())
        std::cout << "Year: " << (parsed.tm_year + 1900) << '\n'; // Year: 2026

    // Common format specifiers:
    // %Y = 4-digit year, %m = month (01-12), %d = day (01-31)
    // %H = hour (00-23), %M = minute, %S = second
    // %A = weekday name, %B = month name, %I = 12h hour, %p = AM/PM
}
```

---

## 3. Combining manipulators — practical patterns

### Formatted table output

```cpp
#include <iostream>
#include <iomanip>
#include <string>

int main() {
    const int W1 = 16, W2 = 8, W3 = 10;
    std::cout << std::left
              << std::setw(W1) << "Name"
              << std::setw(W2) << "Score"
              << std::setw(W3) << "Grade" << '\n';
    std::cout << std::string(W1 + W2 + W3, '-') << '\n';

    std::cout << std::setw(W1) << "Alice"
              << std::setw(W2) << std::right << std::fixed << std::setprecision(1) << 95.5
              << std::setw(W3) << "A" << '\n';
    std::cout << std::left
              << std::setw(W1) << "Bob"
              << std::setw(W2) << std::right << 78.0
              << std::setw(W3) << "C+" << '\n';
}
// Name            Score     Grade
// ----------------------------------
// Alice             95.5         A
// Bob               78.0        C+
```

### Reading and validating user input

```cpp
#include <iostream>
#include <limits>

int readInt(const std::string& prompt) {
    int x;
    while (true) {
        std::cout << prompt;
        if (std::cin >> x) break;             // success
        std::cin.clear();                      // clear error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flush bad input
        std::cout << "Invalid input, try again.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flush trailing newline
    return x;
}
```

### Redirect `cout` to capture output

```cpp
#include <iostream>
#include <sstream>

int main() {
    std::ostringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // redirect

    std::cout << "Hello, captured world!";

    std::cout.rdbuf(old);                // restore
    std::string captured = buffer.str(); // "Hello, captured world!"
    std::cout << captured << '\n';
}
```

---

## 4. Important distinctions

- `<iostream>` gives you stream objects/classes and core stream operations.
- `<iomanip>` gives you formatting manipulators (especially parameterized ones: `setw`, `setprecision`, `setfill`, `setbase`, `put_time`, `put_money`).
- Most flag-style manipulators (`hex`, `fixed`, `left`, `boolalpha`, …) are sticky — they persist until explicitly changed.
- `setw` is the **only** common manipulator that resets after one use.
- Prefer `'\n'` over `std::endl` in performance-sensitive loops; `endl` flushes on every call.