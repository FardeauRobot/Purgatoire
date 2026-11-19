# CHECK — CPP09 (ex00, ex01, ex02) — 2026-11-02

Sources :
- **Grille 42evalhub** : `https://www.42evalhub.com/data/common/cpp09.html`, récupérée aujourd'hui. Hors balises du formulaire (boutons Oui/Non), son texte est **identique** au cache `correction/.grid-cpp09.md` du 21/10.
- **Sujet** : `CPP09.pdf`, version 3.1, lu en entier en texte (chap. II à VIII).
- **Mesures** : toutes les sorties citées ici ont été **mesurées aujourd'hui**. `| cat -v` rend visibles les codes couleur (`^[[0m`). Valgrind n'est pas installé sur ce Mac, donc les fuites sont vérifiées avec `leaks --atExit`.

Ce fichier remplace `REVIEW.md` du 23/10 pour ex00 et ex01 : tous les points de ce REVIEW ont été revérifiés, et ceux qui restent ouverts sont repris ici.

## Verdict

| | Grille 42evalhub | Sujet pris littéralement |
|---|---|---|
| **ex00 btc** | ✅ Passe | ⚠️ erreurs sur stderr et en couleur |
| **ex01 RPN** | ✅ Passe | ❌ affiche `RESULT = 42` au lieu de `42`, et `Fatal: …` au lieu de `Error` |
| **ex02 PmergeMe** | ✅ Passe, à condition d'**expliquer** Ford-Johnson | ✅ (les couleurs sont un choix assumé) |

**Aucun point éliminatoire mesuré.** Le principal risque de la défense n'est pas dans le code : il faut savoir expliquer et modifier en direct l'algorithme d'ex02 (voir « Défense »).

---

## À faire, par priorité

| # | Ex | Quoi | Type |
|---|---|---|---|
| 1 | ex02 | Réécrire / réexpliquer Ford-Johnson sans aide, comme prévu | DÉFENSE |
| 2 | ex01 | Afficher `42` seul, sans `RESULT = ` ni couleur | LITTÉRAL |
| 3 | ex01 | Afficher `Error` sur stderr, sans `Fatal:` ni couleur | LITTÉRAL |
| 4 | repo | Retirer le binaire `ex02/PmergeMe` de git et ignorer les 3 binaires de CPP09 | HYGIÈNE |
| 5 | ex02 | Préparer des tests à montrer (exigence de la grille) | GRILLE |
| 6 | ex00 | Décider : erreurs sur stdout ou stderr, avec ou sans couleur | LITTÉRAL |
| 7 | ex00 | Corriger la notation scientifique et le `-0` | POLISH |
| 8 | tous | Petits nettoyages (`#define MAP`, `isdigit`, guillemet, helpers inutilisés) | POLISH |

Signification des types :
- **GRILLE** : peut faire échouer la grille.
- **LITTÉRAL** : différence visible avec la sortie du sujet. Le chap. II dit : *« the examples can show requirements that are not explicitly written in the instructions »*.
- **POLISH** : un correcteur peut tiquer, mais ce n'est pas bloquant.
- **DÉFENSE** : à préparer à l'oral.

---

## Vérifications communes (mesurées sur les 3 exercices)

| Check | ex00 | ex01 | ex02 |
|---|---|---|---|
| `make fclean && make` sans warning, bon nom de binaire | ✅ `btc` | ✅ `RPN` | ✅ `PmergeMe` |
| Pas de relink (`Nothing to be done for 'all'`) | ✅ | ✅ | ✅ |
| `c++ -Wall -Wextra -Werror` sans `-std`, puis avec `-std=c++98 -pedantic -Wshadow` | ✅ | ✅ | ✅ |
| Chaque `.hpp` compilé seul (`-fsyntax-only`) | ✅ 3/3 | ✅ 3/3 | ✅ 3/3 |
| Pas de `using namespace`, `friend`, `printf`, `*alloc`, `free`, ni C++11 (grep) | ✅ | ✅ | ✅ |
| Forme canonique (4 membres) | ✅ | ✅ | ✅ |
| Seule fonction écrite dans un header : un template (autorisé) | ✅ | ✅ | ✅ `Print` |
| Conteneurs | `std::map` | `std::stack` sur `std::list` | `std::vector` + `std::deque` |
| ASan + UBSan (copie de travail) | 0 rapport | 0 rapport | 0 rapport |
| `leaks --atExit` | 0 fuite | 0 fuite | 0 fuite (3000 nombres) |

