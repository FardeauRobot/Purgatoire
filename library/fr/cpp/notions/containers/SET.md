# `std::set` — Clés uniques triées

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/SET.md)

> **TL;DR.** `std::set<T>` est un conteneur associatif **trié, à clés seules**. Implémentation : un **arbre rouge-noir**. Chaque opération (insertion, suppression, recherche) est en O(log n). Les itérateurs sont **bidirectionnels, stables, et parcourent l'arbre dans l'ordre trié**. Les clés sont *immuables* à travers les itérateurs — modifier une clé sur place casserait l'invariant de tri. Pour un petit N ou une charge « écrit une fois, lu souvent », un `vector` trié + `lower_bound` bat souvent `set`.

Voir aussi : [`MULTISET.md`](MULTISET.md) · [`MAP.md`](MAP.md) · [`VECTOR.md`](VECTOR.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <set>`

---

## 1. Modèle mental

Un arbre rouge-noir est un **arbre binaire de recherche auto-équilibré** — tous les chemins de la racine à une feuille ont à peu près la même longueur, donc chaque opération est en O(log n) dans le pire cas (pas seulement en moyenne).

```
   set<int> après l'insertion de 5, 3, 8, 1, 4, 7, 9

                    5 (N)
                   / \
                3 (R) 8 (R)
                / \   / \
            1(N) 4(N) 7(N) 9(N)

   ── N = noir, R = rouge. Les règles de couleur maintiennent l'arbre équilibré.
   ── Le parcours infixe (gauche → racine → droite) donne : 1 3 4 5 7 8 9
```

Ce que ça implique pour toi :
- **Chaque élément est une allocation séparée sur le tas** (un nœud d'arbre par clé).
- **Les itérateurs parcourent l'arbre dans l'ordre trié** — `for (it = s.begin(); it != s.end(); ++it)` visite 1, 3, 4, 5, 7, 8, 9 dans cet ordre, quel que soit l'ordre d'insertion.
- **Pas de doublons.** Insérer une clé déjà présente ne fait rien (et `insert` renvoie `(iterator, false)` pour te le dire).
- **Les clés sont triées avec `operator<` par défaut** (ou avec un comparateur que tu fournis).

---

## 2. Construction

```cpp
#include <set>
#include <functional>          // for std::greater

std::set<int> a;                                    // empty, default-sorted ascending
std::set<int> b(arr, arr + 10);                     // from a range — auto-deduplicates and sorts
std::set<int> c(b);                                  // copy

std::set<int, std::greater<int> > d;                // sorted descending

// Custom comparator
struct ByLength {
    bool operator()(const std::string &a, const std::string &b) const {
        return a.size() < b.size();
    }
};
std::set<std::string, ByLength> e;
```

**Le comparateur doit être un ordre faible strict** (*strict weak ordering*). `comp(a,b)` et `comp(b,a)` ne peuvent pas être vrais tous les deux. Si `!comp(a,b)` et `!comp(b,a)`, `set` les considère équivalents — ce qui veut dire **qu'au plus un des deux vit dans le set**.

---

## 3. API essentielle

| Méthode                   | Ce qu'elle fait                                             | Coût       |
|---------------------------|-------------------------------------------------------------|------------|
| `insert(x)`               | Insère si absent. Renvoie `pair<iterator, bool>`.           | O(log n)   |
| `insert(it, x)`           | Pareil, avec un itérateur indice (proche du point d'insertion). | O(1) amorti si l'indice est bon |
| `insert(first, last)`     | Insère tout un intervalle.                                  | O(m log n) |
| `erase(it)`               | Retire l'élément pointé.                                    | O(1) amorti |
| `erase(key)`              | Retire l'élément égal à `key`. Renvoie 1 si retiré, 0 si absent. | O(log n) |
| `erase(first, last)`      | Retire un intervalle.                                       | O(log n + m) |
| `find(key)`               | Renvoie un itérateur, ou `end()` si introuvable.            | O(log n)   |
| `count(key)`              | 0 ou 1 pour `set` (clés uniques). Préfère `find` si possible — même coût, sens plus clair. | O(log n) |
| `lower_bound(key)`        | Premier élément ≥ `key`.                                    | O(log n)   |
| `upper_bound(key)`        | Premier élément > `key`.                                    | O(log n)   |
| `equal_range(key)`        | `pair(lower_bound, upper_bound)`. Pour `set`, l'intervalle contient 0 ou 1 élément. | O(log n) |
| `clear()`                 | Détruit chaque nœud. O(n).                                  | O(n)       |
| `size()` / `empty()`      | O(1).                                                       | O(1)       |
| `begin()` / `end()` / `rbegin()` / `rend()` | Itérateurs bidirectionnels dans l'ordre trié. | O(1)       |

**Pas d'`operator[]`, pas d'`at`, pas d'accès direct.** Le set est trié par *valeur*, pas indexé par position.

---

## 4. Le retour `pair<iterator, bool>` d'`insert`

C'est l'outil de base :

```cpp
std::set<int> s;
std::pair<std::set<int>::iterator, bool> r = s.insert(42);
if (r.second)
    std::cout << "newly inserted\n";
else
    std::cout << "was already present\n";
// either way, r.first points to the element with value 42
```

**Pourquoi une paire et pas juste un bool ?** Parce que te donner l'itérateur ne coûte rien de plus (l'arbre vient de descendre jusque-là pour décider), et que tu veux souvent faire tout de suite quelque chose avec l'élément inséré/existant.

---

## 5. Stabilité des itérateurs — le super-pouvoir discret de set

| Opération             | Itérateurs / références invalidés ?                             |
|-----------------------|-----------------------------------------------------------------|
| `insert(x)`           | **Aucun.** Tous les itérateurs existants restent valides.       |
| `erase(it)`           | Seulement `it` lui-même.                                        |
| `erase(key)`          | Seulement l'itérateur vers cette clé (si elle existait).        |
| `clear()`             | Tous les itérateurs (chaque nœud est libéré).                   |
| `swap(other)`         | Aucun.                                                          |

C'est une garantie plus forte que celle de vector : dans un `set`, tu peux garder un itérateur à travers **autant d'insertions que tu veux**, il continue de pointer sur le même élément. Les nœuds de l'arbre ne bougent jamais — ils sont juste rechaînés.

---

## 6. Les clés sont immuables à travers les itérateurs

```cpp
std::set<int>::iterator it = s.find(5);
// *it = 99;       // ERROR — *it is `const int &`
                    // (libstdc++/libc++ make it `const T` by spec)
```

Si tu pouvais modifier la clé, tu violerais l'ordre de tri — l'élément serait au mauvais endroit dans l'arbre, et find/insert donneraient de mauvaises réponses. Le standard l'empêche en ne te donnant qu'un accès `const` à travers les itérateurs.

Pour « modifier » un élément d'un set : `erase` le, puis `insert` la nouvelle valeur.

---

## 7. Astuces d'efficacité

### 7.1 Utilise `lower_bound` / `upper_bound` pour les requêtes par intervalle

```cpp
// Find all elements in [10, 20]
std::set<int>::iterator it = s.lower_bound(10);
std::set<int>::iterator end = s.upper_bound(20);
for (; it != end; ++it)
    std::cout << *it << " ";
```

`find(key) == end()` vérifie « est-ce que exactement cette valeur est présente » ; `lower_bound(key)` répond à « où irait-elle » — utile aussi pour « insérer avec indice ».

### 7.2 Utilise l'`insert` avec indice pour une insertion en masse de données triées

```cpp
std::set<int>::iterator hint = s.end();
for (std::size_t i = 0; i < sorted_data.size(); ++i)
    hint = s.insert(hint, sorted_data[i]);   // amortized O(1) if hint is correct
```

Pour des données dont on sait qu'elles sont déjà triées, ça transforme une insertion en masse en `n log n` en environ O(n).

### 7.3 Si tu construis une fois et interroges souvent — le vector trié gagne

```cpp
std::vector<int> v(arr, arr + n);
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
// ...
bool found = std::binary_search(v.begin(), v.end(), key);
```

Même recherche en O(log n), mais **une seule allocation** contre n allocations, et **une mémoire contiguë** où les succès de cache dominent. Pour n < ~1000, ça bat souvent `std::set` de 3 à 10× en mesures réelles.

Utilise `std::set` quand insertions et recherches s'entremêlent ; utilise un vector trié quand la phase de construction se termine avant que la phase de requêtes commence.

### 7.4 `count(key)` vs `find(key)` — préfère `find`

Les deux sont en O(log n). `find` est sémantiquement plus clair et te donne l'itérateur. N'utilise `count` que si tu veux littéralement juste le booléen (et même là, `find(k) != end()` se lit mieux).

### 7.5 Passe par `const &`

Copier un set copie chaque nœud — n allocations. Passe toujours par référence.

---

## 8. Quand NE PAS utiliser set

| Si tu as besoin de…                                | Prends…                                        |
|----------------------------------------------------|------------------------------------------------|
| Clés en double                                     | `multiset`                                     |
| Une association clé → valeur                       | `map`                                          |
| Une recherche en O(1) (et l'ordre t'est égal)      | `unordered_set` du C++11+ — **interdit en C++98 / à 42** |
| Un accès trié, mais écrit une fois / lu souvent    | `vector` trié + `binary_search`                |
| Un parcours dans l'ordre d'insertion               | `vector` (dédoublonner à la main si besoin)    |

---

## 9. Limites du C++98

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_set`   | C++11       | Utiliser `std::set` (O(log n) au lieu de O(1) en moyenne). |
| `emplace` / `emplace_hint` | C++11   | `insert` — paie une copie.                  |
| Sémantique de déplacement | C++11    | Passer par `const &`.                       |
| Constructeur par liste d'initialisation | C++11 | Constructeur par intervalle depuis un tableau C temporaire. |
| Comparateurs en lambda | C++11       | Struct foncteur avec `operator()`.          |

---

## 10. Pièges

- **Les clés sont `const` à travers les itérateurs.** Impossible de faire `*it = x;`. Erase + insert si tu dois modifier.
- **Le comparateur doit être un ordre faible strict.** Renvoyer true à la fois pour `comp(a,b)` et `comp(b,a)` est un UB.
- **Deux clés sont « équivalentes » si aucune n'est `<` à l'autre** — pas forcément égales au sens d'`operator==`. Avec un comparateur comme `ByLength`, `"abc"` et `"def"` sont équivalentes et seule la première insérée survit.
- **`insert` renvoie `pair<iterator, bool>`.** Oublier `.first` pour récupérer l'itérateur est un bug courant.
- **Chaque nœud est une allocation séparée sur le tas.** Des sets de millions de petites clés ont un énorme surcoût mémoire comparé à un vector trié.
- **`erase(key)` renvoie le nombre d'éléments effacés** (toujours 0 ou 1 pour set), pas un itérateur. `erase(it)` renvoie `void` en C++98 (il ne renvoie l'itérateur suivant qu'à partir du C++11). Pour effacer pendant un parcours, le motif sûr en C++98 est :
  ```cpp
  for (std::set<int>::iterator it = s.begin(); it != s.end(); ) {
      if (should_erase(*it)) {
          std::set<int>::iterator dead = it++;
          s.erase(dead);
      } else {
          ++it;
      }
  }
  ```

---

## 11. Exemple complet — mots uniques triés

```cpp
#include <set>
#include <string>
#include <iostream>

int main(void)
{
    const char *words[] = {"pear", "apple", "banana", "apple", "cherry", "pear"};
    std::set<std::string> unique_sorted(words, words + 6);

    for (std::set<std::string>::const_iterator it = unique_sorted.begin();
         it != unique_sorted.end(); ++it)
        std::cout << *it << "\n";
    // apple
    // banana
    // cherry
    // pear

    if (unique_sorted.find("apple") != unique_sorted.end())
        std::cout << "found apple\n";

    std::pair<std::set<std::string>::iterator, bool> r = unique_sorted.insert("date");
    std::cout << (r.second ? "added date\n" : "date was already there\n");
    return 0;
}
```

Construire le set depuis un intervalle (dédoublonne et trie automatiquement), le parcourir dans l'ordre trié, chercher par clé, observer le retour `pair<iterator, bool>`. Chaque opération est en O(log n).

---

> **En une ligne.** `std::set` est un conteneur trié, à clés uniques, en log n, adossé à un arbre rouge-noir. Les itérateurs restent valides à travers les insertions ; les clés sont immuables à travers eux. Pour des jeux de données petits ou construits une seule fois, un vector trié + binary_search est souvent plus rapide.
