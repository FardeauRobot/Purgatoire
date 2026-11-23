# 🎭 CPP06 — Les casts : dire tout haut ce que le cast C cachait

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP06.md)

> **TL;DR.** En C, `(type)x` fait *n'importe quoi* : il convertit des nombres, jette le `const` et transforme une adresse en entier, le tout avec la même syntaxe. C++ le découpe en **quatre casts nommés**, et chacun ne sait faire qu'un seul type de chose. C'est pour ça que le nom, qu'on peut chercher avec grep, dit au lecteur (et à ton évaluateur) exactement ce que tu voulais faire. CPP06 attribue un cast à chaque exercice : `static_cast` pour les conversions de valeurs (ex00), `reinterpret_cast` pour réinterpréter les bits bruts (ex01), et `dynamic_cast` pour demander à un objet ce qu'il est vraiment à l'exécution (ex02). **Ton choix de cast est noté.**

Voir aussi : [`CASTS.md`](../../../../en/cpp/lexique/CASTS.md) (EN) · [`STATIC.md`](../../../../en/cpp/lexique/STATIC.md) (EN) · [`POLYMORPHISM.md`](../../notions/oop/POLYMORPHISM.md) · [`VIRTUAL.md`](../../../../en/cpp/lexique/VIRTUAL.md) (EN) · [`TRY_CATCH_THROW.md`](../../../../en/cpp/lexique/TRY_CATCH_THROW.md) (EN) · [`TYPEID.md`](../../../../en/cpp/lexique/TYPEID.md) (EN) (interdit ici) · [`CMATH.md`](../../../../en/cpp/notions/fundamentals/CMATH.md) (EN) · [`STRING.md`](../../../../en/cpp/notions/fundamentals/STRING.md) (EN) · Précédent : [`CPP05.md`](CPP05.md)

---

## 1. Le problème, dans le C que tu connais déjà

```c
double      d  = 42.9;
int         i  = (int)d;              /* value conversion: 42               */
const char *s  = "hello";
char       *w  = (char *)s;           /* drops const: writing w[0] is UB     */
uintptr_t   u  = (uintptr_t)&d;       /* address → integer                   */
Animal     *a  = (Animal *)some_ptr;  /* "trust me" downcast, never checked  */
```

Les quatre lignes utilisent la même syntaxe, et le compilateur les accepte toutes. Quand l'une d'elles plante, tu ne peux pas retrouver le cast dangereux avec `grep`, parce que tous les casts se ressemblent.

**La réponse du C++ :** quatre mots-clés, chacun avec un rôle étroit. Si tu utilises le mauvais, le compilateur refuse.

---

## 2. Le mécanisme, en un tableau

| Cast | Ce qu'il peut faire | Vérifié quand ? | CPP06 |
|---|---|---|---|
| `static_cast<T>(x)` | Les conversions que le langage connaît déjà : `double`→`int`, `int`→`char`, `void*`→`T*`, pointeurs base↔dérivée **sans vérification** | À la compilation | **ex00** |
| `reinterpret_cast<T>(x)` | Lire **les mêmes bits** comme un autre type : pointeur ↔ entier, pointeur ↔ pointeur sans rapport | Rien n'est vérifié. Tu promets | **ex01** |
| `dynamic_cast<T>(x)` | Base → dérivée, **vérifié à l'exécution** grâce à la vtable de l'objet. Exige un type polymorphe | À l'exécution | **ex02** |
| `const_cast<T>(x)` | Ajouter ou retirer `const` / `volatile`. Rien d'autre | À la compilation | (pas utilisé) |
| `(T)x` cast C | Essaie `const_cast`, `static_cast`, puis `reinterpret_cast`, et prend le premier qui compile | — | **À éviter** : il cache lequel a été appliqué |

```
                à quel point le cast « fait-il confiance » ?
 sûr ◄─────────────────────────────────────────────────────► dangereux
 implicite   dynamic_cast       static_cast       const_cast     reinterpret_cast
 (promotion) (vérifié à l'exéc.)(non vérifié)     (retire const) (bits bruts)
```

La grille de notation utilise le même vocabulaire : *« Accept the use of implicit casts for **promotion casts only**. »* Une promotion est un élargissement qui ne peut pas perdre d'information, comme `char`→`int` ou `float`→`double`. Toute conversion *restrictive* (`double`→`int`, `int`→`char`, `double`→`float`) doit être un `static_cast` explicite.

---

## 3. Le schéma de la « classe qu'on ne peut pas instancier » (ex00 et ex01)

ex00 et ex01 demandent tous deux une classe qui n'est qu'un *namespace avec un nom* : des méthodes statiques, et aucun objet autorisé.