Les fichiers en plus (`errors.*`, `utils.*`) sont permis par le chap. II : *« You can add some additional files if you need to […] as long as you turn in the mandatory files »*.

---

## ex00 — btc

### Ce qui passe
- **L'exemple du sujet** donne exactement les 6 lignes attendues sur stdout (`0.9 0.6 0.3 0.36 0.32 7.1`), et les 3 erreurs sortent au bon endroit.
- **Années bissextiles :** `2012-02-29 => 1 = 4.98` est accepté, `2013-02-29` est refusé (`bad input`).
- **Date inférieure :** `2011-01-05` utilise bien le taux `0.3`.
- **Date antérieure à la base :** `2009-01-01` donne `no rate available before 2009-01-01`.
- **Fichier vide :** pas de crash et aucune sortie. Sans argument, le programme affiche `Error: could not open file.`

### Les erreurs vont sur stderr, alors que l'ex00 ne le demande pas   [LITTÉRAL]
Chap. II : *« Unless specified otherwise, every output message must […] be displayed to the standard output »*. Ex01 et ex02 précisent « standard error », mais **pas ex00**.

```
$ ./btc input.txt 2>/dev/null     # les 3 lignes Error: disparaissent
```

- **Où :** `ex00/errors.cpp:8` (`std::cerr`).
- **Ton choix :** passer ces erreurs sur `std::cout`, ou garder stderr et préparer l'argument « stdout pour les résultats, stderr pour les erreurs ». Les deux se défendent.

### `Error:` est en couleur   [LITTÉRAL]
```
^[[1;31mError: ^[[0mnot a positive number.
```
Le sujet affiche `Error: not a positive number.` C'est invisible dans un terminal, mais un `diff` le détecte.
- **Où :** `ex00/errors.cpp:8` (`BOLD_RED`, `RESET`).
- **Ton choix :** c'est le même que pour les couleurs d'ex02. Si tu les gardes, assume-les dans les trois exercices.

### Grands résultats en notation scientifique   [POLISH]
```
2022-03-29 => 1000 = 4.71159e+07      (valeur exacte : 47115930)
```
Par défaut, le flux affiche 6 chiffres significatifs, comme `%g` en C. La grille demande de *« compare some dates manually »*, et ce format fait tiquer.
- **Où :** `ex00/BitcoinExchange.cpp:137`.
- **Piste :** `std::setprecision(15)` **sans** `std::fixed`. `std::fixed` transformerait `0.9` en `0.900000`.

### `-0` est accepté   [POLISH]
```
2011-01-03 => -0 = -0
```
`-0.0 < 0` est faux, donc le test `value < 0` laisse passer le zéro négatif (`BitcoinExchange.cpp:120`). Choisis entre le refuser et savoir l'expliquer.

### Hygiène   [POLISH]
- `# define MAP std::map<std::string, double>` (`BitcoinExchange.hpp:7`) : en C++98, le bon outil est `typedef`, comme tu le fais déjà dans ex01.
- `F_WarnMsg` et `F_ErrExit` ne sont pas utilisés.
- `idees.txt` est **commité** dans `ex00/`, à retirer du rendu.
- `data.csv` est lu avec un chemin relatif (`BitcoinExchange.cpp:91`) : lance `./btc` depuis `ex00/` pendant la défense.

---

## ex01 — RPN

### Ce qui passe (les valeurs sont toutes justes)
| Expression | Attendu | Valeur obtenue |
|---|---|---|
| `8 9 * 9 - 9 - 9 - 4 - 1 +` (sujet + grille) | 42 | 42 |
| `7 7 * 7 -` (sujet) | 42 | 42 |
| `1 2 * 2 / 2 * 2 4 - +` (sujet) | 0 | 0 |
| `9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -` (grille) | 42 | 42 |
| `1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /` (grille) | 15 | 15 |
| `(1 + 1)`, `1 0 /`, `""`, `1 +` | erreur | erreur sur stderr, rc=1 |

