# C++ Module 02 — Guide d'apprentissage

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP02.md)

**Thème du module :** *polymorphisme ad hoc, surcharge d'opérateurs
et forme canonique orthodoxe (OCF).*

C'est dans ce module que les classes cessent d'être des structs C
avec des méthodes pour devenir des **valeurs** — des objets qu'on
peut additionner, comparer, envoyer dans un flux et copier comme des
nombres natifs. Le support, c'est une classe de nombres à virgule
fixe `Fixed`. Les maths sont du décor ; le vrai apprentissage, c'est
OCF + surcharge d'opérateurs + const-correctness.

Voir aussi : le `TOPICS.md` du module dans [`LEVEL4/C++1/Mod02/`](../../../../../LEVEL4/C++1/Mod02/TOPICS.md)
pour l'approfondissement exercice par exercice, et [`FLOATING.md`](../../../../../LEVEL4/C++1/Mod02/FLOATING.md)
pour le contexte virgule flottante / virgule fixe.

---

## Forme canonique orthodoxe — les quatre membres que tu écriras 100 fois

Toute classe à partir de Mod 02 doit fournir :

1. **Constructeur par défaut** — `Fixed()`
2. **Constructeur de copie** — `Fixed(Fixed const& src)`
3. **Opérateur d'affectation par copie** — `Fixed& operator=(Fixed const& other)`
4. **Destructeur** — `~Fixed()`

Pourquoi les quatre, alors que le compilateur les générerait pour
toi ? Parce que la moulinette note les **traces** que tu affiches
dans chacun. Les versions générées sont muettes, donc tu ne peux pas
les réutiliser.

Fiche complète : [`oop/ORTHODOX_CANONICAL_FORM.md`](../../notions/oop/ORTHODOX_CANONICAL_FORM.md).

L'idiome canonique dans `operator=` :

```cpp
Fixed& Fixed::operator=(Fixed const& other) {
    if (this != &other)       // self-assignment guard
        m_raw = other.m_raw;
    return *this;             // allow chaining: a = b = c;
}
```

Pour une classe qui ne possède que des types primitifs (le `Fixed`
de Mod 02), la garde est du théâtre. Pour une classe qui possède de
la mémoire sur le tas (le `Dog` de Mod 04), elle évite un double
free. Prends l'habitude dès maintenant.

---

## Exercice 00 — Ma première classe en OCF

### Ce que tu apprends
**Écrire une classe minimale en OCF complète**, avec des traces,
en respectant la séparation en-tête / implémentation du projet.

### Forme de la classe
- Privé : `int m_raw`, `static const int m_fract_bits = 8`.
- Public : les quatre membres de l'OCF + `int getRawBits() const` et
  `void setRawBits(int const raw)`.

### Astuces
- Initialise `m_raw` dans la **liste d'initialisation** du
  constructeur quand c'est possible : `Fixed() : m_raw(0) { ... }`.
  Liste d'initialisation avant le corps, c'est l'idiome C++.
- `static const int m_fract_bits = 8;` est une *déclaration +
  initialisation* en une seule ligne dans l'en-tête — une exception
  accordée aux `const` entiers en c++98. Pas besoin de définition
  hors de la classe.
- `getRawBits()` est `const` — la méthode promet de ne pas modifier
  `*this`. Tout accesseur qui ne fait que lire doit être `const`.

### Piège
Le sujet veut que chaque membre de l'OCF et `getRawBits` affichent
une trace. `setRawBits` n'en affiche généralement pas. Relis la
sortie d'exemple.

---

## Exercice 01 — Une classe à virgule fixe utile

### Ce que tu apprends
**Convertir entre `int` / `float` et la représentation `Fixed`,
et surcharger `operator<<`** comme fonction non membre.

### Les quatre conversions, au même endroit

```cpp
raw_from_int(int n)       →  n << 8
raw_from_float(float f)   →  roundf(f * 256)
to_int(int raw)           →  raw >> 8
to_float(int raw)         →  raw / 256.0f
```