```cpp
class ScalarConverter
{
private:
    ScalarConverter();
    ScalarConverter(ScalarConverter const &other);
    ScalarConverter &operator=(ScalarConverter const &other);
    ~ScalarConverter();

public:
    static void convert(std::string const &literal);
};
```

- **Des constructeurs privés** font que `ScalarConverter sc;` ne compile pas hors de la classe. La grille demande exactement *« une classe avec un constructeur privé, et des méthodes statiques ? »*.
- **L'OCF s'applique toujours** (CPP02 → CPP09). Déclarer les quatre membres en privé la satisfait et bloque aussi les copies.
- `static` signifie que la méthode n'a pas besoin de `this`, donc tu l'appelles `ScalarConverter::convert(av[1])`. C'est la version, rangée dans une classe, d'une simple fonction C.

> En C, tu aurais utilisé un fichier avec des fonctions `static`. Ici, la *classe* sert de namespace, et le constructeur privé sert de verrou.

---

## 4. ex00 — `ScalarConverter` (static_cast)

### Ce que demande le sujet
`./convert <literal>` reçoit **un** littéral C++ sous forme de chaîne. Il détecte son type, le convertit vers ce type réel, puis le convertit **explicitement** vers les trois autres types et affiche les quatre.

| Type | Littéraux acceptés |
|---|---|
| `char` | `'c'`, `'a'`… Les caractères non affichables ne sont jamais utilisés en entrée. Un chiffre seul compte comme un `int` |
| `int` | `0`, `-42`, `42` |
| `float` | `0.0f`, `-4.2f`, `4.2f`, plus les pseudo-littéraux **`-inff`, `+inff`, `nanf`** |
| `double` | `0.0`, `-4.2`, `4.2`, plus les pseudo-littéraux **`-inf`, `+inf`, `nan`** |

Quand une conversion n'a pas de sens ou déborde, affiche `impossible`. Quand un char n'est pas affichable, affiche `Non displayable`.

```
$> ./convert 0                 $> ./convert nan             $> ./convert 42.0f
char: Non displayable          char: impossible             char: '*'
int: 0                         int: impossible              int: 42
float: 0.0f                    float: nanf                  float: 42.0f
double: 0.0                    double: nan                  double: 42.0
```

### Le pipeline

```
 "42.0f"
    │
    ▼
 ┌───────────────── détecter ───────────────┐
 │ nanf  +inff  -inff         → FLOAT       │
 │ nan   +inf   -inf          → DOUBLE      │
 │ longueur 1 && !isdigit     → CHAR        │
 │ [+-]?chiffres              → INT (*)     │
 │ [+-]?chiffres.chiffres f   → FLOAT       │
 │ [+-]?chiffres.chiffres     → DOUBLE      │
 │ tout le reste              → erreur, stop│
 └──────────────────────────────────────────┘
    │  par ex. FLOAT
    ▼
 parser UNE SEULE FOIS vers le type réel :
    float f = static_cast<float>(std::strtod(s, NULL));
    │
    ▼
 static_cast vers les trois AUTRES types, chacun protégé :
    char   ← nan ou hors de [0,127] ?    → "impossible"
             non affichable ?            → "Non displayable"
    int    ← nan ou hors de la plage int ? → "impossible"
    double ← static_cast<double>(f)        (tient toujours)
```

(*) `2147483648` correspond au motif INT mais ne tient pas dans un `int`. Décide ce que tu en fais (le traiter comme un `double`, ou afficher `impossible` pour char et int) et dis-le en soutenance.

Le sujet insiste sur **détecter d'abord, puis convertir depuis le type réel**. Certains étudiants parsent tout en `double` et affichent à partir de là. La grille dit *« please don't be too uncompromising… if the spirit of the exercise is respected »*, mais sois prêt à expliquer ce que tu as fait.

### Extrait central : les gardes côté sortie

```cpp
static void printChar(double d)
{
    std::cout << "char: ";
    if (d != d || d < 0 || d > 127)
        std::cout << "impossible" << std::endl;
    else if (!std::isprint(static_cast<int>(d)))
        std::cout << "Non displayable" << std::endl;
    else
        std::cout << "'" << static_cast<char>(d) << "'" << std::endl;
}

static void printInt(double d)
{
    std::cout << "int: ";
    if (d != d || d < static_cast<double>(INT_MIN) || d > static_cast<double>(INT_MAX))
        std::cout << "impossible" << std::endl;
    else
        std::cout << static_cast<int>(d) << std::endl;
}
```

