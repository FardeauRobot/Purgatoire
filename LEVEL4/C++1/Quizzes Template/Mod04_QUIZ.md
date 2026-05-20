# C++ Module 04 — Quiz (15 questions)

> Subtype polymorphism, vtables, abstract classes, interfaces.

---

### 1. The one question Mod 04 asks
For `Animal* p = new Dog();` then `p->makeSound();`, which version runs? Answer **for both cases**:
- `makeSound` declared `virtual` in `Animal`: 
- `makeSound` not virtual: 

> 

---

### 2. Static vs dynamic dispatch
Define each in one sentence:
- Static dispatch: 
- Dynamic dispatch: 

> 

---

### 3. The vtable, in three lines
Describe what a vtable is, where the per-instance "vptr" lives, and what `p->makeSound()` compiles down to (the two extra loads).

> 

---

### 4. Code prediction — virtual vs non-virtual
```cpp
struct A           { void f() { std::cout << "A\n"; } };
struct B : A       { void f() { std::cout << "B\n"; } };

struct V           { virtual void g() { std::cout << "V\n"; } };
struct W : V       { void g() { std::cout << "W\n"; } };

A* a = new B(); a->f();
V* v = new W(); v->g();
```
What two lines does this print, and why?

> 

---

### 5. The virtual destructor rule
You write:
```cpp
class Animal { public: ~Animal() {} };       // non-virtual dtor
class Dog : public Animal { Brain* b; public: Dog(): b(new Brain()) {} ~Dog() { delete b; } };

Animal* a = new Dog();
delete a;
```
What goes wrong (precise term) and how do you fix it?

> 

---

### 6. Why deep copy matters
```cpp
Dog basic;          // basic.brain = new Brain()
{
    Dog tmp = basic;   // copy ctor
}                       // tmp dies
basic.brain->ideas[0] = "hello";
```
With a default (shallow) copy ctor, what crashes/breaks? Explain in terms of the heap.

> 

---

### 7. Deep copy of `Dog`
Fill in the missing line:
```cpp
Dog::Dog(Dog const& src) : Animal(src), brain(_______________) {}
```

> 

---

### 8. Copy-assign with owned heap memory
What are the **three** things `Dog::operator=` must do, in order, when assigning from another dog?

> 

---

### 9. Pure virtual syntax
Write the declaration of a pure virtual `makeSound` in `Animal`. What does the `= 0` actually do?

> 

---

### 10. Abstract class — what is forbidden?
With `Animal` abstract, mark each line legal/illegal:
- `Animal a;` 
- `Animal* p = new Dog();` 
- `Animal& r = some_dog;` 
- `Animal* arr = new Animal[10];` 

> 

---

### 11. Concrete class must override
If `Dog` declares `makeSound` but mistypes the signature (`void makeSound(int)` instead of `void makeSound() const`), what happens? Two consequences.

> 

---

### 12. Slicing
```cpp
Dog d;
Animal a = d;       // slicing
a.makeSound();
```
Explain what was *cut off* by the assignment, and what `a.makeSound()` actually calls (assume `makeSound` is virtual).

> 

---

### 13. The Prototype pattern (`clone`)
Why does `MateriaSource::createMateria` rely on `clone()` instead of constructing `Ice` / `Cure` directly by name? What does this let you do later?

> 

---

### 14. Ownership transfer — `unequip`
The subject says `unequip(i)` must **not** delete the materia. Who owns the materia after `unequip`, and why is "defensively deleting" wrong?

> 

---

### 15. Code prediction — virtual dtor chain
```cpp
class Animal  { public: virtual ~Animal()  { std::cout << "~Animal\n"; } };
class Dog : public Animal { Brain* b;
public:
    Dog(): b(new Brain()) {}
    ~Dog() { delete b; std::cout << "~Dog\n"; }
};

Animal* a = new Dog();
delete a;
```
What lines does this print, in what order, and why does the order matter?

> 
