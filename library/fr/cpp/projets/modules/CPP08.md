# 🧰 CPP08 — Conteneurs, itérateurs, algorithmes : arrête d'écrire la boucle

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP08.md)

> **TL;DR.** La STL, ce sont trois familles qui s'emboîtent. Les **conteneurs** stockent les données (`vector`, `list`, `deque`…). Les **itérateurs** sont des pointeurs généralisés qui parcourent n'importe quel conteneur de la même façon. Les **algorithmes** (`find`, `sort`, `min_element`…) prennent une paire d'itérateurs et se moquent du conteneur qu'il y a dessous. CPP08 te note sur l'*utilisation* de ce système : *« not using the STL is a bad grade even if the code works. »* Les exercices sont `easyfind` (algorithme + itérateur), `Span` (algorithmes sur un vector, plus une insertion par plage) et `MutantStack` (ouvrir un adaptateur pour exposer ses itérateurs).

Voir aussi : [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`ALGORITHMS.md`](../../../../en/cpp/notions/advanced/ALGORITHMS.md) (EN) · [`TEMPLATES.md`](../../../../en/cpp/notions/advanced/TEMPLATES.md) (EN) · [`containers/INDEX.md`](../../notions/containers/INDEX.md) · [`VECTOR.md`](../../notions/containers/VECTOR.md) · [`STACK.md`](../../notions/containers/STACK.md) · [`DEQUE.md`](../../notions/containers/DEQUE.md) · [`TYPENAME.md`](../../../../en/cpp/lexique/TYPENAME.md) (EN) · [`INHERITANCE.md`](../../notions/oop/INHERITANCE.md) · Précédent : [`CPP07.md`](CPP07.md)

---

## 1. Le problème, dans le C que tu connais déjà

```c
/* find 42 in an int array */
int *ft_find(int *arr, size_t n, int v)
{
    size_t i = 0;
    while (i < n && arr[i] != v)
        i++;
    return (i < n) ? &arr[i] : NULL;
}
/* ...and again for a linked list (t_list), with a completely different loop.
   ...and again for "find the smallest", "count", "sort"… per data structure. */
```

La libft t'a déjà fait écrire `ft_lstiter`, `ft_lstmap` et `ft_memchr` séparément pour chaque forme de données. **N algorithmes × M structures de données = N×M fonctions.**

**La réponse de la STL :** écrire chaque algorithme **une seule fois**, en termes d'itérateurs. Donner des itérateurs à chaque conteneur. On passe alors à **N + M**.

```cpp
std::vector<int>::iterator it = std::find(v.begin(), v.end(), 42);
std::list<int>::iterator   jt = std::find(l.begin(), l.end(), 42);   // same algorithm
```

---

## 2. Le mécanisme, en un schéma

```
         ┌───────────────────┐        ┌───────────────────┐
         │    CONTENEURS     │        │    ALGORITHMES    │
         │ vector list deque │        │ find sort count   │
         │ map set stack*    │        │ min_element copy  │
         └─────────┬─────────┘        └─────────┬─────────┘
                   │ .begin() .end()            │ prennent (first, last)
                   ▼                            ▼
              ┌──────────────────────────────────────┐
              │              ITÉRATEURS              │
              │   *it   ++it   it != end   (--it)    │
              └──────────────────────────────────────┘

 [ begin ...................................... end )
   ▲                                             ▲
   premier élément                               UN CRAN APRÈS le dernier. Ne jamais le déréférencer.
   « pas trouvé » se signale en renvoyant end().

 * stack, queue et priority_queue sont des ADAPTATEURS : ils enveloppent un autre conteneur
   et cachent volontairement ses itérateurs. ex02 consiste à ouvrir cette porte.
```

Un itérateur est calqué sur un pointeur. Pour un `vector`, c'en *est* pratiquement un : `*it` lit, `++it` avance. La « plage semi-ouverte » (*half-open range*) `[begin, end)` est la même convention que `for (i = 0; i < n; i++)` en C, où `n` est un cran après le dernier indice valide.

### Quel conteneur, quand (C++98)