Ligne par ligne :
- **`d != d`** est le test de NaN en C++98. NaN est la seule valeur qui n'est pas égale à elle-même. (`std::isnan` est du C++11 dans `<cmath>`. La macro `isnan` du C99 marche souvent, mais elle n'est pas garantie en `-std=c++98`.)
- **La vérification de plage vient *avant* le cast.** `static_cast<int>(1e10)` est un **comportement indéfini**, pas un débordement propre. Vérifie d'abord, caste ensuite.
- **`static_cast<char>(d)` ne s'exécute qu'une fois qu'on sait que `d` tient.** C'est le cast que la grille cherche.
- `±inf` échoue tout seul aux deux vérifications de plage, donc pas besoin de cas particulier.
- Ces utilitaires prennent un `double` pour rester courts. Si ton type réel est `int`, le passer en argument est une conversion implicite `int`→`double`, qui n'est **pas** une promotion : écris `printChar(static_cast<double>(i))`.

### Afficher `42.0f` au lieu de `42f`

`std::cout << 42.0f` affiche `42`, parce que le flux supprime les zéros de fin. Tu dois ajouter le `.0` toi-même quand la valeur est entière :

```cpp
float f = static_cast<float>(d);
std::cout << "float: " << f;
if (f == std::floor(f) && std::fabs(f) < 1e6)
    std::cout << ".0";
std::cout << "f" << std::endl;
```

- `f == std::floor(f)` est vrai quand il n'y a pas de partie fractionnaire. C'est faux pour `nan` et vrai pour `inf`…
- …donc la garde `std::fabs(f) < 1e6` empêche `inf.0f`. Elle empêche aussi `1e+10.0f`, parce que la précision par défaut (6) passe les grands nombres en notation scientifique.
- `nan` s'affiche `nan`, et `+ "f"` le transforme en `nanf`. C'est exactement le pseudo-littéral attendu par le sujet.

L'alternative est `std::fixed << std::setprecision(1)`. Ça affiche toujours une décimale, mais ça perd des chiffres en silence : `4.25` s'affiche `4.2`, et `4.35` `4.3`. Le flux par défaut perd aussi des chiffres, simplement plus tard : 6 chiffres significatifs, donc `3.14159265` s'affiche `3.14159`. Les deux approches sont acceptées. Choisis-en une et justifie-la.

### ⚠️ Pièges
- **`'a'` vs `a`.** Le sujet montre les littéraux char avec des apostrophes, mais le shell les supprime sauf si tu écris `./convert "'a'"`. La plupart des étudiants acceptent un caractère seul qui n'est pas un chiffre. Décide, et dis-le en soutenance.
- **`"0"` est un int, pas un char.** Sa conversion en char donne `Non displayable` parce que la *valeur* 0 est NUL. Le caractère `'0'` a la valeur 48, qui s'affiche sans problème.
- **`2147483648`** ressemble à un int mais déborde. Parse avec `strtol` ou `strtod`, puis vérifie la plage avec `INT_MIN`/`INT_MAX`. Ne compte pas sur `errno == ERANGE` de `strtol` : `long` fait 64 bits sur ton Mac, donc il ne déborde pas là. Ne le parse jamais avec `atoi`, qui est un UB en cas de débordement.
- `+inf` s'affiche `inf`, pas `+inf`. C'est correct : l'exemple du sujet lui-même ne montre que `nan`.
- Inclus `<cctype>` pour `isprint`, `<cmath>` pour `floor` et `fabs`, `<climits>` pour `INT_MAX`, et `<cstdlib>` pour `strtod`.

---

## 5. ex01 — `Serializer` (reinterpret_cast)

### Ce que demande le sujet
- Une classe non instanciable `Serializer` avec :
  - `static uintptr_t serialize(Data *ptr);`
  - `static Data *deserialize(uintptr_t raw);`
- Une struct `Data` **non vide**, avec de vrais membres.
- Un test qui montre que `deserialize(serialize(&data)) == &data`.

### Extrait central

```cpp
#include <stdint.h>

uintptr_t Serializer::serialize(Data *ptr)
{
    return reinterpret_cast<uintptr_t>(ptr);
}

Data *Serializer::deserialize(uintptr_t raw)
{
    return reinterpret_cast<Data *>(raw);
}
```

### Ce qui se passe en mémoire

```
 pile                                       les mêmes 8 octets, lus différemment
 ┌──────────────────────┐
 │ Data data            │ ◄── vit à 0x7ffee40e23b0
 │   int id = 42        │
 │   std::string name   │
 └──────────────────────┘

 Data *ptr = &data        →  0x7ffee40e23b0      (typé : « pointe vers un Data »)
          │ reinterpret_cast<uintptr_t>
          ▼
 uintptr_t raw            →  140732724552624    (juste un nombre : tu peux l'afficher, le stocker, l'envoyer)
          │ reinterpret_cast<Data *>
          ▼
 Data *back               →  0x7ffee40e23b0      back == ptr ✓   back->id == 42 ✓
```

