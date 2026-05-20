# C++ Module 01 — Quiz (15 questions)

> Memory, references, pointers, fstream, member-function pointers, switch.

---

### 1. The four storage choices
Match each scenario to **stack / heap / pointer / reference**:
- A local 4-byte counter inside a 30-line function: 
- An object of unknown size at compile time, returned to the caller: 
- A class member that must always refer to *some* object, never null: 
- A class member that may swap targets later or be initially absent: 

> 

---

### 2. `new` / `delete` rules
Fill in the answers:
- Every `new` must be matched with: 
- Every `new[]` must be matched with: 
- What happens if you do `delete[] p;` where `p = new T;`? 

> 

---

### 3. Code prediction — Zombie traces
```cpp
void randomChump(std::string name) {
    Zombie z(name);
    z.announce();
}
int main() {
    randomChump("Foo");
    Zombie* z = newZombie("Bar");
    z->announce();
    delete z;
}
```
List the printed lines in order (assume each ctor prints `"<name> born"` and each dtor prints `"<name> dies"`, and `announce()` prints `"<name>: BraiiiiiinnnzzzZ..."`).

> 

---

### 4. Why default-ctor + setName for `zombieHorde`
Explain in one sentence why you cannot write `new Zombie[N]("Bob");` in C++98, and what the canonical workaround is.

> 

---

### 5. The `delete` / `delete[]` mismatch
You wrote:
```cpp
Zombie* horde = new Zombie[5];
delete horde;   // <-- typo
```
What category of bug is this (precise term)? What does Valgrind report?

> 

---

### 6. Reference vs pointer — addresses
```cpp
std::string s = "hello";
std::string* p = &s;
std::string& r = s;
std::cout << &s << '\n' << p << '\n' << &r << '\n';
```
What relationship exists between these three printed values? Why?

> 

---

### 7. Reference rules
Tick true/false:
- A reference can be `NULL`. (T/F): 
- A reference can be reseated to refer to a different object after construction. (T/F): 
- `T& ref;` (uninitialised) is legal. (T/F): 
- A reference member must be initialised in the constructor's init list. (T/F): 

> 

---

### 8. `HumanA` vs `HumanB` — design intent
Why does `HumanA` store its `Weapon` as `Weapon&` while `HumanB` stores it as `Weapon*`? Phrase it in terms of *the relationship between the human and the weapon*.

> 

---

### 9. Why `getType()` returns `const std::string&`
Two reasons: one performance, one safety.

> 

---

### 10. Code prediction — weapon update propagates
```cpp
Weapon club("crude spiked club");
HumanA bob("Bob", club);
bob.attack();
club.setType("some other type of club");
bob.attack();
```
What does this print (sketch)? What property of `HumanA`'s storage makes the second line use the new type?

> 

---

### 11. `<fstream>` flow
Sketch the **5 lines** that open `argv[1]`, check for failure, copy its full contents into a `std::string`, and close. Use only `<fstream>` and `<sstream>` if you want.

> 

---

### 12. `find` + `substr` replace — the infinite-loop bug
What input combination causes a naive `while (str.find(s1) != npos)` replacement loop to spin forever? How do you reject it?

> 

---

### 13. Pointer to member function — syntax
Fill in the type and call:
```cpp
void (Harl::*p)() = &Harl::warning;
Harl h;
________________  // ← call p on h
```

> 

---

### 14. Why no `if/else` in Harl 2.0?
The pedagogical point is to *hold* a pointer to a member function at least once. Describe in one sentence the **dispatch table** approach that the exercise is teaching.

> 

---

### 15. `switch` fall-through (Harl filter)
This switch is intentional. Explain what is printed when `level = 2` (WARNING).
```cpp
switch (level) {
    case 0: harl.complain("DEBUG");
    case 1: harl.complain("INFO");
    case 2: harl.complain("WARNING");
    case 3: harl.complain("ERROR"); break;
    default: std::cout << "Unknown\n";
}
```

> 
