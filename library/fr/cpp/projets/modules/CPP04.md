# C++ Module 04 — Guide d'apprentissage

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP04.md)

**Thème du module :** *polymorphisme par sous-typage — liaison
dynamique, classes abstraites et interfaces.*

Mod 03 t'a donné l'héritage sans le polymorphisme. Ce module ajoute
la pièce manquante : quand tu manipules un `Dog` à travers un
`Animal*`, appeler `makeSound()` doit aboyer, pas émettre un
« bruit d'animal » générique. C'est la **liaison dynamique**
(*dynamic dispatch*), et le mécanisme qui la permet (vtables, vptrs)
est le cœur de Mod 04.

Voir aussi : le [`TOPICS.md`](../../../../../LEVEL4/C++1/Mod04/TOPICS.md)
du module et [`oop/POLYMORPHISM.md`](../../notions/oop/POLYMORPHISM.md) pour la
référence complète.

---

## La phrase à mémoriser

> **`virtual`** fait que l'appel de méthode est aiguillé selon le
> **type réel** de l'objet (le type que tu as `new`é), et non selon
> le type statique du pointeur ou de la référence à travers lequel tu
> le manipules.

Sans `virtual` → le compilateur choisit la fonction à la compilation
d'après le type déclaré de la variable (liaison statique). Avec
`virtual` → le compilateur émet du code qui lit une **vtable** propre
à la classe à l'exécution et choisit la bonne redéfinition (liaison
dynamique).

---

## La vtable, en un schéma

Pour toute classe ayant au moins une méthode virtuelle :

```
class Animal              vtable d'Animal
┌───────┐                 ┌──────────────────────┐
│ vptr ─┼──────────────►  │ &Animal::makeSound   │
│ type  │                 │ &Animal::~Animal     │
└───────┘                 └──────────────────────┘

class Dog : public Animal  vtable de Dog
┌───────┐                 ┌──────────────────────┐
│ vptr ─┼──────────────►  │ &Dog::makeSound      │   ← redéfinie
│ type  │                 │ &Dog::~Dog           │   ← redéfinie
│ brain │                 └──────────────────────┘
└───────┘
```

Chaque instance a un `vptr` caché dans son en-tête (en général 8
octets). `p->makeSound()` devient *« charger le vptr de p ; charger
l'entrée N ; appeler à travers. »* Deux lectures mémoire de plus,
généralement déjà en cache.

---

## Exercice 00 — Polymorphism

### Ce que tu apprends
**Le mot-clé `virtual` et la différence entre liaison statique et
liaison dynamique**, observées côte à côte.

### Forme des classes
- `Animal` — `protected std::string type` ; `makeSound()` virtuelle.
  Concrète (instanciable).
- `Dog`, `Cat` — fixent `type`, redéfinissent `makeSound()`.
- `WrongAnimal` / `WrongCat` — même forme, **sans `virtual`**.

### Le moment clé dans `main`

```cpp
Animal const* i = new Dog();
Animal const* j = new Cat();
i->makeSound();   // "bark"  ← virtual dispatch works
j->makeSound();   // "meow"
delete i;
delete j;

WrongAnimal const* k = new WrongCat();
k->makeSound();   // generic "wrong animal sound"  ← static dispatch
delete k;
```

Ce contraste, c'est tout l'exercice.

### Astuces
- **Rends `~Animal()` virtuel** même si ex00 n'en a pas besoin — ex01
  en a besoin, et tu oublieras si tu n'en prends pas l'habitude.
- `c++98` n'a pas de mot-clé `override`. Tu signales la redéfinition
  en reproduisant exactement la signature. Une faute de frappe dans un
  type de paramètre crée silencieusement une nouvelle méthode au lieu
  de redéfinir — lance le code et teste.
- `type` est `protected` pour que les constructeurs dérivés puissent
  le fixer sans passer par un setter.

### Piège
**N'oublie pas le `const` de `makeSound() const`.** Si la base
déclare `virtual void makeSound() const;` et la dérivée `virtual void
makeSound();` (sans const), la dérivée **ne redéfinit pas** — c'est
une nouvelle méthode qui masque. L'aiguillage retombe sur la base.

---

## Exercice 01 — I don't want to set the world on fire

### Ce que tu apprends
**La copie profonde des classes qui possèdent une ressource**, et
**pourquoi la règle du destructeur virtuel existe**.

### Forme des classes
- `Brain` — `std::string ideas[100]`. Plutôt bête, pas de pointeurs.
- `Dog` / `Cat` — possèdent un `Brain*` privé. Le constructeur fait
  `new Brain()` ; le destructeur fait `delete brain`.