- **Pourquoi `uintptr_t` ?** C'est le type entier non signé garanti assez grand pour contenir un pointeur. `unsigned int` fait 32 bits sur ton Mac, alors que les pointeurs en font 64, donc un aller-retour par lui tronquerait.
- **Pourquoi pas `static_cast` ?** Un pointeur et un entier ne sont pas des types « apparentés », donc `static_cast<uintptr_t>(ptr)` ne compile pas. `reinterpret_cast` est le seul cast qui dit « mêmes bits, autre type ».
- **« Sérialiser » ici veut seulement dire transformer le pointeur en nombre.** Rien n'est copié : le `Data` doit encore être vivant quand tu désérialises. Ce n'est pas une vraie sérialisation (aucun octet n'est écrit dans un fichier). L'exercice porte sur le cast.

### ⚠️ Pièges
- **Utilise `<stdint.h>` plutôt que `<cstdint>`.** `<cstdint>` est officiellement du C++11. La libc++ d'Apple le laisse passer, mais une configuration plus stricte pourrait le refuser.
- **La grille veut `reinterpret_cast` exactement deux fois**, une dans chaque sens. Si tu ajoutes un cast C quelque part, ce sera relevé.
- Un `struct Data {};` vide fait échouer l'exercice. Mets-y au moins un vrai membre, et affiche-le après l'aller-retour pour prouver que la struct est toujours utilisable.

---

## 6. ex02 — Identifier le type réel (dynamic_cast)

### Ce que demande le sujet
- `Base` n'a **qu'un destructeur virtuel public**. `A`, `B` et `C` héritent publiquement de `Base` et sont vides. Ces quatre classes sont exemptées de l'OCF.
- Trois fonctions libres :
  - `Base *generate(void);` renvoie aléatoirement un `A`, un `B` ou un `C`.
  - `void identify(Base *p);` affiche `A`, `B` ou `C`.
  - `void identify(Base &p);` affiche la même chose. **Un pointeur est interdit dans cette fonction.**
- **Inclure `<typeinfo>` est interdit**, donc `typeid` est exclu.

### Pourquoi `dynamic_cast` marche : la vtable sait

```
 Base *p = new B;

 objet B sur le tas                  vtable de B (construite par le compilateur)
 ┌──────────────────┐               ┌────────────────────────────┐
 │ vptr ────────────┼─────────────► │ infos de type : « je suis  │
 ├──────────────────┤               │  un B, je dérive de Base » │
 │ (aucun membre)   │               │ &B::~B                     │
 └──────────────────┘               └────────────────────────────┘

 dynamic_cast<A *>(p)  → lit la vtable : B n'est pas un A  → NULL
 dynamic_cast<B *>(p)  → lit la vtable : oui               → p, typé B*
```

Seules les classes qui ont **au moins une fonction virtuelle** ont une vtable. C'est pour ça que le sujet donne à `Base` un destructeur virtuel, et pour ça qu'un `dynamic_cast` sur une classe non polymorphe est une **erreur de compilation** (`'Base' is not polymorphic`). Le même destructeur virtuel fait aussi que `delete p` exécute `~B`, ce que tu as appris dans CPP04.

### Extrait central : version pointeur

```cpp
void identify(Base *p)
{
    if (dynamic_cast<A *>(p))
        std::cout << "A" << std::endl;
    else if (dynamic_cast<B *>(p))
        std::cout << "B" << std::endl;
    else if (dynamic_cast<C *>(p))
        std::cout << "C" << std::endl;
}
```

Un cast de pointeur raté **renvoie NULL**. La grille vérifie exactement ça : *« should check if the cast return is NULL »*.

### Extrait central : version référence

Une référence ne peut pas être NULL, donc un cast de référence raté **lance une exception** à la place :

```cpp
void identify(Base &p)
{
    try
    {
        (void)dynamic_cast<A &>(p);
        std::cout << "A" << std::endl;
        return;
    }
    catch (std::exception &) {}
    try
    {
        (void)dynamic_cast<B &>(p);
        std::cout << "B" << std::endl;
        return;
    }
    catch (std::exception &) {}
    try
    {
        (void)dynamic_cast<C &>(p);
        std::cout << "C" << std::endl;
    }
    catch (std::exception &) {}
}
```

- L'exception lancée est `std::bad_cast`, mais **`std::bad_cast` est déclarée dans `<typeinfo>`**, qui est interdit. Attrape plutôt sa classe de base `std::exception &` (de `<exception>`). L'interdiction de `<typeinfo>` transforme ça en révision de CPP05 : attraper par référence à la base.
- `(void)` jette le résultat du cast. Seul compte le fait qu'il ait lancé ou non.
- **N'écris pas `identify(&p)` à l'intérieur.** Ça crée un pointeur, ce que le sujet interdit.

### `generate`

```cpp
Base *generate(void)
{
    switch (std::rand() % 3)
    {
        case 0:  return new A;
        case 1:  return new B;
        default: return new C;
    }
}
```

Appelle `std::srand(static_cast<unsigned int>(std::time(NULL)))` **une seule fois**, dans `main`. Le cast compte dans ce module : `time_t` → `unsigned int` est une conversion restrictive, pas une promotion. Pense à `delete` ce que `generate` renvoie. Le destructeur virtuel rend ça correct.

### ⚠️ Pièges
- `grep -r typeinfo .` avant de pousser. La grille : *« the header `<typeinfo>` must not appear anywhere. »*
- N'identifie pas le type via une méthode virtuelle `name()`. L'exercice porte sur `dynamic_cast`, et `Base` ne doit avoir *que* le destructeur.

---

## 7. Ce que vérifie l'évaluateur

D'après la grille de notation officielle de 42evalhub :

**Prérequis**
- [ ] `c++ -Wall -Wextra -Werror`, C++98, aucune fonctionnalité C++11.
- [ ] Aucun corps de fonction dans un en-tête (sauf templates). Le Makefile utilise `c++` avec les flags.
- [ ] Pas de `*alloc`, `*printf` ni `free`. Pas de `using namespace`. Pas de `friend`. Aucune bibliothèque externe.
- [ ] Aucune fuite.

**ex00 : static_cast**
- [ ] Une classe avec un **constructeur privé** et des **méthodes statiques**.
- [ ] Les conversions utilisent **`static_cast`**. Les casts implicites ne sont acceptés que pour les promotions.
- [ ] Le programme fonctionne comme demandé. L'évaluateur est indulgent sur le formatage exact « if the spirit is respected ».

**ex01 : reinterpret_cast**
- [ ] Une classe avec un constructeur privé et des méthodes statiques.
- [ ] `reinterpret_cast` est utilisé **deux fois** : `Data*` → `uintptr_t`, puis `uintptr_t` → `Data*`.
- [ ] Le `Data` obtenu est utilisable.

**ex02 : dynamic_cast**
- [ ] Le type réel est identifié avec `dynamic_cast`.
- [ ] `identify(Base*)` teste **NULL**.
- [ ] `identify(Base&)` utilise **try/catch**.
- [ ] **`<typeinfo>` n'apparaît nulle part.**

**Questions de soutenance à préparer**
- « Pourquoi `static_cast` et pas un cast C ? » → Il est explicite, se trouve avec grep, et refuse les conversions dangereuses (§2).
- « Pourquoi `reinterpret_cast` est-il nécessaire dans ex01 ? » → Pointeur ↔ entier n'est pas une conversion de valeur.
- « Que se passerait-il sans le destructeur virtuel dans `Base` ? » → `dynamic_cast` ne compilerait pas, et un `delete` via un `Base*` serait un UB.

---

## 8. Aide-mémoire

| Besoin | Écriture C++98 |
|---|---|
| Conversion numérique restrictive | `static_cast<int>(d)` après vérification de plage |
| Test de NaN sans C++11 | `d != d` |
| Pointeur → entier et retour | `reinterpret_cast<uintptr_t>(p)`, `reinterpret_cast<T *>(u)` avec `<stdint.h>` |
| « Cette Base est-elle vraiment un B ? » (pointeur) | `if (B *b = dynamic_cast<B *>(p))` → NULL en cas d'échec |
| Pareil, avec une référence | `dynamic_cast<B &>(r)` dans un `try`, `catch (std::exception &)` |
| Classe non instanciable | Constructeur, constructeur de copie, `operator=` et destructeur privés, plus des méthodes `static` |
| Retirer const | `const_cast<T *>(p)` (écrire à travers est un UB si l'objet était const à l'origine) |

**Ce que ce module te laisse :** chaque conversion de ton code affiche désormais son *intention*. Quand une relecture demande « qu'est-ce que tu voulais faire ici ? », le nom du cast répond.

➡️ Suivant : [`CPP07.md`](CPP07.md). Au lieu de convertir entre types, tu écris du code qui ne se soucie pas du tout du type.
