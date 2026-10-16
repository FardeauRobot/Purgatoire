# `std::multimap` — Clé → valeur triée avec clés en double

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/MULTIMAP.md)

> **TL;DR.** `std::multimap<K, V>`, c'est `std::map<K, V>` sans la contrainte d'unicité. Même arbre rouge-noir de `pair<const K, V>`, mêmes opérations en O(log n) — mais **une même clé peut apparaître plusieurs fois**, chacune pointant vers un `V` (éventuellement différent). Deux conséquences : il n'y a **pas d'`operator[]`** (quelle entrée renverrait-il ?), et `equal_range(k)` devient l'outil quotidien pour « donne-moi toutes les valeurs associées à `k` ».

Voir aussi : [`MAP.md`](MAP.md) · [`MULTISET.md`](MULTISET.md) · [`SET.md`](SET.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <map>` *(oui — le même en-tête que `std::map`)*

---

## 1. Modèle mental

Même arbre que `map`, mais plusieurs nœuds peuvent partager une clé :

```
   multimap<std::string, int> après l'insertion de :
     ("cat", 1), ("dog", 4), ("cat", 2), ("cat", 5), ("ant", 9)

                   ("cat", 1)
                   /          \
            ("ant", 9)    ("cat", 2)
                                \
                            ("cat", 5)
                                  \
                              ("dog", 4)

   Parcours infixe : ("ant", 9) ("cat", 1) ("cat", 2) ("cat", 5) ("dog", 4)
```

Toutes les entrées `("cat", *)` se regroupent dans le parcours infixe. C'est exactement ce que renvoie `equal_range("cat")`.

---

## 2. Les différences avec `map`

### 2.1 Pas d'`operator[]`

```cpp
std::map<std::string, int>      m;  m["cat"] = 5;     // OK
std::multimap<std::string, int> mm; mm["cat"] = 5;    // ERROR — no operator[]
```

Impossible de définir `mm[k]` de façon sensée quand plusieurs entrées `("cat", *)` existent. Utilise `insert` à la place.

### 2.2 `insert` renvoie juste un `iterator`

```cpp
std::map<K,V>::iterator       r1; std::pair<r1, bool> = m.insert(...);
std::multimap<K,V>::iterator  r2 =                      mm.insert(...);
```

Comme pour `set` → `multiset` : l'insertion réussit toujours, pas de booléen à renvoyer.

### 2.3 `erase(key)` retire TOUTES les entrées correspondantes

```cpp
mm.insert(std::make_pair("cat", 1));
mm.insert(std::make_pair("cat", 2));
mm.insert(std::make_pair("cat", 5));
std::cout << mm.erase("cat") << "\n";    // 3
```

Exactement comme `multiset`. Pour ne retirer qu'une entrée précise, utilise `erase(iterator)` :

```cpp
std::multimap<std::string, int>::iterator it = mm.find("cat");
if (it != mm.end()) mm.erase(it);        // removes only one
```

`find(k)` renvoie un itérateur vers **une** entrée correspondante — en général la première dans l'ordre de l'arbre, mais le standard ne garantit pas laquelle quand il y en a plusieurs.

---

## 3. `equal_range` est l'outil quotidien

Toute la raison d'utiliser une multimap : « donne-moi chaque valeur associée à la clé K ».

```cpp
std::pair<std::multimap<std::string, int>::iterator,
          std::multimap<std::string, int>::iterator>
    r = mm.equal_range("cat");

for (std::multimap<std::string, int>::iterator it = r.first;
     it != r.second; ++it)
    std::cout << it->first << " → " << it->second << "\n";
// cat → 1
// cat → 2
// cat → 5
```

`equal_range(k)`, c'est `(lower_bound(k), upper_bound(k))` en une seule descente dans l'arbre. Coût : O(log n) plus O(m) pour parcourir les m correspondants.

---

## 4. API essentielle (les différences avec map en **gras**)

| Méthode                   | Renvoie / comportement                                        | Coût       |
|---------------------------|---------------------------------------------------------------|------------|
| `insert(pair)`            | **`iterator` (pas de bool — réussit toujours).**              | O(log n)   |
| `insert(it, pair)`        | Pareil, avec indice.                                          | O(1) amorti si l'indice est bon |
| `insert(first, last)`     | Insère tout un intervalle.                                    | O(m log n) |
| `erase(it)`               | Retire une entrée.                                            | O(1) amorti |
| `erase(key)`              | **Retire TOUS les correspondants. Renvoie le nombre effacé.** | O(log n + m) |
| `find(key)`               | Itérateur vers **une** entrée correspondante.                 | O(log n)   |
| `count(key)`              | **Nombre d'entrées avec cette clé (peut être > 1).**          | O(log n + m) |
| `equal_range(key)`        | **Paire d'itérateurs sur la suite de clés égales — l'outil quotidien.** | O(log n)   |
| `lower_bound(key)`        | Première entrée de clé ≥ celle donnée.                        | O(log n)   |
| `upper_bound(key)`        | Première entrée de clé > celle donnée.                        | O(log n)   |
| **`operator[]`**          | **N'existe pas.**                                             | —          |
| **`at(k)`**               | **N'existe pas** (et `at` est du C++11 de toute façon).       | —          |
| Autres opérations         | Comme `map`.                                                  | Pareil     |

---

## 5. Invalidation des itérateurs — comme map / set

`insert` n'invalide rien ; `erase(it)` n'invalide que `it`. Les itérateurs vers les autres entrées restent stables à travers les modifications. Même arbre rouge-noir.

---

## 6. Astuces d'efficacité

### 6.1 Utilise `equal_range` pour parcourir le groupe d'une clé

Une seule descente dans l'arbre pour les deux bornes — un peu moins cher que `lower_bound(k)` + `upper_bound(k)` séparément.

### 6.2 Choisir entre `multimap<K, V>` et `map<K, vector<V> >`

Ce sont deux formes différentes pour la même idée. Compromis :

| Forme                          | Avantages                                       | Inconvénients                              |
|--------------------------------|-------------------------------------------------|--------------------------------------------|
| `multimap<K, V>`               | Insertions en O(log n) ; chaque entrée est stable. | Parcourir toutes les valeurs exige `equal_range`. Mémoire : un nœud d'arbre par paire. |
| `map<K, std::vector<V> >`      | Toutes les valeurs d'une clé sont dans un seul vector contigu — parcours favorable au cache. Intention plus claire. | Chaque nouvelle valeur peut déclencher une réallocation du vector. |

Pour « beaucoup de clés, peu de valeurs par clé » → `multimap`. Pour « peu de clés, beaucoup de valeurs par clé » → `map<K, vector<V> >`. Les deux marchent ; la seconde est en général plus claire en relecture de code.

### 6.3 Pour effacer une paire (clé, valeur) précise, parcours `equal_range`

```cpp
std::pair<It, It> r = mm.equal_range("cat");
for (It it = r.first; it != r.second; ) {
    if (it->second == 5) {
        mm.erase(it++);                 // post-increment trick — see §10
    } else {
        ++it;
    }
}
```

`erase(key)` effacerait tous les `("cat", *)`. Parcourir equal_range permet de viser une valeur précise.

### 6.4 Passe par `const &`

Comme pour tous les autres conteneurs associatifs.

---

## 7. Quand NE PAS utiliser multimap

| Si tu as besoin de…                                | Prends…                                        |
|----------------------------------------------------|------------------------------------------------|
| Des clés uniques                                   | `map`                                          |
| La syntaxe `m[k]`                                  | `map`                                          |
| Regrouper par clé quand chaque groupe est gros     | `map<K, vector<V> >`                           |
| Compter (clé → nombre)                             | `map<K, int>`                                  |
| Beaucoup de clés, une ou peu de valeurs par clé    | `map`                                          |

Si tu peux exprimer ton problème sous forme de `map<K, vector<V> >`, c'est presque toujours plus propre. Réserve `multimap` aux cas où la stabilité de chaque nœud dans l'arbre compte (par ex. tu distribues des itérateurs à longue durée de vie vers des entrées individuelles).

---

## 8. Limites du C++98

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_multimap` | C++11    | Utiliser `std::multimap`.                   |
| `emplace`              | C++11       | `insert(make_pair(k, v))` — paie une copie. |
| Constructeur par liste d'initialisation | C++11 | Construire un tableau de paires, utiliser le constructeur par intervalle. |
| Sémantique de déplacement | C++11    | Passer par `const &`.                       |

---

## 9. Pièges

- **Pas d'`operator[]`.** `mm[k]` est une erreur de compilation. Utilise `insert(make_pair(k, v))`.
- **`erase(key)` retire TOUTES les entrées correspondantes.** Utilise `erase(iterator)` après `find` pour n'en retirer qu'une — et même là, `find` renvoie *une* correspondance, pas une en particulier.
- **`insert` renvoie juste un `iterator`.** Du code qui attend `pair<iterator, bool>` (style set/map) ne compilera pas.
- **`find(k)` ne te dit pas quelle correspondance il a renvoyée.** Si tu as besoin d'un (clé, valeur) précis, utilise `equal_range` et cherche.
- **L'ordre entre doublons est défini par l'implémentation en C++98** (l'ordre d'insertion est une garantie du C++11).
- **Chaque entrée est une allocation séparée sur le tas.** Pour « beaucoup de valeurs par clé », `map<K, vector<V> >` est en général plus économe en mémoire.
- **`erase(it)` renvoie `void` en C++98** (le C++11 renvoie l'itérateur suivant). Utilise l'idiome du post-incrément :
  ```cpp
  for (it = mm.begin(); it != mm.end(); ) {
      if (should_erase(it))
          mm.erase(it++);
      else
          ++it;
  }
  ```

---

## 10. Exemple complet — étudiant → cours

```cpp
#include <map>
#include <string>
#include <iostream>

int main(void)
{
    std::multimap<std::string, std::string> enrolled;
    enrolled.insert(std::make_pair("alice", "algebra"));
    enrolled.insert(std::make_pair("alice", "history"));
    enrolled.insert(std::make_pair("alice", "biology"));
    enrolled.insert(std::make_pair("bob",   "algebra"));
    enrolled.insert(std::make_pair("bob",   "physics"));

    // All courses Alice is in
    std::pair<std::multimap<std::string, std::string>::iterator,
              std::multimap<std::string, std::string>::iterator>
        r = enrolled.equal_range("alice");

    std::cout << "Alice: ";
    for (std::multimap<std::string, std::string>::iterator it = r.first;
         it != r.second; ++it)
        std::cout << it->second << " ";
    std::cout << "\n";        // algebra biology history  (sorted by key, then in tree order)

    // How many courses does Bob take?
    std::cout << "Bob count: " << enrolled.count("bob") << "\n";    // 2

    // Drop Alice from biology
    for (std::multimap<std::string, std::string>::iterator it = enrolled.lower_bound("alice");
         it != enrolled.upper_bound("alice"); ) {
        if (it->second == "biology")
            enrolled.erase(it++);          // post-increment idiom
        else
            ++it;
    }

    std::cout << "After drop, Alice count: " << enrolled.count("alice") << "\n";  // 2
    return 0;
}
```

`equal_range` parcourt les cours d'un étudiant ; `count` répond à « combien ». Le motif d'effacement ciblé pendant le parcours montre la forme sûre en C++98.

---

> **En une ligne.** `std::multimap`, c'est `map` avec les clés en double autorisées. Pas d'`operator[]`, `insert` renvoie juste un itérateur, `erase(key)` retire tous les correspondants, et `equal_range` est l'outil quotidien. Souvent, `map<K, vector<V> >` est une alternative plus claire.