- Main : un tableau d'`Animal*`, moitié `Dog`, moitié `Cat`, puis
  `delete` de chacun à travers le pointeur de base.

### La règle du destructeur virtuel

```cpp
Animal* a = new Dog();
delete a;
```

- Si `~Animal()` est **virtuel**, le compilateur émet une recherche
  dans la vtable → `~Dog()` s'exécute d'abord, supprime le `Brain`,
  puis enchaîne sur `~Animal()`. Propre.
- Si `~Animal()` n'est **pas virtuel**, le compilateur code en dur un
  appel direct à `~Animal()`. `~Dog()` ne s'exécute jamais → le
  `Brain` fuit. **Comportement indéfini** selon le standard.

Règle : **quand tu comptes supprimer via un pointeur de base, le
destructeur de la base doit être virtuel.** `-Wnon-virtual-dtor`
avertit ; valgrind attrape la fuite.

### Copie profonde — pourquoi et comment

```cpp
Dog basic;
Dog copy = basic;    // copy ctor
```

Le constructeur de copie par défaut (généré par le compilateur) fait
une copie membre à membre — `basic` et `copy` pointent alors vers le
**même `Brain`**. Quand `copy` meurt, il supprime le cerveau. Le
`brain` de `basic` pointe désormais dans le vide. Quand `basic` meurt :
double free.

La correction — constructeur de copie + affectation par copie
personnalisés :

```cpp
Dog::Dog(Dog const& src) : Animal(src), brain(new Brain(*src.brain)) {
    std::cout << "Dog copy constructed" << std::endl;
}

Dog& Dog::operator=(Dog const& other) {
    if (this != &other) {
        Animal::operator=(other);
        delete brain;                           // free old
        brain = new Brain(*other.brain);        // deep-copy new
    }
    return *this;
}
```

Trois choses à remarquer :
1. **Transmettre la copie de la partie base** — `Animal(src)` / `Animal::operator=(other)`.
2. **Supprimer l'ancien avant d'allouer le nouveau** dans `operator=`.
   Sinon fuite.
3. **Garde contre l'auto-affectation** — sinon `delete brain; brain = new
   Brain(*other.brain);` libère justement ce que tu t'apprêtes à copier.

### Astuces
- **Teste la copie profonde** : modifie les idées de `copy`, affiche
  celles de `basic` — elles doivent différer. Sinon, ta copie est
  superficielle.
- **Valgrind** : `--show-leak-kinds=all ./animals`. Toute fuite ou
  lecture invalide signifie que le destructeur virtuel ou le schéma
  de copie profonde est faux.
- La classe `Brain` elle-même n'a pas besoin d'une OCF personnalisée —
  un tableau de `std::string` se copie déjà en profondeur, élément par
  élément.

### Piège
**Le tableau d'`Animal*` dans main.** Crée chacun avec `new Dog()` ou
`new Cat()`, stocke-le comme `Animal*`. La conversion vers la base
(*upcast*) est implicite — aucun cast nécessaire. Pour les `delete` :
`for (i=0; i<N; ++i) delete zoo[i];`.

---

## Exercice 02 — Abstract class

### Ce que tu apprends
**Les fonctions virtuelles pures et les classes abstraites.**

### La syntaxe

```cpp
class Animal {
public:
    virtual void makeSound() const = 0;   // pure virtual
};
```

`= 0` à la fin de la déclaration la marque comme pure. Une classe qui
a au moins une virtuelle pure est **abstraite** — tu ne peux pas
l'instancier :

```cpp
Animal a;    // ← compile error: Animal is abstract
Animal* p = new Dog();    // ← fine
```

### Le contrat

Toute classe dérivée concrète **doit redéfinir** chaque virtuelle
pure de la base. Tu en oublies une → la dérivée est aussi abstraite →
elle non plus n'est pas instanciable. Vérification à la compilation :
impossible d'oublier.

### Astuces
- Renomme `Animal` → `AAnimal` (la convention du préfixe `A` pour les
  classes abstraites) si tu veux coller au nommage `AMateria` de ex03.
  Optionnel, mais cohérent côté style.
- **Ne rends pas le destructeur virtuel pur.** Tu *peux*
  (`virtual ~Animal() = 0;` + un corps), mais c'est un anti-patron ici.
  Un simple virtuel suffit.
- Tout ce qui vient de ex01 doit encore compiler et se comporter de la
  même façon — le côté `Dog`/`Cat` ne change pas.

### Piège
Même les classes abstraites ont des vtables et des vptrs.
`sizeof(Animal)` est inchangé par rapport à ex01. Le langage refuse
simplement de te laisser instancier la classe abstraite.

---

## Exercice 03 — Interface & recap (bonus)

### Ce que tu apprends
**Les interfaces (classes abstraites pures) et le patron Prototype.**

### La distribution

- **`AMateria`** — abstraite. `getType()`, `clone()` (pure), `use()`.
- **`Ice`**, **`Cure`** — concrètes. Redéfinissent `clone()` et `use()`.
- **`ICharacter`** — interface abstraite pure. `getName`, `equip`,
  `unequip`, `use`.
- **`Character`** — implémente `ICharacter`. Inventaire de 4 emplacements.
- **`IMateriaSource`** — abstraite pure. `learnMateria`,
  `createMateria`.
- **`MateriaSource`** — implémente `IMateriaSource`. Jusqu'à 4
  modèles.

### Le patron Prototype

`MateriaSource::createMateria(type)` doit renvoyer un `AMateria*` du
bon type concret — mais `MateriaSource` ne connaît pas les types
dérivés (pas de `if type == "ice" return new Ice()`). Solution :
conserver un modèle de chaque type, et appeler `clone()` sur le
modèle :

```cpp
AMateria* Ice::clone() const { return new Ice(); }