| Conteneur | Organisation mémoire | Accès par indice | Insertion/suppression au milieu | Insertion aux extrémités | Vois-le comme |
|---|---|---|---|---|---|
| `vector` | Un seul bloc contigu | O(1) `v[i]` | O(n), décale tout | Fin : O(1) amorti | Un tableau `malloc` qui grandit |
| `deque` | Morceaux + table d'index | O(1) | O(n) | **Début et fin** O(1) | Un tableau qui grandit des deux côtés |
| `list` | Nœuds doublement chaînés | ❌ aucun | **O(1)** avec un itérateur | O(1) | `t_list` avec `prev` |
| `map<K,V>` | Arbre équilibré (rouge-noir) | O(log n) par clé | O(log n) | — | Dictionnaire trié |
| `set<K>` | Arbre équilibré | O(log n) recherche | O(log n) | — | Clés triées et uniques |
| `stack<T>` | Adaptateur sur `deque` par défaut | ❌ sommet seulement | ❌ | push/pop au sommet | Enveloppe LIFO |

Pages complètes : [`containers/INDEX.md`](../../notions/containers/INDEX.md).

---

## 3. ex00 — `easyfind`

### Ce que demande le sujet
- Un template de fonction `easyfind(T, int)`. `T` est un conteneur d'`int`.
- Trouver la **première occurrence** de la valeur.
- Si elle n'est pas trouvée, lancer une exception ou renvoyer une valeur d'erreur. À toi de choisir, mais regarde comment fait la STL.
- Les conteneurs associatifs (`map`, `set`) n'ont pas besoin d'être gérés.

### Extrait central

```cpp
#include <algorithm>
#include <stdexcept>

template <typename T>
typename T::iterator easyfind(T &container, int value)
{
    typename T::iterator it = std::find(container.begin(), container.end(), value);
    if (it == container.end())
        throw std::runtime_error("easyfind: value not found");
    return it;
}
```

Ligne par ligne :
- **`typename T::iterator`** : `T` est encore inconnu quand le template est analysé, donc `T::iterator` pourrait être un type *ou* un membre statique. `typename` dit au compilateur « c'est un type ». Oublie-le et clang affiche `missing 'typename' prior to dependent type name 'T::iterator'`. Les clang récents le tolèrent à quelques endroits avec un avertissement *C++20 extension*, mais avec `-Werror` c'est une erreur bloquante, et en C++98 c'est tout simplement du code faux. Voir [`TYPENAME.md`](../../../../en/cpp/lexique/TYPENAME.md) (EN).
- **`T &container`** est une référence non const, donc l'itérateur renvoyé pointe dans *ton* conteneur, pas dans une copie qui meurt au retour de la fonction. Le sujet écrit `easyfind(T, int)`, mais passer par valeur renverrait un **itérateur pendant** (*dangling iterator*).
- **`std::find`** est l'algorithme. Il renvoie `end()` quand rien ne correspond, exactement comme `ft_strchr` renvoie `NULL`.
- Lancer une exception sur « pas trouvé » utilise ce que CPP05 t'a appris. La convention propre à la STL est de renvoyer `end()`. Les deux sont autorisés, alors sois prêt à justifier ton choix.

### Utilisation

```cpp
std::vector<int> v;
v.push_back(1);
v.push_back(42);
v.push_back(3);

std::list<int> l(v.begin(), v.end());

std::cout << *easyfind(v, 42) << std::endl;
std::cout << *easyfind(l, 3) << std::endl;
try
{
    easyfind(v, 7);
}
catch (std::exception &e)
{
    std::cerr << e.what() << std::endl;
}
```

`std::list<int> l(v.begin(), v.end())` est le **constructeur par plage** (*range constructor*). Tous les conteneurs en ont un, et c'est la façon la plus simple de construire des données de test.

### ⚠️ Pièges
- **Une boucle manuelle est notée comme fausse.** La grille : *« It HAS to use STL algorithms. If it does not (like manual search using iterators for example), count it as wrong. »*
- Pour supporter les conteneurs `const`, ajoute une surcharge qui prend `T const &` et renvoie `typename T::const_iterator`.

---

## 4. ex01 — `Span`

### Ce que demande le sujet
- `Span(unsigned int N)` contient au plus N `int`.
- `addNumber(int)` lance une exception quand le Span est plein.
- `shortestSpan()` renvoie la plus petite distance entre *deux* nombres stockés quelconques. `longestSpan()` renvoie la plus grande. Les deux lancent une exception quand moins de 2 nombres sont stockés.
- Teste avec **au moins 10 000 nombres**.
- Ajoute une fonction qui insère **toute une plage d'itérateurs** en un seul appel.

```cpp
Span sp = Span(5);
sp.addNumber(6);
sp.addNumber(3);
sp.addNumber(17);
sp.addNumber(9);
sp.addNumber(11);
std::cout << sp.shortestSpan() << std::endl;   // 2   (9 and 11)
std::cout << sp.longestSpan() << std::endl;    // 14  (17 - 3)
```

