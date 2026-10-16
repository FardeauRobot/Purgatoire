# `std::list` — La liste doublement chaînée

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/LIST.md)

> **TL;DR.** Une `std::list<T>` est une **liste doublement chaînée de nœuds alloués sur le tas**. Chaque nœud contient une valeur et deux pointeurs (`prev`, `next`). Elle offre l'insertion/la suppression en O(1) **n'importe où** dès que tu tiens un itérateur, et **les itérateurs restent valides** après n'importe quelle insertion ou suppression, sauf sur l'élément retiré. Le prix : pas d'accès direct (`l[i]` n'existe pas), et chaque parcours coûte un défaut de cache par nœud.

Voir aussi : [`VECTOR.md`](VECTOR.md) · [`DEQUE.md`](DEQUE.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <list>`

---

## 1. Modèle mental

C'est le `t_list *` de la libft, mais **doublement chaîné** et en template :

```
   list<int> avec trois éléments : [10, 20, 30]

       ┌─────┐      ┌─────┐      ┌─────┐
   …──→│ 10  │←────→│ 20  │←────→│ 30  │←──…
       │prev │      │prev │      │prev │
       │next │      │next │      │next │
       └─────┘      └─────┘      └─────┘
         ▲                          ▲
         │                          │
       front()                    back()

   Chaque nœud = une allocation sur le tas, éparpillée n'importe où en RAM.
```

En C, avec les conventions de la libft :

```c
typedef struct s_node {
    int             value;
    struct s_node   *prev;
    struct s_node   *next;
}   t_node;
```

C'est tout le secret. Chaque `push_back` est un `malloc(sizeof(t_node))` + le rechaînage de deux pointeurs. Pas de redimensionnement, pas de realloc, pas de copie de toute la structure. C'est pour ça que les itérateurs restent valides : le nœud vers lequel pointe un itérateur ne bouge jamais.

---

## 2. Construction

```cpp
#include <list>

std::list<int> a;                            // empty
std::list<int> b(5);                         // five default-constructed elements (0)
std::list<int> c(5, 42);                     // five 42s
std::list<int> d(c);                         // copy
std::list<int> e(c.begin(), c.end());        // range copy
std::list<int> f(arr, arr + 10);             // from a C array

a = c;
a.assign(5, 7);
a.assign(c.begin(), c.end());
```

Mêmes constructeurs que `vector`. Pas de surprise ici.

---

## 3. API essentielle

| Méthode                       | Ce qu'elle fait                                               |
|-------------------------------|---------------------------------------------------------------|
| `push_back(x)` / `push_front(x)` | Alloue un nœud, le chaîne. Les deux en O(1).               |
| `pop_back()` / `pop_front()`  | Déchaîne + supprime le nœud d'extrémité. O(1).                |
| `insert(it, x)`               | Insère avant `it`. O(1) **une fois** `it` connu.              |
| `erase(it)`                   | Déchaîne + supprime. O(1). Renvoie un itérateur vers le suivant. |
| `clear()`                     | Détruit tous les nœuds. O(n) — libère réellement la mémoire (pas de buffer). |
| `front()` / `back()`          | Référence vers la première / dernière valeur. O(1). UB si vide. |
| `size()` / `empty()`          | O(1) en C++11+ ; **O(n) dans certaines implémentations C++98** de libstdc++ pré-4.x. `empty()` est le choix sûr. |
| `begin()` / `end()`           | Itérateurs bidirectionnels (pas de `+`, `-`, ni `<`).         |
| `rbegin()` / `rend()`         | Itérateurs inverses.                                          |

**Pas d'`operator[]`. Pas d'`at()`.** L'accès direct est en O(n) par définition — le standard refuse de l'exposer.

### Membres spécifiques à list

Ils existent sur `std::list` parce qu'ils exploitent la structure chaînée pour coûter moins cher que l'algorithme générique :

| Méthode             | Ce qu'elle fait                                                   | Coût |
|---------------------|-------------------------------------------------------------------|------|
| `splice(pos, other)` | Déplace tout `other` dans `*this` avant `pos`. **Aucune copie, aucune allocation.** | O(1) |
| `splice(pos, other, it)` | Déplace un élément. | O(1) |
| `splice(pos, other, first, last)` | Déplace un intervalle. | O(1) en C++98 (taille non maintenue) |
| `merge(other)`      | Fusionne deux listes **triées** en une liste triée. | O(n+m) |
| `sort()`            | Trie sur place. List ne peut pas utiliser `std::sort` (qui exige des itérateurs à accès direct). | O(n log n) |
| `reverse()`         | Inverse la chaîne en échangeant les pointeurs `prev`/`next`. | O(n) |
| `unique()`          | Supprime les doublons consécutifs (trie d'abord si tu veux l'unicité globale). | O(n) |
| `remove(val)`       | Efface chaque nœud égal à `val`. | O(n) |
| `remove_if(pred)`   | Efface chaque nœud qui satisfait `pred`. | O(n) |

`std::sort(l.begin(), l.end())` **ne compile pas** pour une list — ses itérateurs sont bidirectionnels, `std::sort` exige l'accès direct. Utilise le membre : `l.sort();`.

---

## 4. Aide-mémoire des complexités

| Opération                     | Coût   | Remarque                                      |
|-------------------------------|--------|-----------------------------------------------|
| `push_back` / `push_front`    | O(1)   | Un `malloc`. Jamais de réallocation.          |
| `pop_back` / `pop_front`      | O(1)   |                                                |
| `insert(it, x)`               | O(1)   | Mais trouver `it` a pu coûter O(n).           |
| `erase(it)`                   | O(1)   |                                                |
| `find(begin, end, x)`         | O(n)   | Ni arbre ni hachage — parcours complet.       |
| `l.sort()`                    | O(n log n) | Fonction membre ; utilise un tri fusion.  |
| `l.splice(pos, other)`        | O(1)   | L'atout maître. Déplace toute une liste, sans copie. |
| `size()`                      | O(1) (C++11+) / O(n) (certains C++98)  | Utilise `empty()` quand seul le booléen t'intéresse. |
| `clear()`                     | O(n)   | Libère chaque nœud.                           |
| Accès direct `l[i]`           | **N/A** | N'existe pas. Avancer un itérateur à la main avec `++`, c'est du O(n). |

---

## 5. Invalidation des itérateurs — le super-pouvoir de list

C'est là que `list` brille :

| Opération                | Itérateurs invalidés ?                                     |
|--------------------------|------------------------------------------------------------|
| `push_back` / `push_front` | **Aucun.** Les itérateurs existants restent valides.     |
| `insert(it, x)`          | **Aucun.** `it` pointe toujours au même endroit.           |
| `erase(it)`              | Seulement `it` lui-même.                                   |
| `splice(...)`            | **Aucun.** Les nœuds déplacés gardent leur identité (et leurs itérateurs !), même transférés dans une autre liste. |
| `clear()`                | Tous les itérateurs (chaque nœud est libéré).              |
| `swap(other)`            | `end()` change de sens, mais les itérateurs vers les éléments restent valides. |

C'est la raison décisive de préférer `list` à `vector` : si tu as besoin d'itérateurs qui vivent longtemps à travers de nombreuses insertions, list est le seul conteneur séquentiel qui te les offre.

---

## 6. Astuces d'efficacité

### 6.1 Utilise `splice` pour déplacer des nœuds entre listes gratuitement

```cpp
std::list<int> a; a.push_back(1); a.push_back(2);
std::list<int> b; b.push_back(99);

b.splice(b.begin(), a);      // a is now empty, b is [1, 2, 99]
                              // ZERO allocations, ZERO copies — just pointer relinking
```

C'est impossible avec `vector` (il faudrait une copie en O(n)). C'est la meilleure raison d'être de `std::list`.

### 6.2 Utilise `sort` et `merge` membres, pas les versions `std::`

```cpp
l.sort();                    // member — works
l.merge(other);              // member — assumes both sorted; merges in O(n+m)

std::sort(l.begin(), l.end()); // ERROR — bidirectional iterators reject random-access op
```

Pourquoi list a son propre tri : l'algorithme `std::sort` a besoin de `it + n` (accès direct). List ne peut pas le fournir sans parcourir. Le tri membre utilise un tri fusion, qui n'avance qu'avec `++` / `--`.

### 6.3 Ne calcule pas `size()` dans une boucle

En C++98 strict, certaines implémentations de `list` (libstdc++ avant 4.7 avec l'ancienne ABI) font un `size()` en O(n) — elles parcourent toute la liste pour compter. Donc :

```cpp
// Possibly O(n²) on old libstdc++
for (std::size_t i = 0; i < l.size(); ++i) { … }

// Always O(n)
for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it) { … }
```

libc++ et libstdc++ modernes stockent la taille dans l'en-tête de la liste (O(1)). Préfère quand même le parcours par itérateur aux boucles indexées sur une list.

### 6.4 Évite `list` pour des collections éphémères — vector gagne

`malloc` coûte cher — facilement 100× plus lent qu'incrémenter un pointeur. Pour mille `push_back`, `vector` (avec un `reserve`) fait une allocation ; `list` en fait mille. List est faite pour des collections **qui durent**, avec de fréquentes insertions/suppressions au milieu, pas pour accumuler vite.

### 6.5 `remove`/`remove_if`, c'est l'idiome erase–remove intégré

```cpp
l.remove(0);                 // erase every 0 — O(n)
l.remove_if(IsNegative());    // erase every negative — O(n)
```

Pour vector, tu écris `v.erase(std::remove(v.begin(), v.end(), 0), v.end())`. List l'intègre dans un appel membre.

---

## 7. Quand NE PAS utiliser list

| Si tu as besoin de…                                | Prends…                                        |
|----------------------------------------------------|------------------------------------------------|
| Accès direct par indice                            | `vector` ou `deque`                            |
| Parcourir rapidement des millions d'éléments       | `vector` (la localité de cache domine)         |
| Un simple LIFO / FIFO                              | `stack` / `queue`                              |
| Des clés uniques triées                            | `set`                                          |
| Une recherche clé → valeur                         | `map`                                          |

**Par défaut, `vector`.** Ne prends `list` que si tu as mesuré que l'insertion/la suppression au milieu est ton goulot d'étranglement, ou si tu as réellement besoin d'itérateurs durables à travers les modifications.

---

## 8. Limites du C++98

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `emplace` / `emplace_back` | C++11   | `insert` / `push_back` — paie une copie.    |
| `std::forward_list`    | C++11       | Utiliser `std::list` (nœuds un peu plus lourds). |
| Sémantique de déplacement | C++11    | Utiliser `splice` (qui déplace les nœuds gratuitement). |
| Constructeur par liste d'initialisation | C++11 | Constructeur par intervalle depuis un tableau C temporaire. |

---

## 9. Pièges

- **`std::sort(l.begin(), l.end())` ne compile pas.** Utilise `l.sort()`.
- **`size()` peut être en O(n)** sur certaines implémentations C++98. Utilise `empty()` quand tu veux juste savoir « y a-t-il quelque chose ? »
- **Pas d'`operator[]`, pas d'`at`.** Si tu te surprends à vouloir un accès direct sur une list, passe à `vector` ou `deque`.
- **Chaque nœud est une allocation séparée sur le tas** — une list de 1M d'ints, c'est ~24 Mo de surcoût en plus des 4 Mo de données. Les défauts de cache dominent le coût du parcours.
- **Un `splice` depuis une autre liste n'invalide pas les itérateurs déplacés** — ils continuent de fonctionner dans la nouvelle liste. C'est la garantie d'itérateur la plus forte de toute la STL.
- **`erase(it)` renvoie l'itérateur suivant**, comme pour vector. Le motif « effacer dans une boucle » est le même : `it = l.erase(it);`.

---

## 10. Exemple complet — splice gratuit

```cpp
#include <list>
#include <iostream>

