# `std::queue` — L'adaptateur de conteneur FIFO

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/QUEUE.md)

> **TL;DR.** `std::queue<T>` est la sœur FIFO de [`std::stack`](STACK.md) : un fin **adaptateur** posé sur un conteneur sous-jacent. Le support par défaut est `deque` (car la file a besoin à la fois de `push_back` et de `pop_front` en O(1)). Tu **ne peux pas utiliser `vector`** comme support — `vector` n'a pas de `pop_front`. Même piège du `pop()` qui renvoie `void` que pour stack.

Voir aussi : [`STACK.md`](STACK.md) · [`PRIORITY_QUEUE.md`](PRIORITY_QUEUE.md) · [`DEQUE.md`](DEQUE.md) · [`LIST.md`](LIST.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <queue>`

---

## 1. Modèle mental

```
   std::queue<int>            (par défaut = deque en dessous)

   front ────►  ┌───┬───┬───┬───┬───┐  ◄──── back
                │ 1 │ 2 │ 3 │ 4 │ 5 │
                └───┴───┴───┴───┴───┘
                  ▲                 ▲
                  │                 │
            on retire ici      on ajoute ici

      push() ──→ underlying.push_back()
      pop()  ──→ underlying.pop_front()
      front()──→ underlying.front()
      back() ──→ underlying.back()
```

Le standard C++98 définit `std::queue` à peu près ainsi :

```cpp
template <class T, class Container = std::deque<T> >
class queue {
protected:
    Container c;
public:
    void  push(const T& x) { c.push_back(x); }
    void  pop()             { c.pop_front(); }
    T&    front()           { return c.front(); }
    T&    back()            { return c.back(); }
    // size, empty, comparisons …
};
```

C'est ce `pop_front` qui fait que **vector ne peut pas servir de support à une queue**. Le `erase(begin())` de vector est en O(n), et il n'a aucun membre `pop_front`. Le standard exige que le conteneur support fournisse un vrai `pop_front` — seuls `deque` et `list` conviennent.

---

## 2. Construction & choix du conteneur sous-jacent

```cpp
#include <queue>
#include <list>

std::queue<int> q;                          // default — deque
std::queue<int, std::list<int> > ql;        // backed by list (mind the space before >)

std::deque<int> seed;
seed.push_back(1); seed.push_back(2);
std::queue<int> q2(seed);                   // q2.front() == 1, q2.back() == 2
```

| Support  | Avantages                               | Inconvénients                     |
|----------|-----------------------------------------|-----------------------------------|
| `deque`  | O(1) aux deux bouts ; correct côté cache au sein des blocs | Au minimum le surcoût d'un bloc |
| `list`   | Vrai push/pop en O(1) ; la stabilité des itérateurs est sans intérêt pour une queue | Un `malloc` par push — lent |
| `vector` | **Interdit** — pas de `pop_front`       | —                                 |

**Règle par défaut :** garde `deque`. Ne change pas sans raison mesurée.

---

## 3. API essentielle

| Méthode     | Ce qu'elle fait                                 | Coût  |
|-------------|-------------------------------------------------|-------|
| `push(x)`   | Ajoute `x` à la fin.                            | Celui du `push_back` sous-jacent. |
| `pop()`     | Retire l'élément de tête. **Renvoie `void`.**   | Celui du `pop_front` sous-jacent. |
| `front()`   | Référence vers l'élément le plus ancien. UB si vide. | O(1)  |
| `back()`    | Référence vers l'élément le plus récent. UB si vide. | O(1)  |
| `size()`    | Nombre d'éléments.                              | O(1)  |
| `empty()`   | `size() == 0`.                                  | O(1)  |
| `==`, `<`, etc. | Comparaison lexicographique via le conteneur sous-jacent. | O(n)  |

**Pas d'itérateurs, pas d'indexation.** Même interface restreinte que stack — par conception.

---

## 4. Le même piège du `pop()` qui renvoie `void`

Raisonnement identique à [`STACK.md` §4](STACK.md#4.%20Le%20pi%C3%A8ge%20du%20pop%28%29%20qui%20renvoie%20void) : le C++98 sépare la *lecture* de la *destruction* pour qu'une copie qui lance ne puisse pas perdre de données.

```cpp
// WRONG
int x = q.pop();

// RIGHT
int x = q.front();
q.pop();
```

---

## 5. Astuces d'efficacité

### 5.1 Par défaut `deque`. `list` en vaut rarement la peine.

Le coût amorti de la deque est O(1) par push et O(1) par pop, avec la localité de cache à l'intérieur de chaque bloc. List ajoute un `malloc`/`free` par push/pop — facilement 10× plus lent sur des charges serrées.

### 5.2 Passe par `const &`

Copier une queue copie chaque élément. Ne le fais pas par accident.

### 5.3 Si tu dois jeter un œil au milieu, l'adaptateur ne te suffit plus

Utilise `std::deque<T>` directement. Tout l'intérêt de l'adaptateur est *d'empêcher* l'accès aux éléments du milieu ; si tu en as besoin, l'abstraction ne convient pas.

### 5.4 Besoin d'une priorité plutôt que de l'ordre d'arrivée ? Voir `PRIORITY_QUEUE.md`

`std::queue` est strictement FIFO. Si « le prochain élément à sortir » dépend d'une priorité basée sur la valeur plutôt que sur l'heure d'arrivée, utilise plutôt [`std::priority_queue`](PRIORITY_QUEUE.md).

---

## 6. Quand NE PAS utiliser queue

- Tu dois inspecter ou modifier d'autres éléments que `front` / `back` → utilise `deque`.
- Tu dois réorganiser l'ordre → utilise `vector` et trie, ou `priority_queue`.
- Tu as plusieurs producteurs/consommateurs sur plusieurs threads → `std::queue` n'est **pas thread-safe**. Le C++98 n'a pas de file concurrente intégrée ; il faudrait l'envelopper d'un mutex (et `<thread>` est du C++11+, interdit dans le cursus 42 jusqu'à plus tard).

Utilise queue quand :
- L'algorithme consiste à « traiter dans l'ordre d'arrivée » — BFS, ordonnanceur de tâches, boucle d'événements, pool de travail.
- Tu veux que la discipline FIFO soit imposée par le type.

---

## 7. Limites du C++98

| Fonctionnalité | Version C++ | Alternative C++98                |
|----------------|-------------|----------------------------------|
| `emplace`      | C++11       | `push` — paie une copie.         |
| Sémantique de déplacement | C++11 | Passer par `const &`.        |
| Surcharges de `std::queue` depuis une liste d'initialisation | C++11 | Construire d'abord la deque, puis construire la queue à partir d'elle. |

---

## 8. Pièges

- **`pop()` renvoie `void`** — `front()` d'abord, puis `pop()`.
- **Impossible d'utiliser `vector`** comme conteneur support. L'erreur du compilateur est cryptique (« no member named `pop_front` »). Le standard n'autorise que `deque` et `list`.
- **`front()` / `back()` sur une queue vide est un UB.** Toujours `if (!q.empty())` d'abord.
- **Pas d'itérateurs** — même restriction que stack.
- **Pas thread-safe.** Deux threads qui pushent en même temps, c'est un UB.
- **`std::list<int> >` a besoin d'un espace** entre les chevrons en C++98.

---

## 9. Exemple complet — squelette de BFS (parcours en largeur)

```cpp
#include <queue>
#include <vector>
#include <iostream>

void    bfs(int start, const std::vector<std::vector<int> > &graph)
{
    std::vector<bool> seen(graph.size(), false);
    std::queue<int>   frontier;

    frontier.push(start);
    seen[start] = true;

    while (!frontier.empty())
    {
        int node = frontier.front();
        frontier.pop();                     // §4 — front first, then pop
        std::cout << node << " ";

        for (std::size_t i = 0; i < graph[node].size(); ++i)
        {
            int next = graph[node][i];
            if (!seen[next])
            {
                seen[next] = true;
                frontier.push(next);
            }
        }
    }
    std::cout << "\n";
}
```

Le BFS est l'usage canonique d'une queue : traiter les nœuds dans l'ordre où ils ont été découverts. Une stack à la place te donnerait un DFS (parcours en profondeur) — même squelette, autre conteneur.

---

> **En une ligne.** `std::queue` est la jumelle FIFO de `std::stack`. Support par défaut `deque` (vector interdit), `pop()` est `void`, pas d'itérateurs. Utilise-la pour traiter dans l'ordre d'arrivée.
