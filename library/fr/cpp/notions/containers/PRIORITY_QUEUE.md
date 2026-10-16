# `std::priority_queue` — L'adaptateur adossé à un tas

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/PRIORITY_QUEUE.md)

> **TL;DR.** `std::priority_queue<T>` n'est **pas une file** — c'est un **tas binaire** (*binary heap*) déguisé en file. Le prochain élément à sortir est toujours le **plus grand** (tas max par défaut). Adossé à un `vector` + les fonctions de tas de `<algorithm>` (`std::make_heap`, `std::push_heap`, `std::pop_heap`). Push et pop en O(log n), top en O(1). Pour un tas min, passe `std::greater<T>` comme comparateur.

Voir aussi : [`STACK.md`](STACK.md) · [`QUEUE.md`](QUEUE.md) · [`VECTOR.md`](VECTOR.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <queue>` *(oui — le même en-tête que `std::queue`)*

---

## 1. Modèle mental

Un tas binaire stocké dans un vector. Le vector est le stockage ; les fonctions de tas de `<algorithm>` imposent l'invariant « parent ≥ enfants ».

```
   priority_queue<int> avec les éléments poussés dans l'ordre : 3, 1, 4, 1, 5, 9, 2, 6

   le tas vu comme un arbre :                    le tas vu comme un vector :

              9                                  indice :  0  1  2  3  4  5  6  7
            /   \                                valeurs : 9  6  4  3  5  1  2  1
           6     4
          / \   / \                              parent de i :  (i-1) / 2
         3   5 1   2                             enfants de i :  2*i+1, 2*i+2
        /
       1                                         top() = vector[0] = 9
```

Push : on ajoute à la fin du vector, puis on « remonte » l'élément (*sift up* : on l'échange avec son parent tant que le parent est plus petit). Pop : on échange la racine avec le dernier élément, on raccourcit le vector d'une case, puis on « redescend » la nouvelle racine (*sift down*). Les deux sont en O(log n).

Le standard C++98 la définit à peu près ainsi :

```cpp
template <class T,
          class Container = std::vector<T>,
          class Compare = std::less<T> >
class priority_queue {
protected:
    Container c;
    Compare   comp;
public:
    void  push(const T& x) {
        c.push_back(x);
        std::push_heap(c.begin(), c.end(), comp);
    }
    void  pop() {
        std::pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }
    const T& top() const   { return c.front(); }
    // size, empty, no comparisons
};
```

Note le comparateur : `std::less<T>` produit un **tas max**, parce que les fonctions de tas placent l'élément « le plus grand » en tête quand le comparateur est « inférieur à ».

---

## 2. Construction & choix du comparateur

```cpp
#include <queue>
#include <vector>
#include <functional>          // for std::greater

std::priority_queue<int> max_pq;                 // default — max-heap

// Min-heap: change comparator to greater
std::priority_queue<int, std::vector<int>, std::greater<int> > min_pq;

// Pre-populate from a range — the ctor calls make_heap once
int data[] = {3, 1, 4, 1, 5, 9, 2, 6};
std::priority_queue<int> pq(std::less<int>(), std::vector<int>(data, data + 8));
```

**Conteneur support :** il doit fournir `front`, `push_back`, `pop_back` **et** des itérateurs à accès direct (les algorithmes de tas en ont besoin). Donc `vector` (par défaut) ou `deque`. `list` est refusée.

> **La signature du constructeur en C++98 est maladroite** — elle prend d'abord le comparateur, puis le conteneur. Le C++11 a ajouté des constructeurs plus propres. En pratique, tu utilises surtout le constructeur par défaut et tu `push` les éléments.

---

## 3. API essentielle

| Méthode     | Ce qu'elle fait                                          | Coût      |
|-------------|----------------------------------------------------------|-----------|
| `push(x)`   | Insère et rétablit le tas.                               | O(log n)  |
| `pop()`     | Retire le sommet. **Renvoie `void`.**                    | O(log n)  |
| `top()`     | Référence vers le plus grand élément. UB si vide.        | O(1)      |
| `size()`    | Nombre d'éléments.                                       | O(1)      |
| `empty()`   | `size() == 0`.                                           | O(1)      |

**Pas de parcours, pas d'accès direct, pas de `begin`/`end`.** Et, cas unique parmi les adaptateurs : **pas d'opérateurs de comparaison** (`==`, `<`). Tout ce que tu peux faire : push, regarder le sommet, pop.

---

## 4. Tas max par défaut — et comment l'inverser

Ça piège tous les débutants. `std::less<T>` (le comparateur par défaut) signifie « inférieur à » — mais le tas range les éléments de sorte qu'**aucun autre élément ne soit plus grand** que celui en tête. Donc la *tête* est le *plus grand*. C'est un tas max.

```cpp
std::priority_queue<int> pq;
pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);
std::cout << pq.top() << "\n";                  // 5  (the maximum)
```

Pour un **tas min** (le plus petit sort en premier), utilise `std::greater<int>` :

```cpp
#include <functional>
std::priority_queue<int, std::vector<int>, std::greater<int> > pq;
pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);
std::cout << pq.top() << "\n";                  // 1  (the minimum)
```

Les comparateurs personnalisés fonctionnent de la même façon — fournis un foncteur dont l'`operator()(const T &a, const T &b) const` renvoie true quand `a` doit passer **après** `b` dans l'ordre de priorité.

```cpp
struct ByLength {
    bool operator()(const std::string &a, const std::string &b) const {
        return a.size() < b.size();             // longest string is "greatest"
    }
};
std::priority_queue<std::string, std::vector<std::string>, ByLength> pq;
```

---

## 5. Même règle : `pop()` renvoie `void`

Identique à stack et queue :

```cpp
// WRONG
int x = pq.pop();