int main(void)
{
    std::list<int> hot;       // recently used
    std::list<int> cold;      // archived

    for (int i = 0; i < 5; ++i)
        hot.push_back(i);     // hot = [0, 1, 2, 3, 4]

    // "Archive" the front element — move one node from hot to cold.
    cold.splice(cold.end(), hot, hot.begin());
    // hot = [1, 2, 3, 4], cold = [0]
    // ZERO allocations: the node was unlinked from hot and linked into cold.

    // Print both
    for (std::list<int>::const_iterator it = hot.begin(); it != hot.end(); ++it)
        std::cout << *it << " ";
    std::cout << "| ";
    for (std::list<int>::const_iterator it = cold.begin(); it != cold.end(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";        // 1 2 3 4 | 0
    return 0;
}
```

Compile avec les flags 42 :

```
c++ -Wall -Wextra -Werror -Wswitch -std=c++98 main.cpp -o demo
valgrind --leak-check=full ./demo
```

Chaque `push_back` est un `malloc` ; le destructeur de `std::list` suit la chaîne et `free` chaque nœud. Valgrind rapporte zéro fuite.

---

> **En une ligne.** `std::list`, c'est le `t_list` de la libft avec des pointeurs `prev` et une valeur en template. Prends-la pour des **itérateurs durables à travers des insertions/suppressions au milieu**, ou pour **transférer des nœuds entre listes en O(1) avec `splice`**. Sinon, vector gagne.
