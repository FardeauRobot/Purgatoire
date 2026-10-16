# `std::vector` — Le conteneur séquentiel par défaut

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/VECTOR.md)

> **TL;DR.** Un `vector<T>` est un **tableau contigu redimensionnable dynamiquement** de `T`. Il stocke trois pointeurs — `begin`, `end`, `end_of_storage` — et grandit en se réallouant dans un bloc plus grand (en général ×2) quand il manque de place. C'est le bon choix **par défaut** ; ne prends `list`, `deque`, `set` ou `map` que si tu as une raison mesurable.

Voir aussi : [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`TEMPLATES.md`](../../../../en/cpp/notions/advanced/TEMPLATES.md) (EN) · [`MEMORY.md`](../../../../en/cpp/notions/fundamentals/MEMORY.md) (EN) · [`NEW.md`](../../../../en/cpp/lexique/NEW.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <vector>`

---

## Table des matières

1. [Modèle mental — ce qu'est vraiment un vector](#1.%20Mod%C3%A8le%20mental%20%E2%80%94%20ce%20qu%27est%20vraiment%20un%20vector)
2. [Construction](#2.%20Construction)
3. [Capacité — `size` vs `capacity`](#3.%20Capacit%C3%A9%20%E2%80%94%20size%20vs%20capacity)
4. [Accès aux éléments](#4.%20Acc%C3%A8s%20aux%20%C3%A9l%C3%A9ments)
5. [Modificateurs](#5.%20Modificateurs)
6. [Itérateurs](#6.%20It%C3%A9rateurs)
7. [Aide-mémoire des complexités](#7.%20Aide-m%C3%A9moire%20des%20complexit%C3%A9s)
8. [Invalidation des itérateurs — le tueur silencieux](#8.%20Invalidation%20des%20it%C3%A9rateurs%20%E2%80%94%20le%20tueur%20silencieux)
9. [Efficacité : dix astuces](#9.%20Efficacit%C3%A9%20%3A%20dix%20astuces)
10. [Algorithmes qui vont bien avec vector](#10.%20Algorithmes%20qui%20vont%20bien%20avec%20vector)
11. [`vector<bool>` — l'exception maudite](#11.%20vector%3Cbool%3E%20%E2%80%94%20l%27exception%20maudite)
12. [Quand NE PAS utiliser vector](#12.%20Quand%20NE%20PAS%20utiliser%20vector)
13. [Limites du C++98](#13.%20Limites%20du%20C%2B%2B98)
14. [Pièges](#14.%20Pi%C3%A8ges)

---

## 1. Modèle mental — ce qu'est vraiment un vector

Un `vector<T>` est en gros une minuscule struct qui contient trois pointeurs (ou un pointeur + deux tailles — ça dépend des implémentations) :

```
   struct vector_internals {
       T *begin_;         // premier élément
       T *end_;            // une case après le dernier élément
       T *end_of_storage_; // une case après le buffer alloué
   };
```

En image :

```
   vector<int> v avec size = 4, capacity = 8

      begin_              end_                end_of_storage_
       │                   │                   │
       v                   v                   v
     ┌───┬───┬───┬───┬───┬───┬───┬───┐
     │ 3 │ 1 │ 4 │ 1 │ . │ . │ . │ . │   ← alloué sur le tas, contigu
     └───┴───┴───┴───┴───┴───┴───┴───┘
       0   1   2   3
       └── size = 4 ──┘
       └────── capacity = 8 ──────┘
```

Tout l'intérêt : **les éléments vivent dans un seul bloc contigu sur le tas**, exactement comme le tableau C `new T[N]` que tu as utilisé en CPP01 — mais avec une croissance automatique. Parce que c'est contigu, tu obtiens :

- Un accès direct en O(1) (`v[i]` = `*(begin_ + i)`).
- Une compatibilité maximale avec le cache CPU (le préchargeur adore la mémoire séquentielle).
- Un pointeur brut vers les données via `&v[0]` — utilisable avec n'importe quelle API C qui attend un `T *`.

Quand tu dépasses `capacity`, le vector :
1. Alloue un **nouveau buffer, plus grand** (en général 2× l'ancienne capacité avec libc++ / libstdc++).
2. **Copie** (ou construit par copie) chaque élément dans le nouveau buffer.
3. Détruit les originaux et libère l'ancien buffer.
4. Met à jour les trois pointeurs.

C'est cette réallocation qui fait que `push_back` est en O(1) *amorti*, pas strictement O(1). 99 % des push ne font qu'avancer un pointeur ; 1 % font une copie complète. La stratégie de doublement garantit que le coût moyen par push reste constant.

### Si tu viens du C — c'est un tableau `realloc`-é, **pas** une liste chaînée

L'intuition « vector = un mélange de tableau C et de liste chaînée » est tentante parce que les deux grandissent à la demande. Elle est trompeuse. Un `std::vector<T>` est essentiellement un tableau C `realloc`-é enveloppé dans une classe — pas de nœuds, pas de pointeurs `next`, rien qui ressemble à une liste.

Si tu en écrivais un en C avec les conventions de la libft, ça ressemblerait presque exactement à ça :

```c
typedef struct s_vec {
    int     *data;       // begin_     — heap buffer
    size_t  size;        // logical count
    size_t  capacity;    // physical count
}   t_vec;

void    vec_push_back(t_vec *v, int x)
{
    if (v->size == v->capacity)
    {
        v->capacity = v->capacity ? v->capacity * 2 : 1;
        v->data = realloc(v->data, v->capacity * sizeof(int));
        // ↑ realloc may MOVE the buffer to a new address.
        //   Any pointer you held into the old buffer now dangles.
    }
    v->data[v->size++] = x;
}
```

Voilà `std::vector<int>` en douze lignes de C. Chaque `push_back` est un `realloc + affectation`. Chaque `&v[0]` est `v->data`. Chaque `v[i]` est `v->data[i]`.

Le *mécanisme* de croissance est l'inverse d'une liste chaînée : une liste grandit en allouant **un nouveau nœud** et en pointant dessus ; un vector grandit en allouant **tout un nouveau buffer plus grand** et en y recopiant tout.

| Propriété                                         | Tableau C          | `t_list *` de la libft      | `std::vector`                          |
| ------------------------------------------------- | ------------------ | --------------------------- | -------------------------------------- |
| Disposition mémoire                               | Contiguë           | Nœuds éparpillés            | **Contiguë** ← comme un tableau        |
| Accès direct `v[i]`                               | O(1)               | O(n)                        | **O(1)** ← comme un tableau            |
| Compatibilité cache                               | Excellente         | Mauvaise (chaque nœud = un défaut de cache) | **Excellente** ← comme un tableau |
| Peut grandir à la demande ?                       | Non                | Oui                         | **Oui** ← comme une liste              |
| Insertion **à la fin**                            | N/A                | O(1)                        | O(1) amorti                            |
| Insertion **au milieu**                           | N/A                | O(1) (on rechaîne)          | **O(n) — il faut décaler** ← comme un tableau |
| Un pointeur vers un élément reste valide pour toujours ? | Oui (jusqu'au `free`) | Oui (jusqu'à la libération du nœud) | **Non — invalidé à la croissance** |

Vector se comporte comme un tableau sur toutes les colonnes sauf « peut grandir ». Et cette unique propriété façon liste chaînée est elle-même implémentée à la manière d'un tableau (via `realloc`), pas d'une liste (via un chaînage de nœuds).

Si tu veux vraiment l'équivalent d'une liste chaînée, c'est [`std::list`](LIST.md) — le même `t_list *` que tu as écrit dans la libft, juste en template et avec des pointeurs `prev` en plus. Le vrai « meilleur des deux mondes », c'est **`std::deque`** : une liste de tableaux de taille fixe. Accès direct en O(1), push/pop aux deux bouts en O(1), mais pas contigu dans son ensemble :

```
   std::deque<int> en interne (conceptuellement)
   ┌──────────────┐
   │ block_ptrs[] │ → [bloc A] [bloc B] [bloc C] [bloc D]
   └──────────────┘    (4 ints) (4 ints) (4 ints) (4 ints)
```

> **Modèle mental :** `std::vector<T>` est un tableau C sur le tas qui connaît sa `size` et sa `capacity` et sait se `realloc` lui-même. La disposition contiguë le rend rapide. La croissance automatique est un confort construit par-dessus. Chaque piège du §8 découle de ce seul fait.

---

## 2. Construction

Le C++98 te donne ces constructeurs (et l'affectation par copie) :

```cpp
#include <vector>

std::vector<int> a;                            // empty,   size=0, capacity=0
std::vector<int> b(5);                         // 5 elems, value-initialized → 0
std::vector<int> c(5, 42);                     // 5 elems all == 42
std::vector<int> d(c);                         // copy of c
std::vector<int> e(c.begin(), c.end());        // range copy
std::vector<int> f(arr, arr + 10);             // from a C array

a = c;                                          // copy-assign — clears a, copies c's elements
a.assign(5, 7);                                 // replace contents with 5 sevens
a.assign(c.begin(), c.end());                   // replace contents with c's range
```

> **C++11+ uniquement (interdit en Mod08) :** `std::vector<int> v = {1, 2, 3};` — constructeur par liste d'initialisation entre accolades.
> En C++98, tu construis d'abord un petit tableau puis tu utilises le constructeur par intervalle : `int tmp[] = {1,2,3}; std::vector<int> v(tmp, tmp + 3);`.

---

## 3. Capacité — `size` vs `capacity`

C'est *le* concept que la plupart des débutants ratent.

| Fonction       | Signification                                                                   |
|----------------|---------------------------------------------------------------------------------|
| `size()`       | Combien d'éléments tu as, logiquement.                                          |
| `capacity()`   | Combien d'éléments tiendraient avant que le buffer doive grandir.               |
| `empty()`      | `size() == 0`. Utilise-le — plus clair et jamais faux.                          |
| `max_size()`   | Le maximum théorique — en général `2^31` ou `2^63`. Inutile en pratique.        |
| `reserve(n)`   | Si `capacity < n`, réalloue pour au moins `n`. **Ne change pas la taille.**     |
| `resize(n)`    | Force `size == n`. Construit par défaut les nouveaux éléments, détruit l'excédent. |
| `resize(n, x)` | Pareil, mais remplit les nouveaux éléments avec `x`.                            |

```cpp
std::vector<int> v;
std::cout << v.size() << " " << v.capacity() << "\n";  // 0 0
v.reserve(100);
std::cout << v.size() << " " << v.capacity() << "\n";  // 0 100 (size unchanged!)
v.push_back(42);
std::cout << v.size() << " " << v.capacity() << "\n";  // 1 100 (no realloc — capacity was enough)
v.resize(5, 0);
std::cout << v.size() << " " << v.capacity() << "\n";  // 5 100 (added four zeros)
```

**Règle :** `reserve` sert la performance — il évite des réallocations. `resize` sert la *sémantique* — il crée / détruit réellement des éléments.

---

## 4. Accès aux éléments

| Forme        | Vérifie les bornes ?  | Renvoie                              |
|--------------|-----------------------|--------------------------------------|
| `v[i]`       | **Non.** UB si `i ≥ size`. | `T&` (ou `const T&` sur un vector const) |
| `v.at(i)`    | Oui — lance `std::out_of_range`. | Pareil |
| `v.front()`  | Non — UB si vide.     | Le premier élément.                   |
| `v.back()`   | Non — UB si vide.     | Le dernier élément.                   |
| `&v[0]`      | Non — UB si vide.     | `T *` vers le buffer sous-jacent. La porte de sortie vers le tableau C. |

```cpp
std::vector<int> v(3, 0);
v[0] = 10;
v.at(1) = 20;       // safe but slower (one branch per access)
int x = v.front();  // 10
int y = v.back();   // 0
int *raw = &v[0];   // pass to a C API: read(fd, raw, sizeof(int) * v.size())
```

> **C++11+ :** `v.data()` renvoie le pointeur brut même quand le vector est vide (une valeur convertible en null). En C++98, `&v[0]` sur un vector vide est un **UB** — vérifie toujours `!v.empty()` d'abord.

---

## 5. Modificateurs

### 5.1 Ajouter / retirer à la fin — le chemin rapide

```cpp
v.push_back(x);          // amortized O(1) — the workhorse
v.pop_back();            // O(1) — destroys the last element, decrements size
v.clear();               // O(n) — destroys all elements, size = 0, capacity UNCHANGED
```

**`clear()` ne libère pas la mémoire.** Il ne fait que détruire les éléments. Si tu dois rendre le buffer, voir l'[astuce du swap](#9.%20Efficacit%C3%A9%20%3A%20dix%20astuces).

### 5.2 Insérer / effacer au milieu — le chemin lent

```cpp
std::vector<int>::iterator it = v.begin() + 2;
v.insert(it, 99);                       // insert 99 at index 2, shift the rest right — O(n)
v.insert(it, 3, 99);                    // insert three 99s — O(n)
v.insert(it, src.begin(), src.end());   // insert a range — O(n + m)

it = v.begin() + 2;
it = v.erase(it);                       // erase one element, shift the rest left — O(n)
                                         // returns iterator to the element AFTER the erased one
v.erase(v.begin() + 1, v.begin() + 4);  // erase a range — O(n)
```

Chaque insertion / suppression au milieu **décale tous les éléments situés après la modification**. Ça fait O(n) déplacements mémoire. Fais-le dans une boucle et tu as créé du O(n²).

### 5.3 Swap

```cpp
std::vector<int> a, b;
a.swap(b);              // O(1) — only swaps the three internal pointers
std::swap(a, b);        // same, specialised by the standard
```

Le swap est en temps constant *et* ne lance jamais d'exception. C'est la base de plusieurs idiomes d'efficacité.

### 5.4 Assign

```cpp
v.assign(5, 0);                // replace contents with five 0s
v.assign(src.begin(), src.end());
```

Équivalent à `clear()` suivi de `insert(end(), …)`, mais l'implémentation peut réutiliser le stockage existant.

---

## 6. Itérateurs

Chaque vector expose quatre types d'itérateurs :

```cpp
std::vector<int>::iterator               it;
std::vector<int>::const_iterator         cit;
std::vector<int>::reverse_iterator       rit;
std::vector<int>::const_reverse_iterator crit;
```

Obtenus via :

| Méthode           | Renvoie                                            |
|-------------------|----------------------------------------------------|
| `begin() / end()` | iterator (ou const_iterator sur un vector const)   |
| `rbegin() / rend()` | reverse_iterator (parcourt `back()` → `front()`) |

Les itérateurs de vector sont à **accès direct** (*random-access*) : tu peux faire `it + 5`, `it2 - it1`, `it[3]`, `it < it2`. C'est pour ça que `std::sort` (qui exige l'accès direct) marche sur les vectors mais pas sur `std::list`.

```cpp
for (std::vector<int>::const_iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
}
```

**Le C++98 n'a pas de `for` sur intervalle** (`for (int x : v)`). Tu écris la boucle explicite, ou tu utilises `std::for_each`.

---

## 7. Aide-mémoire des complexités

| Opération                       | Coût                | Remarque                                   |
|---------------------------------|---------------------|--------------------------------------------|
| `v[i]`, `v.at(i)`               | O(1)                | `at` est un peu plus lent (un test).       |
| `v.front()`, `v.back()`         | O(1)                |                                            |
| `v.push_back(x)`                | O(1) **amorti**     | O(n) occasionnel à la réallocation.        |
| `v.pop_back()`                  | O(1)                |                                            |
| `v.insert(end(), x)`            | O(1) amorti         | Comme `push_back`.                         |
| `v.insert(middle, x)`           | O(n)                | Décale.                                    |
| `v.erase(middle)`               | O(n)                | Décale.                                    |
| `v.clear()`                     | O(n)                | Les destructeurs s'exécutent ; capacité inchangée. |
| `v.size()`, `v.capacity()`, `v.empty()` | O(1)        |                                            |
| `v.reserve(n)`                  | O(n) s'il grandit, O(1) sinon | Coût ponctuel.                   |
| `v.resize(n)`                   | O(n)                |                                            |
| `v.swap(w)`                     | O(1)                | Échange juste trois pointeurs.             |
| Chercher une valeur (`std::find`) | O(n)              | Parcours linéaire.                         |
| `std::sort(v.begin(), v.end())` | O(n log n)          | Introsort en interne.                      |
| `std::lower_bound` sur v trié   | O(log n)            | Vector bat `std::set` ici pour un petit N. |

---

## 8. Invalidation des itérateurs — le tueur silencieux

Quand tu modifies un vector, **les itérateurs, pointeurs et références vers ses éléments peuvent devenir pendants** (*dangling*). Les déréférencer est un comportement indéfini — valgrind le détecte parfois, ASan le détecte de façon fiable, ton évaluateur peut-être pas.

| Opération                                           | Qu'est-ce qui meurt ?                                         |
|-----------------------------------------------------|---------------------------------------------------------------|
| `push_back` / `insert` **sans** réallocation        | Les itérateurs **au niveau ou après** le point d'insertion.   |
| `push_back` / `insert` **avec** réallocation        | **Tous** les itérateurs, pointeurs, références.               |
| `pop_back`                                          | `end()` et l'itérateur/la référence vers le dernier élément retiré. |
| `erase(it)`                                         | `it` et tout ce qui le suit.                                  |
| `clear()`                                           | Tous les itérateurs et références (mais `end()` reste valide comme nouvelle fin). |
| `reserve(n)` avec `n > capacity`                    | Tous les itérateurs, pointeurs, références.                   |
| `resize(n)` qui dépasse la capacité                 | Tous les itérateurs, pointeurs, références.                   |
| `swap(other)`                                       | Aucun des tiens — mais `end()` change de sens.                |

**Le bug classique — un pointeur vers un élément gardé pendant un `push_back` :**

```cpp
std::vector<int> v;
v.push_back(10);
int *p = &v[0];          // points into v's buffer
v.push_back(20);         // if capacity was 1, buffer moves — p now dangles
*p = 99;                 // UB
```

La correction : utilise un indice, pas un pointeur. Ou fais un `reserve` suffisant au départ pour qu'aucune réallocation n'ait lieu pendant la durée de vie de `p`.

**L'autre classique — effacer dans une boucle :**

```cpp
// WRONG — erase returns the next valid iterator; you must use it.
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if (*it == 0)
        v.erase(it);     // it is now invalid; ++it is UB
}

