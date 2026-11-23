# Surcharge d'opérateurs — Guide complet

> 🇬🇧 [English version](../../../../en/cpp/notions/oop/OPERATOR_OVERLOADING.md)

> **Zooms sur les mots-clés :** [`OPERATOR`](../../../../en/cpp/lexique/OPERATOR.md) (EN) · [`FRIEND`](../../../../en/cpp/lexique/FRIEND.md) (EN) · [`EXPLICIT`](../../../../en/cpp/lexique/EXPLICIT.md) (EN)

La surcharge d'opérateurs (*operator overloading*) te permet de donner un sens aux opérateurs intégrés (`+`, `==`, `<<`, `[]`, …) quand on les applique à ta classe.

```cpp
Fixed a(3);
Fixed b(4);
Fixed c = a + b;             // a.operator+(b) — because you defined it
std::cout << c;              // operator<<(std::cout, c) — because you defined it
```

Sous le capot, `a + b` n'est *qu'*un appel de fonction : `operator+(a, b)` ou `a.operator+(b)`. Tout le reste en découle.

---

## Table des matières

1. [Pourquoi surcharger des opérateurs ?](#1.%20Pourquoi%20surcharger%20des%20op%C3%A9rateurs%20%3F)
2. [La liste des opérateurs surchargeables](#2.%20La%20liste%20des%20op%C3%A9rateurs%20surchargeables)
3. [Membre ou non-membre](#3.%20Membre%20ou%20non-membre)
4. [Signatures canoniques](#4.%20Signatures%20canoniques)
5. [Opérateurs arithmétiques](#5.%20Op%C3%A9rateurs%20arithm%C3%A9tiques)
6. [Affectation composée (`+=`, `-=`, …)](#6.%20Affectation%20compos%C3%A9e%20%28%2B%3D%2C%20-%3D%2C%20%E2%80%A6%29)
7. [Opérateurs de comparaison](#7.%20Op%C3%A9rateurs%20de%20comparaison)
8. [Incrémentation et décrémentation (préfixe ou suffixe)](#8.%20Incr%C3%A9mentation%20et%20d%C3%A9cr%C3%A9mentation%20%28pr%C3%A9fixe%20ou%20suffixe%29)
9. [Insertion et extraction de flux (`<<`, `>>`)](#9.%20Insertion%20et%20extraction%20de%20flux%20%28%3C%3C%2C%20%3E%3E%29)
10. [Indice `[]`, appel de fonction `()`](#10.%20Indice%20%5B%5D%2C%20appel%20de%20fonction%20%28%29)
11. [Opérateur d'affectation `=`](#11.%20Op%C3%A9rateur%20d%27affectation%20%3D)
12. [Opérateurs de conversion](#12.%20Op%C3%A9rateurs%20de%20conversion)
13. [Dans CPP02 — la classe Fixed](#13.%20Dans%20CPP02%20%E2%80%94%20la%20classe%20Fixed)
14. [Règles pratiques](#14.%20R%C3%A8gles%20pratiques)
15. [Pièges](#15.%20Pi%C3%A8ges)

---

## 1. Pourquoi surcharger des opérateurs ?

Parce que les types utilisateur qui ressemblent à des nombres, qu'on compare ou qu'on affiche **se lisent plus naturellement** avec des opérateurs qu'avec des méthodes nommées :

```cpp
// Without overloading
result = sum(mul(a, b), c);

// With overloading
result = a * b + c;
```

Le compilateur traduit la seconde forme en la première. Même code machine, clarté différente.

**Ne surcharge pas pour le plaisir.** Seulement quand l'opérateur a un sens évident et universellement admis pour ton type. `+` sur `Fixed`, c'est bien. `+` sur `Car` pour dire « ajouter une option » rendra tout le monde malheureux.

---

## 2. La liste des opérateurs surchargeables

Tu peux surcharger presque tous les opérateurs :

```
Arithmétique :   +   -   *   /   %
Comparaison :    ==  !=  <   >   <=  >=
Logique :        !   &&  ||
Bit à bit :      &   |   ^   ~   <<  >>
Composés :       +=  -=  *=  /=  %=  &=  |=  ^=  <<= >>=
Incrément :      ++  --
Indice :         []
Appel :          ()
	Membre :         ->  ->*
Affectation :    =
Adresse :        &                (rarement une bonne idée d'y toucher)
Flux :           <<  >>           (décalage, réutilisé pour les E/S)
Allocation :     new  delete  new[]  delete[]
Virgule :        ,                (rarement une bonne idée non plus)
```

Ceux qu'on **ne peut pas** surcharger :
```
::   .   .*   ?:   sizeof   typeid
```
Et tu ne peux pas inventer de nouveaux opérateurs.

---

## 3. Membre ou non-membre

La plupart des opérateurs binaires peuvent s'écrire des deux façons. C'est le type de l'**opérande de gauche** qui décide.

### Fonction membre

L'opérande de gauche est `*this` :

```cpp
class Fixed {
public:
    Fixed operator+(const Fixed& rhs) const;   // a + b  →  a.operator+(b)
};
```

### Fonction non-membre (libre)

Les deux opérandes sont des paramètres explicites :

```cpp
Fixed operator+(const Fixed& lhs, const Fixed& rhs);   // a + b  →  operator+(a, b)
```

### Quand tu **dois** utiliser un non-membre

Quand l'opérande de gauche n'est pas ta classe :

```cpp
std::cout << f;    // left operand is std::ostream — NOT your class
                   // → must be a non-member
```

Donc `operator<<` et `operator>>` sont toujours des fonctions libres.

### Accès aux membres privés

Un `operator+` non-membre ne voit pas directement les champs privés. Deux solutions :
- Le construire à partir des méthodes publiques (`toRaw()`, etc.)
- Le déclarer `friend` :
  ```cpp
  class Fixed {
      int _raw;
      friend Fixed operator+(const Fixed& a, const Fixed& b);
  };
  ```

---

## 4. Signatures canoniques

Mémorise-les une fois ; tu les écriras cent fois.

```cpp
// Binary arithmetic — returns new valual e, both sides const
T   operator+(const T& lhs, const T& rhs);

// Compound assignment — modifies *this, returns *this by reference
T&  T::operator+=(const T& rhs);

// Comparison — always bool, both sides const
bool operator==(const T& lhs, const T& rhs);

// Stream insertion — non-member, returns the stream by reference
std::ostream& operator<<(std::ostream& os, const T& v);

// Stream extraction — reads into a non-const T
std::istream& operator>>(std::istream& is, T& v);

// Assignment — returns *this by reference, takes const T&
T& T::operator=(const T& rhs);

// Prefix ++/--
T& T::operator++();                     // ++a
// Postfix ++/--
T  T::operator++(int);                  // a++ (dummy int disambiguates)

// Subscript — usually two overloads: const and non-const
T& T::operator[](size_t i);
const T& T::operator[](size_t i) const;
```

Ces types de retour comptent — ce sont eux qui font marcher les expressions enchaînées : `a = b = c`, `std::cout << a << b`.

---

## 5. Opérateurs arithmétiques

Schéma canonique : **définis `+=` comme membre, puis implémente `+` à partir de lui comme non-membre.** Aucune logique dupliquée.

```cpp
class Fixed {
    int _raw;
public:
    Fixed& operator+=(const Fixed& rhs) { _raw += rhs._raw; return *this; }
    int    toRaw() const { return _raw; }
};

Fixed operator+(Fixed lhs, const Fixed& rhs) {   // lhs by value — copy we'll modify
    lhs += rhs;
    return lhs;
}
```

### Pourquoi `+` renvoie par valeur, et pas par référence

`a + b` crée un *nouvel* objet. Renvoyer une référence, ce serait renvoyer une référence vers une variable locale → référence pendante.

### Pourquoi `+=` renvoie `*this` par référence

Pour pouvoir enchaîner : `(a += b) += c;`. C'est aussi la convention standard du C++ — le même comportement que les types intégrés.

---

## 6. Affectation composée (`+=`, `-=`, …)

Ce sont les membres naturels. Implémente-les en premier :

```cpp
Fixed& Fixed::operator+=(const Fixed& rhs) { _raw += rhs._raw;  return *this; }
Fixed& Fixed::operator-=(const Fixed& rhs) { _raw -= rhs._raw;  return *this; }
Fixed& Fixed::operator*=(const Fixed& rhs) { _raw = (_raw * rhs._raw) >> SCALE; return *this; }
Fixed& Fixed::operator/=(const Fixed& rhs) { _raw = (_raw << SCALE) / rhs._raw; return *this; }
```

Puis déduis-en les formes binaires :

```cpp
Fixed operator+(Fixed lhs, const Fixed& rhs) { return lhs += rhs; }
Fixed operator-(Fixed lhs, const Fixed& rhs) { return lhs -= rhs; }
// etc.
```

---

## 7. Opérateurs de comparaison

Tous renvoient un `bool`, tous prennent `const T&` des deux côtés. Par cohérence, définis-les comme un **ensemble cohérent**. En général, tu écris `==` et `<` de zéro, puis tu en déduis le reste :

```cpp
bool operator==(const Fixed& a, const Fixed& b) { return a.toRaw() == b.toRaw(); }
bool operator< (const Fixed& a, const Fixed& b) { return a.toRaw() <  b.toRaw(); }

bool operator!=(const Fixed& a, const Fixed& b) { return !(a == b); }
bool operator> (const Fixed& a, const Fixed& b) { return   b <  a;  }
bool operator<=(const Fixed& a, const Fixed& b) { return !(b <  a); }
bool operator>=(const Fixed& a, const Fixed& b) { return !(a <  b); }
```

C'est l'idiome C++98. (Le C++20 ajoute `operator<=>` pour condenser tout ça, mais tu ne l'as pas.)

---

## 8. Incrémentation et décrémentation (préfixe ou suffixe)

Le préfixe (`++a`) et le suffixe (`a++`) ont besoin de signatures distinctes. Le paramètre `int` factice les distingue :

```cpp
class Fixed {
public:
    Fixed& operator++()    { ++_raw; return *this; }        // prefix
    Fixed  operator++(int) { Fixed tmp(*this); ++_raw; return tmp; }  // postfix
};
```

- Le **préfixe** renvoie l'objet par référence, après modification.
- Le **suffixe** sauvegarde l'ancienne valeur, modifie l'objet, puis renvoie l'ancienne valeur par **valeur**.

**Astuce perf :** le préfixe est plus rapide — il renvoie une référence, le suffixe fait une copie. Préfère `++it` à `it++` dans les boucles.

---

## 9. Insertion et extraction de flux (`<<`, `>>`)

Toujours non-membres. Ils renvoient toujours le flux par référence (pour que l'enchaînement marche).

```cpp
std::ostream& operator<<(std::ostream& os, const Fixed& f) {
    os << f.toFloat();
    return os;
}

std::istream& operator>>(std::istream& is, Fixed& f) {
    float x;
    is >> x;
    f = Fixed(x);
    return is;
}

std::cout << a << b << '\n';   // works because each call returns std::cout&
```

Si l'opérateur a besoin d'accéder au privé, déclare-le `friend`.

---

## 10. Indice `[]`, appel de fonction `()`

### Indice

En général une paire — une version `const`, une non :

```cpp
class Array {
    int* _data;
public:
    int&       operator[](size_t i)       { return _data[i]; }
    const int& operator[](size_t i) const { return _data[i]; }
};

Array       a(10);   a[0] = 42;
const Array b(10);   int v = b[0];   // calls the const overload
```

### Appel de fonction — les foncteurs

Surcharger `operator()` rend un objet **appelable** :

```cpp
struct Adder {
    int base;
    Adder(int n) : base(n) {}
    int operator()(int x) const { return base + x; }
};

Adder add5(5);
add5(10);    // 15 — calls Adder::operator()
```

C'est ainsi que la STL recevait des « fonctions » avant l'arrivée des lambdas : `std::sort(..., MyComparator());`.

---

## 11. Opérateur d'affectation `=`

Il fait partie de l'OCF. Voir [ORTHODOX_CANONICAL_FORM.md](ORTHODOX_CANONICAL_FORM.md). Le squelette :

```cpp
Fixed& Fixed::operator=(const Fixed& rhs) {
    if (this != &rhs) {        // self-assignment check
        _raw = rhs._raw;
    }
    return *this;
}
```

Points clés :
- **Il prend un `const T&`**, parce que tu ne modifies pas la source.
- **Il renvoie un `T&` vers `*this`**, pour que `a = b = c` fonctionne.
- **La vérification d'auto-affectation** (`this != &rhs`) évite des bugs quand on copie des ressources possédées sur le tas.

---

## 12. Opérateurs de conversion

Ils permettent à ta classe de se convertir *vers* un autre type :

```cpp
class Fixed {
public:
    operator float() const { return _raw / (float)(1 << SCALE); }
    operator int()   const { return _raw >> SCALE; }
};

Fixed f(3);
float x = f;             // implicit: calls operator float()
int   i = static_cast<int>(f);
```

Note : les opérateurs de conversion n'ont **pas de type de retour** dans la syntaxe — le nom *est* le type cible.

**Attention :** les opérateurs de conversion implicites sont le miroir des constructeurs implicites. Si tu ne veux pas de conversions silencieuses, à 42 tu dois parfois les marquer (le C++11 ajoute `explicit` pour les opérateurs de conversion ; le C++98 n'a rien de tel — c'est pour ça qu'ils peuvent te surprendre).

---

## 13. Dans CPP02 — la classe Fixed

Tout le module est une étude de la surcharge d'opérateurs.

**ex02** — tu implémentes, sur une classe à virgule fixe :
- `operator+`, `-`, `*`, `/` (arithmétique)
- `operator==`, `!=`, `<`, `>`, `<=`, `>=` (les six comparaisons)
- `operator++`, `--` (préfixe et suffixe)
- `operator<<` (affichage)
- Deux surcharges statiques `min`/`max`

**ex03** — la classe `Fixed` devient une brique de base pour `Point` et les tests de triangle. Le but : que tous les opérateurs paraissent naturels : `(a + b) / 2`, `if (p1 == p2)`, `std::cout << f`.

Le bon ordre pour les construire :
1. L'OCF (constructeur par défaut, constructeur de copie, `operator=`, destructeur).
2. `toFloat()` / `toInt()` / `toRaw()` — les accesseurs.
3. `operator<<` — pour pouvoir faire `std::cout` de ta classe dès le premier jour (ça simplifie énormément le débogage).
4. `operator==`, `<` — la base de toutes les comparaisons.
5. Les quatre autres comparaisons, à partir de `==` et `<`.
6. `operator+=`, `-=`, `*=`, `/=` — puis on en déduit `+`, `-`, `*`, `/`.
7. `operator++`, `--` — les deux formes.

---

## 14. Règles pratiques

- **Imite la sémantique des types intégrés.** `+` ne doit pas avoir d'effets de bord. `==` doit être réflexif et symétrique. `<` doit être un ordre faible strict.
- **`+=` avant `+`.** Écris d'abord l'opérateur composé, déduis-en le binaire.
- **Les types de retour comptent.** Arithmétique binaire : par valeur. Affectation composée : par référence vers `*this`. Comparaisons : `bool`. Flux : le flux par référence.
- **Mets `const` tout ce qui peut l'être.** Une comparaison ne modifie rien ; chaque paramètre et `*this` doivent être `const`.
- **Non-membre dès que l'opérande de gauche peut ne pas être ta classe** (flux, arithmétique nombre-classe).
- **`friend` seulement si nécessaire.** Si un opérateur libre peut s'écrire avec les méthodes publiques, n'en fais pas un friend.

---

## 15. Pièges

- **Mélanger des surcharges membre et non-membre du même opérateur** crée des ambiguïtés. Choisis une seule forme.
- **Un `operator=` sans vérification d'auto-affectation** casse de façon bizarre quand tu écris `obj = obj` (ou quand `obj` est aliasé via un autre pointeur).
- **Un `operator<<` qui prend un `T&` non const** ne peut pas afficher de temporaires. Toujours `const T&`.
- **Un `++`/`--` suffixe qui renvoie une référence**, c'est un UB — tu renverrais une référence vers une variable locale.
- **Surcharger `&&` ou `||`** fait perdre l'évaluation en court-circuit ; les versions surchargées deviennent de simples appels de fonction, qui évaluent les deux côtés. Ne le fais pas.
- **`operator,`** a l'air inoffensif et c'est un piège sans fond. Ne le surcharge pas.
- **Opérateurs de conversion + constructeur** peuvent créer des ambiguïtés : avec `Fixed(int)` et `operator int() const`, `a + 1` a deux interprétations valides. Marque les constructeurs `explicit` quand ils ne sont pas censés convertir en silence.
- **Un `operator[]` qui renvoie par valeur** fait échouer `a[0] = 42` en silence (tu affecterais un temporaire). Renvoie toujours par référence.

---

## 16. Vue matérielle — ce que donnent les opérateurs surchargés une fois compilés

### `operator+` sur une classe quasi primitive

```cpp
Fixed Fixed::operator+(Fixed const& rhs) const {
    Fixed r;
    r.setRawBits(m_raw + rhs.m_raw);
    return r;
}
```

Se compile (avec RVO) à peu près en :

```asm
Fixed::operator+(Fixed const&):
    mov  eax, [rdi]           ; this->raw
    add  eax, [rsi]           ; + rhs.raw
    mov  [rdx], eax           ; store into the caller's return slot
    ret
```

Ici, `rdx` est l'emplacement de « retour par valeur » que l'appelant
a préalloué. Aucun `Fixed` temporaire n'est construit puis copié —
la RVO le construit sur place. C'est pour ça que la trace du
constructeur de copie **ne s'affiche pas** quand tu écris
`Fixed c = a + b;`.

### `operator*` avec le décalage Q8

```cpp
Fixed Fixed::operator*(Fixed const& rhs) const {
    Fixed r;
    r.setRawBits((m_raw * rhs.m_raw) >> 8);
    return r;
}
```

```asm
Fixed::operator*(Fixed const&):
    mov  eax, [rdi]
    imul eax, [rsi]           ; signed multiply
    sar  eax, 8               ; arithmetic shift right
    mov  [rdx], eax
    ret
```

Un `imul`, un `sar`. Sur un x86 moderne, `imul` prend 3 cycles,
`sar` 1 cycle. La multiplication en virgule fixe complète fait
~5 cycles. À comparer avec une multiplication flottante (`mulss`) :
~5 cycles aussi, mais avec des blocages de 20 cycles sur les
dénormaux et la gestion des NaN. La virgule fixe gagne quand tu
travailles sur les pipelines entiers.

### `operator<<` pour l'insertion dans un flux

```cpp
std::ostream& operator<<(std::ostream& os, Fixed const& f) {
    os << f.toFloat();
    return os;
}
```

Se compile en un appel à `std::ostream::operator<<(float)`, précédé
du corps de `Fixed::toFloat()` (en général inliné). L'opérateur de
flux n'est **pas** virtuel — la résolution se fait à la compilation
d'après le type `float` de l'argument. C'est pour ça que
l'extensibilité des flux « marche toute seule » : ajoute une
surcharge `operator<<` pour ton type, le compilateur la trouve grâce
à la recherche dépendante des arguments (*argument-dependent lookup*).

### Pré-incrémentation contre post-incrémentation — la différence de coût

```cpp
Fixed& operator++();      // pre
Fixed  operator++(int);   // post
```

Pré-incrémentation : **une écriture** (`++m_raw`), on renvoie
`*this`. Une lecture (la référence renvoyée) au point d'appel.

Post-incrémentation : **un constructeur de copie** (la sauvegarde
d'avant), **une écriture** (`++m_raw`), **un destructeur** sur le
temporaire après que l'appelant l'a lu. Trois opérations contre une.

Pour un `Fixed` (un seul `int`), la copie est gratuite et la
différence disparaît. Pour une classe avec un membre `std::string`,
la copie est une allocation. **Règle : préfère `++i` à `i++` quand
tu n'as pas besoin de l'ancienne valeur.** Les boucles en
pré-incrémentation sont un petit gain gratuit sur tout objet qui
alloue dans son constructeur de copie.

### Opérateurs de comparaison — le chemin rapide

```cpp
bool operator<(Fixed const& rhs) const { return m_raw < rhs.m_raw; }
```

```asm
Fixed::operator<(Fixed const&):
    mov  eax, [rdi]
    cmp  eax, [rsi]
    setl al
    ret
```

Trois instructions. Sans branchement. Les CPU modernes l'exécutent
en 1 cycle. Les comparaisons sur `Fixed` sont aussi rapides que sur
un `int` brut.

Comparer des `float` exige `ucomiss` + un branchement sur les
drapeaux d'état, plus la gestion des NaN — plus lent et plein de cas
limites. Encore une raison pour laquelle la virgule fixe gagne dans
le code de comparaison critique.
