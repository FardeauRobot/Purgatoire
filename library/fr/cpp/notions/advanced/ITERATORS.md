# Les itérateurs — La colle de la STL

> 🇬🇧 [English version](../../../../en/cpp/notions/advanced/ITERATORS.md)

> **TL;DR.** Un itérateur est un **pointeur généralisé** : un objet qui marque une position dans un conteneur et qui répond à `*it`, `++it`, `it == other`. Chaque conteneur fournit des itérateurs via `begin()` / `end()`, chaque algorithme les consomme, et c'est pour ça qu'un seul `std::find` marche aussi bien sur un `vector`, une `list` qu'un tableau C. Ce qu'un itérateur *sait* faire dépend de sa **catégorie** (input → forward → bidirectional → random-access), et le moment où il *cesse* d'être valide dépend du **conteneur** d'où il vient.

Voir aussi : [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`ALGORITHMS.md`](../../../../en/cpp/notions/advanced/ALGORITHMS.md) (EN) · [`TEMPLATES.md`](../../../../en/cpp/notions/advanced/TEMPLATES.md) (EN) · [`TYPENAME.md`](../../../../en/cpp/lexique/TYPENAME.md) (EN) · [`containers/INDEX.md`](../containers/INDEX.md) · [`CPP08.md`](../../../../en/cpp/projets/modules/CPP08.md) (EN)

En-tête : `#include <iterator>` (pour les fonctions utilitaires et les adaptateurs — les types d'itérateurs eux-mêmes viennent avec l'en-tête de chaque conteneur)

---

## Table des matières