// RIGHT — let erase advance you.
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ) {
    if (*it == 0)
        it = v.erase(it);
    else
        ++it;
}

// BEST — erase–remove idiom (see §10).
v.erase(std::remove(v.begin(), v.end(), 0), v.end());
```

---

## 9. Efficacité : dix astuces

### 9.1 `reserve()` avant un remplissage en masse

```cpp
std::vector<int> v;
v.reserve(1000);              // one allocation
for (int i = 0; i < 1000; ++i)
    v.push_back(i);
```

Sans `reserve`, le vector se réalloue ~10 fois pour 1000 éléments, en recopiant chaque élément à chaque fois. Avec `reserve`, tu as exactement une allocation. Sur des types non triviaux, ça peut faire un gain de 5 à 10×.

### 9.2 Passe par **`const &`** (ou `&` si tu modifies)

```cpp
void print(const std::vector<int> &v);    // GOOD — no copy
void print(std::vector<int> v);            // BAD — copies the entire buffer
```

`vector` est un type valeur. Le passer par valeur copie chaque élément.

### 9.3 L'**astuce du swap** — rendre la capacité en trop (le substitut C++98 de `shrink_to_fit`)

```cpp
std::vector<int> v;
v.reserve(10000);
v.push_back(42);
// v.size() == 1, v.capacity() == 10000 — wasteful

