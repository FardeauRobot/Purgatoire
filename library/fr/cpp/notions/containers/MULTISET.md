# `std::multiset` — Clés triées avec doublons

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/MULTISET.md)

> **TL;DR.** `std::multiset<T>`, c'est `std::set<T>` sans la contrainte d'unicité. Même arbre rouge-noir, mêmes opérations en O(log n), même stabilité des itérateurs — mais **insérer une clé déjà présente ajoute un doublon** au lieu de ne rien faire. Deux conséquences : `insert` renvoie juste un `iterator` (pas de `bool`), et `erase(key)` retire **tous les éléments correspondants**, pas un seul.

Voir aussi : [`SET.md`](SET.md) · [`MULTIMAP.md`](MULTIMAP.md) · [`MAP.md`](MAP.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <set>` *(oui — le même en-tête que `std::set`)*

---

## 1. Modèle mental

Même arbre que set, mais un nœud peut avoir des voisins de clé égale :

```
   multiset<int> après l'insertion de 5, 3, 8, 5, 4, 5

                    5
                   / \
                3    5
                 \    \
                  4    8
                       /
                      5

   Le parcours infixe donne : 3 4 5 5 5 8
   Remarque : trois 5, tous conservés.
```

Les éléments égaux sont stockés **dans un ordre stable** (l'ordre d'insertion) depuis le C++11. En C++98, l'ordre entre éléments égaux est **défini par l'implémentation** — en général l'ordre d'insertion avec libstdc++/libc++, mais ne compte pas dessus.

---

## 2. Les deux différences d'API avec `set`

### 2.1 `insert(x)` renvoie juste un `iterator`

```cpp
std::set<int>      s; std::pair<std::set<int>::iterator, bool>  r1 = s.insert(5);
std::multiset<int> m; std::multiset<int>::iterator              r2 = m.insert(5);
```

Il n'y a pas de booléen à renvoyer — l'insertion réussit toujours (même s'il existe des doublons). L'itérateur pointe sur le nœud nouvellement inséré.

### 2.2 `erase(key)` retire **tous** les éléments correspondants

```cpp
std::multiset<int> m;
m.insert(5); m.insert(5); m.insert(5);
std::cout << m.erase(5) << "\n";      // 3 — erased all three
std::cout << m.size()    << "\n";      // 0
```

Si tu veux en retirer **un seul**, utilise `erase(iterator)` après `find` :

```cpp
std::multiset<int>::iterator it = m.find(5);
if (it != m.end())
    m.erase(it);                        // erases just that one node
```

C'est le tueur silencieux quand on passe de `set` à `multiset`. `m.erase(5)` sur un multiset `{5, 5, 5, 7}` laisse `{7}`, pas `{5, 5, 7}`.

---

## 3. Ici, `count` et `equal_range` servent vraiment

Pour `set`, `count(key)` vaut toujours 0 ou 1. Pour `multiset`, c'est « combien de fois cette clé » — et `equal_range(key)` te donne la paire d'itérateurs pour parcourir uniquement celles-là :

```cpp
std::multiset<int> m;
m.insert(1); m.insert(2); m.insert(2); m.insert(2); m.insert(3);

std::cout << m.count(2) << "\n";               // 3

std::pair<std::multiset<int>::iterator, std::multiset<int>::iterator>
    r = m.equal_range(2);
for (std::multiset<int>::iterator it = r.first; it != r.second; ++it)
    std::cout << *it << " ";                    // 2 2 2
std::cout << "\n";
```

`equal_range(k)` n'est que `(lower_bound(k), upper_bound(k))` emballés ensemble. Pour `multiset`, il te donne la suite contiguë des clés égales en O(log n + m).

---

## 4. API essentielle (les différences avec set en **gras**)

| Méthode                   | Renvoie / comportement                                        | Coût       |
|---------------------------|---------------------------------------------------------------|------------|
| `insert(x)`               | **`iterator` (pas de bool — réussit toujours).**              | O(log n)   |
| `insert(it, x)`           | Pareil, avec indice.                                          | O(1) amorti si l'indice est bon |
| `insert(first, last)`     | Insère tout un intervalle.                                    | O(m log n) |
| `erase(it)`               | Retire un élément.                                            | O(1) amorti |
| `erase(key)`              | **Retire TOUS les correspondants. Renvoie le nombre effacé.** | O(log n + m) où m = nombre de correspondants |
| `find(key)`               | Itérateur vers **un** élément correspondant (n'importe lequel). | O(log n)   |
| `count(key)`              | **Nombre de correspondants (peut être > 1).**                 | O(log n + m) |
| `lower_bound(key)`        | Premier ≥ `key`.                                              | O(log n)   |
| `upper_bound(key)`        | Premier > `key`.                                              | O(log n)   |
| `equal_range(key)`        | **Paire d'itérateurs sur la suite de clés égales.**           | O(log n)   |
| Autres opérations         | Comme `set`.                                                  | Pareil     |

---

## 5. Invalidation des itérateurs — comme set

Garanties identiques : `insert` n'invalide rien ; `erase(it)` n'invalide que `it`. Tu peux garder des itérateurs à travers les modifications.

---

## 6. Astuces d'efficacité

### 6.1 Pour effacer un seul correspondant, utilise `find` + `erase(iterator)`

```cpp
// WRONG if you only want to remove one — erases all matches
m.erase(5);

// RIGHT — erase just one
std::multiset<int>::iterator it = m.find(5);
if (it != m.end()) m.erase(it);
```

### 6.2 `equal_range` bat deux appels séparés à `lower_bound`/`upper_bound`

Même complexité, mais `equal_range` fait les deux parcours en une seule descente dans l'arbre :

```cpp
std::pair<It, It> r = m.equal_range(key);     // one descent
// vs
It lo = m.lower_bound(key);
It hi = m.upper_bound(key);                    // two descents
```

### 6.3 Si tu veux surtout « toutes les valeurs de la clé K », envisage une autre forme

Une `std::map<Key, std::vector<Value> >` est souvent plus claire et plus rapide qu'une `multimap` (la cousine de `multiset`). Pour du simple comptage, une `std::map<Key, int>` (un histogramme) fait encore mieux.

### 6.4 La forme avec indice pour une insertion en masse de données triées

Même astuce que pour set : passe un indice correct pour transformer une insertion en masse en O(n log n) en O(n).

---

## 7. Quand NE PAS utiliser multiset

| Si tu as besoin de…                                | Prends…                                             |
|----------------------------------------------------|-----------------------------------------------------|
| Compter des occurrences                            | `std::map<Key, int>` — plus clair, même complexité. |
| Grouper des valeurs par clé                        | `std::map<Key, std::vector<Value> >`.               |
| Une recherche rapide sans ordre                    | (`unordered_multiset` du C++11 — interdit en C++98) |
| Garder l'ordre d'insertion                         | Un `std::vector` de paires, trié ensuite.           |

---

## 8. Limites du C++98

Comme pour `set` :

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_multiset` | C++11    | Utiliser `std::multiset`.                   |
| `emplace`              | C++11       | `insert` — paie une copie.                  |
| Sémantique de déplacement | C++11    | Passer par `const &`.                       |
| Constructeur par liste d'initialisation | C++11 | Constructeur par intervalle depuis un tableau C temporaire. |

---

## 9. Pièges

- **`erase(key)` retire TOUS les correspondants.** C'est la source de bugs n°1 du multiset. Utilise `erase(find(key))` pour n'en retirer qu'un.
- **`insert` renvoie `iterator`, pas `pair<iterator, bool>`.** Du code qui suppose `.second` ne compilera plus en passant de `set` à `multiset`.
- **`find(key)` renvoie *un* correspondant, pas un en particulier.** Si tu as besoin d'une position précise, utilise `equal_range` et parcours.
- **L'ordre entre clés égales est défini par l'implémentation en C++98.** Ne dépends pas de l'ordre d'insertion — cette garantie date du C++11.
- **Même coût d'une allocation par nœud que set.** Le surcoût mémoire pour beaucoup de petites clés est important.
- **Les clés sont immuables à travers les itérateurs** — comme pour set.

---

## 10. Exemple complet — regrouper les anagrammes

```cpp
#include <set>
#include <string>
#include <algorithm>
#include <iostream>

int main(void)
{
    // multiset stores all words; sorted by signature (sorted letters).
    // Custom comparator sorts by canonical form so anagrams cluster.
    struct ByCanonical {
        static std::string canonical(const std::string &s) {
            std::string c = s;
            std::sort(c.begin(), c.end());
            return c;
        }
        bool operator()(const std::string &a, const std::string &b) const {
            return canonical(a) < canonical(b);
        }
    };

    std::multiset<std::string, ByCanonical> bag;
    const char *words[] = {"listen", "silent", "enlist", "google", "gogole", "hello"};
    bag.insert(words, words + 6);

    // All anagrams of "listen" land in equal_range:
    std::pair<std::multiset<std::string, ByCanonical>::iterator,
              std::multiset<std::string, ByCanonical>::iterator>
        r = bag.equal_range("listen");

    for (; r.first != r.second; ++r.first)
        std::cout << *r.first << " ";
    std::cout << "\n";        // listen silent enlist  (anagrams of "listen")
    return 0;
}
```

Le comparateur personnalisé trie par « signature de lettres », donc les anagrammes sont équivalentes et se regroupent dans l'arbre. `equal_range` parcourt ensuite le groupe.

---

> **En une ligne.** `std::multiset`, c'est `set` avec les doublons autorisés. Deux pièges d'API : `insert` renvoie juste un itérateur, et `erase(key)` retire **tous** les correspondants. `count`, `equal_range` et `lower_bound`/`upper_bound` sont les outils du quotidien.
