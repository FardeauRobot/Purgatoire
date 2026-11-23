# La forme canonique orthodoxe (OCF) — Guide complet

> 🇬🇧 [English version](../../../../en/cpp/notions/oop/ORTHODOX_CANONICAL_FORM.md)

> **Zooms sur les mots-clés :** [`OPERATOR`](../../../../en/cpp/lexique/OPERATOR.md) (EN) · [`THIS`](../../../../en/cpp/lexique/THIS.md) (EN) · [`EXPLICIT`](../../../../en/cpp/lexique/EXPLICIT.md) (EN) · [`CONST`](../../../../en/cpp/lexique/CONST.md) (EN) · [`NEW`](../../../../en/cpp/lexique/NEW.md) (EN) · [`DELETE`](../../../../en/cpp/lexique/DELETE.md) (EN)

## Table des matières
1. [Qu'est-ce que la forme canonique orthodoxe ?](#Qu%27est-ce%20que%20la%20forme%20canonique%20orthodoxe%20%3F)
2. [Pourquoi existe-t-elle ?](#Pourquoi%20existe-t-elle%20%3F)
3. [Les quatre membres obligatoires](#Les%20quatre%20membres%20obligatoires)
   - [1. Constructeur par défaut](#1.%20Constructeur%20par%20d%C3%A9faut)
   - [2. Constructeur de copie](#2.%20Constructeur%20de%20copie)
   - [3. Opérateur d'affectation par copie](#3.%20Op%C3%A9rateur%20d%27affectation%20par%20copie)
   - [4. Destructeur](#4.%20Destructeur)
4. [La règle de trois (et de cinq, et de zéro)](#La%20r%C3%A8gle%20de%20trois%20%28et%20de%20cinq%2C%20et%20de%20z%C3%A9ro%29)
5. [Copie profonde contre copie superficielle](#Copie%20profonde%20contre%20copie%20superficielle)
6. [Pièges courants](#Pi%C3%A8ges%20courants)
7. [Exemples complets](#Exemples%20complets)

---

## Qu'est-ce que la forme canonique orthodoxe ?

La **forme canonique orthodoxe** (*Orthodox Canonical Form*, OCF), parfois appelée **forme de Coplien** (d'après James Coplien, qui l'a décrite dans *Advanced C++ Programming Styles and Idioms*, 1992), est une convention de code C++ qui exige que chaque classe **définisse explicitement quatre fonctions membres précises** :

1. Un **constructeur par défaut**
2. Un **constructeur de copie**
3. Un **opérateur d'affectation par copie** (`operator=`)
4. Un **destructeur**

Cette convention est **imposée strictement à 42** (et dans beaucoup de bases de code C++98 professionnelles) pour toutes les classes non triviales. Elle garantit que toi, le programmeur, tu **gardes toujours le contrôle** sur la façon dont tes objets sont créés, copiés, affectés et détruits.

> **En bref** : l'OCF est le contrat que tu signes avec le compilateur — « je sais exactement comment mon objet se comporte pendant toute sa vie ».

---

## Pourquoi existe-t-elle ?

Quand tu écris une classe C++ sans définir ces fonctions, le **compilateur les génère implicitement**. Ça a l'air pratique, mais c'est dangereux :

- Le constructeur de copie généré fait une **copie superficielle** (*shallow copy*, membre par membre). Si ta classe gère une ressource (mémoire sur le tas, descripteur de fichier, socket, mutex…), ça mène à des **double free**, des **pointeurs pendants** (*dangling pointers*) et des **comportements indéfinis**.
- Les fonctions implicites sont silencieuses — tu ne peux pas voir d'un coup d'œil ce que fait ta classe quand elle est copiée ou détruite.
- Sans destructeur explicite, les ressources que tu as allouées (par ex. avec `new`) vont **fuir**.

Définir les quatre fonctions explicitement rend ta classe :
- **Prévisible** : le comportement est documenté dans le code.
- **Sûre** : aucune copie superficielle silencieuse des ressources possédées.
- **Maintenable** : les futurs lecteurs voient tout de suite comment le cycle de vie est géré.

---

## Les quatre membres obligatoires

Prenons une classe `Foo` comme exemple fil rouge.

### 1. Constructeur par défaut

**Signature :**
```cpp
Foo();
```

**Ce qu'il fait :**
Le constructeur par défaut est appelé quand tu crées un objet **sans arguments**. Il initialise les membres de l'objet dans un état valide et défini.

**Quand est-il appelé ?**
- `Foo a;` — déclaration explicite sans arguments.
- `Foo arr[10];` — quand un tableau d'objets est créé.
- Quand la classe est membre d'une autre classe qui utilise elle-même son constructeur par défaut.
- Quand tu écris `Foo()` pour créer un temporaire.

**Pourquoi le définir explicitement ?**
- Si tu définis **n'importe quel autre constructeur** (par ex. `Foo(int x)`), le compilateur **cesse de générer** celui par défaut. Donc `Foo a;` ne compilerait plus, sauf si tu définis `Foo()` toi-même.
- Il garantit que tes membres démarrent dans un **état connu** (pas de valeurs poubelles pour les types primitifs comme `int`, qui ne sont pas initialisés à zéro par défaut en C++98).

---

### 2. Constructeur de copie

**Signature :**
```cpp
Foo(const Foo& other);
```

**Ce qu'il fait :**
Construit un **nouvel objet** comme copie d'un objet existant. Il est appelé quand un objet est **créé à partir d'un autre objet du même type**.

**Quand est-il appelé ?**
- `Foo a; Foo b(a);` — construction par copie explicite.
- `Foo b = a;` — initialisation par copie (attention : ce n'est **pas** une affectation, c'est une construction !).
- Quand un objet est **passé par valeur** à une fonction : `void func(Foo f);` appelée avec `func(a);`.
- Quand un objet est **renvoyé par valeur** par une fonction (même si le compilateur peut l'éliminer avec la RVO/NRVO).
- Quand on insère un objet dans un conteneur de la STL (en C++98).

**Pourquoi le définir explicitement ?**
- La version générée copie chaque membre **bit à bit**. Si un membre est un pointeur, l'original et la copie pointeront vers **la même mémoire** — quand l'un des deux est détruit et libère cette mémoire, l'autre devient un **pointeur pendant**.
- Tu peux vouloir **tracer** les copies, ou les **interdire** (en déclarant le constructeur de copie `private` sans le définir).

**Important :**
- Le paramètre doit être une **référence `const`** (`const Foo&`). S'il était passé par valeur, appeler le constructeur de copie exigerait lui-même… un constructeur de copie → récursion infinie.

---

### 3. Opérateur d'affectation par copie

**Signature :**
```cpp
Foo& operator=(const Foo& other);
```

**Ce qu'il fait :**
Affecte le contenu d'un objet **déjà construit** à un autre objet **déjà construit**. Il ne crée **pas** de nouvel objet — il en modifie un existant.

**Quand est-il appelé ?**
- `Foo a; Foo b; b = a;` — affectation explicite entre objets existants.
- **Pas** appelé par `Foo b = a;` (ça, c'est le constructeur de copie !).

**Pourquoi le définir explicitement ?**
- Même raison que pour le constructeur de copie : le compilateur génère une copie superficielle, bit à bit.
- Si ton objet possède des ressources, tu dois :
  1. **Libérer** toute ressource actuellement détenue par `*this`.
  2. **Allouer de nouvelles ressources** et copier les données depuis `other`.
  3. **Renvoyer `*this`** par référence pour permettre l'enchaînement : `a = b = c;`.

**Liste des bonnes pratiques :**
- ✅ Vérifier l'**auto-affectation** (*self-assignment*) : `if (this != &other)` — affecter un objet à lui-même peut mener à libérer la mémoire puis essayer d'y copier.
- ✅ Renvoyer `*this` par référence (`Foo&`).
- ✅ Prendre le paramètre en `const Foo&`.

---

### 4. Destructeur

**Signature :**
```cpp
~Foo();
```

**Ce qu'il fait :**
Nettoie un objet avant que sa mémoire soit récupérée. C'est la **dernière fonction** appelée dans la vie d'un objet.

**Quand est-il appelé ?**
- Quand un objet alloué sur la pile **sort de portée** : `{ Foo a; } // ~Foo() called here`.
- Quand `delete` est appelé sur un objet alloué sur le tas : `Foo* a = new Foo(); delete a;`.
- Quand un objet temporaire est détruit à la fin de l'expression complète.
- Quand un objet qui le contient est détruit (les destructeurs des membres sont appelés automatiquement, dans l'ordre inverse de déclaration).

**Pourquoi le définir explicitement ?**
- Si ta classe alloue des ressources (`new`, `open`, `malloc`, etc.), tu **dois** les libérer dans le destructeur, sinon tu fuis.
- Le destructeur est le **seul endroit** où tu peux garantir que le nettoyage a lieu — même si des exceptions sont lancées.
- Pour les classes de base polymorphes, le destructeur doit être **`virtual`** pour que les destructeurs dérivés s'exécutent quand on supprime via un pointeur de base.

---

## La règle de trois (et de cinq, et de zéro)

La **règle de trois** (*Rule of Three*, C++98) dit :

> Si une classe a besoin d'un **destructeur**, d'un **constructeur de copie** ou d'un **opérateur d'affectation par copie** explicite, elle a presque certainement besoin des trois.

Pourquoi ? Parce que les trois s'occupent de la **gestion des ressources**. Si ton destructeur libère de la mémoire, ton constructeur de copie doit dupliquer cette mémoire (sinon les deux copies libèrent le même bloc), et ton opérateur d'affectation doit faire pareil (en nettoyant aussi d'abord la ressource existante).

La **forme canonique orthodoxe** en est une version **plus stricte** : elle ajoute le **constructeur par défaut** à la liste, ce qui en fait la **règle de quatre**.

> *(Le C++ moderne ajoute la règle de cinq — avec le constructeur de déplacement et l'affectation par déplacement — et la règle de zéro — concevoir des classes qui n'ont besoin d'aucun d'eux en s'appuyant sur des types RAII comme `std::vector`, `std::unique_ptr`. Mais c'est postérieur au C++98 et hors du programme de 42 pour CPP00–CPP04.)*

---

## Copie profonde contre copie superficielle

C'est la **raison centrale** de l'existence de l'OCF.

### Copie superficielle (par défaut, dangereuse quand on possède des ressources)
```cpp
class Bad {
    int* data;
public:
    Bad() { data = new int(42); }
    ~Bad() { delete data; }
    // No copy constructor → compiler generates one that copies the POINTER, not the data
};

Bad a;
Bad b(a);   // b.data == a.data (same memory!)
// When a and b are destroyed → delete called twice on the same pointer → undefined behavior 💥
```

### Copie profonde (correcte)
```cpp
class Good {
    int* data;
public:
    Good() : data(new int(42)) {}
    Good(const Good& other) : data(new int(*other.data)) {} // allocate new memory, copy value
    Good& operator=(const Good& other) {
        if (this != &other) {
            delete data;
            data = new int(*other.data);
        }
        return *this;
    }
    ~Good() { delete data; }
};
```

---

## Pièges courants

| Piège | Conséquence |
|---|---|
| Oublier `const` sur le paramètre du constructeur de copie | Impossible de copier des objets `const` ou des temporaires |
| Oublier de renvoyer `*this` dans `operator=` | Impossible d'enchaîner les affectations (`a = b = c;`) |
| Oublier la vérification d'auto-affectation | Ressource libérée avant d'être copiée — UB |
| Oublier `virtual` sur le destructeur d'une classe de base | Le destructeur dérivé ne s'exécute pas quand on supprime via un pointeur de base — fuites |
| Définir un constructeur non par défaut sans redéfinir celui par défaut | `Foo a;` ne compile plus |
| Implémenter la copie comme une copie superficielle alors que la classe possède de la mémoire | Double free, pointeurs pendants |
| Renvoyer `operator=` par valeur au lieu de par référence | Copie inutile, casse la sémantique d'enchaînement |

---

## Exemples complets

### Exemple 1 : une classe triviale (aucune ressource possédée)

```cpp
// Animal.hpp
#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <string>

class Animal {
private:
    std::string _name;
    int         _age;

public:
    Animal();                                    // default constructor
    Animal(const std::string& name, int age);    // parameterized constructor
    Animal(const Animal& other);                 // copy constructor
    Animal& operator=(const Animal& other);      // copy assignment operator
    ~Animal();                                   // destructor

    const std::string& getName() const;
    int                getAge() const;
};

#endif
```

```cpp
// Animal.cpp
#include "Animal.hpp"
#include <iostream>

Animal::Animal() : _name("Unnamed"), _age(0) {
    std::cout << "Default constructor called" << std::endl;
}

Animal::Animal(const std::string& name, int age) : _name(name), _age(age) {
    std::cout << "Parameterized constructor called for " << _name << std::endl;
}

Animal::Animal(const Animal& other) : _name(other._name), _age(other._age) {
    std::cout << "Copy constructor called for " << _name << std::endl;
}

Animal& Animal::operator=(const Animal& other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other) {
        _name = other._name;
        _age  = other._age;
    }
    return *this;
}

Animal::~Animal() {
    std::cout << "Destructor called for " << _name << std::endl;
}

const std::string& Animal::getName() const { return _name; }
int                Animal::getAge()  const { return _age; }
```

```cpp
// main.cpp
#include "Animal.hpp"

int main() {
    Animal a;                       // default constructor
    Animal b("Rex", 5);             // parameterized constructor
    Animal c(b);                    // copy constructor
    Animal d = b;                   // ALSO copy constructor (initialization)
    a = b;                          // copy assignment operator

    return 0;
    // destructors called for d, c, b, a (reverse order of construction)
}
```

**Sortie attendue :**
```
Default constructor called
Parameterized constructor called for Rex
Copy constructor called for Rex
Copy constructor called for Rex
Copy assignment operator called
Destructor called for Rex
Destructor called for Rex
Destructor called for Rex
Destructor called for Rex
```

---

### Exemple 2 : une classe qui gère une ressource sur le tas (copie profonde obligatoire)

```cpp
// Buffer.hpp
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstddef>

class Buffer {
private:
    char*  _data;
    size_t _size;

public:
    Buffer();                                    // default constructor
    Buffer(size_t size);                         // parameterized constructor
    Buffer(const Buffer& other);                 // copy constructor (deep copy)
    Buffer& operator=(const Buffer& other);      // copy assignment operator (deep copy)
    ~Buffer();                                   // destructor (frees memory)

    size_t getSize() const;
    char*  getData() const;
};

#endif
```

```cpp
// Buffer.cpp
#include "Buffer.hpp"
#include <cstring>
#include <iostream>

Buffer::Buffer() : _data(NULL), _size(0) {
    std::cout << "Default Buffer constructed (empty)" << std::endl;
}

Buffer::Buffer(size_t size) : _data(new char[size]), _size(size) {
    std::memset(_data, 0, size);
    std::cout << "Buffer of size " << size << " constructed" << std::endl;
}

Buffer::Buffer(const Buffer& other) : _data(NULL), _size(other._size) {
    if (_size > 0) {
        _data = new char[_size];
        std::memcpy(_data, other._data, _size);
    }
    std::cout << "Buffer copy-constructed (deep copy of " << _size << " bytes)" << std::endl;
}

Buffer& Buffer::operator=(const Buffer& other) {
    std::cout << "Buffer assignment operator called" << std::endl;
    if (this != &other) {            // self-assignment guard
        delete[] _data;              // release old resource
        _size = other._size;
        if (_size > 0) {
            _data = new char[_size]; // allocate new
            std::memcpy(_data, other._data, _size);
        } else {
            _data = NULL;
        }
    }
    return *this;
}

Buffer::~Buffer() {
    std::cout << "Buffer destructed (freeing " << _size << " bytes)" << std::endl;
    delete[] _data;
}

size_t Buffer::getSize() const { return _size; }
char*  Buffer::getData() const { return _data; }
```

```cpp
// main.cpp
#include "Buffer.hpp"

int main() {
    Buffer a(128);     // allocates 128 bytes
    Buffer b(a);       // deep copy → b has its OWN 128 bytes
    Buffer c;          // empty
    c = a;             // deep copy via assignment

    return 0;
    // each destructor cleans up its OWN memory — no double-free
}
```

---

### Exemple 3 : interdire la copie (type singleton / non copiable)

Parfois tu **ne veux pas** qu'une classe soit copiable (par ex. une classe qui gère un descripteur unique). L'idiome C++98 consiste à déclarer le constructeur de copie et l'opérateur d'affectation par copie **`private`** et à **ne pas les implémenter** :

```cpp
// Logger.hpp
class Logger {
public:
    Logger();
    ~Logger();
    void log(const std::string& msg);

private:
    Logger(const Logger& other);              // declared but not defined
    Logger& operator=(const Logger& other);   // declared but not defined
};
```

Désormais, toute tentative de copier un `Logger` échouera **à la compilation** (ou à l'édition de liens si l'appel vient d'un friend ou d'un membre). C'est quand même considéré comme respectant l'OCF — tu as **traité explicitement** les quatre cas.

---

### Exemple 4 : une classe avec un destructeur `virtual` (polymorphisme)

```cpp
class Base {
public:
    Base();
    Base(const Base& other);
    Base& operator=(const Base& other);
    virtual ~Base();              // VIRTUAL — crucial for safe polymorphic deletion
    virtual void speak() const;
};

class Derived : public Base {
private:
    int* _resource;
public:
    Derived();
    Derived(const Derived& other);
    Derived& operator=(const Derived& other);
    ~Derived();                   // automatically virtual because parent's is
    virtual void speak() const;
};
```

Sans `virtual ~Base()`, ce code :
```cpp
Base* b = new Derived();
delete b;   // calls only ~Base() — ~Derived() never runs → leaks _resource!
```
…fuirait en silence. Avec `virtual`, les deux destructeurs s'exécutent dans le bon ordre (le dérivé d'abord, puis la base).

---

## Aide-mémoire

```cpp
class ClassName {
public:
    ClassName();                                       // 1. Default constructor
    ClassName(const ClassName& other);                 // 2. Copy constructor
    ClassName& operator=(const ClassName& other);      // 3. Copy assignment operator
    ~ClassName();                                      // 4. Destructor
};
```

| Fonction | Quand elle s'exécute | Renvoie | Paramètre |
|---|---|---|---|
| Constructeur par défaut | `ClassName a;` | rien | aucun |
| Constructeur de copie | `ClassName b(a);` ou `ClassName b = a;` | rien | `const ClassName&` |
| Affectation par copie | `b = a;` (les deux existent déjà) | `ClassName&` | `const ClassName&` |
| Destructeur | sortie de portée / `delete` | rien | aucun |

---

**TL;DR** : la forme canonique orthodoxe, c'est la discipline C++98 qui consiste à toujours définir explicitement les quatre fonctions du cycle de vie, pour que la création, la copie, l'affectation et la destruction d'un objet ne soient jamais laissées aux valeurs par défaut silencieuses du compilateur. Elle évite les fuites de ressources, les double free et les pointeurs pendants dans toute classe qui possède des ressources — et rend l'intention limpide dans celles qui n'en possèdent pas.

---

## Vue matérielle — ce que donnent les quatre membres une fois compilés

### Constructeur par défaut (sans trace, membres primitifs)

```cpp
class Fixed { int raw; };
Fixed::Fixed() : raw(0) {}
```

Se compile en (x86-64) :

```asm
Fixed::Fixed():
    mov  dword [rdi], 0       ; this->raw = 0
    ret
```

Trois instructions. Une écriture et un retour. Avec `-O2` plus
l'inlining au point d'appel, le constructeur peut disparaître
complètement si l'emplacement de pile de l'appelant est déjà
initialisé à zéro.

### Constructeur de copie d'une classe à membres primitifs uniquement

```cpp
Fixed::Fixed(Fixed const& src) : raw(src.raw) {}
```

```asm
Fixed::Fixed(Fixed const&):
    mov  eax, [rsi]           ; load other.raw
    mov  [rdi], eax           ; this->raw = other.raw
    ret
```

Une lecture de 4 octets, une écriture de 4 octets. Pour une classe
avec un membre `std::string`, ajoute une allocation pour le tampon sur
le tas — des dizaines de nanosecondes contre ~1 cycle pour le cas
primitifs seulement.

### Affectation par copie avec garde d'auto-affectation

```cpp
Fixed& operator=(Fixed const& o) {
    if (this != &o) raw = o.raw;
    return *this;
}
```

```asm
Fixed::operator=(Fixed const&):
    cmp  rdi, rsi             ; this vs &other
    je   .skip
    mov  eax, [rsi]
    mov  [rdi], eax
.skip:
    mov  rax, rdi             ; return *this
    ret
```

La garde coûte deux instructions (`cmp` + `je`). Pour des primitifs,
c'est un branchement. Pour les classes qui possèdent des ressources,
la garde empêche `delete old; this = old;` — ce qui serait
catastrophique.

### Destructeur d'une classe qui possède une ressource

```cpp
class Dog { Brain* brain; };
Dog::~Dog() { delete brain; }
```

`delete p` inclut une vérification implicite de nullité — `delete NULL;`
est une opération sans effet bien définie. C'est pour ça que tu peux
faire `delete` sur un membre sans vérifier d'abord s'il est nul.

### Élision de copie — quand le constructeur de copie est sauté en silence

```cpp
Fixed make() { return Fixed(42); }
Fixed f = make();
```

Avec n'importe quel compilateur moderne, **un seul** `Fixed` est
construit. Le compilateur applique l'**optimisation de la valeur de
retour** (*return value optimisation*, RVO) : il construit
`Fixed(42)` directement dans l'emplacement de `f`, en sautant le
constructeur de copie. La trace de ton constructeur de copie **ne
s'affichera pas**, même si le langage a « conceptuellement » fait une
copie.

Règle pratique : ne compte jamais sur un nombre précis de traces du
constructeur de copie dans du code qui passe ou renvoie par valeur. La
moulinette teste en général des **copies explicites** (`Foo b(a);`,
pas `Foo b = make();`) pour éviter l'ambiguïté de la RVO — mais si ta
sortie contient un « copy ctor called » de moins que prévu, la RVO en
est probablement la cause.

### Pointeur de vtable et OCF

Toute classe avec des méthodes `virtual` a un vptr comme (en général)
premier membre. Le constructeur de copie et l'affectation par copie
générés **ne copient pas** le vptr de la source — chaque constructeur
fait pointer le vptr vers la vtable de sa propre classe. C'est pour ça
que le découpage d'objet (*object slicing*) fonctionne : copier un
`Dog` dans un emplacement `Animal` laisse le vptr de la destination
pointer vers la vtable d'`Animal`, pas celle de `Dog`, donc les appels
virtuels suivants sont aiguillés comme pour un `Animal`.
