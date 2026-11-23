# L'héritage en C++ — Guide complet

> 🇬🇧 [English version](../../../../en/cpp/notions/oop/INHERITANCE.md)

> **Zooms sur les mots-clés :** [`PUBLIC/PRIVATE/PROTECTED`](../../../../en/cpp/lexique/PUBLIC_PRIVATE_PROTECTED.md) (EN) · [`VIRTUAL`](../../../../en/cpp/lexique/VIRTUAL.md) (EN) · [`USING`](../../../../en/cpp/lexique/USING.md) (EN)

L'héritage (*inheritance*) te permet d'exprimer une relation **« est-un »** (*is-a*) entre des types. Un `Dog` est-un `Animal`. Un `ScavTrap` est-un `ClapTrap`. La classe dérivée reçoit automatiquement les membres de la base — plus la possibilité d'ajouter les siens ou de redéfinir ceux hérités.

```cpp
class Animal {
public:
    void breathe() const { std::cout << "..." << '\n'; }
};

class Dog : public Animal {
public:
    void bark() const { std::cout << "Woof!" << '\n'; }
};

Dog d;
d.breathe();   // inherited from Animal
d.bark();      // Dog's own
```

---

## Table des matières

1. [Pourquoi l'héritage existe](#1.%20Pourquoi%20l%27h%C3%A9ritage%20existe)
2. [Syntaxe et les trois modes d'accès](#2.%20Syntaxe%20et%20les%20trois%20modes%20d%27acc%C3%A8s)
3. [`protected` — le juste milieu](#3.%20protected%20%E2%80%94%20le%20juste%20milieu)
4. [Ordre de construction et de destruction](#4.%20Ordre%20de%20construction%20et%20de%20destruction)
5. [Appeler le constructeur du parent](#5.%20Appeler%20le%20constructeur%20du%20parent)
6. [Disposition en mémoire](#6.%20Disposition%20en%20m%C3%A9moire)
7. [Upcasting et downcasting](#7.%20Upcasting%20et%20downcasting)
8. [Masquage de noms](#8.%20Masquage%20de%20noms)
9. [Héritage multiple](#9.%20H%C3%A9ritage%20multiple)
10. [Le problème du diamant et l'héritage virtuel](#10.%20Le%20probl%C3%A8me%20du%20diamant%20et%20l%27h%C3%A9ritage%20virtuel)
11. [Composition contre héritage](#11.%20Composition%20contre%20h%C3%A9ritage)
12. [Dans CPP03 — la famille ClapTrap](#12.%20Dans%20CPP03%20%E2%80%94%20la%20famille%20ClapTrap)
13. [Dans CPP04 — les hiérarchies polymorphes](#13.%20Dans%20CPP04%20%E2%80%94%20les%20hi%C3%A9rarchies%20polymorphes)
14. [Règles pratiques](#14.%20R%C3%A8gles%20pratiques)
15. [Pièges](#15.%20Pi%C3%A8ges)

---

## 1. Pourquoi l'héritage existe

Deux raisons :

1. **Réutiliser du code.** Le `Dog` ne réimplémente pas `breathe()` — il en hérite.
2. **Le polymorphisme.** Un `Dog*` peut être traité comme un `Animal*` partout où le code attend un `Animal`. C'est ce qui te permet d'écrire du code qui gère « n'importe quel animal » sans rien savoir des chiens et des chats en particulier. Voir [POLYMORPHISM.md](POLYMORPHISM.md).

La première est utile mais *ne suffit pas* à elle seule (la composition est souvent plus propre — voir §11). La seconde, c'est la vraie puissance.

---

## 2. Syntaxe et les trois modes d'accès

```cpp
class Base { /* ... */ };

class Derived : public    Base { /* ... */ };   // is-a  — 99% of the time
class Derived : protected Base { /* ... */ };   // rare
class Derived : private   Base { /* ... */ };   // "implemented-in-terms-of"
```

Le spécificateur d'accès sur la ligne d'héritage contrôle **ce que le monde extérieur peut voir** à travers la classe dérivée.

| Type d'héritage | `public` dans Base devient… | `protected` dans Base devient… | `private` dans Base devient… |
|---|---|---|---|
| `public`    | `public`    | `protected` | inaccessible |
| `protected` | `protected` | `protected` | inaccessible |
| `private`   | `private`   | `private`   | inaccessible |

**Les membres `private` de la base ne sont jamais accessibles à la dérivée**, quel que soit le type d'héritage. Seuls les membres `protected` et `public` passent.

**À 42**, tu utilises presque exclusivement l'héritage `public`. Les deux autres existent mais servent rarement.

---

## 3. `protected` — le juste milieu

- **`private`** — seulement la classe elle-même.
- **`public`** — tout le monde.
- **`protected`** — la classe elle-même **et toute classe dérivée**.

```cpp
class Animal {
protected:
    std::string _type;     // derived classes can touch this
};

class Dog : public Animal {
public:
    Dog() { _type = "Dog"; }   // OK — _type is protected
};

Dog d;
d._type = "Cat";               // ERROR — outside the class, _type is hidden
```

`protected` est utile mais poreux : n'importe quelle sous-classe peut venir modifier un état que la classe de base essayait de protéger. Préfère `private` + des accesseurs `protected`, sauf si le membre est *conçu* pour être partagé avec les sous-classes.

---

## 4. Ordre de construction et de destruction

**Le parent d'abord en montant, l'enfant d'abord en descendant.** Toujours.

```
Dog d;  // construction

  1. Animal() s'exécute  — le sous-objet de base est construit
  2. Dog()    s'exécute  — puis le code propre à Dog

~Dog();  // destruction (explicite ou automatique)

  1. ~Dog()    s'exécute — le code propre à Dog d'abord
  2. ~Animal() s'exécute — puis le destructeur de base
```

### Pourquoi cet ordre ?

Parce que le constructeur de `Dog` peut avoir besoin que la partie `Animal` soit prête. (Tu ne peux pas utiliser des champs pas encore initialisés.) Et le destructeur d'`Animal` peut compter sur le fait que l'état propre à `Dog` a déjà été nettoyé.

### Chaîne sur plusieurs niveaux

```cpp
class A { public: A() { std::cout << "A\n"; } ~A() { std::cout << "~A\n"; } };
class B : public A { public: B() { std::cout << "B\n"; } ~B() { std::cout << "~B\n"; } };
class C : public B { public: C() { std::cout << "C\n"; } ~C() { std::cout << "~C\n"; } };

C obj;
// Prints:  A  B  C                 — construction (top-down)
// Then:    ~C  ~B  ~A              — destruction (bottom-up)
```

---

## 5. Appeler le constructeur du parent

Le constructeur par défaut de la base s'exécute automatiquement. Pour passer des arguments à la base, utilise la liste d'initialisation :

```cpp
class Animal {
public:
    Animal(const std::string& t) : _type(t) {}
protected:
    std::string _type;
};

class Dog : public Animal {
public:
    Dog() : Animal("Dog") {}           // explicit base construction
    Dog(const std::string& name)
        : Animal("Dog"), _name(name) {}
private:
    std::string _name;
};
```

**Si la base n'a pas de constructeur par défaut, la liste d'initialisation de la dérivée DOIT la construire explicitement.** L'oublier donne une erreur de compilation du genre « no matching function for call to `Animal::Animal()` ».

### Constructeur de copie et `operator=`

Ils ne sont **pas** hérités tels quels — le compilateur les génère pour la classe dérivée, mais si tu écris les tiens, tu dois relayer explicitement vers la base :

```cpp
Dog::Dog(const Dog& other) : Animal(other) { /* copy Dog-specific fields */ }

Dog& Dog::operator=(const Dog& other) {
    if (this != &other) {
        Animal::operator=(other);        // call base's operator=
        _name = other._name;
    }
    return *this;
}
```

Oublier de relayer vers la base veut dire que le sous-objet de base n'est pas copié — un bug classique.

---

## 6. Disposition en mémoire

```
Dog d;

Mémoire :
┌──────────────────────────────┐  ◄── commence ici
│  Sous-objet Animal :         │
│    _type (std::string)       │
│    [vptr si virtual]         │
├──────────────────────────────┤
│  Membres propres à Dog :     │
│    _name (std::string)       │
└──────────────────────────────┘
```

Propriété clé : un `Dog*` et un `Animal*` pointent tous deux vers **la même adresse** (le sous-objet de base est à l'offset 0). C'est pour ça que l'upcasting est gratuit — aucun ajustement de pointeur nécessaire.

(L'héritage multiple casse un peu ça : la deuxième base vit à un offset non nul, et le cast doit décaler le pointeur.)

---

## 7. Upcasting et downcasting

### Upcasting — implicite et sûr

De la dérivée vers la base. Ça marche toujours, pas besoin de cast :

```cpp
Dog     d;
Animal* a = &d;       // implicit upcast
a->breathe();         // OK
```

C'est la pierre angulaire du polymorphisme : une fonction qui prend un `Animal*` fonctionne pour n'importe quelle sous-classe.

### Downcasting — il faut un cast, et ça peut échouer

De la base vers la dérivée. Le compilateur ne peut pas vérifier le type réel, donc tu dois être explicite :

```cpp
Animal* a = ...;
Dog*    d = static_cast<Dog*>(a);        // you promise it's really a Dog — no check
Dog*    d = dynamic_cast<Dog*>(a);       // checked at runtime — NULL if not a Dog
```

Utilise `dynamic_cast` dès que tu ne peux pas prouver à la compilation que `a` est un `Dog`. Voir [CASTS.md](../../../../en/cpp/lexique/CASTS.md) (EN).

### Le découpage d'objet — le bug silencieux

Un upcast par **valeur** (et non par pointeur ou référence) **découpe** (*slicing*) la partie dérivée :

```cpp
Dog    d;
Animal a = d;         // copies ONLY the Animal sub-object; Dog-specific fields are lost
```

`a` est maintenant un `Animal` nu — plus rien du `Dog`. C'est légal et silencieux. Passe toujours les hiérarchies polymorphes **par pointeur ou par référence**, jamais par valeur.

---

## 8. Masquage de noms

Si une classe dérivée déclare un nom qui existe aussi dans la base, **tout** le nom de la base est masqué (*name hiding*) — même les surcharges qui prennent d'autres arguments :

```cpp
class Base {
public:
    void f(int);
    void f(double);
};

class Derived : public Base {
public:
    void f(int);        // hides BOTH Base::f(int) AND Base::f(double)
};

Derived d;
d.f(3.14);              // ERROR — Base::f(double) is hidden
d.Base::f(3.14);        // OK — explicit
```

Correction : ramène le nom dans la portée avec `using` :
```cpp
class Derived : public Base {
public:
    using Base::f;      // bring all Base::f overloads into scope
    void f(int);        // add your own
};
```

---

## 9. Héritage multiple

Une classe peut hériter de plusieurs bases :

```cpp
class Flier    { public: virtual void fly() = 0;  };
class Swimmer  { public: virtual void swim() = 0; };

class Duck : public Flier, public Swimmer {
public:
    void fly()  { /* ... */ }
    void swim() { /* ... */ }
};
```

Disposition en mémoire :

```
Duck d;
┌──────────────┐  ◄── &d vu comme Duck* ou Flier*
│ partie Flier │
├──────────────┤  ◄── &d vu comme Swimmer*  (offset non nul)
│ partie Swimm.│
├──────────────┤
│ partie Duck  │
└──────────────┘
```

L'upcast vers `Swimmer*` **ajuste le pointeur** — il pointe maintenant au milieu de l'objet. `static_cast` et `dynamic_cast` s'en occupent pour toi.

---

## 10. Le problème du diamant et l'héritage virtuel

```
     A
    / \
   B   C
    \ /
     D
```

Si `B` et `C` héritent tous deux de `A`, et que `D` hérite des deux, `D` contient **deux copies de `A`**. Accéder à n'importe quel membre de `A` depuis `D` est ambigu.

```cpp
class A { public: int n; };
class B : public A {};
class C : public A {};
class D : public B, public C {};

D d;
d.n = 5;          // ERROR — ambiguous: B::A::n or C::A::n?
d.B::n = 5;       // works, but ugly
```

### Correction : l'héritage virtuel

Marque l'héritage de `B` et de `C` depuis `A` comme `virtual` :

```cpp
class A { public: int n; };
class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {};

D d;
d.n = 5;          // OK — single shared A sub-object
```

Désormais `D` contient **un seul** `A` partagé. À 42, CPP04 ex03 (« Interesting idea ») donne un avant-goût contrôlé de l'héritage virtuel. En dehors de ça, tu en écriras rarement.

---

## 11. Composition contre héritage

L'héritage n'est pas le mécanisme de réutilisation par défaut — c'est le mécanisme du polymorphisme. Pour « la classe X utilise la classe Y », utilise en général la **composition** :

```cpp
// Inheritance (is-a) — for polymorphism
class Dog : public Animal { /* ... */ };

// Composition (has-a) — for reuse
class Car {
    Engine _engine;             // Car HAS an Engine
    Wheel  _wheels[4];
public:
    void start() { _engine.ignite(); }
};
```

### Quand choisir quoi

| Question | Héritage | Composition |
|---|---|---|
| X est-il un Y ? | ✅ | ❌ |
| X a-t-il un Y ? | ❌ | ✅ |
| Les appelants manipuleront-ils des `Y*` en voulant un appel virtuel ? | ✅ | ❌ |
| Veux-tu juste réutiliser les méthodes de Y ? | ❌ | ✅ |
| Y peut-il changer pendant la vie de l'objet ? | ❌ | ✅ (on remplace le membre) |

La composition est plus souple, plus facile à tester, et ne fait pas fuiter les détails internes de la base dans la classe dérivée. N'utilise l'héritage que quand tu as besoin de la sémantique « est-un » et du polymorphisme.

---

## 12. Dans CPP03 — la famille ClapTrap

```
ClapTrap
   │
   ├── ScavTrap  (héritage virtuel de ClapTrap dans l'ex03)
   │
   └── FragTrap  (héritage virtuel de ClapTrap dans l'ex03)
         │
       DiamondTrap : ScavTrap, FragTrap   ← le diamant
```

- **ex00** — classe simple, sans héritage. On construit `ClapTrap`.
- **ex01** — héritage simple. `ScavTrap : public ClapTrap`. Leçon clé : la chaîne de construction et de destruction, et l'appel du constructeur de base avec le bon argument.
- **ex02** — `FragTrap : public ClapTrap`. Pareil, un autre frère.
- **ex03** — `DiamondTrap : public ScavTrap, public FragTrap`. C'est là que tu tombes sur le problème du diamant et qu'il te faut l'**héritage virtuel** sur les lignes d'héritage de `ScavTrap` et de `FragTrap`. La sortie attendue trace précisément l'ordre de construction.

Attention à la règle subtile : en héritage virtuel, c'est la **classe la plus dérivée** (`DiamondTrap`) qui est responsable d'initialiser la base virtuelle (`ClapTrap`). Les constructeurs intermédiaires (`ScavTrap`, `FragTrap`) ne peuvent pas le faire.

---

## 13. Dans CPP04 — les hiérarchies polymorphes

- **ex00** — base `Animal` avec les sous-classes `Dog` et `Cat`. Points clés : destructeur virtuel, `makeSound` virtuelle.
- **ex01** — ajout de `Brain` comme membre **composé** (pas hérité !). Discipline de copie profonde (`Dog` possède un `Brain*` ; le constructeur de copie et `operator=` doivent le cloner).
- **ex02** — rendre `Animal` **abstraite** (`makeSound` virtuelle pure). Impossible d'instancier `Animal` directement — seulement `Dog` et `Cat`.
- **ex03** — l'exercice d'interfaces `AMateria` / `Character` / `MateriaSource`. Interfaces = classes abstraites qui n'ont que des méthodes virtuelles pures.

C'est là que l'héritage paie : tu écris du code qui manipule des `Animal*` et les bonnes méthodes s'exécutent automatiquement pour `Dog` ou `Cat`. C'est ça, le polymorphisme. Voir [POLYMORPHISM.md](POLYMORPHISM.md).

---

## 14. Règles pratiques

- **Par défaut, héritage `public`.** Les héritages `private` et `protected` sont des cas limites.
- **Toute classe de base polymorphe a un destructeur `virtual`.** Sans lui, `delete base_ptr` saute le destructeur dérivé → fuites.
- **Passe les objets polymorphes par pointeur ou par référence**, jamais par valeur. (Évite le découpage.)
- **Préfère la composition**, sauf si tu as précisément besoin du polymorphisme.
- **Relaie vers la base dans le constructeur de copie et `operator=`** quand tu les écris.
- **Héritage multiple pour les interfaces seulement.** Hériter de plusieurs classes concrètes, c'est là que vivent les bugs.
- **N'utilise pas de données membres `protected`**, sauf si la classe est spécifiquement conçue pour être étendue. Utilise plutôt des méthodes `protected`.

---

## 15. Pièges

- **Pas de destructeur virtuel sur une base polymorphe** → `delete ptr` n'appelle que le destructeur de base → les ressources de la dérivée fuient. 42 le vérifie.
- **Découpage d'objet** en passage par valeur. `void f(Animal a)` + `f(dog)` perd en silence la partie Dog.
- **Oublier de relayer** dans le constructeur de copie / `operator=` laisse le sous-objet de base intact.
- **Masquage de noms** — un seul `f(int)` dans la dérivée masque toutes les surcharges de `f` de la base.
- **Héritage en diamant sans `virtual`** : tu obtiens deux copies du grand-parent et des accès aux membres ambigus.
- **Appeler une fonction virtuelle depuis un constructeur ou un destructeur** n'aiguille PAS vers la dérivée — pendant ces moments, l'objet n'est pas encore (ou plus) complètement un `Derived`. Tu obtiens la version de la base.
- **L'héritage privé n'est pas un « est-un »** — c'est un « implémenté-à-l'aide-de » (*implemented-in-terms-of*). Le code extérieur ne peut pas upcaster `Derived*` en `Base*`.
- **Les constructeurs par défaut hérités sont générés implicitement**, mais les constructeurs avec arguments ne sont pas hérités. Tu dois écrire chaque surcharge explicitement dans la classe dérivée.

---

## 16. Vue matérielle — ce que fait réellement le CPU

### Héritage simple — sous-objet de base à l'offset 0

```
class Animal { std::string type; };            // 32 octets
class Dog : public Animal { Brain* brain; };   // 32 + 8 = 40 octets

Disposition de Dog :
┌──────────────────────────┐ offset 0
│  Animal::type            │   (std::string, 32 octets)
├──────────────────────────┤ offset 32
│  Dog::brain              │   (Brain*, 8 octets)
└──────────────────────────┘ total 40 octets
```

`Animal* a = &dog;` se compile en zéro instruction machine — le
sous-objet de base commence à l'offset 0, donc `Animal*` et `Dog*`
sont la même adresse. L'upcast est gratuit.

### Avec une méthode virtuelle — le vptr prend la tête

```
class Animal { virtual void makeSound(); std::string type; };
class Dog : public Animal { Brain* brain; };

Disposition de Dog :
┌──────────────────────────┐ offset 0
│  vptr → vtable de Dog    │   (8 octets)
├──────────────────────────┤ offset 8
│  Animal::type            │   (32 octets)
├──────────────────────────┤ offset 40
│  Dog::brain              │   (8 octets)
└──────────────────────────┘ total 48 octets
```

Le vptr est maintenant le premier membre. `Dog*` et `Animal*`
pointent toujours vers la même adresse.

### Héritage multiple — deuxième base à un offset non nul

```
class A { int a_data; };       // 4 octets
class B { int b_data; };       // 4 octets
class C : public A, public B {};

Disposition de C :
┌──────────────────────────┐ offset 0
│  A::a_data               │   (4 octets)
├──────────────────────────┤ offset 4
│  B::b_data               │   (4 octets)
└──────────────────────────┘
```

`A* p_a = &c;` → même adresse que `&c`, aucun ajustement.
`B* p_b = &c;` → l'adresse est `&c + 4`, une instruction `lea`.

### Héritage virtuel — le coût

Le diamant `class D : virtual public A` ajoute un **pointeur de
table de base virtuelle** (vbptr) à chaque classe qui hérite
virtuellement :

```
class A { int a_data; };
class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {};

Disposition de D (conceptuellement) :
┌──────────────────────────┐
│  vbptr de B              │   (offset vers le sous-objet A)
├──────────────────────────┤
│  membres de B            │
├──────────────────────────┤
│  vbptr de C              │
├──────────────────────────┤
│  membres de C            │
├──────────────────────────┤
│  membres de D            │
├──────────────────────────┤
│  A::a_data (partagé)     │   ← une seule copie
└──────────────────────────┘
```

Accéder à `A::a_data` depuis `B` ou `C` exige de lire d'abord le
vbptr : *« où est le sous-objet A partagé ? »*. Une indirection de
plus que l'héritage non virtuel. Le coût est faible (en général une
lecture en cache L1), mais c'est pour ça que la norme ne fait pas
de `virtual` le comportement par défaut.

### L'ordre de construction en assembleur

Pour `Dog d("Rex");` :

```asm
; 1. reserve storage (stack)
sub  rsp, 48

; 2. construct Animal subobject
lea  rdi, [rsp]                  ; this = &d (Animal view)
mov  rsi, "Rex"
call Animal::Animal

; 3. set vptr to Dog's vtable (compiler emitted automatically)
lea  rax, [rip + vtable_for_Dog + 16]
mov  [rsp], rax

; 4. construct Dog-specific members
lea  rdi, [rsp + 40]             ; &d.brain
; ... new Brain(), store pointer ...

; 5. run Dog ctor body (e.g. print trace)
; ...
```

L'étape 3 explique pourquoi les appels virtuels pendant
`Animal::Animal` sont aiguillés vers `Animal` — le vptr n'est pas
encore basculé vers celui de `Dog`. Entre l'étape 2 et l'étape 3, le
« type » de l'objet change à l'exécution.

### Remplissage et alignement

Le compilateur insère du remplissage (*padding*) entre les membres
pour respecter les contraintes d'alignement.
`Dog { char c; Brain* brain; }` fait **16 octets**, pas 9 : le
`Brain*` exige un alignement sur 8 octets, donc le compilateur
insère 7 octets de remplissage après `c`.

Règle pratique : **range les membres du plus grand au plus petit**
pour minimiser le remplissage. Ça ne change rien à la correction,
mais ça compte pour le remplissage du cache dans le code critique.
