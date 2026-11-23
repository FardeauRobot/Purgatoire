# 🧬 CPP07 — Les templates : écris-le une fois, laisse le compilateur le recopier

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP07.md)

> **TL;DR.** Un template est un **patron**, pas du code. Quand tu écris `max(a, b)` avec des `int`, le compilateur fabrique à partir de ton patron une vraie fonction `int max(int, int)`. Avec des `std::string`, il en fabrique une seconde. Ce module a trois exercices : des templates de fonctions (`swap`/`min`/`max`), un template de fonction qui reçoit *une autre* fonction (`iter`), et un template de classe qui possède de la mémoire (`Array<T>`). Le seul piège qui compte : **le corps d'un template doit être visible partout où il est utilisé**, donc il vit dans l'en-tête (ou dans un `.tpp` que l'en-tête inclut).

Voir aussi : [`TEMPLATES.md`](../../../../en/cpp/notions/advanced/TEMPLATES.md) (EN) · [`TEMPLATE.md`](../../../../en/cpp/lexique/TEMPLATE.md) (EN) · [`TYPENAME.md`](../../../../en/cpp/lexique/TYPENAME.md) (EN) · [`ORTHODOX_CANONICAL_FORM.md`](../../notions/oop/ORTHODOX_CANONICAL_FORM.md) · [`TRY_CATCH_THROW.md`](../../../../en/cpp/lexique/TRY_CATCH_THROW.md) (EN) · [`MEMORY.md`](../../../../en/cpp/notions/fundamentals/MEMORY.md) (EN) · Précédent : [`CPP06.md`](CPP06.md)

---

## 1. Le problème, dans le C que tu connais déjà

Tu veux un seul `max` qui marche pour tous les types. Le C te donne deux mauvaises options :

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int i = MAX(x++, y);        /* if x wins, x++ runs TWICE: a silent bug */

void *max_generic(void *a, void *b, int (*cmp)(void *, void *));
                            /* like qsort: no type checking, casts everywhere */
```

- La **macro** est une simple substitution de texte. Elle n'a ni types, ni portée, et elle évalue ses arguments deux fois.
- **`void *`**, c'est ce qu'utilisent la libft et `qsort`. Le type est effacé : le compilateur ne peut pas te dire que tu as passé un `char *` là où un `int *` était attendu.

**La réponse du C++ :** écrire la fonction une fois avec un *paramètre de type*. Le compilateur génère une version entièrement vérifiée pour chaque type que tu utilises réellement.

```cpp
template <typename T>
T const &max(T const &a, T const &b)
{
    return (a > b) ? a : b;
}
```

Chaque argument est évalué une fois et le résultat est vérifié par le compilateur. Aucun cast.

---

## 2. Le mécanisme, en un schéma

```
  TON EN-TÊTE (patron)                  CE QUE LE COMPILATEUR GÉNÈRE
 ┌──────────────────────────────┐
 │ template <typename T>        │ max(1, 2)          ┌───────────────────────────────┐
 │ T const &max(T const &a,     │ ─────────────────► │ int const &max(int const &,   │
 │              T const &b)     │   T = int          │                int const &)   │
 │ { return a > b ? a : b; }    │                    └───────────────────────────────┘
 │                              │ max(s1, s2)        ┌───────────────────────────────┐
 │                              │ ─────────────────► │ std::string const &max(...)   │
 └──────────────────────────────┘   T = std::string  └───────────────────────────────┘

  Aucun appel avec double ? → aucune version double n'existe dans le binaire.
```

Ça s'appelle l'**instanciation** (*instantiation*). Elle a lieu à la compilation, dans chaque `.cpp` qui utilise le template. Deux conséquences :

1. **Le corps doit être visible** dans ce `.cpp`. Si tu mets le corps dans `whatever.cpp`, `main.cpp` ne voit qu'une déclaration. Le compilateur ne peut rien fabriquer, et l'éditeur de liens échoue avec `undefined symbol: max<int>`.
2. **Les erreurs de type apparaissent à l'instanciation.** `max(a, b)` sur un type sans `operator>` compile sans problème jusqu'à ce que quelqu'un l'appelle vraiment avec ce type.

| C / modules précédents | CPP07 |
|---|---|
| Macro `#define` | Template de fonction (vérifié, évaluation unique) |
| `void *` + taille d'élément (`ft_memcpy` de la libft) | `T` connu exactement |
| `int *arr = malloc(n * sizeof(int))` | `Array<int> arr(n)` |
| Une classe par type d'élément | Un template de classe, `Array<T>` |
| Corps de fonctions dans le `.cpp` | Corps de templates dans le `.hpp` / `.tpp` |

---

## 3. ex00 — `swap`, `min`, `max`

### Ce que demande le sujet
- Trois templates de fonctions dans `whatever.hpp` : `swap` (ne renvoie rien), `min` et `max`.
- **Si les deux valeurs sont égales, `min` et `max` renvoient la seconde.**
- Ils doivent marcher avec tout type dont les valeurs sont comparables (qui supporte `<` et `>`).
- Le sujet fournit un `main`, et ta sortie doit correspondre :

```
a = 3, b = 2
min(a, b) = 2
max(a, b) = 3
c = chaine2, d = chaine1
min(c, d) = chaine1
max(c, d) = chaine2
```

### Extrait central

```cpp
#ifndef WHATEVER_HPP
# define WHATEVER_HPP

template <typename T>
void swap(T &a, T &b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

template <typename T>
T const &min(T const &a, T const &b)
{
    return (a < b) ? a : b;
}

template <typename T>
T const &max(T const &a, T const &b)
{
    return (a > b) ? a : b;
}

#endif
```

Ligne par ligne :
- `template <typename T>` introduit un paramètre de type nommé `T`. Ici, `typename` et `class` veulent dire la même chose.
- `swap` prend des **références** parce qu'il doit modifier les variables de l'appelant. C'est la version C++ de `void ft_swap(int *a, int *b)`.
- `min` prend des **références const**, pour que les `std::string` ne soient pas copiées. Il renvoie une référence const vers l'argument qui l'emporte.
- `(a < b) ? a : b` renvoie `b` quand les valeurs sont égales. C'est exactement la règle demandée par le sujet. `max` utilise `>` de la même façon.

### Pourquoi le `main` du sujet écrit `::swap(a, b)`
`<string>` amène `std::swap`, `std::min` et `std::max`. Tu n'as pas droit à `using namespace std`, donc tu pourrais t'attendre à aucun conflit. Mais quand un argument est une `std::string`, la **recherche dépendante des arguments** (*argument-dependent lookup*, ADL) fouille aussi le namespace `std`, celui où vit le type de l'argument. Testé avec Apple clang en `-std=c++98` :

| Appel non qualifié sur deux `std::string` | Résultat |
|---|---|
| `min(c, d)` | ❌ `error: call to 'min' is ambiguous`. Le tien et `std::min` ont la même signature |
| `swap(c, d)` | ⚠️ Compile, mais **appelle `std::swap` en silence**, parce que la surcharge pour les strings est plus spécialisée. Ton template ne s'exécute jamais |

`::swap` veut dire « celui du namespace global », c'est-à-dire le tien. Ça désamorce les deux problèmes.

### ⚠️ Pièges
- **Renvoyer `a` pour des valeurs égales** est l'erreur classique avec `(a <= b) ? a : b`. Teste avec `min(x, x)` sur deux variables différentes qui contiennent la même valeur, et compare leurs adresses.
- Ne mets pas tes templates dans un namespace. Le `main` du sujet appelle `::min`.
- Un **fichier composé uniquement d'un en-tête** convient ici. La règle « pas d'implémentation dans les en-têtes » de la grille exempte les templates.

---

## 4. ex01 — `iter`

### Ce que demande le sujet
`iter(address, length, function)` appelle `function` sur chaque élément du tableau. Elle ne renvoie rien. Elle doit marcher avec **n'importe quel type d'élément**, et la fonction peut être une **instanciation de template de fonction**. Les éléments peuvent être `const` ou non, donc la fonction peut prendre `T &` ou `T const &`.

### La forme de la solution

```cpp
template <typename T, typename F>
void iter(T *array, size_t const length, F func)
{
    if (array == NULL)
        return;
    for (size_t i = 0; i < length; i++)
        func(array[i]);
}
```

Pourquoi deux paramètres de type ?

```
iter(tab, 5, print<int>)
     │    │  └── F = void (*)(int const &)   ← l'appelable que tu passes, quel qu'il soit
     │    └───── length
     └────────── T = int                      ← déduit de int tab[5]

iter(ctab, 3, print<int>)       avec  int const ctab[3]
     └── T = int const → array[i] est un int const& → OK pour un callback `T const &`
```

- `T` vient du tableau. Passe un tableau `const` et `T` devient lui-même `int const`, donc le cas const marche tout seul.
- `F` accepte *tout ce qu'on peut appeler* : un simple pointeur de fonction, un template de fonction instancié, et même un objet foncteur (*functor* ; le C++98 n'a pas de lambdas).
- `F` est aussi plus souple que de figer `void (*)(T &)`. Cette signature **refuserait** un callback `void f(T const &)` sur un tableau non const.

