# `std::map` — Recherche clé → valeur triée

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/MAP.md)

> **TL;DR.** `std::map<K, V>` est un **conteneur associatif trié** qui associe des `K` uniques à des `V`. Même arbre rouge-noir que `std::set`, mais chaque nœud stocke une `std::pair<const K, V>` au lieu d'une simple clé. Toutes les opérations sont en O(log n). Atout maître : les requêtes par intervalle via `lower_bound` (le moteur du BitcoinExchange de CPP09). Piège majeur : `m[k]` **insère silencieusement une valeur construite par défaut** quand `k` est absente — presque toujours un bug si tu voulais seulement chercher.

Voir aussi : [`SET.md`](SET.md) · [`MULTIMAP.md`](MULTIMAP.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <map>`

---

## 1. Modèle mental

Un arbre rouge-noir de nœuds `pair<const K, V>`, ordonnés par `K` :

```
   map<std::string, int>  après l'insertion de ("apple", 3), ("banana", 7), ("cherry", 2)

                  ("banana", 7)
                  /            \
       ("apple", 3)         ("cherry", 2)

   Le parcours infixe donne : ("apple", 3) ("banana", 7) ("cherry", 2)
   — trié par clé, quel que soit l'ordre d'insertion.
```

Le type des éléments est **`std::pair<const K, V>`** — note le `const` sur la clé. Même raisonnement que pour `set` : modifier la clé placerait le nœud au mauvais endroit dans l'arbre. La valeur `V`, elle, est librement modifiable.

```cpp
std::map<std::string, int>::iterator it = m.find("apple");
// it->first  is `const std::string &`  — read-only
// it->second is `int &`                 — modifiable
it->second = 99;        // OK
// it->first = "x";     // ERROR
```

---

## 2. Construction

```cpp
#include <map>

std::map<std::string, int> a;                              // empty

// From a range of pairs (one allocation per element)
std::pair<std::string, int> seed[] = {
    std::make_pair("apple", 3),
    std::make_pair("banana", 7)
};
std::map<std::string, int> b(seed, seed + 2);

std::map<std::string, int> c(b);                           // copy

// Custom comparator — must order by KEY only
std::map<std::string, int, std::greater<std::string> > d;  // descending key order
```

**Pas de constructeur par liste d'initialisation en C++98.** Construis un tableau temporaire et utilise le constructeur par intervalle.

---

## 3. Les quatre façons d'insérer

| Forme                                         | Renvoie                                | Quand l'utiliser                       |
|-----------------------------------------------|----------------------------------------|----------------------------------------|
| `m.insert(std::make_pair(k, v))`              | `pair<iterator, bool>` — le bool dit si c'était nouveau | Tu veux savoir « ai-je écrasé ou ajouté ? » |
| `m.insert(it, std::make_pair(k, v))`          | `iterator` — utilise l'indice          | Insertion en masse de paires presque triées. |
| `m[k] = v`                                    | `void` (enfin, une référence vers la case affectée) | Affectation rapide, **insère un `V` par défaut si `k` était absente.** |
| `m[k]`                                        | `V &`                                  | « Récupère ou construis par défaut. » Même piège d'insertion en cas d'absence. |

La première forme est la plus sûre. La forme crochets est pratique mais traîtresse — voir §5.

---

## 4. API essentielle

| Méthode                   | Renvoie / comportement                                        | Coût       |
|---------------------------|---------------------------------------------------------------|------------|
| `m[k]`                    | Référence vers la valeur de `k`. **Insère une valeur par défaut si absente.** | O(log n)   |
| `m.find(k)`               | `iterator` vers la paire, ou `end()` si absente. Recherche pure. | O(log n)   |
| `m.count(k)`              | 0 ou 1 (les clés d'une map sont uniques).                     | O(log n)   |
| `m.insert(pair)`          | `pair<iterator, bool>`. `bool == false` signifie que la clé était déjà là (pas d'écrasement). | O(log n) |
| `m.erase(it)`             | Retire. Renvoie void en C++98.                                | O(1) amorti |
| `m.erase(k)`              | Retire si présente. Renvoie le nombre effacé (0 ou 1).        | O(log n)   |
| `m.lower_bound(k)`        | Premier élément dont la clé est ≥ `k`.                        | O(log n)   |
| `m.upper_bound(k)`        | Premier élément dont la clé est > `k`.                        | O(log n)   |
| `m.equal_range(k)`        | Paire `(lower_bound, upper_bound)`. L'intervalle contient 0 ou 1 élément. | O(log n)   |
| `m.size()` / `m.empty()`  |                                                                | O(1)       |
| `m.clear()`               | Libère chaque nœud.                                           | O(n)       |
| `m.begin()` / `end()` / `rbegin()` / `rend()` | Itérateurs bidirectionnels dans l'ordre trié par clé. | O(1)       |

> **`at(k)` est du C++11.** En C++98, la recherche pure la plus proche est `find(k)` — et tu dois comparer avec `end()`.

---

## 5. Le piège de `m[k]` qui insère en cas d'absence

```cpp
std::map<std::string, int> counts;
counts["apple"] += 1;             // counts = {"apple" → 1}
counts["apple"] += 1;             // counts = {"apple" → 2}
                                   // perfect for histograms.

// But:
if (counts["banana"] == 0)         // ← THIS INSERTED ("banana", 0)
    std::cout << "no bananas\n";
std::cout << counts.size() << "\n";  // 2 — banana is now in the map
```

**Chaque accès `m[k]` crée l'entrée si elle n'existe pas.** C'est voulu — c'est ce qui fait marcher `counts[word]++` comme histogramme. Mais si tu voulais seulement *vérifier*, tu as pollué la map.

**Le motif de recherche pure :**

```cpp
std::map<std::string, int>::const_iterator it = counts.find("banana");
if (it == counts.end())
    std::cout << "no bananas\n";   // does NOT insert
else
    std::cout << it->second << "\n";
```

**Règle d'or :** `m[k]` pour *écrire* ; `m.find(k)` pour *lire*. Si tu inverses, tu inséreras des fantômes.

### Que fait vraiment `m[k]` ?

```cpp
V& operator[](const K &k) {
    iterator it = lower_bound(k);
    if (it == end() || comp(k, it->first))
        it = insert(it, std::make_pair(k, V()));   // V's default ctor
    return it->second;
}
```

Deux leçons :
- `V` **doit être constructible par défaut** pour utiliser `m[k]`. Si `V` n'a pas de constructeur par défaut, la forme crochets ne compile pas.
- Chaque recherche `m[k]` coûte une descente dans l'arbre — comme `find`. Ne refais pas deux fois les crochets sur la même clé si le coût t'inquiète.

---

## 6. Invalidation des itérateurs — comme set

| Opération             | Itérateurs / références invalidés ?              |
|-----------------------|--------------------------------------------------|
| `insert`              | Aucun.                                           |
| `erase(it)`           | Seulement `it`.                                  |
| `erase(k)`            | Seulement l'itérateur vers `k` (si elle était là). |
| `clear()`             | Tous.                                            |
| `swap(other)`         | Aucun.                                           |

Les itérateurs sont stables à travers les insertions. Même arbre rouge-noir que set.

---

## 7. Astuces d'efficacité

### 7.1 Utilise `lower_bound` pour « la clé précédente la plus proche » — le motif du BitcoinExchange

CPP09 ex00 demande : pour une date donnée, trouver la date la plus proche *qui ne lui est pas postérieure* dans une base de prix. C'est littéralement ce que donne `lower_bound` + un pas en arrière :

```cpp
std::map<std::string, double>::const_iterator it = prices.lower_bound(query_date);
if (it == prices.end() || it->first != query_date) {
    if (it == prices.begin())
        return /* no earlier date */;
    --it;                           // step back to the strictly-earlier date
}
return it->second;
```

Trié par clé + dichotomie en log n = idéal pour chercher dans une série temporelle.

### 7.2 Utilise `insert` (pas `[]`) quand tu veux « ajouter seulement si absente »

```cpp
// SAFE: leaves existing value untouched if "apple" already has one.
m.insert(std::make_pair("apple", 5));

// UNSAFE: overwrites silently
m["apple"] = 5;
```

### 7.3 Parcourir dans l'ordre des clés

L'ordre de parcours par défaut **est** trié par clé. Si c'est ce que tu veux, aucune étape de tri en plus. Si tu veux l'ordre d'insertion, il te faut un `vector<pair<K,V>>`.

### 7.4 Construire une fois + parcourir souvent → envisage un `vector<pair<K,V>>` trié

Même compromis que set contre vector trié. Si la map est construite une fois au démarrage puis interrogée souvent, un vector trié est plus rapide (une allocation, mémoire contiguë, `lower_bound` fonctionne aussi dessus).

### 7.5 Passe par `const &`

Copier une map coûte O(n) et alloue un nœud par élément. Toujours par référence.

---

## 8. Quand NE PAS utiliser map

| Si tu as besoin de…                                | Prends…                                        |
|----------------------------------------------------|------------------------------------------------|
| Clés en double                                     | `multimap`                                     |
| Une recherche en O(1) en moyenne, sans ordre       | `unordered_map` — **C++11, interdit dans les modules C++98 de 42** |
| Garder l'ordre d'insertion                         | `vector<pair<K,V>>`                            |
| Compter (histogramme)                              | `map<K, int>` convient — c'est l'usage canonique |
| Regrouper par clé                                  | `map<K, vector<V> >` est en général plus clair que `multimap` |

---

## 9. Limites du C++98

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_map`   | C++11       | Utiliser `std::map` (O(log n) au lieu de O(1) en moyenne). |
| `at(k)`                | C++11       | `find(k)` + vérifier `!= end()`.            |
| `emplace`              | C++11       | `insert(make_pair(k, v))` — paie une copie. |
| Constructeur par liste d'initialisation `{{a, 1}, {b, 2}}` | C++11 | Construire un tableau de paires, utiliser le constructeur par intervalle. |
| Sémantique de déplacement | C++11    | Passer par `const &`.                       |

---

## 10. Pièges

- **`m[k]` insère en cas d'absence.** Utilise `find(k)` pour une recherche pure.
- **Le type des éléments est `pair<const K, V>`.** Impossible de réaffecter une clé à travers un itérateur.
- **`V` doit être constructible par défaut** pour utiliser `operator[]`.
- **`erase(it)` renvoie `void` en C++98** (le C++11 renvoie l'itérateur suivant). L'idiome « effacer pendant le parcours » a besoin de l'astuce du post-incrément :
  ```cpp
  for (std::map<K, V>::iterator it = m.begin(); it != m.end(); ) {
      if (should_erase(it->first)) {
          std::map<K, V>::iterator dead = it++;
          m.erase(dead);
      } else {
          ++it;
      }
  }
  ```
- **`insert(make_pair(k, v))` n'écrase PAS** une entrée existante. Le booléen de la paire renvoyée te dit si l'insertion a eu lieu.
- **Chaque entrée est une allocation sur le tas.** Le coût mémoire est important pour beaucoup de petites entrées.
- **Les comparateurs personnalisés doivent définir un ordre faible strict sur la clé seule.** Ils ne voient jamais la valeur.

---

## 11. Exemple complet — histogramme de mots + requête par intervalle

```cpp
#include <map>
#include <string>
#include <iostream>

int main(void)
{
    std::map<std::string, int> counts;
    const char *words[] = {"the", "cat", "sat", "the", "mat", "the", "cat"};

    // Histogram via the bracket-insert pattern (this is what [] is good at)
    for (int i = 0; i < 7; ++i)
        counts[words[i]] += 1;

    // Iterate in sorted-by-key order
    for (std::map<std::string, int>::const_iterator it = counts.begin();
         it != counts.end(); ++it)
        std::cout << it->first << ": " << it->second << "\n";
    // cat: 2
    // mat: 1
    // sat: 1
    // the: 3

    // Pure lookup — DON'T use []
    std::map<std::string, int>::const_iterator q = counts.find("dog");
    std::cout << (q == counts.end() ? "no dog\n" : "found\n");
    std::cout << "size still: " << counts.size() << "\n";   // still 4

    // "Closest key not greater than" — the BitcoinExchange pattern
    std::map<std::string, int>::const_iterator lo = counts.lower_bound("mau");
    if (lo != counts.begin()) {
        if (lo == counts.end() || lo->first != "mau")
            --lo;
        std::cout << "nearest <= mau: " << lo->first << "\n";    // mat
    }
    return 0;
}
```

L'histogramme montre la force de la forme crochets. La « clé précédente la plus proche » montre celle de `lower_bound`. Les deux sont en O(log n) par opération.

---

> **En une ligne.** `std::map`, c'est `K → V` trié avec des opérations en O(log n). `m[k]` seulement pour *écrire* (il insère en cas d'absence) ; `find(k)` pour *lire*. `lower_bound` est le moteur des recherches dans les séries temporelles triées. Le type des éléments est `pair<const K, V>` — les clés sont immuables.