AMateria* MateriaSource::createMateria(std::string const& type) {
    for (int i = 0; i < 4; ++i)
        if (templates[i] && templates[i]->getType() == type)
            return templates[i]->clone();
    return 0;   // c++98: NULL is fine too
}
```

Ajouter un nouveau type de materia (par ex. `Fire`) = écrire la classe
+ son `clone()` + enregistrer un modèle via `learnMateria`. Aucune
modification de `MateriaSource`.

### Copie profonde de l'inventaire de `Character`

```cpp
Character& Character::operator=(Character const& other) {
    if (this != &other) {
        for (int i = 0; i < 4; ++i) {
            delete inventory[i];
            inventory[i] = other.inventory[i]
                         ? other.inventory[i]->clone()
                         : 0;
        }
        name = other.name;
    }
    return *this;
}
```

`clone()`, c'est comme ça qu'on « copie un objet polymorphe sans
connaître son type ». C'est **l'**idiome pour ce genre de propriété.

### Le contrat de propriété — `unequip` ne supprime pas

```cpp
void Character::unequip(int idx) {
    if (idx < 0 || idx >= 4) return;
    inventory[idx] = 0;    // removes from inventory, NO delete
}
```

C'est maintenant l'appelant qui possède ce que contenait l'emplacement.
Il peut le rééquiper ailleurs ou le laisser par terre. Si `unequip`
supprimait, le pointeur de l'appelant deviendrait pendant — sans qu'il
puisse le savoir.

### Astuces
- **Parcours protégé contre les nuls.** Les emplacements de
  l'inventaire commencent à `0` (nul). Teste toujours
  `if (inventory[i])` avant d'appeler une méthode.
- **Inventaire plein / mauvais indice** — ne rien faire, ne rien
  afficher (sauf si le sujet dit autrement).
- **`clone()` ne doit pas copier la chaîne de type.** Construis un
  `Ice()` tout neuf (qui fixe son propre type dans son constructeur),
  pas `new Ice(*this)` (qui copierait le type via le constructeur de
  copie de la base).
- **`c++98` utilise `0` ou `NULL`** pour les pointeurs nuls — pas de
  `nullptr`.

### Piège
Le destructeur de `MateriaSource` doit supprimer chaque modèle non nul.
Le destructeur de `Character` doit supprimer chaque emplacement non nul.
Les materias laissées par terre (renvoyées par `unequip`) sont sous la
responsabilité du main — les tests vérifient ça.

---

## Ce que ce module te laisse

- Tu sais lire une hiérarchie de classes et prédire quelle méthode
  s'exécute à chaque appel.
- Le destructeur virtuel est un réflexe, plus une corvée.
- La copie profonde de pointeurs polymorphes est un schéma que tu
  écris sans réfléchir (`clone()` + `delete` de l'ancien + affectation
  du nouveau).
- Virtuelle pure + classe abstraite + convention d'interface (préfixe
  `A*` / `I*`) sont devenus naturels.

Après Mod 04, le C++ ressemble à du C++ — plus à du C avec de la
syntaxe en plus. Les modules restants (templates, STL, itérateurs,
conteneurs) s'appuient sur ces fondations.