Si tu préfères écrire explicitement les types de pointeurs, fournis deux surcharges :

```cpp
template <typename T>
void iter(T *array, size_t const length, void (*func)(T &));

template <typename T>
void iter(T const *array, size_t const length, void (*func)(T const &));
```

Les deux approches passent. Sois prêt à expliquer celle que tu as choisie.

### Passer un template de fonction comme callback

```cpp
template <typename T>
void print(T const &x) { std::cout << x << std::endl; }

int tab[] = { 0, 1, 2, 3, 4 };
::iter(tab, 5, print<int>);     // explicit template argument: names one real function
```

Le `<int>` explicite compte. Un `print` seul désigne une *famille* de fonctions, et avec un `F` générique le compilateur ne peut pas deviner laquelle tu veux.

### Ce que lance l'évaluateur
La grille compile son propre fichier de test avec ton `iter`. Il doit afficher :

```
0
1
2
3
4
42
42
42
42
42
```

Le fichier de test exact n'est pas reproduit ici, donc ne devine pas ses callbacks. Assure-toi que ton `iter` accepte une instanciation de template de fonction (`print<int>`), marche sur un tableau `const`, et marche avec un callback qui prend une référence non const et modifie l'élément. Un paramètre générique `F func` gère les trois cas.

### ⚠️ Pièges
- `length` est `const` dans la description du sujet. Respecte-le.
- N'alloue ni ne copie le tableau. `iter` ne fait que l'emprunter, exactement comme `ft_striteri` dans la libft.