### L'idée : trier, et seuls les voisins comptent

```
 stockés : 6   3   17   9   11
 triés :   3   6   9   11   17
 écarts :    3   3    2    6       ← adjacent_difference
 shortest = écart min = 2               longest = dernier - premier = 17 - 3 = 14
```

Une fois les nombres triés, la paire la plus proche **est forcément adjacente**. Un nombre entre les deux serait plus proche de l'un d'eux. Les n² comparaisons deviennent donc un `sort` plus un seul passage. La grille prévient : *« Finding the shortest span can't be done only by subtracting the two lowest numbers. »* Dans l'exemple, 3 et 6 sont les deux plus petits, avec un écart de 3, mais la réponse est 2.

### Extrait central

```cpp
class Span
{
private:
    unsigned int        _max;
    std::vector<int>    _numbers;

public:
    Span(unsigned int n);
    Span(Span const &other);
    Span &operator=(Span const &other);
    ~Span();

    void            addNumber(int n);
    unsigned int    shortestSpan() const;
    unsigned int    longestSpan() const;

    template <typename It>
    void addRange(It begin, It end)
    {
        if (std::distance(begin, end) > static_cast<long>(_max - _numbers.size()))
            throw std::length_error("Span: range does not fit");
        _numbers.insert(_numbers.end(), begin, end);
    }
};
```

```cpp
unsigned int Span::shortestSpan() const
{
    if (_numbers.size() < 2)
        throw std::logic_error("Span: need at least two numbers");
    std::vector<long> sorted(_numbers.begin(), _numbers.end());
    std::sort(sorted.begin(), sorted.end());
    std::vector<long> gaps(sorted.size());
    std::adjacent_difference(sorted.begin(), sorted.end(), gaps.begin());
    return static_cast<unsigned int>(*std::min_element(gaps.begin() + 1, gaps.end()));
}

unsigned int Span::longestSpan() const
{
    if (_numbers.size() < 2)
        throw std::logic_error("Span: need at least two numbers");
    long lo = *std::min_element(_numbers.begin(), _numbers.end());
    long hi = *std::max_element(_numbers.begin(), _numbers.end());
    return static_cast<unsigned int>(hi - lo);
}
```

Explications :
- **`addRange` est un template membre** (*member template*), donc il accepte les itérateurs de *n'importe quel* conteneur : `vector`, `list`, ou même un `int[]` brut, puisque les pointeurs sont des itérateurs. `vector::insert(pos, first, last)` fait la copie.
- **`std::distance`** compte la plage *avant* d'insérer, donc un Span ne se retrouve jamais à moitié rempli. C'est la même idée « vérifier d'abord, agir ensuite » que l'`operator=` de CPP07.
- **Le constructeur peut appeler `_numbers.reserve(n)`.** C'est une seule allocation au départ au lieu de plusieurs à mesure que le vector grandit. C'est autorisé dans CPP08 ; la règle « pas d'allocation préventive » concernait l'`Array` de CPP07.
- **`shortestSpan` est `const`.** Elle trie une *copie*, pas le membre, donc l'ordre stocké n'est pas touché.
- **`adjacent_difference`** écrit `out[0] = in[0]`, puis `out[i] = in[i] - in[i-1]`. C'est pour ça que le minimum est pris à partir de `gaps.begin() + 1`.
- **En-têtes :** `std::sort` et `std::min_element` viennent de `<algorithm>`, mais `std::adjacent_difference` vit dans `<numeric>` et `std::distance` dans `<iterator>`. La libc++ d'Apple peut te laisser les oublier parce que d'autres en-têtes les incluent, et la libstdc++ de Linux refuse alors de compiler.

### ⚠️ Piège : le dépassement (testé)
L'écart entre `INT_MIN` et `INT_MAX` vaut 4294967295, ce qui ne tient pas dans un `int`. `adjacent_difference` calcule dans le type d'élément **source**. Une première version de cet extrait triait un `std::vector<int>`, et UBSan l'a attrapé :

```
runtime error: signed integer overflow: 2147483647 - -2147483648 cannot be represented in type 'int'
```

C'est pour ça que la copie ci-dessus est un `std::vector<long>`, qui fait 64 bits sur macOS et sur Linux 64 bits, et pour ça que le type de retour est `unsigned int`. Mets ce cas dans ton `main`, parce que les évaluateurs aiment les valeurs extrêmes.