// RIGHT
int x = pq.top();
pq.pop();
```

Même raisonnement : séparer la *lecture* de la *destruction* évite qu'une copie qui lance fasse perdre l'élément.

---

## 6. Astuces d'efficacité

### 6.1 La construction en bloc bat N push

```cpp
// SLOW — N × O(log N) = O(N log N)
std::priority_queue<int> pq;
for (int i = 0; i < N; ++i) pq.push(data[i]);

// FAST — one make_heap call: O(N)
std::vector<int> v(data, data + N);
std::priority_queue<int> pq(std::less<int>(), v);   // ctor calls make_heap once
```

La construction depuis un intervalle est asymptotiquement plus rapide (`make_heap` est en O(N), pas O(N log N)).

### 6.2 Le support par défaut (vector) est presque toujours le bon

`vector` offre aux fonctions de tas une mémoire contiguë à accès direct — exactement ce qu'elles veulent. `deque` marche mais ajoute un surcoût de frontière de bloc à chaque accès. Aucune bonne raison de la préférer pour un tas.

### 6.3 Réserver indirectement

Impossible de faire `reserve` sur une priority_queue (ce membre n'existe pas). Contournement : construis d'abord un vector, réserve dessus, puis construis la file à partir de lui.

### 6.4 Pop dans une boucle = tri

```cpp
while (!pq.empty()) {
    std::cout << pq.top() << " ";
    pq.pop();
}
```

C'est un tri par tas (*heapsort*). O(n log n), en ordre décroissant (tas max) ou croissant (tas min). Utile quand tu veux les éléments un par un dans l'ordre de priorité sans tout trier d'avance.

### 6.5 Ne t'en sers pas comme d'un « vector trié » — `std::set` ou un vector trié + `lower_bound` font en général mieux

Si tu as besoin d'un parcours trié, de requêtes par intervalle, ou de retirer des éléments arbitraires, le tas n'a pas la bonne forme — il ne sait que trouver/retirer le maximum. Choisir le bon outil : priority_queue pour « toujours sortir le meilleur suivant » ; set pour « accès trié, par clé ».

---

## 7. Quand NE PAS utiliser priority_queue

- Tu dois supprimer des éléments arbitraires → utilise `std::set` (ou implémente une « suppression paresseuse » en ignorant les sommets périmés au pop).
- Tu dois mettre à jour la priorité d'un élément → pas supporté directement. Contournement courant : pousser un doublon avec la nouvelle priorité et sauter les entrées périmées au pop.
- Tu as besoin d'un ordre stable pour les éléments de même priorité → priority_queue ne garantit rien sur les égalités. Intègre un numéro de séquence d'insertion dans le comparateur si nécessaire.
- Tu dois parcourir les éléments → mauvais adaptateur. Utilise un vector et `std::sort`/`std::partial_sort`.

---

## 8. Limites du C++98

| Fonctionnalité       | Version C++ | Alternative C++98                                |
|----------------------|-------------|--------------------------------------------------|
| `emplace`            | C++11       | `push` — paie une copie.                         |
| Sémantique de déplacement | C++11  | Passer par `const &`.                            |
| Comparateurs en lambda | C++11     | Définir une struct foncteur avec `operator()`.   |
| Constructeur plus propre (juste des itérateurs) | C++11 | Le constructeur C++98 maladroit prend `(comp, container)`. |

---

## 9. Pièges

- **Par défaut c'est un tas *max*, pas min.** Passe `std::greater<T>` pour un tas min.
- **`pop()` renvoie `void`.** `top()` d'abord.
- **Impossible de retirer un élément arbitraire.** La structure ne le permet pas ; utilise `std::set` si tu en as besoin.
- **Pas d'itérateurs, pas de `begin`/`end`.** Si tu en as besoin, la priority_queue n'est pas ton outil.
- **Le comparateur doit être un *ordre faible strict*** (*strict weak ordering*). Renvoyer `true` à la fois pour `comp(a,b)` et `comp(b,a)` est un UB.
- **La signature du constructeur C++98 est `(Compare, Container)`** — l'ordre compte et on l'inverse facilement par accident.
- **Pas d'opérateurs de comparaison sur les priority_queues elles-mêmes** — `pq1 == pq2` ne compile pas. Compare les conteneurs sous-jacents si besoin.

---

## 10. Exemple complet — les K plus grands éléments

```cpp
#include <queue>
#include <vector>
#include <functional>
#include <iostream>

int main(void)
{
    int data[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    int N = sizeof(data) / sizeof(data[0]);
    int K = 3;

    // Min-heap of size K — keep the K LARGEST elements.
    // Trick: when full and a new element is bigger than the smallest kept, swap them.
    std::priority_queue<int, std::vector<int>, std::greater<int> > heap;

    for (int i = 0; i < N; ++i)
    {
        if ((int)heap.size() < K)
            heap.push(data[i]);
        else if (data[i] > heap.top())
        {
            heap.pop();
            heap.push(data[i]);
        }
    }

    // heap now contains the K largest elements (in min-heap order)
    while (!heap.empty())
    {
        std::cout << heap.top() << " ";        // 5 6 9
        heap.pop();
    }
    std::cout << "\n";
    return 0;
}
```

Temps : O(N log K) au lieu du O(N log N) d'un tri complet. Le tas min de taille K est l'astuce canonique du « top K » — et priority_queue est le bon outil parce qu'elle expose toujours le pire des meilleurs, pour une comparaison bon marché.

---

> **En une ligne.** `std::priority_queue`, c'est un tas max sur un vector avec `push` / `top` / `pop`. Bascule en tas min avec `std::greater<T>`. Push en O(log n), top en O(1). Pas de parcours, pas de suppression arbitraire — elle fait un seul travail (sortir l'extrême) et le fait extrêmement bien.
