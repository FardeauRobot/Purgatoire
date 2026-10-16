# `std::deque` — File à double extrémité (la « liste de blocs »)

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/DEQUE.md)

> **TL;DR.** Une `std::deque<T>` (*double-ended queue*) est **une liste de tableaux de taille fixe** (appelés *blocs* ou *chunks*). Elle offre le push/pop en O(1) **aux deux extrémités** *et* l'accès direct en O(1) — mais le stockage n'est **pas contigu**, donc `&d[0]` n'est pas un pointeur utile vers un buffer. Les itérateurs sont à accès direct mais un peu plus coûteux que ceux d'un vector, car ils peuvent devoir sauter d'un bloc à l'autre.

Voir aussi : [`VECTOR.md`](VECTOR.md) · [`LIST.md`](LIST.md) · [`STACK.md`](STACK.md) · [`QUEUE.md`](QUEUE.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <deque>`

---

## 1. Modèle mental

Une deque contient un petit tableau de pointeurs vers des blocs de taille fixe. Chaque bloc stocke N éléments (typiquement 512 octets / `sizeof(T)`, mais ça dépend de l'implémentation). La deque mémorise où commence la zone utilisée dans le premier bloc et où elle se termine dans le dernier.

```
   std::deque<int> d, après quelques push_back/push_front

   block_map (petit tableau de pointeurs)
   ┌───┬───┬───┬───┐
   │ A │ B │ C │ D │     ← quatre pointeurs de bloc
   └─┬─┴─┬─┴─┬─┴─┬─┘
     │   │   │   │
     v   v   v   v
   [...][...][...][...]   ← quatre blocs de N ints alloués sur le tas
       └─ zone utile ─┘

   front_iterator pointe dans le bloc A, à un certain décalage
   back_iterator  pointe dans le bloc D, à un certain décalage
```

Quand tu fais un `push_front` et que le bloc de tête est plein, la deque alloue un nouveau bloc et ajoute son pointeur au début de la block_map. Quand la block_map elle-même est pleine, c'est *elle* qui est réallouée — mais **les blocs de données, eux, ne bougent pas**. C'est pour ça que les références vers les éléments restent (en grande partie) stables.

Pour l'accès direct `d[i]` : on calcule `(indice_de_bloc, décalage_dans_le_bloc)` puis on déréférence. Deux lectures de tableau au lieu d'une — un poil plus lent que `vector`, mais toujours O(1).

---

## 2. Construction

Même menu que `vector` :

```cpp
#include <deque>

std::deque<int> a;
std::deque<int> b(5);                        // five 0s
std::deque<int> c(5, 42);                    // five 42s
std::deque<int> d(c);                        // copy
std::deque<int> e(c.begin(), c.end());       // range
std::deque<int> f(arr, arr + 10);            // from C array

a = c;
a.assign(5, 7);
```

Pas de `reserve` ni de `capacity()` — une deque n'a pas un buffer unique extensible dans lequel réserver.

---

## 3. API essentielle

| Méthode                             | Coût          | Remarque                               |
|-------------------------------------|---------------|----------------------------------------|
| `push_back(x)` / `push_front(x)`    | O(1) amorti   | Alloue un nouveau bloc si nécessaire.  |
| `pop_back()` / `pop_front()`        | O(1)          | Peut libérer un bloc quand il se vide. |
| `d[i]`, `d.at(i)`                   | O(1)          | Deux indirections (bloc + décalage).   |
| `front()` / `back()`                | O(1)          | UB si vide.                            |
| `insert(it, x)` / `erase(it)`       | O(n) au milieu, O(1) aux extrémités | Décale la moitié la plus courte. |
| `clear()`                           | O(n)          | Libère chaque bloc.                    |
| `size()` / `empty()`                | O(1)          |                                        |
| `resize(n)` / `resize(n, x)`        | O(\|Δ\|)      |                                        |
| `swap(other)`                       | O(1)          | Échange le pointeur de block_map.      |
| `begin()` / `end()` / `rbegin()` / `rend()` | accès direct | Un peu plus lourds que les itérateurs de vector. |

**Pas de `data()`, pas de `reserve`, pas de `capacity`.** La deque gère activement plusieurs blocs ; « le buffer » n'existe pas sous forme d'une allocation unique.

---

## 4. Invalidation des itérateurs — le piège de la deque

C'est là que la plupart des débutants se brûlent. Les règles d'invalidation de la deque sont **plus strictes qu'elles n'en ont l'air** :

| Opération                    | Itérateurs                         | Références / pointeurs                            |
|------------------------------|------------------------------------|---------------------------------------------------|
| `push_back` / `push_front`   | **Tous les itérateurs invalidés.** | Les références vers les éléments existants **restent valides**. |
| `pop_back` / `pop_front`     | Les itérateurs vers l'extrémité retirée + `end()` | Les références vers les autres éléments restent valides. |
| `insert(middle)` / `erase(middle)` | Tous les itérateurs invalidés. | Toutes les références invalidées.            |
| `clear()`                    | Tous les itérateurs                | Toutes les références                             |
| `swap(other)`                | Aucun des tiens                    | Aucune des tiennes                                |

Note l'asymétrie : un push dans une deque **ne déplace pas les éléments existants** (les blocs de données ne sont pas réalloués), donc **pointeurs et références restent valides** — mais l'implémentation de l'itérateur peut devoir recalculer l'état interne de la block_map, donc par prudence tous les itérateurs sont invalidés.

En pratique : si tu tiens un `T*` ou un `T&` vers un élément d'une deque, il survit à `push_back` / `push_front` (propre à la deque). Un itérateur, non. **Utilise des indices** dans les boucles qui modifient, comme pour vector.

---

## 5. Astuces d'efficacité

### 5.1 Choisis deque quand tu pushes vraiment aux deux bouts

```cpp
std::deque<int> d;
d.push_front(1);                  // O(1) — vector would be O(n)
d.push_back(2);                   // O(1) — same as vector
d.pop_front();                    // O(1) — vector would be O(n)
```

Si ton algorithme est « construire une fenêtre glissante » ou « file avec accès direct », deque est le bon outil.

### 5.2 Ne choisis pas deque juste parce que vector « gaspille de la capacité »

La deque alloue par bloc, donc une deque vide possède déjà un bloc (~512 octets de surcoût). Pour de petites collections, un vector avec `reserve(n)` est plus léger.

### 5.3 Le parcours est plus lent qu'avec vector — mesure si ça compte

Chaque déréférencement d'itérateur peut devoir vérifier si tu as franchi une frontière de bloc. Pour des boucles numériques serrées sur des millions d'éléments, vector gagne. Pour des schémas d'accès mixtes, la différence se perd dans le bruit.

### 5.4 `swap` pour rétrécir — même astuce que vector

```cpp
std::deque<int>(d).swap(d);       // shrink to fit
std::deque<int>().swap(d);        // free all blocks
```

`clear()` libère les blocs paresseusement (certaines implémentations en gardent un) ; l'idiome du swap force la libération complète.

---

## 6. Quand NE PAS utiliser deque

| Si tu as besoin de…                                  | Prends…                                   |
|------------------------------------------------------|-------------------------------------------|
| Un buffer à passer à une API C (`read`, `write`, `memcpy`) | `vector` — lui seul donne un pointeur vers un bloc contigu unique. |
| La vitesse de parcours maximale                      | `vector`                                  |
| Beaucoup d'insertions au milieu                      | `list`                                    |
| Un accès trié par clé                                | `set` / `map`                             |
| Du LIFO / FIFO pur avec une API restreinte           | `stack` / `queue` (qui utilisent une deque en interne par défaut) |

**Le terrain idéal de la deque est étroit :** push/pop aux deux bouts + accès direct. En dehors, vector est meilleur.

---

## 7. Limites du C++98

| Fonctionnalité         | Version C++ | Alternative C++98                           |
|------------------------|-------------|---------------------------------------------|
| `emplace_*`            | C++11       | `push_*` — paie une copie.                  |
| `shrink_to_fit()`      | C++11       | L'astuce du swap (§5.4).                    |
| Membre `data()`        | Jamais (la deque n'est pas contiguë) | N/A — par conception. |
| Sémantique de déplacement | C++11    | Indisponible ; copier une deque coûte O(n). |

---

## 8. Pièges

- **`&d[0]` n'est *pas* un pointeur vers toutes les données**, seulement vers le premier bloc. Ne le passe pas à `read`/`write`/`memcpy` en espérant `n * sizeof(T)` octets.
- **`push_front` invalide tous les itérateurs**, même s'il ne réalloue pas les blocs de données. La block_map peut grandir.
- **Les références sont plus stables que les itérateurs.** Un `T&` / `T*` vers un élément existant survit à `push_back` / `push_front`. Les itérateurs, non.
- **La deque a plus de surcoût par élément que vector** pour un petit N — au moins un bloc (souvent 512 octets) plus la block_map. Pour N < ~32, vector gagne presque toujours.
- **Pas de `reserve`.** Impossible de préallouer un nombre d'éléments ; la croissance est automatique, bloc par bloc.

---

## 9. Exemple complet — fenêtre glissante

```cpp
#include <deque>
#include <iostream>

int main(void)
{
    std::deque<int> window;
    int data[] = {3, 1, 4, 1, 5, 9, 2, 6};
    int N = sizeof(data) / sizeof(data[0]);
    int K = 3;

    for (int i = 0; i < N; ++i)
    {
        window.push_back(data[i]);
        if ((int)window.size() > K)
            window.pop_front();             // O(1) — the deque sweet spot

        if ((int)window.size() == K)
        {
            std::cout << "window: ";
            for (std::size_t j = 0; j < window.size(); ++j)
                std::cout << window[j] << " ";
            std::cout << "\n";
        }
    }
    return 0;
}
```

Les deux extrémités bougent à chaque étape. Avec un `vector`, chaque `pop_front` coûterait O(K). La deque le garde en O(1).

---

> **En une ligne.** `std::deque`, c'est une liste de petits tableaux. Utilise-la quand tu pushes aux **deux** bouts *et* que tu veux l'accès direct. Sinon, vector.
