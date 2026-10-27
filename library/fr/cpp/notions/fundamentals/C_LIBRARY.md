# `<cctype>`, `<cstdlib>`, `<cstring>` — la boîte à outils C en C++98

Le C++ hérite de toute la bibliothèque standard du C. Chaque header C `<xxx.h>`
a un jumeau C++ `<cxxx>` qui range les mêmes fonctions dans le namespace `std::` :

| Header C | Header C++ | Contenu |
|---|---|---|
| `<ctype.h>` | `<cctype>` | tests sur les caractères et changement de casse (`isdigit`, `toupper`…) |
| `<stdlib.h>` | `<cstdlib>` | chaîne → nombre, `abs`, `exit`, `rand`… |
| `<string.h>` | `<cstring>` | chaînes `char*` brutes et blocs mémoire (`strlen`, `memset`…) |

Inclus toujours la forme `<cxxx>` et écris `std::isdigit`, pas `isdigit`.
La plupart des implémentations exposent *aussi* le nom global, mais la norme
ne le garantit pas — du code qui compile sur macOS peut casser sur le Linux de l'évaluation.

> Ce sont les fonctions que tu as recodées dans la **libft**. `std::isdigit`
> c'est `ft_isdigit`, `std::strlen` c'est `ft_strlen`, `std::atoi` c'est
> `ft_atoi` — même contrat, mêmes pièges.

Voisins : [`CMATH.md`](CMATH.md) (`<cmath>`), [`STRING.md`](STRING.md) et
[`../io-errors/STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md) (`std::string`).

---

## `<cctype>` — un caractère à la fois

Toutes les fonctions prennent un `int` et renvoient un `int`.

### Tests (`is…`)

| Fonction | Vrai pour | Exemple `vrai` |
|---|---|---|
| `isdigit(c)` | `'0'`–`'9'` | `'7'` |
| `isxdigit(c)` | chiffre hexa : `0-9`, `a-f`, `A-F` | `'B'` |
| `isalpha(c)` | lettre `a-z`, `A-Z` | `'q'` |
| `isalnum(c)` | lettre ou chiffre | `'3'`, `'x'` |
| `isupper(c)` | `A-Z` | `'Q'` |
| `islower(c)` | `a-z` | `'q'` |
| `isspace(c)` | `' '`, `'\t'`, `'\n'`, `'\v'`, `'\f'`, `'\r'` | `'\n'` |
| `ispunct(c)` | imprimable, ni alnum ni espace | `'!'`, `'.'` |
| `isprint(c)` | imprimable, **espace compris** (32–126) | `' '` |
| `isgraph(c)` | imprimable, **espace exclu** (33–126) | `'~'` |
| `iscntrl(c)` | caractères de contrôle (0–31, 127) | `'\0'`, `'\t'` |

`isblank` (espace ou tabulation seulement) est C99 / C++11 — indisponible en `-std=c++98`.

### Conversion

| Fonction | Effet |
|---|---|
| `toupper(c)` | `'a'` → `'A'` ; tout le reste renvoyé tel quel |
| `tolower(c)` | `'A'` → `'a'` ; tout le reste renvoyé tel quel |

Elles renvoient un `int` : recaste en `char` pour le stocker.

```cpp
std::string s = "Hello";
for (std::string::size_type i = 0; i < s.size(); ++i)
    s[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[i])));
```

### Piège 1 — le résultat est « non nul », pas `1`

```cpp
if (std::isdigit(c) == 1)   // FAUX : la glibc peut renvoyer 2048
if (std::isdigit(c))        // correct
```

Les `is…` sont en général une lecture dans une table qui renvoie un masque de
bits : « vrai » peut valoir n'importe quelle valeur non nulle. Ne compare
jamais à `1` ni à `true`.

### Piège 2 — un `char` négatif, c'est un comportement indéfini

L'argument doit être **`EOF` ou une valeur représentable en `unsigned char`**
(0–255). Sur Linux et macOS (x86/ARM), le `char` est signé : `'é'` en Latin-1
ou un octet UTF-8 est négatif → la table est lue hors limites.

```cpp
char c = s[i];
std::isdigit(c);                                // UB si c < 0
std::isdigit(static_cast<unsigned char>(c));    // toujours sûr
```

Même règle pour toutes les fonctions de `<cctype>`. C'est la raison classique
pour laquelle les tests de `ft_isdigit` passent alors que la vraie plante sur
une entrée bizarre.

### Usage typique — « cette chaîne ne contient-elle que des chiffres ? »

```cpp
#include <cctype>
#include <string>

