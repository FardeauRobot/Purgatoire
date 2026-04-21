# Getters and Setters in C++

## What are they?

In C, you access struct members directly:

```c
t_contact contact;
contact.name = "Alice";
printf("%s", contact.name);
```

In C++, class members are usually `private` — you can't touch them from outside.
Getters and setters are **public methods** that act as controlled doors to those private members.

```
Outside world  -->  setter()  -->  private member
Outside world  <--  getter()  <--  private member
```

---

## Basic example

```cpp
class Contact {
private:
    std::string m_name;  // private — nobody can touch this directly

public:
    // Setter — write access
    void setName(const std::string& name) {
        m_name = name;
    }

    // Getter — read access
    std::string getName() const {
        return m_name;
    }
};
```

```cpp
Contact c;
c.setName("Alice");               // goes through the setter
std::cout << c.getName();         // goes through the getter
// c.m_name = "Bob";              // ERROR — private, won't compile
```

---

## Why not just make members public?

Setters let you **validate or control** what goes in:

```cpp
void setAge(int age) {
    if (age < 0 || age > 150)
        return;  // reject bad values
    m_age = age;
}
```

Getters let you **control what comes out**, or compute on the fly:

```cpp
std::string getFullName() const {
    return m_firstName + " " + m_lastName;  // no stored field, computed
}
```

If the member were public, anyone could set `age = -42` and you'd never know.

---

## The `const` on getters

```cpp
std::string getName() const { return m_name; }
//                   ^^^^^
```

The `const` here means "this method does not modify the object."
Always put `const` on getters — it's a contract that reading doesn't change state.
It also allows the method to be called on `const` objects.

---

## Naming conventions

| Style       | Getter          | Setter          |
|-------------|-----------------|-----------------|
| Classic     | `getName()`     | `setName()`     |
| Qt style    | `name()`        | `setName()`     |
| 42 style    | `F_GetName()`   | `F_SetName()`   |

The `get`/`set` prefix is the most common and immediately readable.

---

## In your PhoneBook project

Your `Contact` class likely stores fields like first name, last name, phone number.
Instead of making them public:

```cpp
// Contact.hpp
class Contact {
private:
    std::string m_firstName;
    std::string m_lastName;
    std::string m_phone;

public:
    void        setFirstName(const std::string& name);
    std::string getFirstName() const;

    void        setLastName(const std::string& name);
    std::string getLastName() const;

    void        setPhone(const std::string& phone);
    std::string getPhone() const;
};
```

```cpp
// Contact.cpp
void Contact::setFirstName(const std::string& name) { m_firstName = name; }
std::string Contact::getFirstName() const { return m_firstName; }
```

Then in `PhoneBook.cpp`:
```cpp
contact.setFirstName("Alice");
std::cout << contact.getFirstName();
```

---

## Summary

| Concept  | Role                             | Access    |
|----------|----------------------------------|-----------|
| Getter   | Read a private member            | `public`  |
| Setter   | Write a private member           | `public`  |
| Member   | Stores the actual data           | `private` |

The point is **encapsulation** — the class controls its own data.
The outside world asks nicely through getters/setters instead of reaching in directly.
