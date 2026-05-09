# C++ Module 00 — Quiz (15 questions)

> Answer in the blanks (`>`). Write code where asked. Don't peek at TOPICS.md.

---

### 1. Namespaces & headers
Why is `using namespace std;` forbidden in this project, and what do you write instead when you need `cout`?

>   usi

---

### 2. Streams vs printf
What does this line print, and explain why **without** mentioning a format string:
```cpp
int n = 42;
std::cout << "n=" << n << '\n';
```

> 

---

### 3. `std::endl` vs `"\n"`
Functionally, what is the difference between `std::endl` and `"\n"`? Give a debugging scenario where the difference matters.

> 

---

### 4. C → C++ headers
Where does `std::toupper` come from in C++? What is the include name, and how does it differ from the C header?

> 

---

### 5. Code prediction — Megaphone edge case
What does this program print when called with **no** arguments?
```cpp
int main(int argc, char** argv) {
    if (argc < 2)
        std::cout << "* LOUD AND UNBEARABLE FEEDBACK NOISE *\n";
    for (int i = 1; i < argc; i++)
        for (size_t j = 0; j < std::string(argv[i]).size(); j++)
            std::cout << (char)std::toupper(argv[i][j]);
    return 0;
}
```

> 

---

### 6. Class design — public vs private
In `PhoneBook`, the array `m_contacts[8]` is private. Why? What would go wrong if it were public?

> 

---

### 7. Const-correct getter
Write the **declaration** (header line) for a getter `getFirstName` on `Contact` that returns a `std::string` and promises not to modify the contact. Explain each `const` if any.

> 

---

### 8. `<iomanip>` — sticky vs single-shot
Among `std::setw(10)`, `std::setfill(' ')`, `std::right`, which apply to *only* the next insertion, and which are **sticky** (persist until changed)?

> 

---

### 9. Truncation rule (PhoneBook)
The string `"Bartholomew"` (11 chars) must fit in a 10-character column. What exactly do you print, and why?

> 

---

### 10. `std::cin >> n` followed by `std::getline`
This pattern is broken. What goes wrong, and what is the standard one-line fix?
```cpp
int n;
std::string line;
std::cin >> n;
std::getline(std::cin, line); // ← broken
```

> 

---

### 11. EOF handling
The user presses Ctrl-D during a `std::getline(std::cin, s)` prompt. What state is `std::cin` left in, and what should your loop do to avoid spinning forever?

> 

---

### 12. Static members — declaration vs definition
For `Account`:
```cpp
class Account {
    static int _nbAccounts;
};
```
Where do you put `int Account::_nbAccounts = 0;`, and what error do you get at link time if you forget?

> 

---

### 13. Static method semantics
Why can a `static` member function not access a non-static member? Phrase it in terms of `this`.

> 

---

### 14. Memory layout — what lives where
In an instance `Account a;`, where do these live (stack / heap / .data / .rodata / .bss)?
- `a` itself: 
- `a._amount` (non-static int): 
- `Account::_nbAccounts` (static int, zero-initialised): 

> 

---

### 15. Compile-flag trap
You compile with `-Wall -Wextra -Werror -std=c++98` and get this warning-as-error:
```
warning: comparison between signed and unsigned integer expressions
```
Quote a typical line of Megaphone code that produces it, and give the fix.

> 
