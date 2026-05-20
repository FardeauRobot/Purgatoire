# C++ Module 02 — Quiz (15 questions)

> Orthodox Canonical Form, operator overloading, fixed-point, const-correctness.

---

### 1. The four members
List the four members of the Orthodox Canonical Form, with the *question* each one answers.

> 

---

### 2. Why write OCF when the compiler defaults work?
For `Fixed` (a single `int` member) the compiler-generated copy ctor and assignment do exactly the right thing. Why does the subject force you to write all four explicitly anyway?

> 

---

### 3. Code prediction — order of construction
```cpp
Fixed a;            // (1)
Fixed b(a);         // (2)
Fixed c;            // (3)
c = b;              // (4)
```
What 4 lines does this print (in order), assuming each member prints its canonical trace?

> 

---

### 4. Self-assignment guard
```cpp
Fixed& Fixed::operator=(Fixed const& other) {
    std::cout << "Copy assignment operator called\n";
    if (this != &other)
        m_raw = other.getRawBits();
    return *this;
}
```
For a single-int `Fixed`, the guard is theatre. **When does it become load-bearing**, and why?

> 

---

### 5. Member-init order
```cpp
class Foo {
    int a;
    int b;
public:
    Foo() : b(1), a(b + 1) {}   // <- bug
};
```
What is `a` actually initialised to, and what rule causes this?

> 

---

### 6. Header / implementation split
True/false (project rule):
- Function bodies in `.hpp` are forbidden. (T/F): 
- A `static const int m_fract_bits = 8;` initialisation is allowed inside the class definition. (T/F): 
- A `static const float m_pi = 3.14f;` initialisation is allowed inside the class definition. (T/F): 

> 

---

### 7. `const`-correctness
Explain what each `const` means:
```cpp
Fixed const& min(Fixed const& a, Fixed const& b);
                      ^^^^^      ^^^^^      ^^^^^
                      (1)        (2)        (3)
```
- (1): 
- (2): 
- (3): 

> 

---

### 8. Method-level `const`
```cpp
int getRawBits() const;
```
What does the `const` after the parens promise? What error do you get if you try to call this on a `const Fixed&` and you forgot the `const`?

> 

---

### 9. Member vs free operator
Among `operator=`, `operator<<` (for streaming), `operator+`, `operator==`:
- Which **must** be a member? 
- Which **must** be a free (non-member) function? Why? 
- Which can be either (and why we usually pick member here)? 

> 

---

### 10. Fixed-point math — shifts
For a Q8 `Fixed` (8 fractional bits), explain the shift in each:
- `m_raw = n << 8;` (int constructor): 
- `return m_raw >> 8;` (toInt): 
- `(a.m_raw * b.m_raw) >> 8;` (multiplication): 
- `(a.m_raw << 8) / b.m_raw;` (division): 

> 

---

### 11. Pre vs post increment
Write the **two signatures** for `operator++` on `Fixed`:
- pre-increment: 
- post-increment: 

What is the *purpose* of the `int` parameter on the post-form?

> 

---

### 12. Return types — value vs reference
Why does `operator=` return `Fixed&` while `operator+` returns `Fixed` (by value)? Phrase it in terms of what `a = b = c;` and `a + b + c;` *do*.

> 

---

### 13. `roundf` vs static_cast
```cpp
Fixed::Fixed(float const f)
    : m_raw(static_cast<int>(std::roundf(f * 256))) {}
```
What goes wrong if you swap to `std::roundf(static_cast<int>(f * 256))`?

> 

---

### 14. Code prediction — comparison
```cpp
Fixed a(0.1f);
Fixed b(0.1f);
std::cout << (a == b) << '\n';
```
What does this print, and how does comparing two `Fixed` differ from comparing two `float`s with the same idea?

> 

---

### 15. BSP (bonus): `const` member trap
```cpp
class Point {
    Fixed const x;
    Fixed const y;
};
```
Why is writing a meaningful `Point::operator=` impossible here? What's the typical escape hatch that the moulinette accepts?

> 