---

## 5. ex02 — `Array<T>`

### Ce que demande le sujet
Un template de classe `Array<T>` avec :

| Membre | Comportement |
|---|---|
| `Array()` | Tableau vide |
| `Array(unsigned int n)` | `n` éléments, **initialisés par défaut** (astuce du sujet : essaie `int *a = new int();`) |
| Constructeur de copie / `operator=` | **Copie profonde** : modifier la copie ne doit jamais modifier l'original |
| `operator[]` | Accès par indice. **Lance une `std::exception`** en cas de dépassement |
| `size() const` | Nombre d'éléments. Ne prend aucun paramètre et ne modifie pas l'instance |

Autres règles :
- **La mémoire doit venir de `new[]`.** Pas d'allocation préventive (pas de « réserver 1000 au cas où »), et pas de conteneurs de la STL.
- Ton propre `main` doit le tester.
- Tu peux mettre l'implémentation dans un fichier `.tpp`.

### La forme de la classe

```cpp
#ifndef ARRAY_HPP
# define ARRAY_HPP

# include <exception>
# include <cstddef>

template <typename T>
class Array
{
private:
    T               *_data;
    unsigned int    _size;

public:
    Array();
    Array(unsigned int n);
    Array(Array const &other);
    Array &operator=(Array const &other);
    ~Array();

    T               &operator[](unsigned int i);
    T const         &operator[](unsigned int i) const;
    unsigned int    size() const;

    class OutOfBoundsException : public std::exception
    {
    public:
        virtual const char *what() const throw();
    };
};

# include "Array.tpp"

#endif
```