1. [Modèle mental — un pointeur qui connaît son conteneur](#1.%20Mod%C3%A8le%20mental%20%E2%80%94%20un%20pointeur%20qui%20conna%C3%AEt%20son%20conteneur)
2. [L'intervalle semi-ouvert `[begin, end)`](#2.%20L%27intervalle%20semi-ouvert%20%5Bbegin%2C%20end%29)
3. [Les quatre types imbriqués](#3.%20Les%20quatre%20types%20imbriqu%C3%A9s)
4. [La boucle canonique](#4.%20La%20boucle%20canonique)
5. [Les catégories d'itérateurs](#5.%20Les%20cat%C3%A9gories%20d%27it%C3%A9rateurs)
6. [Les outils de `<iterator>` — `advance`, `distance`, `iterator_traits`](#6.%20Les%20outils%20de%20%3Citerator%3E%20%E2%80%94%20advance%2C%20distance%2C%20iterator_traits)
7. [Les itérateurs inverses et `base()`](#7.%20Les%20it%C3%A9rateurs%20inverses%20et%20base%28%29)
8. [Les adaptateurs d'itérateurs — inserters et itérateurs de flux](#8.%20Les%20adaptateurs%20d%27it%C3%A9rateurs%20%E2%80%94%20inserters%20et%20it%C3%A9rateurs%20de%20flux)
9. [Invalidation des itérateurs — conteneur par conteneur](#9.%20Invalidation%20des%20it%C3%A9rateurs%20%E2%80%94%20conteneur%20par%20conteneur)
10. [Écrire du code générique sur des itérateurs (CPP08)](#10.%20%C3%89crire%20du%20code%20g%C3%A9n%C3%A9rique%20sur%20des%20it%C3%A9rateurs%20%28CPP08%29)
11. [Écrire son propre itérateur](#11.%20%C3%89crire%20son%20propre%20it%C3%A9rateur)
12. [Limites du C++98](#12.%20Limites%20du%20C%2B%2B98)
13. [Pièges](#13.%20Pi%C3%A8ges)
14. [Aide-mémoire](#14.%20Aide-m%C3%A9moire)

---

## 1. Modèle mental — un pointeur qui connaît son conteneur

Tu connais déjà les itérateurs : tu en as utilisé en C chaque fois que tu as parcouru un tableau avec un pointeur.

```c
int  arr[] = {3, 1, 4};
int *p;

for (p = arr; p != arr + 3; ++p)
    printf("%d\n", *p);
```

`p` marque une position, `*p` lit l'élément, `++p` passe au suivant, `p != arr + 3` vérifie si on a fini. Un itérateur, c'est exactement ce contrat, généralisé pour marcher aussi quand « le suivant » n'est pas « l'adresse mémoire d'après » :

```
   vector<int>  (contigu)                 list<int>  (nœuds chaînés)

   ┌───┬───┬───┬───┐                      ┌───┐    ┌───┐    ┌───┐
   │ 3 │ 1 │ 4 │ 1 │                      │ 3 │◄──►│ 1 │◄──►│ 4 │
   └───┴───┴───┴───┘                      └───┘    └───┘    └───┘
     ▲                                      ▲
     it          ++it = « adresse + 1 »     it       ++it = « suivre node->next »
```

Pour un `vector`, l'itérateur est en gros un `T*` emballé. Pour une `list`, il contient un pointeur de nœud et `operator++` fait `node = node->next` — comme parcourir le `t_list` de la libft avec `lst = lst->next`, mais caché derrière la même syntaxe `++`. **L'algorithme ne voit jamais la différence.** C'est tout l'intérêt.

```
    CONTENEURS  ──begin()/end()──►  ITÉRATEURS  ──►  ALGORITHMES
    savent comment les              savent les       ne savent rien du
    données sont stockées           parcourir        stockage, juste ++ * ==
```

---

## 2. L'intervalle semi-ouvert `[begin, end)`

Tout intervalle de la STL est **semi-ouvert** : `begin()` pointe sur le premier élément, `end()` pointe **une case après le dernier** — une sentinelle, pas un élément.

```
     begin()                     end()
       │                           │
       v                           v
     ┌───┬───┬───┬───┐  ┌ ─ ─ ┐
     │ 3 │ 1 │ 4 │ 1 │    ???      ← ne jamais déréférencer end()
     └───┴───┴───┴───┘  └ ─ ─ ┘
```

Pourquoi cette convention, et pas `[first, last]` ?

- **L'intervalle vide est naturel :** `begin() == end()` veut dire « rien ici ». Pas de cas particulier.
- **La taille gratuitement** (random-access) : `end() - begin() == size()`.
- **« Pas trouvé » a une valeur :** `std::find` renvoie `end()` quand il échoue — une position qu'on ne peut pas confondre avec un vrai élément. C'est l'équivalent C++ du `NULL` renvoyé par `ft_strchr`.

```cpp
std::vector<int>::iterator it = std::find(v.begin(), v.end(), 42);
if (it == v.end())
    std::cout << "not found\n";
```

---

## 3. Les quatre types imbriqués

Chaque conteneur définit ses types d'itérateurs comme des **typedefs imbriqués** :

```cpp
std::vector<int>::iterator               it;    // read + write
std::vector<int>::const_iterator         cit;   // read only
std::vector<int>::reverse_iterator       rit;   // walks backward, read + write
std::vector<int>::const_reverse_iterator crit;  // walks backward, read only
```

| Membre | Sur un conteneur non-const renvoie | Sur un conteneur `const` renvoie |
|---|---|---|
| `begin()` / `end()` | `iterator` | `const_iterator` |
| `rbegin()` / `rend()` | `reverse_iterator` | `const_reverse_iterator` |

La surcharge est choisie selon la **constance du conteneur**, pas selon la variable dans laquelle tu ranges le résultat. C'est pour ça qu'une fonction qui prend `const std::vector<int> &v` *doit* utiliser `const_iterator` :

```cpp
void print(const std::vector<int> &v)
{
    for (std::vector<int>::const_iterator it = v.begin(); it != v.end(); ++it)
        std::cout << *it << ' ';
}
```

### `const_iterator` ≠ `const iterator`

Même piège que `const int *` vs `int * const` en C :

```cpp
std::vector<int>::const_iterator a = v.begin();
*a = 5;      // ERROR — the element is read-only
++a;         // OK    — the iterator itself moves

const std::vector<int>::iterator b = v.begin();
*b = 5;      // OK    — the element is writable
++b;         // ERROR — the iterator is frozen
```

| Pointeur C | Équivalent itérateur |
|---|---|
| `const int *p` | `const_iterator` |
| `int * const p` | `const iterator` |

Un `iterator` se convertit implicitement en `const_iterator` (jamais l'inverse), donc on peut les comparer avec `==` / `!=`.

---

## 4. La boucle canonique

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it << '\n';
```

Chaque morceau est voulu :

- **`it != v.end()`, pas `it < v.end()`** — `<` n'existe que pour les itérateurs random-access. `!=` marche pour toutes les catégories, donc la même boucle compile sur une `list` ou une `map`.
- **`++it`, pas `it++`** — le postfixe doit renvoyer une copie de l'ancienne valeur. Pour un pointeur c'est gratuit ; pour un itérateur de type classe (`list`, `map`), c'est un objet gaspillé. Le préfixe n'est jamais pire.
- **`*it`** donne une référence vers l'élément ; **`it->member`** marche quand l'élément est une struct/classe — par exemple `it->first` / `it->second` sur une `map`.

Sur une `map`, l'élément est une `std::pair<const K, V>` :

```cpp
for (std::map<std::string, int>::const_iterator it = m.begin(); it != m.end(); ++it)
    std::cout << it->first << " = " << it->second << '\n';
```

---

## 5. Les catégories d'itérateurs

Tous les itérateurs ne supportent pas toutes les opérations. Un nœud de liste ne peut pas sauter 5 nœuds d'un coup, donc un itérateur de `list` n'offre pas `it + 5`. Le standard range les itérateurs en **cinq catégories** :

```
   InputIterator          OutputIterator
   (lecture unique, ++)   (écriture unique, ++)
          │
          ▼
   ForwardIterator        ── lecture multiple, ++
          │
          ▼
   BidirectionalIterator  ── + --
          │
          ▼
   RandomAccessIterator   ── + it+n, it-n, it[n], <, >, it2-it1
```

*Input* et *output* sont deux branches séparées : un forward iterator raffine **input**. Un forward iterator modifiable peut *aussi* servir là où on attend un output iterator, mais ce n'est pas la même chose qu'être « en dessous » dans la chaîne.

| Catégorie | Opérations ajoutées | Qui la fournit |
|---|---|---|
| Input | `*it` (lecture), `++`, `==`, `!=` — un seul passage | `std::istream_iterator` |
| Output | `*it = x` (écriture), `++` — un seul passage | `std::ostream_iterator`, `std::back_inserter` |
| Forward | multi-passage : des copies de `it` peuvent reparcourir les mêmes éléments | *(aucun conteneur C++98 ne s'arrête là)* |
| Bidirectional | `--it` | `list`, `set`, `multiset`, `map`, `multimap` |
| Random-access | `it + n`, `it - n`, `it += n`, `it[n]`, `it2 - it1`, `<` `>` `<=` `>=` | `vector`, `deque`, `std::string`, **pointeurs bruts** |

`stack`, `queue` et `priority_queue` sont des **adaptateurs** : ils n'exposent volontairement **aucun itérateur** (c'est ce que contourne l'ex02 de CPP08).

### Pourquoi c'est important : les algorithmes annoncent leur exigence

Chaque algorithme documente la catégorie la plus faible qu'il accepte. Passe-lui quelque chose de plus faible et le compilateur refuse — en général avec une longue erreur au fond de `<algorithm>`.

```cpp
std::list<int> l;
std::sort(l.begin(), l.end());   // ERROR — sort needs random-access, list gives bidirectional
l.sort();                        // OK    — list's own member, written for linked nodes
```

| Algorithme | Exige au minimum |
|---|---|
| `find`, `count`, `accumulate`, `copy` (source) | Input |
| `copy` (destination), `fill_n` | Output |
| `min_element`, `max_element`, `replace`, `unique` | Forward |
| `reverse` | Bidirectional |
| `sort`, `nth_element`, `random_shuffle` | Random-access |

Un pointeur brut est un itérateur random-access, c'est pour ça que `std::sort(arr, arr + 3)` marche sur un tableau C.

---

## 6. Les outils de `<iterator>` — `advance`, `distance`, `iterator_traits`

Comme `it + 5` ne compile que sur des itérateurs random-access, `<iterator>` fournit des versions qui marchent pour toutes les catégories :

```cpp
#include <iterator>

std::list<int>::iterator it = l.begin();
std::advance(it, 3);                                       // moves it 3 steps (works on any category)
std::ptrdiff_t n = std::distance(l.begin(), l.end());     // counts steps between two iterators
```

Elles choisissent la meilleure stratégie selon la catégorie : sur un `vector`, `advance(it, 3)` fait `it += 3` (O(1)) ; sur une `list`, trois `++it` (O(n)). Même appel, coût différent — sache sur quel conteneur tu es.

- `advance` **modifie son argument** et renvoie `void`. Il n'y a pas de `std::next` / `std::prev` en C++98 — copie d'abord, puis avance :
  ```cpp
  std::list<int>::iterator third = l.begin();
  std::advance(third, 2);
  ```
- `distance(first, last)` exige que `last` soit atteignable depuis `first` avec `++`. Les inverser est un comportement indéfini sur des itérateurs non random-access.

### `std::iterator_traits`

Comment `advance` connaît-il la catégorie ? Chaque itérateur expose cinq typedefs, qu'on lit via `std::iterator_traits<It>` :

| Typedef | Signification | Pour `std::vector<int>::iterator` |
|---|---|---|
| `value_type` | type de l'élément | `int` |
| `difference_type` | type de `it2 - it1` | `std::ptrdiff_t` |
| `pointer` | `value_type *` | `int *` |
| `reference` | `value_type &` | `int &` |
| `iterator_category` | un **type étiquette** (tag) | `std::random_access_iterator_tag` |

`iterator_traits` est aussi spécialisé pour les pointeurs bruts, donc le code générique marche sur un `int *`. Pratique dans tes propres templates quand tu as besoin du type d'élément d'un itérateur quelconque :

```cpp
template <typename It>
typename std::iterator_traits<It>::value_type sum(It first, It last)
{
    typename std::iterator_traits<It>::value_type total = 0;
    for (; first != last; ++first)
        total += *first;
    return total;
}
```

Note les deux `typename` : `value_type` est un nom *dépendant*, donc il faut dire au compilateur que c'est un type ([`TYPENAME.md`](../../../../en/cpp/lexique/TYPENAME.md) (EN)).

---

## 7. Les itérateurs inverses et `base()`

`rbegin()` pointe sur le **dernier** élément, `rend()` sur « une case avant le premier ». `++` sur un itérateur inverse recule :

```cpp
for (std::vector<int>::reverse_iterator rit = v.rbegin(); rit != v.rend(); ++rit)
    std::cout << *rit << ' ';      // prints in reverse order
```

```
              rend()   rbegin()
                 │        │
                 v        v
            ┌ ─ ┬───┬───┬───┬───┐
              ? │ 3 │ 1 │ 4 │ 1 │
            └ ─ ┴───┴───┴───┴───┘
                  ▲               ▲
               begin()          end()
```

Un itérateur inverse est un adaptateur qui emballe un itérateur normal, stocké **une position plus à droite**. `rit.base()` rend cet itérateur emballé :

```
   *rit  ==  *(rit.base() - 1)
```

Ce décalage de un compte quand tu veux faire `erase` via un itérateur inverse — `erase` n'accepte que des itérateurs normaux :

```cpp
std::vector<int>::reverse_iterator rit = std::find(v.rbegin(), v.rend(), 4);
if (rit != v.rend())
    v.erase((++rit).base());       // ++ first, then base() lands on the right element
```

---

## 8. Les adaptateurs d'itérateurs — inserters et itérateurs de flux

`<iterator>` fournit des itérateurs qui ne pointent dans aucun conteneur — ils **font quelque chose** quand on écrit à travers eux.

### Les inserters — faire grandir un conteneur depuis un algorithme

`std::copy` écrit dans `*dest++` et suppose qu'il y a la place. Copier dans un vector vide est un crash classique :

```cpp
std::vector<int> dst;
std::copy(src.begin(), src.end(), dst.begin());                 // UB — dst has size 0
std::copy(src.begin(), src.end(), std::back_inserter(dst));     // OK — calls dst.push_back()
```

| Adaptateur | Chaque `*it = x` appelle | Le conteneur doit être |
|---|---|---|
| `std::back_inserter(c)` | `c.push_back(x)` | `vector`, `deque`, `list`, `string` |
| `std::front_inserter(c)` | `c.push_front(x)` (inverse l'ordre !) | `deque`, `list` |
| `std::inserter(c, pos)` | `c.insert(pos, x)` | n'importe lequel — le seul possible pour `set` / `map` |

### Les itérateurs de flux — traiter les E/S comme un intervalle

```cpp
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
```

affiche chaque élément suivi d'un espace — sans boucle.

```cpp
std::istringstream iss("3 1 4 1 5");
std::vector<int> v((std::istream_iterator<int>(iss)), std::istream_iterator<int>());
```

lit des ints jusqu'à ce que l'extraction échoue ; un `istream_iterator` construit par défaut est la sentinelle « fin de flux ». Les **parenthèses en plus** autour du premier argument sont obligatoires en C++98 : sans elles, la ligne est lue comme une *déclaration de fonction* (le « most vexing parse »).

---

## 9. Invalidation des itérateurs — conteneur par conteneur

Un itérateur n'est valide que tant que ce qu'il désigne reste à sa place. Modifie le conteneur et certains itérateurs deviennent **pendants** (dangling) — les utiliser est un comportement indéfini, en général silencieux jusqu'au jour où il ne l'est plus. Valgrind ou `-fsanitize=address` l'attrapent souvent.

| Conteneur | Insertion | Suppression |
|---|---|---|
| `vector` | S'il **réalloue** (size > capacity) : **tous** invalidés. Sinon : ceux à partir du point d'insertion. | Ceux à partir de l'élément supprimé. |
| `deque` | N'importe où : **tous les itérateurs** invalidés. (Aux extrémités, les références/pointeurs vers les éléments restent valides.) | Au milieu : tous. Aux extrémités : seulement ceux supprimés (+ `end()`). |
| `list` | **Aucun.** | Seulement l'élément supprimé. |
| `set` / `map` (et multi-) | **Aucun.** | Seulement l'élément supprimé. |

Modèle mental : un `vector` déplace tout son buffer quand il grandit (comme `realloc`), donc tout pointeur vers lui devient périmé. `list` et `map` allouent un nœud par élément et ne déplacent jamais les nœuds, donc les itérateurs survivent à tout sauf à la mort de leur propre nœud.

### Supprimer en parcourant

La mauvaise façon — `it` est mort après `erase`, puis `++it` l'utilise :

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    if (*it % 2 == 0)
        v.erase(it);                // UB
```

`vector`, `deque` et `list` — `erase` renvoie le prochain itérateur valide :

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); )
{
    if (*it % 2 == 0)
        it = v.erase(it);
    else
        ++it;
}
```

`set` / `map` — **en C++98, `erase(it)` renvoie `void`**, donc on avance au-delà de l'élément *avant* de le supprimer grâce au post-incrément :

```cpp
for (std::map<std::string, int>::iterator it = m.begin(); it != m.end(); )
{
    if (it->second == 0)
        m.erase(it++);              // it++ moves on, erase gets the old copy
    else
        ++it;
}
```

Sur un `vector`, la version idiomatique en masse est l'erase–remove :

```cpp
v.erase(std::remove(v.begin(), v.end(), 0), v.end());
```

Détails pour chaque conteneur : [`VECTOR.md`](../containers/VECTOR.md) · [`DEQUE.md`](../containers/DEQUE.md) · [`LIST.md`](../containers/LIST.md) · [`MAP.md`](../containers/MAP.md) · [`SET.md`](../containers/SET.md).

---

## 10. Écrire du code générique sur des itérateurs (CPP08)

CPP08, c'est là où tu arrêtes d'*utiliser* des itérateurs pour commencer à *écrire du code qui en accepte*.

### ex00 — `easyfind` : récupérer le type d'itérateur de `T`

```cpp
template <typename T>
typename T::iterator easyfind(T &container, int value)
{
    typename T::iterator it = std::find(container.begin(), container.end(), value);
    if (it == container.end())
        throw std::runtime_error("value not found");
    return it;
}
```

- `T::iterator` dépend de `T`, donc il faut `typename`.
- Prendre `T &` (et pas `const T &`) est ce qui rend `T::iterator` correct. Une surcharge `const` renverrait `typename T::const_iterator`.

### ex01 — `Span` : ajouter un intervalle avec un membre template

```cpp
class Span
{
public:
    template <typename InputIt>
    void addNumbers(InputIt first, InputIt last)
    {
        if (std::distance(first, last) > static_cast<std::ptrdiff_t>(_max - _numbers.size()))
            throw std::length_error("Span is full");
        _numbers.insert(_numbers.end(), first, last);
    }
    // ...
};
```

Nommer le paramètre `InputIt` documente la catégorie la plus faible acceptée. Comme c'est un template, il accepte aussi bien un `vector<int>::iterator`, un `list<int>::iterator` qu'un simple `int *`.

### ex02 — `MutantStack` : exposer les itérateurs cachés

`std::stack` n'a pas d'itérateurs, mais il emballe un vrai conteneur rangé dans un membre **protected** `c` (une `deque` par défaut) :

```cpp
template <typename T>
class MutantStack : public std::stack<T>
{
public:
    typedef typename std::stack<T>::container_type::iterator iterator;

    iterator begin() { return this->c.begin(); }
    iterator end()   { return this->c.end(); }
};
```

`this->c` est obligatoire : `c` vit dans une classe de base dépendante, donc un `c` non qualifié n'y est pas cherché. N'oublie pas `const_iterator`, `reverse_iterator` et les surcharges `const` si tu veux une interface complète — plus les quatre membres canoniques.

---

## 11. Écrire son propre itérateur

Rarement nécessaire dans les modules CPP, mais ça montre qu'il n'y a pas de magie. Un itérateur **forward** minimal sur une liste simplement chaînée :

```cpp
#include <iterator>

template <typename T>
struct Node
{
    T     value;
    Node *next;
};

template <typename T>
class NodeIterator : public std::iterator<std::forward_iterator_tag, T>
{
public:
    NodeIterator() : _node(NULL) {}
    explicit NodeIterator(Node<T> *node) : _node(node) {}
    NodeIterator(const NodeIterator &other) : _node(other._node) {}
    NodeIterator &operator=(const NodeIterator &other) { _node = other._node; return *this; }
    ~NodeIterator() {}

    T &operator*() const  { return _node->value; }
    T *operator->() const { return &_node->value; }

    NodeIterator &operator++()   { _node = _node->next; return *this; }
    NodeIterator  operator++(int) { NodeIterator old(*this); _node = _node->next; return old; }

    bool operator==(const NodeIterator &o) const { return _node == o._node; }
    bool operator!=(const NodeIterator &o) const { return _node != o._node; }

private:
    Node<T> *_node;
};
```

- Hériter de `std::iterator<Category, T>` (C++98) fournit les cinq typedefs que lit `iterator_traits` — du coup `std::distance` et `std::find` marchent dessus directement.
- `end()` serait `NodeIterator(NULL)` : le `next` nul du dernier nœud sert de sentinelle.
- `operator++(int)` — le `int` factice marque la forme **postfixe** ; note la copie qu'elle doit faire (la raison de préférer `++it`).

---

## 12. Limites du C++98

| Fonctionnalité moderne | Alternative C++98 |
|---|---|
| `auto it = v.begin();` | Écrire le type : `std::vector<int>::iterator it = v.begin();` — ou un `typedef` |
| `for (int x : v)` | La boucle canonique (§4) |
| `v.cbegin()` / `v.cend()` | Ranger dans un `const_iterator`, ou appeler `begin()` sur une référence `const` |
| `std::next(it)` / `std::prev(it)` | Copier, puis `std::advance(copie, ±1)` |
| `std::begin(arr)` / `std::end(arr)` | `arr` et `arr + sizeof(arr) / sizeof(arr[0])` |
| `map::erase(it)` renvoie l'itérateur suivant | Renvoie `void` — utiliser `m.erase(it++)` |

Les noms de types à rallonge, c'est la taxe du C++98. Dompte-les avec un `typedef` :

```cpp
typedef std::map<std::string, int>::const_iterator WordIt;

for (WordIt it = counts.begin(); it != counts.end(); ++it)
    ...
```

---

## 13. Pièges

- **Déréférencer `end()`** — UB. Compare toujours à `end()` avant `*it`, surtout après un `find`.
- **`*v.begin()` sur un conteneur vide** — même chose : `begin() == end()`, donc tu as déréférencé `end()`.
- **Comparer des itérateurs de conteneurs différents** — `a.begin() != b.end()` compile et ne veut rien dire. Méfie-toi quand une fonction renvoie un conteneur **par valeur** et que tu appelles `.begin()` et `.end()` sur deux temporaires distincts.
- **Réutiliser un itérateur après un `push_back`** sur un `vector` — une réallocation l'invalide en silence (§9). Appelle `reserve()` avant, ou garde des indices.
- **`std::sort` sur une `list`** — ne compile pas. Utilise `l.sort()`.
- **Modifier une clé via un itérateur de `set`/`map`** — `*it` sur un `set` est en lecture seule, et le `it->first` d'une map est `const K`. Supprime puis réinsère.
- **Oublier `typename`** devant `T::iterator` dans un template — erreur « expected a type ».
- **`it++` dans des boucles chaudes sur des itérateurs de classe** — une copie pour rien. Utilise `++it`.
- **`std::copy` dans un conteneur vide** — écrit au-delà de la fin. Utilise `std::back_inserter`.

---

## 14. Aide-mémoire

```cpp
// Types
C::iterator   C::const_iterator   C::reverse_iterator   C::const_reverse_iterator

// Ranges
c.begin()  c.end()         // [first, one-past-last)
c.rbegin() c.rend()        // backward

// Walk
for (C::iterator it = c.begin(); it != c.end(); ++it) { *it; it->member; }

// Category-agnostic moves (<iterator>)
std::advance(it, n);              // void, modifies it
std::distance(first, last);       // number of ++ steps

// Adaptors (<iterator>)
std::back_inserter(c)   std::front_inserter(c)   std::inserter(c, pos)
std::ostream_iterator<T>(std::cout, " ")   std::istream_iterator<T>(stream)

// Erase while iterating
it = c.erase(it);    // vector, deque, list
c.erase(it++);       // set, map (C++98: erase returns void)
```

| | `vector` / `deque` / `string` | `list` | `set` / `map` | `stack` / `queue` |
|---|---|---|---|---|
| Catégorie | Random-access | Bidirectional | Bidirectional | — (aucune) |
| `it + n`, `<`, `[]` | ✅ | ❌ | ❌ | — |
| `--it` | ✅ | ✅ | ✅ | — |
| `std::sort` | ✅ | ❌ (`l.sort()`) | ❌ (déjà trié) | — |
| Survit à une insertion ? | ❌ | ✅ | ✅ | — |

> **Résumé en une ligne.** Un itérateur est un pointeur généralisé à n'importe quel conteneur : `begin()`/`end()` donnent un intervalle semi-ouvert, la **catégorie** dit quelles opérations compilent, et le **conteneur** dit combien de temps l'itérateur reste valide.