bool isAllDigits(const std::string& s)
{
    if (s.empty())
        return false;
    for (std::string::size_type i = 0; i < s.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
            return false;
    return true;
}
```

Note le test `empty()` : sans lui, `""` passerait pour un nombre valide.

---

## `<cstdlib>` — nombres, sortie, hasard

### Chaîne → nombre : la famille `ato…` (rapide, aveugle)

| Fonction | Renvoie |
|---|---|
| `atoi(str)` | `int` |
| `atol(str)` | `long` |
| `atof(str)` | `double` (oui, `double`, malgré le `f`) |

Saute les espaces de tête, lit un signe optionnel et des chiffres, s'arrête au
premier caractère invalide. **Aucun signalement d'erreur** : `atoi("abc")` et
`atoi("0")` renvoient tous deux `0`, et le dépassement est un comportement
indéfini. Bien pour un test rapide, jamais pour valider une saisie.

`atoll` est C99/C++11 — absent du C++98.

### Chaîne → nombre : la famille `strto…` (robuste)

| Fonction | Renvoie | En plus |
|---|---|---|
| `strtol(str, &end, base)` | `long` | base 2–36, ou `0` = auto (`0x` → hexa, `0` → octal) |
| `strtoul(str, &end, base)` | `unsigned long` | ⚠️ accepte `"-1"` et le transforme en `ULONG_MAX` |
| `strtod(str, &end)` | `double` | comprend `"1e3"`, `"inf"`, `"nan"`, `"0x1p3"` |

Le second argument `end` pointe **juste après le dernier caractère lu**.
C'est ce qui rend ces fonctions utilisables pour valider :

- `end == str` → rien n'a été lu (`"abc"`, `""`)
- `*end != '\0'` → des déchets à la fin (`"42abc"`)
- `errno == ERANGE` → dépassement (`<cerrno>`)

```cpp
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>

bool parseInt(const std::string& s, int& out)
{
    const char* str = s.c_str();
    char*       end;

    errno = 0;
    long v = std::strtol(str, &end, 10);
    if (end == str || *end != '\0')
        return false;
    if (errno == ERANGE || v < INT_MIN || v > INT_MAX)
        return false;
    out = static_cast<int>(v);
    return true;
}
```

Pourquoi chaque ligne :
- `errno = 0` d'abord — `strtol` ne fait que *positionner* `errno` en cas d'erreur, il ne le remet jamais à zéro.
- `long` puis vérification de plage — `strtol` renvoie un `long` ; en 64 bits,
  `long` est plus large qu'`int`, donc `"3000000000"` tient dans un `long` mais pas dans un `int`.
- `s.c_str()` est le pont entre `std::string` et `const char*`.
- `strtol` saute silencieusement les espaces **de tête**, donc `" 7"` est
  accepté. Si le sujet veut le refuser, teste `std::isspace` sur `s[0]` avant.

C'est le schéma derrière CPP06 (`ScalarConverter`) et CPP09
(parsing des valeurs de `BitcoinExchange`, des arguments de `PmergeMe`). Voir [`../../projets/modules/CPP06.md`](../../projets/modules/CPP06.md).

`strtof`, `strtoll`, `strtold` sont C99/C++11 — en C++98, parse avec `strtod`
puis caste. L'alternative C++ est `std::stringstream` (voir
[`../io-errors/FSTREAM_GUIDE.md`](../io-errors/FSTREAM_GUIDE.md)) :
`ss >> n` puis vérifier `ss.fail()` et `ss.eof()`.

### Valeur absolue

| Fonction | Pour |
|---|---|
| `abs(int)` | `int` |
| `labs(long)` | `long` |
| `fabs(double)` | flottants — celle-là est dans `<cmath>` |

`std::abs(INT_MIN)` est indéfini (la valeur positive ne tient pas).

### Terminer le programme

| Nom | Sens |
|---|---|
| `exit(code)` | vide les flux C, lance les handlers `atexit`, quitte |
| `EXIT_SUCCESS` / `EXIT_FAILURE` | `0` / `1` portables |

⚠️ `exit()` n'appelle **pas** les destructeurs des objets locaux encore sur la
pile — en C++, préfère sortir de `main` par `return`, ou lancer une exception
attrapée dans `main`. Les fuites valgrind après un `exit` viennent souvent de là.

### Nombres aléatoires

| Fonction | Effet |
|---|---|
| `srand(graine)` | initialise le générateur, **une seule fois**, au démarrage |
| `rand()` | `int` pseudo-aléatoire dans `[0, RAND_MAX]` |

```cpp
#include <cstdlib>
#include <ctime>

std::srand(static_cast<unsigned int>(std::time(NULL)));
int dice = std::rand() % 6 + 1;     // 1..6
```

Appeler `srand(time(NULL))` avant chaque `rand()` donne le même nombre
pendant toute une seconde. Utilisé en CPP05 (robotomie à 50 %) et
CPP06 (`generate()`).

### Interdit ici

`malloc`, `calloc`, `realloc`, `free` sont aussi dans `<cstdlib>`, mais les
sujets C++ de 42 les interdisent : utilise `new` / `delete` ([`MEMORY.md`](MEMORY.md)).

---

## `<cstring>` — chaînes C brutes et mémoire

Travaillent sur des `char*` terminés par `'\0'`. En C++, `std::string` fait
tout ça plus sûrement — tu les croises surtout face à une API C, ou sur `argv[i]`.

### Chaînes

| Fonction | Effet | Équivalent `std::string` |
|---|---|---|
| `strlen(s)` | longueur, `'\0'` exclu | `s.size()` / `s.length()` |
| `strcmp(a, b)` | `<0`, `0`, `>0` (ordre lexicographique) | `a == b`, `a < b`, `a.compare(b)` |
| `strncmp(a, b, n)` | compare les `n` premiers caractères | `a.compare(0, n, b, 0, n)` |
| `strcpy(dst, src)` | copie, **sans contrôle de taille** | `dst = src` |
| `strncpy(dst, src, n)` | copie ≤ `n` caractères, peut **ne pas** mettre de `'\0'` | `dst = src.substr(0, n)` |
| `strcat(dst, src)` | concatène, **sans contrôle de taille** | `dst += src` |
| `strchr(s, c)` | premier `c`, ou `NULL` | `s.find(c)` |
| `strrchr(s, c)` | dernier `c`, ou `NULL` | `s.rfind(c)` |
| `strstr(hay, needle)` | première sous-chaîne, ou `NULL` | `s.find(needle)` |

Pièges :
- `strcmp` renvoie **0 quand c'est égal** — `if (strcmp(a, b))` veut dire « différent ».
- `strlen` est en O(n) : il parcourt jusqu'au `'\0'` à chaque appel. Pas dans une condition de boucle.
- `strcpy` / `strcat` débordent de `dst` s'il est trop petit.

```cpp
if (argc == 2 && std::strcmp(argv[1], "--help") == 0)
    printUsage();
```

(ou tout simplement `std::string(argv[1]) == "--help"`).

### Blocs mémoire

| Fonction | Effet |
|---|---|
| `memset(p, octet, n)` | remplit `n` octets avec `octet` |
| `memcpy(dst, src, n)` | copie `n` octets — les zones ne doivent **pas** se chevaucher |
| `memmove(dst, src, n)` | pareil, chevauchement autorisé |
| `memcmp(a, b, n)` | compare `n` octets, `0` si égaux |

⚠️ Ne fais jamais de `memset`/`memcpy` sur un objet qui a des constructeurs,
des fonctions virtuelles ou des membres `std::string` — ça court-circuite le
constructeur et écrase le vptr. Utilise le constructeur de copie /
`operator=` (voir [`../oop/ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md)).

---

## Antisèche — « je veux… »

| Je veux… | C++98 |
|---|---|
| tester si un char est un chiffre | `std::isdigit(static_cast<unsigned char>(c))` |
| passer un char en majuscule | `static_cast<char>(std::toupper(static_cast<unsigned char>(c)))` |
| sauter les espaces | `while (std::isspace(static_cast<unsigned char>(s[i]))) ++i;` |
| parser un `int` proprement | `std::strtol` + `end` + `errno` + vérif de plage |
| parser un `double` / `float` | `std::strtod`, caste pour `float` |
| parser vite fait | `std::atoi` (aucune détection d'erreur) |
| comparer deux `char*` | `std::strcmp(a, b) == 0` |
| longueur d'un `char*` | `std::strlen(s)` |
| `std::string` → `char*` | `s.c_str()` |
| entier aléatoire dans `[0, n)` | `std::rand() % n` après un seul `std::srand` |
| quitter avec un code d'erreur | `return EXIT_FAILURE;` depuis `main` |