Le `.tpp` se place **en bas** de l'en-tête, après la déclaration de la classe. Tout ce qui inclut `Array.hpp` récupère aussi les corps, ce qui règle le problème de visibilité du §2.

### Extraits centraux de `Array.tpp`

```cpp
template <typename T>
Array<T>::Array() : _data(NULL), _size(0) {}

template <typename T>
Array<T>::Array(unsigned int n) : _data(new T[n]()), _size(n) {}

template <typename T>
Array<T>::Array(Array const &other) : _data(NULL), _size(0)
{
    *this = other;
}

template <typename T>
Array<T> &Array<T>::operator=(Array const &other)
{
    if (this != &other)
    {
        T *fresh = new T[other._size]();
        for (unsigned int i = 0; i < other._size; i++)
            fresh[i] = other._data[i];
        delete[] _data;
        _data = fresh;
        _size = other._size;
    }
    return *this;
}

template <typename T>
Array<T>::~Array() { delete[] _data; }

template <typename T>
T &Array<T>::operator[](unsigned int i)
{
    if (i >= _size)
        throw OutOfBoundsException();
    return _data[i];
}

template <typename T>
const char *Array<T>::OutOfBoundsException::what() const throw()
{
    return "Array: index out of bounds";
}
```

Explications :
- **`template <typename T>` se place avant *chaque* définition de membre.** Chacune est son propre petit template, et tu la qualifies avec `Array<T>::`.
- **`new T[n]()`** : les `()` à la fin initialisent les éléments par valeur (*value-initialise*), donc un tableau d'`int` commence rempli de `0` au lieu de déchets. Sans eux, `Array<int> a(5); a[0]` lit de la mémoire non initialisée, ce que valgrind signale par « conditional jump depends on uninitialised value ».
- **`new T[0]` est légal**, et `delete[]` dessus ne pose pas de problème. `delete[] NULL` ne fait rien non plus. C'est pour ça que le constructeur par défaut peut simplement utiliser `NULL`.
- **`operator=` alloue d'abord et supprime ensuite.** Si `new` lance une exception (`std::bad_alloc`), l'objet garde ses anciennes données valides. Supprimer d'abord laisserait un `_data` pendant (*dangling*).
- **`i >= _size` constitue toute la vérification de bornes.** L'indice est `unsigned`, donc `a[-1]` boucle vers 4294967295 et se fait attraper par le même test.
- **Le constructeur de copie met d'abord `_data(NULL)`**, puis réutilise `operator=`. Sinon `operator=` ferait un `delete[]` sur un pointeur non initialisé.

### Copie profonde contre copie superficielle, en mémoire

```
 SUPERFICIELLE (copie générée)              PROFONDE (ce que tu écris)
   pile               tas                     pile               tas
 ┌──────────────┐                           ┌──────────────┐
 │ a._data ─────┼──┐                        │ a._data ─────┼──► [1][2][3]
 │ a._size 3    │  ├──► [1][2][3]           │ a._size 3    │
 ├──────────────┤  │                        ├──────────────┤
 │ b._data ─────┼──┘                        │ b._data ─────┼──► [1][2][3]  (un nouveau bloc)
 │ b._size 3    │                           │ b._size 3    │
 └──────────────┘                           └──────────────┘
 b[0] = 42 modifie aussi a.                b[0] = 42 laisse a tranquille.
 ~b puis ~a → double delete[].             ~b et ~a libèrent chacun leur bloc.
```

C'est la même histoire que la copie profonde du `Brain` de CPP04. Le template n'y change rien ; il l'applique simplement à chaque `T`.

### Accès const, et pourquoi il te faut deux `operator[]`

```cpp
Array<int> const frozen(3);
std::cout << frozen[0];     // calls   T const &operator[](unsigned int) const
frozen[0] = 1;              // ❌ compile error: can't assign through a const reference
```

La grille vérifie exactement ça : « lecture et écriture via `operator[]`, ou lecture seule si l'instance est const ». Avec seulement la version non const, `frozen[0]` ne compile pas du tout.

