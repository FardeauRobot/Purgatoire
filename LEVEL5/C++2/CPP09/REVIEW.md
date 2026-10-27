# REVIEW — CPP09 ex00 / ex01 — 2026-10-23

Sources : grille 42evalhub (`correction/.grid-cpp09.md`, récupérée le 2026-10-21) et sujet `CPP09.pdf` v3.1 (chap. II, III, V, VI, lus en texte). Toutes les sorties ci-dessous ont été **mesurées** pendant cette session. `| cat -v` rend visibles les octets ANSI (`^[[1;31m` = code couleur).

## Verdict

| | Grille 42evalhub | Sujet pris littéralement |
|---|---|---|
| **ex00 btc** | ✅ **Passe** | ⚠️ 2 écarts (flux d'erreur, couleurs) |
| **ex01 RPN** | ✅ **Passe** | ❌ 2 écarts visibles (`RESULT = 42` au lieu de `42`, `Fatal:` au lieu de `Error`) |

**Aucun point éliminatoire.** Ça compile avec les flags de la grille, il n'y a pas de relink, pas de leak, pas de crash (ASan/UBSan propres), pas de fonction interdite, pas de corps de fonction dans un header, et les conteneurs sont différents d'un exercice à l'autre. Les deux exercices passent la grille telle qu'elle est écrite.

En revanche, un évaluateur qui compare ta sortie à celle du sujet verra une différence **dès le premier test de l'ex01**. Le chap. II le dit : *« the examples can show requirements that are not explicitly written in the instructions »*. C'est le point à corriger en priorité.

---

## Vérifications communes (ex00 + ex01)

| Check | ex00 | ex01 |
|---|---|---|
| `make re` sans warning, binaire bien nommé | ✅ `btc` | ✅ `RPN` |
| 2ᵉ `make` → `Nothing to be done for 'all'` (pas de relink) | ✅ | ✅ |
| Règles `$(NAME) all clean fclean re` | ✅ | ✅ |
| `c++ -Wall -Wextra -Werror` sans `-std`, avec `-std=c++98`, avec `-std=c++98 -pedantic -Wshadow` | ✅ ×3 | ✅ ×3 |
| Chaque `.hpp` compilé seul (`-fsyntax-only`) | ✅ 3/3 | ✅ 3/3 |
| grep `using namespace\|friend\|printf\|alloc\|free(\|auto \|nullptr\|override` | rc=1 (aucune occurrence) | rc=1 |
| Fichiers à rendre | ✅ `Makefile main.cpp BitcoinExchange.{cpp,hpp}` | ✅ `Makefile main.cpp RPN.{cpp,hpp}` |
| Conteneur | `std::map` | `std::stack<int, std::list<int> >`, pas de `std::map` (grep) |
| `leaks --atExit` (valgrind n'est **pas installé** sur ce Mac) | `0 leaks for 0 total leaked bytes` sur 6 cas | `0 leaks for 0 total leaked bytes` sur 7 cas |
| Build ASan+UBSan (copie dans le scratchpad) | 0 rapport | 0 rapport |

---

## ex00 — btc

### Ce qui passe (grille)
- **Exemple du sujet** : stdout donne exactement les 6 lignes attendues (`0.9 0.6 0.3 0.36 0.32 7.1`), et les 3 erreurs sortent dans l'ordre.
- **Fichier vide, `/dev/null`, fichier absent, sans argument, 2 arguments** : pas de crash, `Error: could not open file.` quand il faut.
- **Fichier traité en entier** malgré les erreurs : 36 lignes piégées, toutes traitées.
- **Dates fausses rejetées** : `2001-02-29`, `2013-02-29`, `2012-13-01`, `2012-00-10`, `2012-01-32`, `2011-1-3`. Années bissextiles OK : `2012-02-29 => 1 = 4.98`, et `2000-02-29` est reconnue valide (→ « no rate available before », puisque la date est antérieure à la DB).
- **Valeurs** : `-1` → `not a positive number.` ; `2147483648` et `1000.0001` → `too large a number.` ; `0` et `1000` acceptés.
- **Date inférieure**, vérifiée à la main dans `data.csv` : `2011-01-05` et `2011-01-06` → `0.3`, le taux du `2011-01-04` (et pas `0.32`, celui du `2011-01-07`). `2011-01-07` → `0.32`. `2009-01-01` → `no rate available before`, la DB commençant au `2009-01-02`.
- `input.csv` de la grille : `curl` renvoie toujours **http=403**. Pas testé contre le fichier officiel.

### L1 — Les erreurs partent sur stderr alors que l'ex00 ne le demande pas   [LITTÉRAL]
Chap. II : *« Unless specified otherwise, every output message must end with a newline character and be displayed to the **standard output**. »* L'ex01 et l'ex02 précisent « standard error ». **L'ex00, non.** En lecture stricte, `Error: …` devrait donc aller sur stdout.
```
$ ./btc input.txt 2>/dev/null      # les 3 lignes Error: disparaissent
```
- **Où** : `ex00/errors.cpp:8` (`std::cerr`)
- **Piste** : soit envoyer les erreurs de l'ex00 sur `std::cout`, soit préparer ta justification (« convention : stdout pour les résultats, stderr pour les erreurs »). La plupart des évaluateurs l'acceptent, mais c'est contestable texte en main.

### L2 — Codes couleur dans `Error:`   [LITTÉRAL]
```
$ ./btc input.txt 2>&1 | diff expected.txt -  (extrait)
< Error: not a positive number.
> ^[[1;31mError: ^[[0mnot a positive number.
```
Dans un terminal, ça s'affiche correctement. Mais dès qu'on redirige vers un fichier ou un `diff`, les octets ne correspondent plus à la sortie du sujet.
- **Où** : `ex00/errors.cpp:8` (`BOLD_RED`, `RESET`)
- **Piste** : écrire `"Error: "` sans couleur dans ce module.

### P1 — Grands résultats en notation scientifique   [POLISH]
```
2022-03-29 => 1000 = 4.71159e+07     (exact : 47115930)
2030-06-15 => 1 = 47115.9            (taux : 47115.93)
```
C'est la précision par défaut du flux, 6 chiffres significatifs (comme `%g` en C). La grille demande de *« compare some dates manually »*, et ce format fait tiquer. Avec `std::setprecision(15)` **sans** `std::fixed`, la sortie du sujet reste identique (mesuré le 21/10). `std::fixed` casserait `0.9` → `0.900000`.
- **Où** : `ex00/BitcoinExchange.cpp:137`

### P2 — Cas limites qu'un évaluateur peut taper   [POLISH]
- `2011-01-03 | -0` → `2011-01-03 => -0 = -0`. `-0.0 < 0` est faux, donc le zéro négatif passe le test (`BitcoinExchange.cpp:120`).
- Valeur de 300 chiffres → `bad input` au lieu de `too large a number.`, et sa version négative → `bad input` au lieu de `not a positive number.`. Le `>>` échoue en débordement avant que tes tests de borne ne s'exécutent.
- `./btc /tmp` (un dossier) → aucune sortie, rc=0. Sur macOS, `ifstream` ouvre un dossier sans erreur.
- Fichier en CRLF → chaque ligne devient `bad input => …^M`.
- Lancé depuis un autre dossier : `Error: could not open data.csv`. Le chemin est relatif (`BitcoinExchange.cpp:91`). C'est normal, mais lance bien le programme depuis `ex00/` pendant la défense.
- `.5` et `5.` sont acceptés, `+5` et `1e2` refusés. Les deux politiques se défendent, à condition de savoir l'expliquer.

### P3 — Hygiène   [POLISH]
- `# define MAP std::map<std::string, double>` (`BitcoinExchange.hpp:7`) : ici, le bon outil C++98 est `typedef`. Ex01 le fait d'ailleurs déjà (`typedef … Stack`).
- `F_WarnMsg`, `F_ErrExit` et `DBG` ne sont pas utilisés. `idees.txt` est toujours dans le dossier rendu.

---

## ex01 — RPN

### Ce qui passe (grille)
| Expression | Attendu | Obtenu (valeur) |
|---|---|---|
| `8 9 * 9 - 9 - 9 - 4 - 1 +` (sujet + grille) | 42 | 42 |
| `7 7 * 7 -` (sujet) | 42 | 42 |
| `1 2 * 2 / 2 * 2 4 - +` (sujet) | 0 | 0 |
| `(1 + 1)` (sujet) | Error | erreur, rc=1 |
| `9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -` (grille) | 42 | 42 |
| `1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /` (grille) | 15 | 15 |

Erreurs bien gérées, toutes avec rc=1 et rien sur stdout : `""`, `"   "`, `"1 +"`, `"+"`, `"1 2"`, `"1 0 /"`, `"12 3 +"`, `"1 2 %"`, `"1.5 2 +"`, `"-1 2 +"`, `"1 2 +3"`, `"1 2 + x"`, un non-ASCII (`\xe9`), 0 ou 2 arguments.
Résultats négatifs et division entière corrects : `0 3 -` → -3, `0 9 - 2 /` → -4.
Overflow : `9 9 * … *` → `Integer overflow`. J'ai construit **INT_MIN** exactement (`0 2 -` puis 30× `2 *`) → `-2147483648`, et `INT_MIN / -1` comme `INT_MIN * -1` → `Integer overflow`. Ton passage par `long` fonctionne.
Conteneur : `std::stack` sur `std::list`, aucun conteneur de l'ex00 → la case « container present in the first exercise » est respectée.

### L1 — La sortie n'est pas `42` mais `RESULT = 42` en couleur   [LITTÉRAL — à corriger en priorité]
```
$ ./RPN "7 7 * 7 -" | cat -v
^[[1;37mRESULT = 42^[[0m
```
Le sujet montre :
```
$> ./RPN "7 7 * 7 -"
42
```
C'est le premier test de la grille (*« Check that the program runs correctly »*), avec le sujet ouvert à côté. Un évaluateur strict peut considérer que la sortie est fausse, et un script de test qui compare `42` échouera à coup sûr.
- **Où** : `ex01/main.cpp:31`
- **Piste** : `std::cout << instance.getStack().top() << std::endl;`, sans préfixe ni couleur.
- **Pourquoi** : en 42, les exemples du sujet font office de spécification de sortie (chap. II, cité plus haut).

### L2 — Le message d'erreur n'est pas `Error`   [LITTÉRAL]
```
$ ./RPN "(1 + 1)" 2>&1 | cat -v
^[[1;31mFatal: ^[[0mInvalid token : (1
```
Le sujet affiche `Error` sur stderr. Le flux est bon (stderr ✅), mais pas le texte : préfixe `Fatal:`, codes ANSI, détail en plus.
- **Où** : `ex01/errors.cpp:19` (`F_ErrExit` construit `"Fatal: "` en couleur), affiché par `ex01/main.cpp` dans le `catch`.
- **Piste** : dans le `catch`, écrire `std::cerr << "Error" << std::endl;` (ou `"Error: " << e.what()` si tu veux garder le détail, mais sans couleur et en commençant par `Error`).

### P1 — Petits défauts   [POLISH]
- Message d'usage avec un guillemet non fermé : `Should be prototyped: "a b c * +` (`main.cpp:15`).
- `std::isdigit(token[0])` sans cast en `unsigned char` (`main.cpp:23`). Passer un `char` négatif (ex. `\xe9`) est un UB en théorie. En pratique, aucun crash ni rapport UBSan n'a été mesuré. Ex00 fait déjà le cast correctement : aligne les deux.
- `getStack()` renvoie une référence non-const (`RPN.hpp:19`) : `main` empile directement dans l'objet, donc la classe n'encapsule que la moitié du travail. Prévois une réponse si on te demande « pourquoi le parsing est dans le main ? ».
- `F_ErrMsg` et `F_WarnMsg` ne sont pas utilisés dans l'ex01.

### Question de défense probable
« Pourquoi `std::stack` ? Et pourquoi sur une `std::list` plutôt que sur la `std::deque` par défaut ? » Le LIFO correspond exactement à l'évaluation RPN : on dépile deux opérandes, on empile le résultat. La `list` libère `deque` pour l'ex02. Attention : la règle du module (*« Once a container is used you cannot use it for the rest of the module »*) compte probablement `list` **et** `stack` comme utilisés.

---

## Hygiène du repo (avant de rendre)
- `ex00/`, `ex01/` et `correction/` sont **non suivis** par git (`??`). La grille dit *« Only grade the work that is in the student or group's GiT repository »*. Pense à `data.csv`, qui est nécessaire à l'exécution.
- Le `.gitignore` racine couvre `*.o`, `*.d` et les binaires de CPP08 (`easyfind`, `span`, `mutantstack`), mais **pas ceux de CPP09** : `git check-ignore -v ex00/btc ex01/RPN ex02/PmergeMe` → rc=1, aucune règle ne correspond. Ils seraient donc commités. Ajoute les trois lignes sur le modèle de CPP08. J'ai lancé `make fclean` dans ex00 et ex01, donc aucun binaire n'est présent pour l'instant.
- `ex00/idees.txt` : à retirer du rendu.

## Préparer ex02 (PmergeMe)
- Déjà utilisés : `std::map` (ex00), `std::stack` + `std::list` (ex01). **Restent `std::vector` et `std::deque`**, le choix classique.
- La grille impose Ford-Johnson **pour chaque conteneur** (le sujet conseille d'éviter une fonction générique), au moins 3000 entiers (`jot -r 3000 1 1000 | tr '\n' ' '`), `Error` sur stderr pour `-1`, et 4 lignes `Before:` / `After:` / `Time … std::vector` / `Time … std::deque`. Les doublons sont à ta discrétion.
- Scaffold créé avec `ADDUtils ex02 PmergeMe`. J'ai corrigé une ligne : ADDUtils écrit `NAME = ex02` alors que le sujet impose `PmergeMe`, comme pour btc et RPN. Le squelette compile, puis `make fclean`.