`<< 8`, c'est un décalage à gauche de 8 = multiplier par 256. `>> 8`
est un décalage arithmétique à droite sur un int signé (avec
extension de signe). `roundf` vient de `<cmath>`.

### `operator<<` comme fonction libre

```cpp
std::ostream& operator<<(std::ostream& os, Fixed const& f) {
    os << f.toFloat();
    return os;
}
```

- **Libre (non membre)**, parce que l'opérande de gauche est un
  `std::ostream&` — tu ne peux pas ajouter un membre à `ostream`.
- **Renvoie le flux** pour que l'enchaînement marche :
  `std::cout << a << b`.
- **Pas besoin de `friend`** si elle passe par des accesseurs
  publics (`toFloat()`).

### Astuces
- **Piège d'ordre des opérations dans le constructeur float.**
  Applique `roundf` *avant* la conversion en `int`, pas après. La
  bonne forme est `static_cast<int>(roundf(f * 256))`. Écrire
  `roundf(static_cast<int>(f * 256))` arrondit une valeur déjà
  tronquée — ça ne fait rien.
- **`std::cout` affiche les floats avec 6 chiffres significatifs
  par défaut.** `Fixed(3.14f)` stocké en raw `804` s'affiche
  `3.14062` — c'est la résolution Q8, pas un bug.
- **Ambiguïté :** `Fixed a(42.0)` (un littéral `double`) est ambigu
  avec `Fixed(int)` + `Fixed(float)`. Les tests du sujet n'utilisent
  que `int` et `float`, donc tu n'as pas besoin d'une surcharge
  `double`.

### Piège
Inclus `<cmath>` pour `std::roundf`. Pas `<math.h>` — convention C++.

---

## Exercice 02 — « Now we're talking »

### Ce que tu apprends
Le **jeu complet d'opérateurs** sur une classe qui se comporte comme
un nombre : comparaisons, arithmétique, incrémentation /
décrémentation, et **`min` / `max` statiques** avec surcharges const
et non const.

Voir [`oop/OPERATOR_OVERLOADING.md`](../../notions/oop/OPERATOR_OVERLOADING.md).

### Comparaisons — six opérateurs, six lignes

```cpp
bool Fixed::operator<(Fixed const& rhs) const {
    return m_raw < rhs.m_raw;
}
```

Les deux opérandes partagent la même échelle, donc comparer les
entiers bruts donne la même réponse que comparer les valeurs réelles.
Les six (`<`, `>`, `<=`, `>=`, `==`, `!=`) sont des variantes de
cette ligne.

### Arithmétique — calcul sur le brut, avec un décalage pour `*` et `/`

| Opérateur | Formule brute          | Pourquoi le décalage |
|----------|----------------------|---------------|
| `a + b`  | `a_raw + b_raw`      | les échelles concordent |
| `a - b`  | `a_raw - b_raw`      | les échelles concordent |
| `a * b`  | `(a_raw * b_raw) >> 8` | le produit brut est à double échelle, on décale à droite pour compenser |
| `a / b`  | `(a_raw << 8) / b_raw` | le quotient brut perd l'échelle, on décale à gauche pour la rétablir |

`*` et `/` sont les plus intéressants. La preuve tient en une ligne
d'arithmétique — voir [`TOPICS.md` de Mod02 §ex02](../../../../../LEVEL4/C++1/Mod02/TOPICS.md).

### Incrémentation / décrémentation — pré vs post

```cpp
Fixed& Fixed::operator++() {          // pre: ++a
    ++m_raw;
    return *this;
}
Fixed Fixed::operator++(int) {        // post: a++
    Fixed tmp(*this);
    ++m_raw;
    return tmp;
}
```

Le paramètre `int` de la forme post est une **étiquette** que le
compilateur utilise pour choisir la surcharge — il n'est jamais lu.
La forme pré renvoie une référence ; la forme post renvoie une copie
de l'état d'avant.

Le pas est le **plus petit ε représentable** = raw ±1 = `1/256`
en valeur réelle.

### `min` / `max` statiques — deux surcharges chacun

