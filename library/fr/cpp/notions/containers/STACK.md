# `std::stack` — L'adaptateur de conteneur LIFO

> 🇬🇧 [English version](../../../../en/cpp/notions/containers/STACK.md)

> **TL;DR.** `std::stack<T>` n'est **pas un conteneur** — c'est un fin **adaptateur** posé sur un autre conteneur (`deque` par défaut ; tu peux mettre `vector` ou `list` à la place). Il n'expose que `push`, `pop`, `top`, `size`, `empty`. Le piège le plus courant : **`pop()` renvoie `void`** — il retire l'élément du sommet mais ne te le *donne pas*. Toujours `top()` d'abord, puis `pop()`.

Voir aussi : [`QUEUE.md`](QUEUE.md) · [`DEQUE.md`](DEQUE.md) · [`VECTOR.md`](VECTOR.md) · [`LIST.md`](LIST.md) · [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`INDEX.md`](INDEX.md)

En-tête : `#include <stack>`

---

## 1. Modèle mental

Une stack n'est qu'une enveloppe qui cache tout sauf la fin d'un conteneur sous-jacent :

```
   std::stack<int>            (par défaut = deque en dessous)

      ┌──────────────┐
      │   stack<T>   │
      │              │
      │  push() ──── │──→ underlying.push_back()
      │  pop()  ──── │──→ underlying.pop_back()
      │  top()  ──── │──→ underlying.back()
      └──────────────┘
              │
              v
       std::deque<int>           ← les vraies données vivent ici
       [a, b, c, d, e]
                    ↑
                  top
```

C'est toute l'implémentation. Le standard C++98 définit littéralement `std::stack` ainsi :

```cpp
template <class T, class Container = std::deque<T> >
class stack {
protected:
    Container c;
public:
    void  push(const T& x) { c.push_back(x); }
    void  pop()             { c.pop_back(); }
    T&    top()             { return c.back(); }
    // size, empty, comparisons …
};
```

Le « patron adaptateur » (*adapter pattern*) — restreindre l'interface d'un conteneur existant pour imposer une discipline (LIFO).

---

## 2. Construction & choix du conteneur sous-jacent

```cpp
#include <stack>
#include <vector>
#include <list>

std::stack<int> s;                          // default — deque underneath
std::stack<int, std::vector<int> > sv;      // backed by vector
std::stack<int, std::list<int> > sl;        // backed by list

std::deque<int> seed;                       // pre-populate from another container
seed.push_back(1); seed.push_back(2);
std::stack<int> s2(seed);                   // s2's top is 2
```

> **Note l'espace dans `std::list<int> >`.** En C++98, `>>` est lu comme l'opérateur de décalage à droite. Le C++11 a corrigé ça ; en C++98 tu dois garder un espace entre deux chevrons fermants de template adjacents.

**Quel conteneur sous-jacent ?**

| Support       | Avantages                           | Inconvénients                     |
|---------------|-------------------------------------|-----------------------------------|
| `deque` (défaut) | push/pop en O(1), jamais de copie à la croissance, pas de grosse réallocation | Surcoût par élément dû aux blocs |
| `vector`      | Meilleure localité de cache ; mémoire minimale pour un petit N | Réalloue et copie tous les éléments quand la capacité est épuisée |
| `list`        | Vrai push/pop en O(1), itérateurs stables à travers les modifications (sans intérêt pour une stack) | Un `malloc` par push — lent |

**Règle par défaut :** garde `deque`. Ne passe à `vector` que si tu as mesuré que la localité de cache compte plus que le coût rare d'une réallocation.

---

## 3. API essentielle

| Méthode     | Ce qu'elle fait                                    | Coût  |
|-------------|----------------------------------------------------|-------|
| `push(x)`   | Ajoute `x` au sommet.                              | Celui du `push_back` sous-jacent. |
| `pop()`     | Retire le sommet. **Renvoie `void`.**              | Celui du `pop_back` sous-jacent.  |
| `top()`     | Référence vers l'élément au sommet. UB si vide.    | O(1)  |
| `size()`    | Nombre d'éléments.                                 | O(1)  |
| `empty()`   | `size() == 0`.                                     | O(1)  |
| `==`, `<`, etc. | Comparaison lexicographique via le conteneur sous-jacent. | O(n)  |

**C'est tout.** Pas de `begin`, pas de `end`, pas d'`operator[]`, pas d'itérateur. Par conception.

---

## 4. Le piège du `pop()` qui renvoie `void`

Tout le monde tombe dedans exactement une fois :

```cpp
// WRONG — pop returns void; this won't compile.
int x = s.pop();

// RIGHT — read with top, then remove with pop.
int x = s.top();
s.pop();
```

**Pourquoi `pop` est-il `void` ?** Parce que renvoyer l'élément retiré par valeur exigerait une copie. Si la copie lance une exception (par ex. `T` est un type lourd dont le constructeur de copie alloue et qu'un `bad_alloc` est levé), l'élément serait **détruit** mais **jamais remis à l'appelant** — la valeur serait perdue. Le C++98 a choisi la sûreté : séparer la lecture (`top`) de la destruction (`pop`), pour qu'une copie qui lance ne puisse pas perdre de données.