std::vector<int>(v).swap(v);
// builds a *new* vector with capacity == size, swaps it in,
// the old (oversized) buffer is destroyed at end of full-expression
```

Pour descendre jusqu'à une capacité nulle :

```cpp
std::vector<int>().swap(v);   // v is now empty AND has freed its buffer
```

### 9.4 Préfère `push_back` et `pop_back` aux insertions / suppressions au milieu

Si tu peux reformuler ton algorithme pour ne toucher qu'à la fin du vector, tu passes de O(n) par opération à O(1) par opération. Parfois un tri + un passage linéaire est une meilleure forme que « recherche + suppression au milieu ».

### 9.5 L'**idiome erase–remove** — supprimer toutes les occurrences en O(n)

```cpp
v.erase(std::remove(v.begin(), v.end(), value), v.end());
```

Ce qui se passe :
- `std::remove` n'efface rien en réalité — il tasse les éléments « conservés » vers l'avant et renvoie un itérateur vers la nouvelle fin logique.
- `v.erase(new_end, v.end())` coupe ensuite la queue en O(1) par élément restant.

Total : O(n) au lieu de O(n²) pour « effacer dans une boucle ».

Même idée avec `std::remove_if` et un foncteur :

```cpp
struct IsNegative {
    bool operator()(int x) const { return x < 0; }
};
v.erase(std::remove_if(v.begin(), v.end(), IsNegative()), v.end());
```

### 9.6 Trier une fois, puis `lower_bound` — bat `std::set` pour un petit N

Si tu construis une collection puis l'interroges souvent sans nouvelles insertions :

```cpp
std::sort(v.begin(), v.end());
std::vector<int>::iterator it = std::lower_bound(v.begin(), v.end(), key);
bool found = (it != v.end() && *it == key);
```

Tu obtiens une recherche en O(log n) avec la localité de cache du vector. `std::set` est aussi en O(log n), mais chaque nœud est une allocation séparée sur le tas — vector gagne pour `n < ~1000` dans les benchmarks réels.

### 9.7 Construire dans une variable locale, puis `swap` vers la destination

```cpp
void rebuild(std::vector<int> &target) {
    std::vector<int> tmp;
    tmp.reserve(target.size());
    // … fill tmp …
    target.swap(tmp);                     // O(1); old target's elements die with tmp
}
```

Garantie forte d'exception gratuite — si la construction de `tmp` lance, `target` n'est pas touché.

### 9.8 Utilise des indices, pas des itérateurs, quand une réallocation peut survenir

```cpp
for (std::size_t i = 0; i < v.size(); ++i) {
    if (some_condition(v[i]))
        v.push_back(derived(v[i]));       // safe: i still indexes correctly after realloc
}
```

Une version à base d'itérateurs de cette boucle pendouillerait dès la première réallocation.

### 9.9 `clear()` ≠ libérer. Utilise `swap` pour vraiment rendre la mémoire.

```cpp
v.clear();                                 // size = 0, capacity unchanged — buffer still held
std::vector<int>().swap(v);                // size = 0, capacity = 0 — buffer freed
```

### 9.10 Construis directement avec le constructeur (nombre, valeur) ou par intervalle

```cpp
std::vector<int> v(1000, 0);               // one allocation, 1000 zeroes
// vs
std::vector<int> v;
for (int i = 0; i < 1000; ++i) v.push_back(0);   // multiple reallocs, slower
```

Les constructeurs de la bibliothèque standard connaissent la taille finale dès le départ — ils peuvent pré-allouer une seule fois.

---

## 10. Algorithmes qui vont bien avec vector

Ils vivent tous dans `<algorithm>`. Ils travaillent sur des itérateurs, donc ils sont indépendants du conteneur, mais les itérateurs contigus à accès direct du vector sont les moins chers possibles — chaque algorithme tourne à pleine vitesse.

| Algorithme                     | Usage                                                        |
|--------------------------------|--------------------------------------------------------------|
| `std::sort`                    | Introsort en O(n log n). Exige des itérateurs à accès direct. |
| `std::stable_sort`             | Comme `sort` mais préserve l'ordre des éléments égaux.       |
| `std::find` / `std::find_if`   | Recherche linéaire. Renvoie `end()` si rien n'est trouvé.    |
| `std::count` / `std::count_if` | Combien correspondent.                                       |
| `std::remove` / `std::remove_if` | À associer à `erase` — voir §9.5.                          |
| `std::unique`                  | Supprime les doublons **adjacents**. Trie d'abord si tu veux l'unicité globale. |
| `std::reverse`                 | Inversion sur place.                                         |
| `std::lower_bound` / `upper_bound` | Recherche dichotomique sur un vector trié.               |
| `std::binary_search`           | Renvoie juste un bool.                                       |
| `std::accumulate` (dans `<numeric>`) | Somme / pliage (*fold*).                               |
| `std::copy` / `std::copy_backward` | Copier des intervalles entre itérateurs.                 |
| `std::fill` / `std::fill_n`    | Écrire une valeur sur tout un intervalle.                    |
| `std::min_element` / `std::max_element` | Itérateurs vers le plus petit / le plus grand.      |
| `std::nth_element`             | Tri partiel — garantit seulement que le n-ième élément est à sa place. O(n). |

Exemple idiomatique — trier et dédoublonner :

```cpp
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
```

---

## 11. `vector<bool>` — l'exception maudite

`std::vector<bool>` n'est **pas** un vrai vector de `bool`. C'est une spécialisation compactée en bits — chaque élément prend 1 bit, pas `sizeof(bool)` octets. Conséquences :

- `&v[0]` n'est **pas** un `bool *` — impossible de le passer à une API C.
- `v[i]` renvoie une **référence proxy**, pas un `bool &`. `bool &b = v[i];` ne compile pas.
- Les itérateurs ont des types surprenants ; du code générique échoue à l'instanciation.

**Contournements :**

- `std::vector<char>` si tu veux un tableau de booléens normal, un octet par élément.
- `std::vector<int>` si tu le veux encore plus gros et stable en valeur.
- `std::deque<bool>` si tu veux vraiment un vrai conteneur de `bool`.

C'est largement considéré comme la plus grosse erreur de la STL. Évite `vector<bool>` sauf si tu as spécifiquement besoin du gain de place et que tu as vérifié chaque utilisation.

---

## 12. Quand NE PAS utiliser vector

Vector est le choix par défaut. N'en prends un autre que si tu as une raison *précise* :

| Si tu as besoin de…                                    | Prends…                                        |
|--------------------------------------------------------|------------------------------------------------|
| Beaucoup d'insertions/suppressions **au milieu**       | `std::list` (splice / insertion à un itérateur en O(1)) |
| Push/pop rapides **aux deux bouts** + accès direct     | `std::deque`                                   |
| Stabilité des itérateurs/références lors des insertions | `std::list`, `std::deque` (début/fin seulement), ou `std::map` |
| Recherche clé → valeur en O(log n)                     | `std::map`                                     |
| Clés uniques, ordonnées                                | `std::set`                                     |
| LIFO / FIFO avec interface restreinte                  | `std::stack`, `std::queue`                     |
| Un buffer de taille fixe, qui ne grandit jamais        | Tableau C ou membre `T[N]`                     |

Pour les exercices de CPP09 (`MutantStack`, `BitcoinExchange`, `RPN`) : vector gagne en général pour RPN (push/pop à la fin seulement), map gagne pour BitcoinExchange (recherche de date triée avec `lower_bound`).

---

## 13. Limites du C++98

Ce que tu **n'as pas**, et que le code récent utilise :

| Fonctionnalité         | Version C++ | Alternative C++98                                |
|------------------------|-------------|--------------------------------------------------|
| `auto`                 | C++11       | Écrire le type de l'itérateur en toutes lettres. |
| `for` sur intervalle   | C++11       | Boucle sur indice ou boucle explicite sur itérateur. |
| `emplace_back`         | C++11       | `push_back` — paie une copie mais fonctionne.    |
| `shrink_to_fit()`      | C++11       | L'**astuce du swap** (§9.3).                     |
| Membre `data()`        | C++11       | `&v[0]` (UB si vide — protéger avec `!empty()`). |
| Constructeur par liste d'initialisation `{1,2,3}` | C++11 | Construire d'abord un tableau C, puis construire par intervalle. |
| Sémantique de déplacement (`vector<X>` de types non copiables) | C++11 | Stocker des `X *` et gérer la durée de vie à la main. |
| `cbegin` / `cend`      | C++11       | `const_iterator` depuis un `const vector&`.      |
| `std::vector` depuis une `initializer_list` de paires | C++11 | Construire avec `std::make_pair` et `push_back`. |

Les flags 42 `-std=c++98 -Werror` refusent net toutes les formes C++11. Tant mieux — ça t'oblige à apprendre ce que chacune fait vraiment sous le capot.

---

## 14. Pièges

- **`v.size() - 1` boucle à l'envers (*underflow*) quand `v` est vide.** `size()` renvoie un `size_t` (non signé). `0u - 1u == SIZE_MAX`. Vérifie toujours `!v.empty()` avant de calculer `size() - 1`.
- **Mélange signé/non signé dans les boucles.** `for (int i = 0; i < v.size(); ++i)` déclenche un warning avec `-Wall` parce que `i` est un `int` et `v.size()` un `size_t`. Utilise `std::size_t` ou caste délibérément.
- **`v[i]` ne vérifie pas les bornes.** En build debug, libc++/libstdc++ le font parfois ; en release, non. Utilise `at` pendant le développement, `[]` dans les boucles serrées.
- **Un vector d'objets n'est *pas* un vector de pointeurs.** `std::vector<MyClass> v; v.push_back(MyClass());` appelle le constructeur de copie. Si `MyClass` coûte cher à copier, stocke plutôt des `MyClass *` — et pense à `delete` chaque pointeur à la main (pas de pointeurs intelligents en C++98 hormis `std::auto_ptr`, qui est cassé pour un usage dans des conteneurs).
- **`v == w` compare élément par élément** et utilise l'`operator==` de `T`. Assure-toi que ta classe en a un si tu compares des vectors.
- **Le facteur de croissance de la capacité dépend de l'implémentation.** N'écris pas de code qui suppose exactement ×2 (libc++ utilise 2, libstdc++ utilise 2, MSVC utilise 1,5).
- **`.front()` / `.back()` sur un vector vide est un UB**, pas une exception. Ils ne vérifient pas les bornes. Toujours `if (!v.empty())` d'abord.
- **L'auto-insertion est un UB.** `v.insert(v.end(), v.begin(), v.end())` peut corrompre les données, car l'insertion peut réallouer et invalider les itérateurs source en pleine copie. Fais d'abord une copie.

---

## Exemple complet — les idiomes 42 classiques en un programme

```cpp
#include <vector>
#include <algorithm>
#include <iostream>

int main(void)
{
    std::vector<int> v;
    v.reserve(10);                                  // §9.1

    for (int i = 0; i < 10; ++i)
        v.push_back(i % 4);

    std::sort(v.begin(), v.end());                  // §10

    v.erase(std::unique(v.begin(), v.end()),        // §10 — unique-sort
            v.end());

    v.erase(std::remove(v.begin(), v.end(), 2),     // §9.5 — erase–remove
            v.end());

    for (std::vector<int>::const_iterator it = v.begin();
         it != v.end(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";

    std::vector<int>().swap(v);                     // §9.3 — release memory
    return 0;
}
```

Compile avec les flags 42 :

```
c++ -Wall -Wextra -Werror -Wswitch -std=c++98 main.cpp -o demo
valgrind --leak-check=full ./demo
```

Si tu utilises correctement reserve, push_back, sort et erase–remove, valgrind rapporte zéro fuite et zéro lecture invalide. C'est l'objectif.

---

> **En une ligne.** `std::vector`, c'est un `new T[]` qui sait grandir. Fais un `reserve` au départ, un swap pour rétrécir, ne garde jamais un pointeur à travers un `push_back`, et utilise erase–remove au lieu d'effacer dans une boucle. Tout le reste n'est que note de bas de page.
