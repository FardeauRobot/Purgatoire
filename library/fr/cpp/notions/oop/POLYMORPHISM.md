# Polymorphisme et fonctions virtuelles — Guide complet

> 🇬🇧 [English version](../../../../en/cpp/notions/oop/POLYMORPHISM.md)

> **Zooms sur les mots-clés :** [`VIRTUAL`](../../../../en/cpp/lexique/VIRTUAL.md) (EN) · [`TYPEID`](../../../../en/cpp/lexique/TYPEID.md) (EN)

Le polymorphisme, c'est **« une interface, plusieurs comportements »**. Tu écris du code qui manipule des `Animal*` — et à l'exécution, il appelle `Dog::makeSound()` si l'objet réel est un `Dog`, `Cat::makeSound()` si c'est un `Cat`. Le même point d'appel produit des résultats différents selon le vrai type caché derrière le pointeur de base.

```cpp
Animal* zoo[3];
zoo[0] = new Dog();
zoo[1] = new Cat();
zoo[2] = new Dog();

for (int i = 0; i < 3; ++i)
    zoo[i]->makeSound();    // "Woof!" "Meow!" "Woof!"
```

Ça ne marche que parce que `makeSound` est **virtuelle**.

---

## Table des matières

1. [Deux sortes de polymorphisme](#1.%20Deux%20sortes%20de%20polymorphisme)
2. [Le mot-clé `virtual`](#2.%20Le%20mot-cl%C3%A9%20virtual)
3. [Liaison statique contre liaison dynamique](#3.%20Liaison%20statique%20contre%20liaison%20dynamique)
4. [La vtable — comment ça marche en mémoire](#4.%20La%20vtable%20%E2%80%94%20comment%20%C3%A7a%20marche%20en%20m%C3%A9moire)
5. [Fonctions virtuelles pures et classes abstraites](#5.%20Fonctions%20virtuelles%20pures%20et%20classes%20abstraites)
6. [Les interfaces en C++](#6.%20Les%20interfaces%20en%20C%2B%2B)
7. [La règle du destructeur virtuel](#7.%20La%20r%C3%A8gle%20du%20destructeur%20virtuel)
8. [Le découpage d'objet — le tueur silencieux](#8.%20Le%20d%C3%A9coupage%20d%27objet%20%E2%80%94%20le%20tueur%20silencieux)
9. [Appeler des virtuelles depuis les constructeurs/destructeurs](#9.%20Appeler%20des%20virtuelles%20depuis%20les%20constructeurs%2Fdestructeurs)
10. [Le coût de virtual](#10.%20Le%20co%C3%BBt%20de%20virtual)
11. [Dans CPP04](#11.%20Dans%20CPP04)
12. [Règles pratiques](#12.%20R%C3%A8gles%20pratiques)
13. [Pièges](#13.%20Pi%C3%A8ges)

---

## 1. Deux sortes de polymorphisme

Le C++ en a deux :

- **Le polymorphisme ad hoc (à la compilation)** — le comportement est choisi à la compilation. Surcharge de fonctions, surcharge d'opérateurs, templates.
  ```cpp
  void print(int);
  void print(const std::string&);
  print(42);      // calls the int overload — compiler picks
  ```
- **Le polymorphisme de sous-typage (à l'exécution)** — le comportement est choisi **à l'exécution**, selon le type réel de l'objet. Ça exige des fonctions `virtual`.
  ```cpp
  Animal* a = new Dog();
  a->makeSound();   // dispatched at runtime to Dog::makeSound
  ```

Ce guide traite de la seconde sorte. Pour les templates (ad hoc), voir [TEMPLATES.md](../../../../en/cpp/notions/advanced/TEMPLATES.md) (EN).

---

## 2. Le mot-clé `virtual`

Une méthode marquée `virtual` **peut être redéfinie** (*overridden*) par une sous-classe, et les appels passant par un pointeur ou une référence de base sont aiguillés vers la version dérivée.

```cpp
class Animal {
public:
    virtual void makeSound() const { std::cout << "..." << '\n'; }
    virtual ~Animal() {}
};

class Dog : public Animal {
public:
    void makeSound() const { std::cout << "Woof!" << '\n'; }   // override
};
```

Règles :
- La méthode de la sous-classe doit avoir **la même signature** (même type de retour, mêmes paramètres, même `const`). Sinon, tu déclares une méthode *différente* qui masque celle de la base.
- Tu peux réécrire `virtual` sur la redéfinition — facultatif en C++98, utile comme pense-bête.
- Le C++11 ajoute le mot-clé `override` pour que le compilateur vérifie la redéfinition. Tu ne l'as pas en C++98.

### Sans `virtual`

C'est la méthode de base qui est appelée, même à travers un pointeur vers une dérivée :

```cpp
class Animal { public: void makeSound() { std::cout << "...\n"; } };
class Dog : public Animal { public: void makeSound() { std::cout << "Woof!\n"; } };

Dog d;
Animal* a = &d;
a->makeSound();        // "..." — the base version, NOT Dog's
d.makeSound();         // "Woof!" — called directly on Dog
```

Sans virtual : **liaison statique** (*static dispatch*) — c'est le type déclaré du pointeur qui décide.
Avec virtual : **liaison dynamique** (*dynamic dispatch*) — c'est le type réel de l'objet qui décide.

---

## 3. Liaison statique contre liaison dynamique

La **liaison statique**, c'est ce que tu as en C : le compilateur regarde le type écrit dans le code et inscrit en dur l'adresse de la fonction.

```
Dog d;
d.bark();
  │
  └─► le compilateur émet un appel direct à Dog::bark — adresse connue à la compilation
```

La **liaison dynamique** ajoute un niveau d'indirection. Le compilateur cherche la vraie méthode à l'exécution, via une table accrochée à l'objet.

```
Animal* a = new Dog();
a->makeSound();
  │
  ├─► lit a->vptr
  ├─► lit [vptr + offset_de_makeSound]     ← la case de la vtable pour cette méthode
  └─► appel indirect à cette adresse       ← Dog::makeSound
```

Trois lectures + un appel indirect contre un appel en dur. C'est le coût ; il est faible mais pas nul.

---

## 4. La vtable — comment ça marche en mémoire

Pour chaque classe qui a au moins une fonction virtuelle, le compilateur construit une **vtable** — un tableau de pointeurs de fonctions, une case par méthode virtuelle. Chaque objet de cette classe reçoit un membre caché, le **vptr**, qui pointe vers la vtable de sa classe.

```
class Animal {
    virtual void makeSound() const;
    virtual ~Animal();
};
class Dog : public Animal { void makeSound() const; };
class Cat : public Animal { void makeSound() const; };

        vtable de Dog                 vtable de Cat
       ┌──────────────────────┐     ┌──────────────────────┐
       │ &Dog::makeSound      │     │ &Cat::makeSound      │
       │ &Dog::~Dog           │     │ &Cat::~Cat           │
       └──────────────────────┘     └──────────────────────┘

Dog d;                                Cat c;
┌─────────────────┐                  ┌─────────────────┐
│ vptr ──► vtable │                  │ vptr ──► vtable │
│ de Dog          │                  │ de Cat          │
├─────────────────┤                  ├─────────────────┤
│ _type ...       │                  │ _type ...       │
└─────────────────┘                  └─────────────────┘
```

Quand tu écris `a->makeSound()` avec `a` de type `Animal*` :
1. Le compilateur sait que `makeSound` est la case n° 0 de la vtable d'`Animal`.
2. Il lit `a->vptr` — la vtable de l'objet réel, quelle qu'elle soit.
3. Il lit la case n° 0 de cette vtable — il obtient `Dog::makeSound` si c'est un Dog.
4. Il appelle cette adresse.

Le vptr est réglé par le **constructeur** de chaque classe de la hiérarchie. Pendant le constructeur d'`Animal`, le vptr pointe vers la vtable d'`Animal`. Puis le constructeur de `Dog` s'exécute et met à jour le vptr vers la vtable de `Dog`. C'est pour ça qu'appeler une virtuelle depuis un constructeur de base donne la version de base (§9).

### Quelle taille fait l'objet ?

Un pointeur de plus que sans virtual. Sur une machine 64 bits, ça fait 8 octets de vptr en plus par objet — partagés entre *toutes* les fonctions virtuelles, peu importe leur nombre.

---

## 5. Fonctions virtuelles pures et classes abstraites

Une fonction **virtuelle pure** n'a pas de corps dans la classe de base. Syntaxe : `= 0` à la fin de la déclaration.

```cpp
class Shape {
public:
    virtual ~Shape() {}
    virtual double area() const = 0;        // pure virtual — no implementation
};
```

Une classe qui a au moins une virtuelle pure est **abstraite** :

- Tu ne peux pas l'instancier. `Shape s;` est une erreur de compilation.
- Tu peux quand même avoir des `Shape*` et des `Shape&`.
- Les sous-classes **doivent** redéfinir toutes les virtuelles pures pour devenir concrètes.

```cpp
class Circle : public Shape {
    double _r;
public:
    Circle(double r) : _r(r) {}
    double area() const { return 3.14159 * _r * _r; }   // provides the missing piece
};

Shape* s = new Circle(5);                    // OK
std::cout << s->area();                       // 78.5...
delete s;
```

### Pourquoi les classes abstraites existent

Pour dire : « cette catégorie de choses *doit* avoir cette opération, mais je ne peux pas fournir de valeur par défaut qui ait du sens ». Toute `Shape` a une `area`, mais il n'y a pas de défaut raisonnable — donc tu obliges chaque classe dérivée à définir la sienne.

---

## 6. Les interfaces en C++

Le C++ n'a pas de mot-clé `interface`. À la place, une **interface** est simplement une classe abstraite avec :

- Uniquement des fonctions virtuelles pures
- Un destructeur virtuel
- Aucune donnée membre

```cpp
class IClickable {
public:
    virtual ~IClickable() {}
    virtual void onClick() = 0;
    virtual bool isEnabled() const = 0;
};
```

Toute classe qui hérite en `public` d'`IClickable` et implémente les deux méthodes **est un** `IClickable`. C'est l'idiome C++ pour les interfaces de Java/C#.

Une classe peut hériter de **plusieurs** interfaces sans tomber dans les problèmes habituels de l'héritage multiple, parce que les interfaces n'ont pas de données susceptibles d'entrer en conflit.

---

## 7. La règle du destructeur virtuel

**Si quelqu'un risque de faire `delete` sur ton objet via un pointeur de base, ton destructeur de base DOIT être virtuel.**

```cpp
class Animal {
public:
    /* virtual */ ~Animal() {}
};
class Dog : public Animal {
    int* _brain;
public:
    Dog() : _brain(new int[100]) {}
    ~Dog() { delete[] _brain; }
};

Animal* a = new Dog();
delete a;
```

- **Sans `virtual` sur `~Animal()` :** seul `~Animal()` s'exécute. `_brain` fuit. C'est un comportement indéfini selon la norme.
- **Avec `virtual` sur `~Animal()` :** `~Dog()` s'exécute d'abord (et libère `_brain`), puis `~Animal()`.

La règle à 42 : **toute classe de base a un `virtual ~ClassName();`**. Toujours. Même si tu penses que personne ne fera de `delete` via un pointeur de base aujourd'hui, quelqu'un le fera demain, et le coût se limite à un vptr que tu as déjà pour les autres virtuelles.

---

## 8. Le découpage d'objet — le tueur silencieux

Affecter une dérivée **par valeur** à une base ne copie que le sous-objet de base. La partie dérivée est perdue en silence.

```cpp
Dog    d;
Animal a = d;       // SLICE: only the Animal sub-object copies
a.makeSound();      // base version — there's no more Dog here
```

Même chose avec les arguments de fonction :

```cpp
void treat(Animal a);         // by value — slices anything passed in
treat(d);                     // Dog's -ness is gone the moment we enter the function

void treat(const Animal& a);  // by reference — safe, a really is a Dog
treat(d);                     // calls Dog's virtuals correctly
```

**Règle : passe les objets polymorphes par pointeur ou par référence.** Jamais par valeur.

Les conteneurs ont le même problème : `std::vector<Animal>` découpe. Si tu as besoin d'un conteneur d'animaux mélangés, il faut un `std::vector<Animal*>`.

---

## 9. Appeler des virtuelles depuis les constructeurs/destructeurs

**Pendant un constructeur, l'objet n'est pas encore un « Dog » — il est en cours d'assemblage.** La base de `Dog` s'exécute en premier, donc le vptr pointe vers la vtable d'`Animal` pendant que le constructeur d'`Animal` tourne :

```cpp
class Animal {
public:
    Animal()          { init(); }      // calls whose init()?
    virtual void init() { std::cout << "Animal::init\n"; }
};
class Dog : public Animal {
public:
    void init() { std::cout << "Dog::init\n"; }
};

Dog d;    // prints "Animal::init" — NOT "Dog::init"
```

La même chose se produit à l'envers pendant la destruction : quand `~Animal()` s'exécute, la partie `Dog` a déjà disparu, donc les appels virtuels se résolvent vers la version d'`Animal`.

**N'appelle pas de virtuelles depuis les constructeurs ou les destructeurs**, sauf si tu as compris ce mécanisme. Le schéma le plus sûr : construire complètement l'objet, puis appeler la virtuelle explicitement.

---

## 10. Le coût de virtual

- **Mémoire :** un pointeur en plus par objet (le vptr), une vtable par classe.
- **Temps :** deux lectures mémoire en plus et un appel indirect par appel virtuel. Les CPU modernes les prédisent assez bien, mais un appel indirect empêche l'inlining.
- **Taille du binaire :** légèrement plus gros — toutes les vtables sont émises dans l'exécutable.

Pour du code applicatif normal, le coût est négligeable. Dans des boucles internes serrées, la liaison dynamique peut compter. Le principe de conception du C++ — « tu ne paies pas pour ce que tu n'utilises pas » — veut dire que les fonctions non virtuelles coûtent autant qu'un appel C ; seules les classes avec des virtuelles paient le prix de la vtable.

---

## 11. Dans CPP04

C'est le module où le polymorphisme fait enfin tilt.

- **ex00 — base `Animal`, sous-classes `Dog`/`Cat`, et un `WrongAnimal`.**
  Compare le comportement avec et sans `virtual makeSound()`. La hiérarchie `WrongAnimal` existe pour montrer ce qui se passe quand tu oublies `virtual` : c'est la méthode de la base qui est appelée via un `WrongAnimal*`, alors que l'objet est en réalité un `WrongCat`.

- **ex01 — `Brain` comme membre composé.**
  `Dog` et `Cat` possèdent chacun un `Brain*`. C'est là que la discipline de copie profonde compte : le constructeur de copie doit cloner le `Brain` ; `operator=` doit gérer l'auto-affectation et libérer l'ancien cerveau. Un `std::vector<Animal*>` de `Dog` et de `Cat` mélangés, puis on copie le vecteur — tout doit survivre.

- **ex02 — rendre `Animal` abstraite.**
  Transforme `Animal::makeSound` en `= 0`. Désormais `Animal a;` est une erreur de compilation, mais `Animal* a = new Dog();` marche toujours — et c'est la seule façon d'en manipuler un. Ça impose « tu ne peux pas avoir un Animal *générique* ; choisis une espèce ».

- **ex03 — l'exercice d'interfaces `IMateria` / `AMateria` / `Character` / `MateriaSource`.**
  Une vraie hiérarchie d'interfaces C++. `IMateria` (ou équivalent) est une classe abstraite avec uniquement des virtuelles pures ; `AMateria` est une implémentation partielle ; `Ice` et `Cure` sont concrètes. `Character` contient un `AMateria*[4]`. Tu manies un tableau polymorphe sans connaître le type concret — c'est tout l'intérêt.

À la fin, tu as vu : les fonctions virtuelles, l'effet des vtables, les classes abstraites, les interfaces, les tableaux polymorphes, et la copie profonde au sein d'une hiérarchie polymorphe.

---

## 12. Règles pratiques

- **Destructeur virtuel sur toute classe de base qui sera supprimée de façon polymorphe.** Toujours.
- **Si tu ajoutes une fonction virtuelle, demande-toi : ai-je déjà un destructeur virtuel ?** En général oui s'il y a la moindre virtuelle, mais vérifie.
- **Les signatures des redéfinitions doivent correspondre exactement** (nom, paramètres, `const`). Sinon, tu ajoutes une nouvelle méthode au lieu d'en redéfinir une.
- **Des classes abstraites pour les contrats « à implémenter obligatoirement ».** Ne fournis pas de défaut bancal pour quelque chose que chaque sous-classe doit personnaliser.
- **Interfaces = classes abstraites avec uniquement des virtuelles pures.** Garde-les minimales.
- **Passe les objets polymorphes par pointeur ou par référence.** Jamais par valeur.
- **N'appelle pas de fonctions virtuelles depuis les constructeurs ou les destructeurs.**

---

## 13. Pièges

- **Oublier `virtual` sur le destructeur** → `delete` via un pointeur de base fait fuir des ressources.
- **Signature de redéfinition qui ne correspond pas** (par ex. `const` sur la redéfinition mais pas sur la base, ou l'inverse) → crée en silence une nouvelle méthode qui ne redéfinit rien. C'est toujours la version de la base qui est appelée.
- **Découpage** en passage par valeur ou dans un conteneur de valeurs. `std::list<Animal>` découpe ; `std::list<Animal*>` non.
- **Les virtuelles dans les constructeurs/destructeurs ne sont pas aiguillées vers la dérivée.**
- **Les virtuelles pures ont quand même besoin d'une définition si tu veux les appeler via `Base::foo()` depuis une redéfinition.** Rare mais légal.
- **L'héritage multiple de classes concrètes est un champ de mines.** Les interfaces (virtuelles pures uniquement) ne posent pas de problème.
- **`dynamic_cast` exige une base polymorphe** (au moins une virtuelle). Voir [CASTS.md](../../../../en/cpp/lexique/CASTS.md) (EN).
- **Confusion statique / dynamique.** `Animal* a = new Dog; typeid(*a) == typeid(Dog)` est vrai ; `typeid(a) == typeid(Animal*)` est vrai aussi. `*a` est dynamique ; `a` est statique.

---

## 14. Vue matérielle — ce que fait réellement le CPU

### Un appel virtuel, en pseudo-assembleur x86-64

`a->makeSound()` où `a` est un `Animal*` :

```asm
mov  rax, [a]            ; load the vptr (first 8 bytes of the object)
mov  rax, [rax + 0]      ; load slot 0 of the vtable (makeSound)
mov  rdi, a              ; pass `this` as arg 0
call rax                 ; indirect call through the loaded pointer
```

Deux lectures mémoire dépendantes, puis un appel indirect. À
comparer avec un appel non virtuel, qui est un simple `call <symbol>`
direct.

### Prédiction de branchement

Les CPU modernes ont un **prédicteur de branchements indirects** qui
mémorise la cible des appels indirects récents. Quand `a` pointe
généralement vers le même type concret (point d'appel monomorphe),
le prédicteur fait mouche et l'appel est quasi gratuit — 1 à 2
cycles plus les lectures. Quand le point d'appel voit beaucoup de
types différents (mégamorphe), le prédicteur rate et tu paies un
vidage de pipeline de 10 à 20 cycles à chaque échec.

C'est pour ça que la dévirtualisation (le compilateur prouve le type
concret à la compilation) est une cible d'optimisation majeure. Dans
ton code de 42, considère qu'un appel virtuel coûte ~5 cycles et
passe à autre chose.

### Impact sur le cache

Le vptr occupe en général les 8 premiers octets de l'objet. Accéder
à n'importe quel membre ramène aussi le vptr (même ligne de cache).
Donc la première lecture de vtable est en général déjà en L1. La
vtable elle-même est partagée par toutes les instances — une ligne de
cache par vtable de classe, elle aussi en général chaude.

### Où se trouve la vtable dans le binaire

Les vtables vivent dans `.rodata` (section de données en lecture
seule). Chaque vtable contient les adresses des méthodes virtuelles
de la classe, plus les infos RTTI (utilisées par `dynamic_cast` et
`typeid`). L'éditeur de liens dédoublonne les vtables entre unités de
traduction grâce à la « règle de définition unique » (*one definition
rule*) + des symboles faibles.

`objdump -d -s -j .rodata ./program | grep -A 8 "vtable for Dog"`
te la montre si tu es curieux.

### Ce que fait réellement `dynamic_cast<Dog*>(animal_ptr)`

1. Lit le vptr.
2. Remonte jusqu'aux infos RTTI attachées à la vtable.
3. Compare le type RTTI à la RTTI de `Dog`.
4. En cas de correspondance (directe ou via la chaîne des bases),
   renvoie le pointeur ajusté. Sinon, renvoie `NULL`.

L'étape 3 est une **comparaison de chaînes** sur les noms de types
dans beaucoup d'ABI (ABI C++ Itanium). C'est lent — des
microsecondes pour une hiérarchie profonde. N'utilise `dynamic_cast`
que quand tu as vraiment besoin de distinguer les types à
l'exécution.

### Le bouchon des virtuelles pures

Une fonction virtuelle pure, dans la vtable, pointe vers
`__cxa_pure_virtual` sur l'ABI Itanium. Si elle est appelée par un
moyen ou un autre (par ex. une virtuelle appelée depuis un
constructeur de base avant que la dérivée existe), elle appelle
`std::terminate` — le programme s'arrête. C'est la vérification à
l'exécution du caractère abstrait, en plus de l'interdiction
d'instancier vérifiée à la compilation.
