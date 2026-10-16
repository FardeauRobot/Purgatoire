# `cpp/containers/` — Une page par conteneur de la STL

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/INDEX.md)

Fiches détaillées pour chaque conteneur (*container*) de la STL en C++98. C'est le point d'entrée ; pour la vue d'ensemble « à quoi sert la STL », voir [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN).

> Chaque page suit la même structure : TL;DR → modèle mental (avec schéma ASCII) → construction → API essentielle → tableau des complexités → invalidation des itérateurs → astuces d'efficacité → quand ne pas l'utiliser → limites du C++98 → pièges → exemple complet.

---

## Les 10 conteneurs, classés par famille

### Conteneurs séquentiels — *stockage indexé par position*

| Conteneur        | Fichier                  | En une ligne                                                                                  | Par défaut ? |
| ---------------- | ------------------------ | --------------------------------------------------------------------------------------------- | ------------ |
| `std::vector<T>` | [`VECTOR.md`](VECTOR.md) | Tableau dynamique contigu. Le choix par défaut. Croissance façon `realloc`, accès direct O(1). | ✅            |
| `std::list<T>`   | [`LIST.md`](LIST.md)     | Liste doublement chaînée de nœuds sur le tas. `splice` en O(1), itérateurs stables, pas d'accès direct. |              |
| `std::deque<T>`  | [`DEQUE.md`](DEQUE.md)   | Liste de blocs — O(1) aux deux extrémités *et* accès direct. Références stables lors d'un push. |              |

`std::string` (techniquement un conteneur séquentiel) est traité dans [`STRING_FUNCTIONS.md`](../../../../en/cpp/notions/io-errors/STRING_FUNCTIONS.md) (EN).

### Adaptateurs de conteneur (*container adapters*) — *interface restreinte posée sur un conteneur séquentiel*

| Conteneur | Fichier | En une ligne | Conteneur sous-jacent par défaut |
|---|---|---|---|
| `std::stack<T>` | [`STACK.md`](STACK.md) | LIFO. `push` / `top` / `pop`. Seul `pop()` renvoie `void` — lire avec `top()` d'abord. | `deque` |
| `std::queue<T>` | [`QUEUE.md`](QUEUE.md) | FIFO. Même forme que stack mais `front()` + `pop()`. Impossible de l'appuyer sur un vector. | `deque` |
| `std::priority_queue<T>` | [`PRIORITY_QUEUE.md`](PRIORITY_QUEUE.md) | Tas binaire max (*max-heap*). push/pop en O(log n), top en O(1). `std::greater<T>` pour un tas min. | `vector` |

### Conteneurs associatifs — *accès trié par clé (arbre rouge-noir)*

| Conteneur | Fichier | En une ligne | Doublons autorisés ? |
|---|---|---|---|
| `std::set<K>` | [`SET.md`](SET.md) | Clés uniques triées. insert/erase/find en O(log n). Itérateurs stables. | Non |
| `std::multiset<K>` | [`MULTISET.md`](MULTISET.md) | Comme `set` mais avec doublons. `equal_range` est l'outil du quotidien. | Oui |
| `std::map<K, V>` | [`MAP.md`](MAP.md) | `K → V` trié. `m[k]` insère si la clé manque (le piège). L'itérateur pointe sur un `pair<const K, V>`. | Non (clés) |
| `std::multimap<K, V>` | [`MULTIMAP.md`](MULTIMAP.md) | `K → V` trié avec clés en double. Pas d'`operator[]`. | Oui |

---

## Arbre de décision — « quel conteneur choisir ? »

```
   Besoin d'une recherche par clé ?
   ├── Oui → ordonné, log n, parcours trié par clé ?
   │         ├── Oui, avec une valeur ?   → map / multimap
   │         └── Oui, clé seule ?         → set / multiset
   └── Non → schéma d'accès ?
             ├── Push/pop à un seul bout ?            → stack (LIFO) ou queue (FIFO)
             ├── Toujours retirer le max ?            → priority_queue
             ├── Push/pop aux DEUX bouts + accès direct ? → deque
             ├── Beaucoup d'insertions/suppressions au milieu ? → list
             └── Sinon (le cas par défaut)            → vector
```

Dans le doute : **`std::vector`**. N'en choisis un autre que si tu as une raison mesurée.

---

## Thèmes transversaux couverts dans les fiches

Sujets qui reviennent d'un fichier à l'autre — chaque page traite la version propre à son conteneur, mais voici ce qu'il faut y chercher :

- **Règles d'invalidation des itérateurs** — chaque conteneur a les siennes ; celles de vector sont les plus strictes, celles de list les plus souples. Voir la section « Invalidation des itérateurs » de chaque fiche.
- **Limites du C++98** — ce qui *manque* (pas d'`emplace`, pas d'`auto`, pas de `shrink_to_fit`, pas d'`unordered_*`) et le substitut C++98 pour chacun.
- **La règle « `pop()` renvoie `void` »** — vaut pour les trois adaptateurs (stack, queue, priority_queue). Toujours `top()` / `front()` *avant* `pop()`.
- **Passage par `const &`** — tout conteneur est un type valeur ; les copies coûtent cher. Toujours passer par référence, sauf si tu veux vraiment une copie.
- **La forme d'`insert` avec indice (*hint*)** — pour les conteneurs ordonnés (set/multiset/map/multimap), fournir un indice correct transforme une insertion en masse en O(n log n) en O(n).

---

## Où chaque conteneur apparaît dans le cursus 42

| Conteneur | Module | Pourquoi |
|---|---|---|
| `vector` | CPP08 ex01 (Span), CPP09 ex01 (RPN) | Accès direct, beaucoup de push_back. |
| `list`   | rare              | Apparaît quand la stabilité des itérateurs est explicitement le sujet. |
| `deque`  | CPP08 ex02 (sous MutantStack) | Conteneur sous-jacent par défaut de `std::stack`. |
| `stack`  | CPP08 ex02 (MutantStack), CPP09 ex01 (RPN) | Discipline LIFO. |
| `queue`  | rare dans le cursus, courant en algorithmique | BFS, ordonnancement. |
| `priority_queue` | rare dans le cursus       | Sélection par tas. |
| `set`    | CPP08 ex01 (Span)              | Tri automatique + dédoublonnage. |
| `map`    | CPP09 ex00 (BitcoinExchange)   | Recherche dans une série temporelle via `lower_bound`. |
| `multiset`/`multimap` | rare              | Quand les doublons ont un sens métier. |

---

## Ordre de lecture si tu pars de zéro

1. **`VECTOR.md`** — pose le modèle mental ; tous les autres se comparent à lui.
2. **`STL.md` (dans `../advanced/`, EN)** — vue d'ensemble conteneurs + itérateurs + algorithmes.
3. **`LIST.md`** — le contraste de la liste chaînée ; apprend la stabilité des itérateurs.
4. **`MAP.md`** — le conteneur associatif le plus courant ; introduit `pair<const K, V>` et `lower_bound`.
5. **`STACK.md` / `QUEUE.md`** — le principe d'adaptateur et la discipline « `pop()` est `void` ».
6. **`SET.md`, `DEQUE.md`, `PRIORITY_QUEUE.md`** — à compléter selon les besoins.
7. **`MULTISET.md`, `MULTIMAP.md`** — à lire une fois maîtrisés leurs cousins à clés uniques.