Le découpage `top()` + `pop()` est l'un des choix de conception les plus défendus du standard, et l'un de ceux qui font le plus râler les débutants.

---

## 5. Astuces d'efficacité

### 5.1 Par défaut `deque`. Ne passe à `vector` qu'avec une raison.

Pour la plupart des usages, `deque` convient. Pourquoi vector peut tenter :

```cpp
std::stack<int, std::vector<int> > s;       // contiguous storage
```

La vitesse de parcours (si tu contournes l'adaptateur — voir §6) est meilleure avec vector. Mais pour une charge purement push/pop, le coût amorti de la deque est en général identique.

### 5.2 Préallouer avec un vector en support — mais `reserve` est inaccessible

L'adaptateur cache `reserve`. Pour préallouer :

```cpp
std::vector<int> backing;
backing.reserve(1000);
std::stack<int, std::vector<int> > s(backing);   // copies — but capacity carries over
```

Ou hériter de `std::stack` (le membre `c` est *volontairement* protected pour le permettre). C'est l'exercice classique `MutantStack` de CPP08.

### 5.3 L'astuce `MutantStack` — exposer les itérateurs sous-jacents

Stack cache les itérateurs. Pour les exposer, hérite de `std::stack` et va chercher le membre protected :

```cpp
template <typename T>
class MutantStack : public std::stack<T> {
public:
    typedef typename std::stack<T>::container_type::iterator iterator;
    iterator begin() { return this->c.begin(); }   // c is protected, accessible from derived
    iterator end()   { return this->c.end();   }
};
```

Tu peux maintenant parcourir une stack — l'adaptateur standard ne le permet pas, mais le conteneur sous-jacent si. C'est tout l'enjeu de CPP08 ex02.

### 5.4 Passe par `const &`

Même règle que pour tous les autres conteneurs — copier une stack copie chaque élément en dessous.

---

## 6. Quand NE PAS utiliser stack

Si tu as besoin de *quoi que ce soit* au-delà de push / pop / top — parcours, indexation, recherche — utilise directement le conteneur sous-jacent. L'adaptateur empêche activement l'accès à ces opérations.

Utilise stack quand :
- Tu veux que la discipline LIFO soit imposée au niveau du type (pour que le compilateur te rappelle que « regarder l'élément `i` » n'a pas de sens pour cette collection).
- Tu implémentes un parseur, un évaluateur d'expressions, un parcours en profondeur, un buffer d'annulation, ou tout ce où le LIFO est l'invariant de l'algorithme.

Évite stack quand :
- Tu devras inspecter le contenu plus tard → utilise un vector.
- Tu devras transférer des éléments entre stacks → utilise une list.
- Tu fais « à peu près » du LIFO mais as aussi besoin d'autres opérations → utilise le conteneur sous-jacent.

---

## 7. Limites du C++98

| Fonctionnalité | Version C++ | Alternative C++98                |
|----------------|-------------|----------------------------------|
| `emplace`      | C++11       | `push` — paie une copie.         |
| Sémantique de déplacement | C++11 | Passer par `const &`.        |

---

## 8. Pièges

- **`pop()` renvoie `void`.** Tu dois faire `top()` d'abord.
- **`top()` sur une stack vide est un UB**, pas une exception. Toujours `if (!s.empty())` d'abord.
- **Pas d'itérateurs par défaut.** `MutantStack` est le contournement si tu en as besoin.
- **`std::list<int> >` a besoin d'un espace** en C++98 — `>>` est lu comme un décalage à droite.
- **Comparer deux stacks compare élément par élément** via l'`operator==` du conteneur sous-jacent — assure-toi donc que `T` le définit.
- **L'affectation d'une stack est une copie profonde.** Copier une stack d'1M d'éléments copie les 1M d'éléments.

---

## 9. Exemple complet — parenthèses équilibrées

```cpp
#include <stack>
#include <string>
#include <iostream>

bool    is_balanced(const std::string &s)
{
    std::stack<char> st;
    for (std::size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{')
            st.push(c);
        else if (c == ')' || c == ']' || c == '}')
        {
            if (st.empty())
                return false;
            char open = st.top();
            st.pop();                       // §4 — top first, then pop
            if ((c == ')' && open != '(')
             || (c == ']' && open != '[')
             || (c == '}' && open != '{'))
                return false;
        }
    }
    return st.empty();
}

int main(void)
{
    std::cout << is_balanced("([{}])") << "\n";    // 1
    std::cout << is_balanced("([)]")   << "\n";    // 0
    return 0;
}
```

Discipline LIFO classique — chaque « ouvrant » pushe, chaque « fermant » pope + vérifie. L'adaptateur stack t'interdit de lire un autre élément que `top`, ce qui est exactement la contrainte dont l'algorithme a besoin.

---

> **En une ligne.** `std::stack` est une discipline, pas un conteneur. Push/top/pop, jamais `pop()` utilisé comme une expression, le support par défaut convient, et sors `MutantStack` si tu as un jour besoin de parcourir.
