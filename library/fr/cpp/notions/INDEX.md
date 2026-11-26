# 🧠 NOTIONS — Les concepts par thème

> 🇬🇧 [English version](../../../en/cpp/notions/INDEX.md)

La couche « vue d'ensemble » : chaque fiche explique un concept proprement — modèle mental, schémas, exemples commentés, pièges. Les mots-clés ont leurs pages de zoom dans [`../lexique/`](../../../en/cpp/lexique/INDEX.md) (EN) ; la lecture guidée par les exercices se trouve dans [`../projets/`](../projets/INDEX.md).

> 🌟 **Commence ici :** [`PHILOSOPHY.md`](../../../en/cpp/notions/PHILOSOPHY.md) (EN) — les 5 piliers du C++ et l'état d'esprit pour écrire du code correct. À relire avant chaque nouveau module.

---

## 🧱 `fundamentals/` — la couche de base

| Fiche | Sujet |
|---|---|
| [`BASICS.md`](../../../en/cpp/notions/fundamentals/BASICS.md) (EN) | Syntaxe de base, unité de compilation, namespaces — le passage du C au C++. |
| [`CONTRACTS.md`](../../../en/cpp/notions/fundamentals/CONTRACTS.md) (EN) | Le modèle mental derrière `const`, `private`, `explicit` : des promesses que le compilateur fait respecter. |
| [`REFERENCE.md`](../../../en/cpp/notions/fundamentals/REFERENCE.md) (EN) | Références contre pointeurs, sémantique des lvalues. |
| [`MEMORY.md`](../../../en/cpp/notions/fundamentals/MEMORY.md) (EN) | Pile contre tas, `new`/`delete`, RAII, durée de vie, valgrind. |
| [`ALLOCATION_FAILURE.md`](../../../en/cpp/notions/fundamentals/ALLOCATION_FAILURE.md) (EN) | Quand `new` échoue — `bad_alloc` contre NULL, les entrailles d'`operator new`. |
| [`STRING.md`](../../../en/cpp/notions/fundamentals/STRING.md) (EN) | `std::string` en profondeur — SSO contre COW, croissance, la passerelle `char*`. |
| [`CMATH.md`](../../../en/cpp/notions/fundamentals/CMATH.md) (EN) | `<cmath>` — puissances, racines, trigo, arrondis ; C++98 contre C++11. |
| [`C_LIBRARY.md`](fundamentals/C_LIBRARY.md) | `<cctype>`, `<cstdlib>`, `<cstring>` — `isdigit`, `strtol`, `strcmp`… et leurs pièges. |

## 🧬 `oop/` — l'orienté objet

| Fiche | Sujet |
|---|---|
| [`ORTHODOX_CANONICAL_FORM.md`](oop/ORTHODOX_CANONICAL_FORM.md) | Les trois grands + le constructeur par défaut. L'incontournable de 42. |
| [`GETTERS_SETTERS.md`](oop/GETTERS_SETTERS.md) | Les schémas d'encapsulation. |
| [`OPERATOR_OVERLOADING.md`](oop/OPERATOR_OVERLOADING.md) | Opérateurs membres ou libres, règle de symétrie. |
| [`INHERITANCE.md`](oop/INHERITANCE.md) | Héritage public/protected/private, diamants. |
| [`POLYMORPHISM.md`](oop/POLYMORPHISM.md) | Virtual, vtables, virtuelles pures, classes abstraites. |

## ⚙️ `advanced/` — le plus costaud

| Fiche | Sujet |
|---|---|
| [`TEMPLATES.md`](../../../en/cpp/notions/advanced/TEMPLATES.md) (EN) | Templates de fonctions et de classes, spécialisation. |
| [`STL.md`](../../../en/cpp/notions/advanced/STL.md) (EN) | Conteneurs + itérateurs + algorithmes — la page carrefour. |
| [`ITERATORS.md`](advanced/ITERATORS.md) | Les itérateurs en profondeur — catégories, traits, adaptateurs, invalidation, schémas de CPP08. |
| [`ALGORITHMS.md`](../../../en/cpp/notions/advanced/ALGORITHMS.md) (EN) | Référence exhaustive de `<algorithm>`/`<numeric>`, annotée C++98. |
| [`MEMBER_FUNCTION_POINTERS.md`](../../../en/cpp/notions/advanced/MEMBER_FUNCTION_POINTERS.md) (EN) | `.*` / `->*`, tables de dispatch, le schéma de Harl. |

*Les quatre casts ont déménagé dans le lexique : [`lexique/CASTS.md`](../../../en/cpp/lexique/CASTS.md) (EN).*

## 📦 `containers/` — une page par conteneur de la STL

Modèle mental + schéma ASCII + API complète + complexité + invalidation des itérateurs, pour chacun. **Carrefour : [`containers/INDEX.md`](containers/INDEX.md)** — `vector`, `list`, `deque`, `stack`, `queue`, `priority_queue`, `set`, `multiset`, `map`, `multimap`.

## 🌊 `io-errors/` — flux et échecs

| Fiche | Sujet |
|---|---|
| [`FSTREAM_GUIDE.md`](../../../en/cpp/notions/io-errors/FSTREAM_GUIDE.md) (EN) | `ifstream`/`ofstream`/`stringstream`. |
| [`OPEN.md`](../../../en/cpp/notions/io-errors/OPEN.md) (EN) | Drapeaux et modes d'ouverture de fichiers. |
| [`STRING_FUNCTIONS.md`](../../../en/cpp/notions/io-errors/STRING_FUNCTIONS.md) (EN) | Aide-mémoire de l'API `std::string` (en profondeur : [`fundamentals/STRING.md`](../../../en/cpp/notions/fundamentals/STRING.md) (EN)). |
| [`ERROR_MANAGEMENT.md`](../../../en/cpp/notions/io-errors/ERROR_MANAGEMENT.md) (EN) | Exceptions, `try`/`catch`, garanties de sûreté. |

## 🛠️ `tooling/` — compilation et édition de liens

| Fiche | Sujet |
|---|---|
| [`MAKEFILE_CPP.md`](../../../en/cpp/notions/tooling/MAKEFILE_CPP.md) (EN) | Recettes de Makefile C++, `-Wall -Wextra -Werror -std=c++98`. |
| [`LIBRARIES.md`](../../../en/cpp/notions/tooling/LIBRARIES.md) (EN) | Bibliothèques statiques contre partagées, ordre d'édition des liens. |

---

## 🧭 Ordres de lecture suggérés

- **Début de la piscine :** `PHILOSOPHY` → `fundamentals/BASICS` → `fundamentals/MEMORY` → `oop/ORTHODOX_CANONICAL_FORM`
- **L'héritage ne passe pas :** `oop/INHERITANCE` → `oop/POLYMORPHISM` → [`lexique/VIRTUAL.md`](../../../en/cpp/lexique/VIRTUAL.md) (EN)
- **Entrée dans Mod08 / les conteneurs :** `advanced/STL` → `advanced/ITERATORS` → `containers/INDEX` → `advanced/ALGORITHMS`