### ⚠️ Pièges
- **`typename` pour les types dépendants.** Dans un template, un nom comme `Array<T>::Something` dépend de `T`, donc ce peut être un type ou une valeur, et le compilateur suppose une *valeur*. Quand tu l'utilises comme type à cet endroit, écris `typename Array<T>::Something`. Hors de tout template, le nom n'est pas dépendant : dans `main`, `catch (Array<int>::OutOfBoundsException &e)` n'a pas besoin de `typename`, et en ajouter un est une erreur en C++98 (`'typename' outside of a template is a C++11 extension`). Voir [`TYPENAME.md`](../../../../en/cpp/lexique/TYPENAME.md) (EN).
- **Teste avec un `T` complexe.** La grille te demande de prouver que ça marche avec des *types simples et complexes*. `Array<std::string>` est le minimum. Une petite classe avec une OCF écrite à la main est mieux, parce qu'elle prouve que `fresh[i] = other._data[i]` appelle *son* `operator=`.
- **`delete` au lieu de `delete[]`** est un comportement indéfini, et valgrind le signale comme une incohérence (*mismatch*). La mémoire issue de `new[]` doit être libérée avec `delete[]`.
- N'ajoute pas de `resize` ni de `push_back`. Le sujet ne les demande pas, et une logique d'allocation en plus, c'est un risque de plus en soutenance.

---

## 6. Ce que vérifie l'évaluateur

D'après la grille de notation officielle de 42evalhub :

**Prérequis** (en échouer un, et l'exercice, voire tout le projet, n'est pas noté)
- [ ] Compile avec `c++ -Wall -Wextra -Werror`. Le projet est en C++98 et n'utilise aucune fonctionnalité C++11.
- [ ] Aucune fonction implémentée dans un en-tête, **sauf les templates**.
- [ ] Pas de `*alloc`, `*printf` ni `free`. Pas de `using namespace`. Pas de `friend`. Aucune bibliothèque externe.
- [ ] Aucune fuite (valgrind / `leaks`).

**ex00**
- [ ] La sortie correspond au sujet pour les types simples (`int`).
- [ ] Marche aussi avec des **types complexes**. L'évaluateur utilise son propre fichier de test.

**ex01**
- [ ] Le fichier de test de l'évaluateur compile avec ton `iter` et affiche `0 1 2 3 4` puis cinq fois `42`.

**ex02**
- [ ] **La mémoire vient de `new[]`.** Sinon l'exercice n'est pas noté.
- [ ] Tu peux montrer sur le moment que ça marche avec **des types simples et complexes**.
- [ ] On peut construire un tableau vide et un tableau de taille `n`.
- [ ] `operator[]` lit et écrit, lit seulement sur une instance const, et **lance une `std::exception`** hors des bornes.

**Questions de soutenance à préparer**
- « Pourquoi le corps du template est-il dans l'en-tête ? » → L'instanciation a lieu dans chaque unité de traduction qui l'utilise (§2).
- « Qu'est-ce que `new T[n]()` fait de différent de `new T[n]` ? » → L'initialisation par valeur : les types natifs démarrent à zéro.
- « Que se passe-t-il si je copie puis modifie la copie ? » → Montre-le dans ton `main`.

---

## 7. Aide-mémoire

| Besoin | Écriture C++98 |
|---|---|
| Template de fonction | `template <typename T> T const &f(T const &a)` |
| Deux paramètres de type | `template <typename T, typename F>` |
| Instancier explicitement | `print<int>` |
| Membre d'un template de classe hors de la classe | `template <typename T> void Array<T>::f() { … }` |
| Mettre les corps ailleurs | `Array.tpp`, `#include` en bas de `Array.hpp` |
| Tableau initialisé par défaut | `new T[n]()` |
| Nom de type dépendant | `typename Container<T>::iterator` |
| Accès const et non const | Deux surcharges d'`operator[]`, dont une `const` |

**Ce que ce module te laisse :** tu peux lire `std::vector<T>` dans un message d'erreur sans paniquer, parce que tu en as écrit un petit toi-même. CPP08 te donne les vrais conteneurs de la STL, tous construits exactement de cette façon.

➡️ Suivant : [`CPP08.md`](CPP08.md). Les conteneurs de la STL sont aussi des templates. Maintenant tu apprends à les *utiliser*, via les itérateurs et les algorithmes.