### La sortie n'est pas `42`   [LITTÉRAL — à corriger en premier]
```
$ ./RPN "7 7 * 7 -" | cat -v
^[[1;37mRESULT = 42^[[0m
```
Le sujet attend :
```
$> ./RPN "7 7 * 7 -"
42
```
C'est le **premier test de la grille**, sujet ouvert à côté. Contrairement aux couleurs, le préfixe `RESULT = ` est **visible** dans le terminal.
- **Où :** `ex01/main.cpp:31`.
- **Piste :** `std::cout << instance.getStack().top() << std::endl;`

### Le message d'erreur n'est pas `Error`   [LITTÉRAL]
```
$ ./RPN "(1 + 1)" 2>&1 | cat -v
^[[1;31mFatal: ^[[0mInvalid token : (1
```
Le flux est bon (stderr), mais pas le texte : le sujet affiche `Error`.
- **Où :** `ex01/errors.cpp:19` construit `"Fatal: "`, et le `catch` de `ex01/main.cpp` l'affiche.
- **Piste :** dans le `catch`, écrire `std::cerr << "Error" << std::endl;`. Si tu veux garder le détail, écris `"Error: " << e.what()`.

### Petits défauts   [POLISH]
- **Guillemet non fermé** dans le message d'usage : `Should be prototyped: "a b c * +` (`main.cpp:15`).
- **Cast manquant :** `std::isdigit(token[0])` est appelé sans `static_cast<unsigned char>` (`main.cpp:23`). Ex00 et ex02 font ce cast : aligne les trois.
- **Helpers inutilisés :** `F_ErrMsg` et `F_WarnMsg` ne servent pas.
- **Encapsulation :** `getStack()` renvoie une référence modifiable, et `main` empile directement dans l'objet. Prépare la réponse à « pourquoi le parsing est dans le main ? ».

---

## ex02 — PmergeMe

### Ce qui passe
- **Exemple du sujet :** `./PmergeMe 3 5 9 7 4` donne `After: 3 4 5 7 9`.
- **Commande de la grille :** `jot -r 3000 1 1000`, pleine de doublons, donne une sortie **identique à `sort -n`**.
- **Erreurs :** `-1 2`, `0 1`, `1 a`, `+3`, `""`, `2147483648` et l'absence d'argument affichent tous `Error` sur stderr, sans rien sur stdout.
- **Ford-Johnson, vérifié dans les deux conteneurs :** sur 3000 tirages pour chaque taille de 1 à 33, avec et sans doublons, aucun tri n'est faux et le nombre de comparaisons ne dépasse jamais le minimum théorique F(n) (13 pour n=7, 66 pour n=21, 126 pour n=33).
- **Temps :** environ 15 500 µs pour `vector` et 23 500 µs pour `deque` sur 3000 nombres. Le chrono couvre le remplissage et le tri, comme le demande le sujet.
- **Implémentation :** une par conteneur, sans template, comme le conseille le sujet.