```cpp
static Fixed&       min(Fixed& a, Fixed& b);         // non-const
static Fixed const& min(Fixed const& a, Fixed const& b);  // const
```

Les deux versions sont nécessaires — la non const permet à
l'appelant de modifier la référence renvoyée
(`Fixed::min(x, y) = z;`), la version const fonctionne sur des
entrées const.

### Astuces
- `operator+` renvoie par **valeur** (un temporaire tout neuf) ;
  `operator=` renvoie par **référence** (`*this`). La différence a une
  raison — lis [`oop/OPERATOR_OVERLOADING.md §5`](../../notions/oop/OPERATOR_OVERLOADING.md).
- Déclare tous les opérateurs de comparaison `const` — ils ne
  modifient pas leurs opérandes.
- N'utilise pas `std::min` ni `std::max` (pas de STL). Écris les
  versions de la classe.

### Piège
`Fixed(256) * Fixed(256)` donne un produit brut
`65536 * 65536 = 2^32` — dépassement d'un int signé 32 bits. Les
tests du sujet restent bien dans la zone sûre, mais connais la limite.

---

## Exercice 03 — BSP (bonus)

### Ce que tu apprends
**Utiliser `Fixed` pour de la géométrie** : déterminer si un point
se trouve *strictement* à l'intérieur d'un triangle.

### Forme de la classe
`Point` avec des membres privés `Fixed const x` et `Fixed const y` —
le `const` sur les membres est la subtilité de l'exercice.

### Le piège des membres `const`
`Fixed const x;` signifie que `x` ne peut pas être affecté après la
construction. Donc `operator=` ne peut pas faire son travail
habituel :

```cpp
Point& Point::operator=(Point const&) {
    // x = other.x;  // ← compile error
    return *this;
}
```

**Échappatoire acceptée :** un `operator=` au corps vide qui renvoie
`*this`. La classe est de fait immuable après construction ; c'est
l'intention de conception.

### Point dans un triangle — signe des produits vectoriels

Étant donnés un triangle `abc` et un point `p` :

```
d1 = cross(a, b, p)
d2 = cross(b, c, p)
d3 = cross(c, a, p)

strictement à l'intérieur ⟺ les trois signes concordent ET aucun n'est nul
```

`cross(a, b, p) = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x)`.

Positif = `p` est à gauche de l'arête orientée `a→b`. Négatif = à
droite. Zéro = sur la droite qui porte l'arête.

### Pourquoi `Fixed` (et pas `float`) ici
Deux raisons :
1. **Déterminisme** d'une machine et d'un compilateur à l'autre — les
   calculs entiers ne dérivent pas.
2. **Test « sur l'arête » fiable.** `d == 0` est exact en virgule
   fixe ; avec des floats, les erreurs d'arrondi le font clignoter.

### Astuces
- `bsp` est une **fonction libre**, pas un membre. Elle vit dans
  `bsp.cpp`, déclarée dans un en-tête que `main` inclut.
- Le sens de parcours (`abc` horaire ou antihoraire) n'a pas
  d'importance — « les trois signes concordent » couvre les deux cas.
- Triangle dégénéré (`a, b, c` alignés) → les trois produits sont
  nuls → renvoie `false`. Très bien.

### Piège
« Strictement à l'intérieur » signifie que **les sommets et les
arêtes renvoient `false`**. Ajoute les tests `d_i != 0`, pas
seulement la concordance des signes.

---

## Ce que ce module te laisse

- Tu écris une OCF sans y réfléchir.
- Tu sais quels opérateurs sont des membres et lesquels sont des
  fonctions libres.
- La const-correctness est automatique, plus une corvée.
- Tu as compris que la virgule fixe est une idée simple (un entier
  avec une virgule implicite) et un outil puissant (géométrie
  déterministe, égalités exactes).

Tout ce qui vient à partir de Mod 03 **suppose** que tu as intégré
l'OCF et la surcharge d'opérateurs. Si elles te semblent encore
maladroites, repasse par ex00/01 avant d'attaquer Mod 03.