### Tester avec plus de 10 000 nombres

```cpp
std::srand(static_cast<unsigned int>(std::time(NULL)));
std::vector<int> big(10000);
for (size_t i = 0; i < big.size(); i++)
    big[i] = std::rand();
Span huge(10000);
huge.addRange(big.begin(), big.end());
std::cout << huge.shortestSpan() << " " << huge.longestSpan() << std::endl;
```

`std::generate(big.begin(), big.end(), std::rand)` remplit le vector avec un algorithme au lieu d'une boucle, ce qui est encore plus dans l'esprit STL.

---

## 5. ex02 — `MutantStack`

### Ce que demande le sujet
`std::stack` n'a pas d'itérateurs. Écris `MutantStack<T>` :
- C'**est** une `std::stack`, avec toutes ses fonctions membres.
- Elle **ajoute des itérateurs**.
- Le `main` du sujet doit afficher la même chose quand on remplace `MutantStack` par `std::list` (avec `push_back`/`back`/`pop_back`).
- `std::stack<int> s(mstack);` doit compiler.

### Comment est construit un adaptateur

```
 std::stack<T, Container = std::deque<T> >
 ┌────────────────────────────────────────────┐
 │ public:   push()  pop()  top()  size()  …  │  ← tous implémentés par-dessus `c`
 │ protected:                                 │
 │   Container c;  ──────────►  std::deque<T> │  ← le vrai stockage, AVEC itérateurs
 └────────────────────────────────────────────┘
                 ▲
                 │ classe dérivée : les membres protected y sont visibles
 ┌────────────────────────────────────────────┐
 │ MutantStack<T>                             │
 │   typedef … container_type::iterator …     │
 │   begin() { return this->c.begin(); }      │
 │   end()   { return this->c.end(); }        │
 └────────────────────────────────────────────┘
```

La norme C++ dit que le conteneur sous-jacent est un **membre `protected` nommé `c`**. C'est voulu, pour qu'une classe dérivée puisse l'atteindre. L'exercice revient en réalité à « lire la définition de `std::stack` dans la norme ».

### Extrait central

```cpp
#include <stack>

template <typename T>
class MutantStack : public std::stack<T>
{
public:
    MutantStack() {}
    MutantStack(MutantStack const &other) : std::stack<T>(other) {}
    MutantStack &operator=(MutantStack const &other)
    {
        std::stack<T>::operator=(other);
        return *this;
    }
    ~MutantStack() {}

    typedef typename std::stack<T>::container_type::iterator                iterator;
    typedef typename std::stack<T>::container_type::const_iterator          const_iterator;
    typedef typename std::stack<T>::container_type::reverse_iterator        reverse_iterator;
    typedef typename std::stack<T>::container_type::const_reverse_iterator  const_reverse_iterator;

    iterator                begin()         { return this->c.begin(); }
    iterator                end()           { return this->c.end(); }
    const_iterator          begin() const   { return this->c.begin(); }
    const_iterator          end() const     { return this->c.end(); }
    reverse_iterator        rbegin()        { return this->c.rbegin(); }
    reverse_iterator        rend()          { return this->c.rend(); }
    const_reverse_iterator  rbegin() const  { return this->c.rbegin(); }
    const_reverse_iterator  rend() const    { return this->c.rend(); }
};
```

Explications :
- **`: public std::stack<T>`** signifie que `MutantStack` reçoit `push`, `pop`, `top`, `size` et `empty` gratuitement. L'héritage public est aussi ce qui fait compiler `std::stack<int> s(mstack)` : une `MutantStack` *est une* `stack`, donc le constructeur de copie de stack l'accepte.
- **`std::stack<T>::container_type`** est le typedef que la norme donne pour le conteneur enveloppé (`std::deque<T>`). Son `::iterator` est un *type dépendant*, donc `typename` est de nouveau obligatoire.
- **`this->c`, pas simplement `c`.** La classe de base dépend de `T`, donc le C++ n'y cherche pas les noms automatiquement. Écris `c` tout court et tu obtiens `use of undeclared identifier 'c'`. `this->` repousse la recherche jusqu'à l'instanciation de la classe. C'est l'erreur la plus fréquente de cet exercice.
- **OCF :** tout délègue à la copie et à l'affectation propres à `std::stack`, qui copient `c`.
- Les corps des membres sont **dans le template de classe**. C'est autorisé, parce que la règle « pas d'implémentation dans les en-têtes » exempte les templates.