### Prouver que le programme marche avec des tests   [GRILLE]
La grille ex02 commence par : *« there has to be enough tests to prove the program works as expected. If there isn't, do not grade this exercise. »* Les tests de cette session ont tourné dans le scratchpad, donc **rien n'est dans ton repo**.
- **Piste :** un petit `ex02/tests.sh`, autorisé comme fichier en plus. Il ferait : les cas d'erreur, une comparaison de `After:` avec `sort -n` (en retirant les codes couleur avec `sed $'s/\033\\[[0-9;]*m//g'`), puis `jot -r 3000 1 1000`.

### Couleurs sur stdout   [choix assumé]
`Before:`, `After:` et `Time` sont colorés, et `endofline` ajoute `\033[0m`. Dans un pipe, la ligne `After:` contient donc des codes ANSI. Ce n'est pas interdit, mais si un correcteur passe la sortie dans `sort -c`, ça échouera. Prépare la commande `sed` ci-dessus pour lui montrer que le tri est bon.

### `.00` affiché sur les temps   [POLISH]
`clock()` n'a qu'une précision de 1 µs ici, donc les temps affichent toujours `.00` (`main.cpp:36`, `setprecision(2)`). Ce n'est pas faux, mais ces deux décimales n'ont aucune valeur réelle. Prépare l'explication, ou passe à `setprecision(0)`.

### Les temps croissent en n² au-delà de 3000   [POLISH]
| Nombres | `vector` | `deque` |
|---|---|---|
| 3 000 | ~15 500 µs | ~23 500 µs |
| 20 000 | ~584 000 µs | ~855 000 µs |

L'exigence de 3000 nombres est largement tenue. La cause, c'est la recherche des perdants (`while` + `erase`) et la mise à jour de `positions`, qui sont toutes deux linéaires à chaque fois. Ça ne change pas le nombre de comparaisons, mais c'est une question possible : *« est-ce que ça passe à l'échelle ? »*.

### Inutilisés   [POLISH]
Aucune fonction de `errors.*` n'est appelée dans ex02, seulement l'enum `SUCCESS`/`FAILURE`. `#include <stdexcept>` est inutile dans `main.cpp`.

---

## Défense

### ex02 — savoir l'expliquer et le modifier   [DÉFENSE — priorité 1]
- **Les exigences :** la grille dit *« A brief explanation is expected. In case of doubt, the evaluation stops here. »* Le chap. VIII annonce qu'on peut te demander **une petite modification en direct**.
- **Le contexte :** le code de Ford-Johnson a été écrit avec Claude dans cette session. Le chap. IV (AI Instructions) prévient précisément contre l'incapacité à expliquer pendant l'évaluation.
- **La piste :** ton plan est le bon. Réécris `fordJohnsonVector` et ses 4 helpers **de mémoire**, puis compare avec le code actuel.

Questions probables :
1. Pourquoi seuls les gagnants passent par la récursion, et jamais les perdants ?
2. Pourquoi `b1` va en tête sans comparaison ?
3. Pourquoi l'ordre de Jacobsthal (1, 3, 5, 11, 21…), et pourquoi en partant de l'indice le plus haut de chaque groupe ?
4. Pourquoi la recherche s'arrête à `positions[idx]`, et pourquoi le straggler cherche dans toute la liste ?
5. Pourquoi calculer `p` **avant** `chain.insert` (invalidation des itérateurs) ?
6. Comment les doublons sont-ils gérés (`erase` de la paire utilisée, `upper_bound`) ?
7. Pourquoi `deque` est plus lente que `vector` (un bloc contigu contre des blocs reliés par une table) ?
8. Pourquoi avoir choisi `vector` et `deque` (accès aléatoire pour la recherche binaire, et `map`/`list` déjà pris) ?

Modifications en direct possibles à t'entraîner : refuser les doublons, afficher aussi la `deque` dans `After:`, changer l'unité des temps.

### ex00 / ex01
- **ex00 :** pourquoi `std::map` (clés triées, `upper_bound` puis `--it`) ?
- **ex01 :** pourquoi `std::stack` sur `std::list` (LIFO, et `deque` gardée libre pour ex02) ?

---

## Repo, avant de rendre   [HYGIÈNE]
- **Le binaire `ex02/PmergeMe` est commité** : `git ls-files` le liste, et `git status` le montre modifié. De plus, le `.gitignore` racine ignore les binaires de CPP08, mais **aucun de CPP09**.
  - **Piste :** ajouter `LEVEL5/C++2/CPP09/ex00/btc`, `.../ex01/RPN` et `.../ex02/PmergeMe` au `.gitignore`, puis `git rm --cached LEVEL5/C++2/CPP09/ex02/PmergeMe`. Le fichier sera ainsi retiré de git, mais gardé sur le disque.
- **Fichiers commités qui n'ont rien à faire dans un rendu :** `ex00/idees.txt`, `REVIEW.md`, `CHECK.md`, `PROGRESS.json`, `correction/`. Ça compte si ce dossier est copié tel quel vers le dépôt de rendu.
- **`data.csv` est bien commité,** et il est nécessaire à ex00 ✅.
- **`PROGRESS.json` :** ex00 et ex01 sont encore « partial », à passer à `done` une fois les points 2, 3 et 6 réglés.