### Sortie vérifiée du `main` du sujet

```cpp
MutantStack<int> mstack;
mstack.push(5);
mstack.push(17);
std::cout << mstack.top() << std::endl;
mstack.pop();
std::cout << mstack.size() << std::endl;
mstack.push(3);
mstack.push(5);
mstack.push(737);
mstack.push(0);
MutantStack<int>::iterator it = mstack.begin();
MutantStack<int>::iterator ite = mstack.end();
++it;
--it;
while (it != ite)
{
    std::cout << *it << std::endl;
    ++it;
}
std::stack<int> s(mstack);
```

```
17
1
5
3
5
737
0
```

Le parcours va **du bas vers le haut** (5 a été empilé en premier), parce qu'il parcourt la deque du début à la fin. Pour prouver l'équivalence avec `std::list`, écris la version list à côté dans ton `main` et fais un `diff` des deux sorties.

### ⚠️ Pièges
- `std::stack` n'a **pas de destructeur virtuel**. Ne `delete` jamais une `MutantStack` via un `std::stack *`. Garde-la sur la pile, comme le fait le sujet.
- « De meilleurs tests » est une ligne à part entière de la grille. Bons candidats : parcours const, parcours inverse, une `MutantStack<std::string>`, copier puis modifier, et la comparaison avec `std::list`.

---

## 6. Ce que vérifie l'évaluateur

D'après la grille de notation officielle de 42evalhub :

**Prérequis**
- [ ] `c++ -Wall -Wextra -Werror`, C++98.
- [ ] Aucun corps de fonction dans un en-tête, **sauf les templates**.
- [ ] Pas de `*alloc`, `*printf` ni `free`. Pas de `using namespace`. Pas de `friend`. Aucune bibliothèque externe.
- [ ] Aucune fuite.

**Pour chaque exercice :** un `main` avec **suffisamment de tests**, et **toute classe qui n'est pas une interface en OCF**. Sinon l'exercice n'est pas noté.

**ex00**
- [ ] Un template de fonction `easyfind(T, int)` qui fait ce que demande le sujet.
- [ ] Il **utilise un algorithme de la STL**. Une boucle manuelle sur les itérateurs est comptée comme fausse.

**ex01**
- [ ] La classe respecte les contraintes du sujet. Les membres utilisent **les algorithmes de la STL autant que possible**.
- [ ] `shortestSpan` n'est pas simplement « les deux plus petits nombres soustraits ».
- [ ] Il existe un meilleur moyen d'ajouter beaucoup de nombres que d'appeler `addNumber` en boucle (une insertion par plage).

**ex02**
- [ ] `MutantStack` **hérite de `std::stack`** et offre tous ses membres.
- [ ] Elle a un itérateur. Au moins les opérations de l'exemple du sujet fonctionnent.
- [ ] `main` contient **plus de tests que celui du sujet**.

**Questions de soutenance à préparer**
- « Pourquoi `vector` pour Span ? » → Mémoire contiguë, `sort` rapide, `reserve` peu coûteux.
- « D'où vient `c` ? » → La norme fait du conteneur sous-jacent un membre protected de `std::stack`.
- « Pourquoi `typename` ? » / « Pourquoi `this->c` ? » → Noms dépendants (§3, §5).

---

## 7. Aide-mémoire

| Besoin | Écriture C++98 |
|---|---|
| Trouver une valeur | `std::find(c.begin(), c.end(), v)` → `end()` si absente |
| Type d'itérateur d'un paramètre de template | `typename T::iterator` |
| Plus petit / plus grand | `*std::min_element(b, e)` / `*std::max_element(b, e)` |
| Différences consécutives | `std::adjacent_difference(b, e, out)` |
| Compter les éléments d'une plage | `std::distance(first, last)` |
| Ajouter une plage à la fin | `v.insert(v.end(), first, last)` |
| Construire depuis une plage | `std::list<int> l(v.begin(), v.end())` |
| Atteindre un membre d'une base template | `this->c` |
| Conteneur enveloppé par un adaptateur | `std::stack<T>::container_type` |

**Ce que ce module te laisse :** avant d'écrire une boucle, tu te demandes « c'est quel algorithme ? ». CPP09 enlève les petites roues : les conteneurs sont les outils, et c'est leur *choix* qui est noté.

➡️ Suivant : [`CPP09.md`](CPP09.md). Trois vrais programmes, chacun avec un conteneur que tu dois défendre.
