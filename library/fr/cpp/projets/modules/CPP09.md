# 🏗️ CPP09 — La STL en pratique : choisis le conteneur, puis défends-le

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP09.md)

> **TL;DR.** Trois vrais programmes : une recherche de cours du Bitcoin (`btc`), une calculatrice en notation polonaise inverse (`RPN`) et un tri par fusion-insertion (`PmergeMe`). La subtilité, c'est une règle sur les **conteneurs**. Tu dois en utiliser au moins un par exercice, et **un conteneur utilisé dans un exercice ne peut plus servir dans les suivants**. PmergeMe en demande deux. En soutenance, on te demande *pourquoi* tu as choisi chacun d'eux. Ce module porte donc vraiment sur la question soulevée par CPP08 : quelle structure de données colle à ce schéma d'accès ?

Voir aussi : [`STL.md`](../../../../en/cpp/notions/advanced/STL.md) (EN) · [`ALGORITHMS.md`](../../../../en/cpp/notions/advanced/ALGORITHMS.md) (EN) · [`containers/INDEX.md`](../../notions/containers/INDEX.md) · [`MAP.md`](../../notions/containers/MAP.md) · [`STACK.md`](../../notions/containers/STACK.md) · [`VECTOR.md`](../../notions/containers/VECTOR.md) · [`DEQUE.md`](../../notions/containers/DEQUE.md) · [`LIST.md`](../../notions/containers/LIST.md) · [`STRING.md`](../../../../en/cpp/notions/fundamentals/STRING.md) (EN) · [`TRY_CATCH_THROW.md`](../../../../en/cpp/lexique/TRY_CATCH_THROW.md) (EN) · Précédent : [`CPP08.md`](../../../../en/cpp/projets/modules/CPP08.md) (EN)

---

## 1. Le problème, dans le C que tu connais déjà

En C, chacun de ces programmes commence par *construire* une structure de données :
- **btc** : un tableau trié de dates plus `bsearch`, ou un ABR écrit à la main.
- **RPN** : un `int stack[64]` fixe et un indice `top` (souvenirs de push_swap).
- **PmergeMe** : des tableaux `malloc`és et beaucoup de `memmove`.

La moitié du travail, et la plupart des bugs, viendraient de la structure. En C++98, les structures existent déjà et sont testées. Il reste la partie qui demande de réfléchir : **faire correspondre la structure au besoin.**

---

## 2. Le mécanisme : un plan de conteneurs pour tout le module

La règle : *« Vous devez utiliser au moins un conteneur pour chaque exercice… Une fois un conteneur utilisé, vous ne pouvez plus l'utiliser pour le reste du module. »* Planifie les trois exercices **avant** d'écrire ex00 :

| Exercice | Conteneur | Pourquoi celui-là (ta réponse en soutenance) |
|---|---|---|
| ex00 `btc` | `std::map<std::string, float>` | Les clés restent **triées**. `upper_bound` trouve « la date antérieure la plus proche » en O(log n). Des dates en chaînes `YYYY-MM-DD` se trient correctement comme du texte |
| ex01 `RPN` | `std::stack<long, std::list<long> >` | La RPN *est* une pile : on empile les opérandes, on en dépile 2 pour chaque opérateur. L'adosser à une `list` garde `deque` libre pour ex02 (voir le piège plus bas) |
| ex02 `PmergeMe` | `std::vector<int>` **et** `std::deque<int>` | Les deux offrent l'accès aléatoire, dont l'insertion dichotomique a besoin. Comparer un bloc contigu à un bloc découpé en morceaux, c'est exactement la question de timing que pose l'évaluateur |

```
         ex00                     ex01                      ex02
   ┌──────────────┐        ┌──────────────┐        ┌──────────────┬──────────────┐
   │     map      │        │ stack<list>  │        │    vector    │    deque     │
   │ arbre trié   │        │ LIFO         │        │ un seul bloc │ morceaux     │
   └──────────────┘        └──────────────┘        └──────────────┴──────────────┘
   map est grillé ────────► list est grillé ───────► vector + deque, tous deux neufs
```

> ⚠️ **Le piège du conteneur caché.** Par défaut, `std::stack<T>` repose sur un `std::deque<T>`. Si ex01 utilise un `std::stack` tout simple et qu'ex02 utilise `deque`, un évaluateur strict peut soutenir que tu as réutilisé `deque`. Nommer le conteneur sous-jacent, comme dans `std::stack<long, std::list<long> >`, coupe court à la discussion. Note l'espace dans `> >` : le C++98 lit `>>` comme l'opérateur de décalage.

**Règles du Makefile pour chaque exercice :** `$(NAME)`, `all`, `clean`, `fclean`, `re`, et **pas de relink**. Un deuxième `make` doit afficher « nothing to be done ».

---

## 3. ex00 — `btc` (Bitcoin Exchange)

### Ce que demande le sujet
- `./btc input.txt`
  - Le programme charge le **`data.csv`** fourni (`date,exchange_rate`, une ligne par jour).
  - Puis il lit le fichier d'entrée ligne par ligne, au format `date | value`.
  - Pour chaque ligne, il affiche `date => value = value × taux`.
- **Date :** `Année-Mois-Jour`, et ce doit être une vraie date. **Valeur :** un float ou un entier positif entre **0 et 1000**.
- **Si la date n'est pas dans la base, utilise la date *inférieure* la plus proche.** Jamais la supérieure.

```
$> ./btc
Error: could not open file.
$> ./btc input.txt
2011-01-03 => 3 = 0.9
2011-01-03 => 2 = 0.6
2011-01-03 => 1 = 0.3
2011-01-03 => 1.2 = 0.36
2011-01-09 => 1 = 0.32
Error: not a positive number.
Error: bad input => 2001-42-42
2012-01-11 => 1 = 7.1
Error: too large a number.
```

### La recherche : `upper_bound`, puis un pas en arrière

```
 map (triée par clé) :
   2011-01-01  2011-01-03  2011-01-07  2011-01-10 ...
                   ▲           ▲
 requête 2011-01-05│           └── upper_bound : première clé  > requête
                   └────────────── --it        : dernière clé <= requête  ✓ on prend ce taux

 requête 2011-01-03 → upper_bound pointe sur 2011-01-07 → --it → 2011-01-03 (correspondance exacte ✓)
 requête 2008-12-31 → upper_bound == begin() → il n'y a AUCUNE date antérieure → ligne d'erreur
```

```cpp
std::map<std::string, float>::const_iterator it = _rates.upper_bound(date);
if (it == _rates.begin())
    throw std::runtime_error("Error: no data before " + date);
--it;
float result = value * it->second;
```

- **`upper_bound(key)`** renvoie le premier élément dont la clé est **strictement supérieure**. Reculer d'un cran donne « égale, ou l'inférieure la plus proche ». Ça gère la correspondance exacte sans cas particulier. `lower_bound` (première clé **≥**) demanderait un test supplémentaire `if (it->first != date)`.
- **Vérifie `begin()` avant `--it`.** Décrémenter `begin()` est un comportement indéfini. Toute requête antérieure à la première date de `data.csv` passe par cette branche.
- **Pourquoi des clés en chaînes fonctionnent :** `"2011-01-05" < "2011-01-07"` se compare caractère par caractère. Des dates ISO complétées par des zéros se trient dans le même ordre que le calendrier. Une fois le format validé, la chaîne *est* la clé.

### Analyser une ligne

```cpp
std::string line;
std::getline(file, line);                               // skip the header "date | value"
while (std::getline(file, line))
{
    std::string::size_type bar = line.find(" | ");
    if (bar == std::string::npos)
    {
        std::cerr << "Error: bad input => " << line << std::endl;
        continue;
    }
    std::string date = line.substr(0, bar);
    std::string rawValue = line.substr(bar + 3);
    ...
}
```

- **`continue`, jamais `return`.** La fiche d'évaluation : *« Le programme ne doit pas arrêter son exécution avant d'avoir effectué les opérations sur tout le fichier. »* Une ligne invalide produit une ligne d'erreur, et le traitement continue.
- Convertis la valeur avec `std::strtod(raw.c_str(), &end)` et vérifie que `*end == '\0'`. Puis teste `< 0` (`not a positive number`) et `> 1000` (`too large a number`).
- ⚠️ `strtod` est généreux : il accepte aussi `nan`, `inf`, l'hexa comme `0x10`, `1e2` et les espaces en tête. Testé : `nan` passe **les deux** tests de plage (toute comparaison avec NaN est fausse) et affiche n'importe quoi. Rejette tout ce qui n'est pas des chiffres avec une partie `.` optionnelle avant de l'appeler.
- **Validation de la date :** mois entre 1 et 12, et jour dans la longueur du mois, **années bissextiles comprises** : `(y % 4 == 0 && y % 100 != 0) || y % 400 == 0`. `2001-42-42` échoue, et `2023-02-29` aussi.

### ⚠️ Pièges
- **Valide la ligne d'en-tête** du fichier d'entrée, mais n'affiche pas de ligne de résultat pour elle.
- Pour btc, le sujet n'exige que les résultats sur stdout, donc le flux d'erreur est à ton choix. `std::cerr` est le choix conventionnel, et l'exemple du sujet n'a l'air mélangé que parce que les deux flux partagent le terminal.
- **Fichier vide / fichier avec seulement un en-tête :** n'affiche rien et ne plante pas. La fiche teste un fichier vide.
- `data.csv` fait partie du projet. Charge-le avec un chemin relatif à l'endroit où le programme s'exécute, et gère son absence avec une erreur propre.

---

## 4. ex01 — `RPN` (notation polonaise inverse)

### Ce que demande le sujet
- `./RPN "<expression>"`. Les opérandes sont des chiffres seuls **< 10** (les résultats peuvent être plus grands). Les opérateurs sont `+ - * /`.
- Affiche le résultat sur stdout, ou `Error` sur stderr.
- Les parenthèses et les nombres décimaux n'ont pas à être gérés.

### Tout l'algorithme tient dans une pile

```
 "8 9 * 9 - 9 - 9 - 4 - 1 +"

 jeton  action              pile (bas → haut)
 8      empile              [8]
 9      empile              [8 9]
 *      dépile 9, 8 ; 8*9   [72]
 9      empile              [72 9]
 -      72-9                [63]
 9 -                        [54]
 9 -                        [45]
 4 -                        [41]
 1 +                        [42]
 fin    il en reste un seul →  42 ✓
```

**L'ordre des opérandes compte :** le *premier* dépilé est l'**opérande de droite**. `"5 3 -"` signifie `5 - 3`, pas `3 - 5`.

### Le cœur du code

```cpp
static long evaluate(std::string const &expr)
{
    std::stack<long, std::list<long> >  st;
    std::istringstream                  in(expr);
    std::string                         tok;

    while (in >> tok)
    {
        if (tok.size() == 1 && std::isdigit(tok[0]))
            st.push(tok[0] - '0');
        else if (tok.size() == 1 && std::string("+-*/").find(tok[0]) != std::string::npos)
        {
            if (st.size() < 2)
                throw std::runtime_error("not enough operands");
            long right = st.top();
            st.pop();
            long left = st.top();
            st.pop();
            if (tok[0] == '+')
                st.push(left + right);
            else if (tok[0] == '-')
                st.push(left - right);
            else if (tok[0] == '*')
                st.push(left * right);
            else
            {
                if (right == 0)
                    throw std::runtime_error("division by zero");
                st.push(left / right);
            }
        }
        else
            throw std::runtime_error("invalid token: " + tok);
    }
    if (st.size() != 1)
        throw std::runtime_error("leftover operands");
    return st.top();
}
```

Pas à pas :
- **`std::istringstream in(expr); in >> tok`** découpe sur les espaces à ta place. C'est `ft_split(expr, ' ')` sans la gestion mémoire.
- **`tok.size() == 1`** fait de `"12"` une erreur (les opérandes doivent être < 10), et rejette `"("` via la branche `else`.
- **Trois cas d'erreur, chacun un `throw` :** moins de 2 opérandes pour un opérateur, division par zéro, et pas exactement une valeur restante à la fin. `main` attrape `std::exception &` et affiche `Error` sur stderr, le schéma de CPP05.
- **`long` au lieu de `int`** repousse le dépassement : `"9 9 * 9 * 9 * …"` grossit vite. Le sujet n'exige pas de détecter le dépassement, mais tu dois savoir où se trouve ta limite.

### Résultats testés

| Expression | Sortie |
|---|---|
| `8 9 * 9 - 9 - 9 - 4 - 1 +` (sujet) | `42` |
| `7 7 * 7 -` (sujet) | `42` |
| `1 2 * 2 / 2 * 2 4 - +` (sujet) | `0` |
| `(1 + 1)` (sujet) | `Error` |
| `9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -` (fiche d'évaluation) | `42` |
| `1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /` (fiche d'évaluation) | `15` |
| `1 0 /` · `1 +` · `1 2` · `12 3 +` | `Error` |

### ⚠️ Pièges
- La division entière tronque vers zéro : `"7 2 /"` donne `3`. C'est normal ; les décimaux ne sont pas demandés.
- Inclus `<cctype>` pour `isdigit`, `<sstream>` pour `istringstream` et `<list>` pour le conteneur sous-jacent de la pile.

---

## 5. ex02 — `PmergeMe` (tri par fusion-insertion de Ford-Johnson)

### Ce que demande le sujet
- `./PmergeMe 3 5 9 7 4` : des entiers positifs en arguments. Tout le reste, comme `"-1"`, donne `Error` sur stderr.
- Trie-les avec le **tri par fusion-insertion** (Ford-Johnson, *TAOCP* de Knuth, vol. 3, p. 184), **une fois par conteneur**. Le sujet conseille une implémentation séparée pour chaque conteneur, plutôt qu'une fonction générique.
- Il doit gérer **au moins 3000** entiers. La gestion des doublons est laissée à ton choix.
- Sortie :

```
$> ./PmergeMe 3 5 9 7 4
Before: 3 5 9 7 4
After: 3 4 5 7 9
Time to process a range of 5 elements with std::[..] : 0.00031 us
Time to process a range of 5 elements with std::[..] : 0.00014 us
$> ./PmergeMe "-1" "2"
Error
```

- Quatre lignes : avant, après, puis une ligne de temps par conteneur qui **nomme le conteneur** (remplis `std::[..]`).
- **Le format du temps est libre**, mais la précision doit rendre visible la différence entre les deux conteneurs. Les chiffres du sujet lui-même sont « volontairement étranges ».
- Le temps couvre **tout ce que fait le conteneur** : le remplissage à partir des arguments *et* le tri.

### Pourquoi cet algorithme existe
Ford-Johnson n'est pas le tri le plus *rapide* en pratique. Il est conçu pour faire **le moins de comparaisons possible**, et pour de petits n il est pratiquement optimal. Notre implémentation testée (ci-dessous) n'a jamais dépassé le minimum théorique dans le pire cas, F(n) = Σ⌈log₂(3k/4)⌉ (13 pour n=7, 66 pour n=21), sur 3000 entrées aléatoires pour chaque n de 1 à 33. C'est l'idée à expliquer en soutenance : **il économise des comparaisons, pas du temps.**

### Les trois étapes

```
 entrée :  8 3 6 1 7 2 5                             (n = 7)

 ① FORMER DES PAIRES et comparer dans chaque paire (⌊n/2⌋ comparaisons)
         (8,3) (6,1) (7,2)   isolé : 5
          a b   a b   a b                a = le plus grand (« gagnant »), b = le plus petit

 ② Trier RÉCURSIVEMENT les gagnants avec ce même algorithme
         a:  6  7  8     → renommer dans l'ordre trié : a1=6 a2=7 a3=8
         b:  1  2  3                                    b1=1 b2=2 b3=3   b4=5 (isolé)

    chaîne principale = b1 a1 a2 a3 = 1 6 7 8   (b1 ≤ a1 ≤ tout le reste, donc il passe devant gratuitement)

 ③ INSÉRER les b en attente dans l'ordre de JACOBSTHAL, chacun par recherche dichotomique,
    en cherchant UNIQUEMENT à gauche de son propre partenaire a_k (puisque b_k ≤ a_k)

    ordre : b3 b2 | b5 b4 | b11 b10 … b6 | b21 … b12 | …    (coupé au nombre de b)

    b3=3 : cherche dans [1 6 7]   (à gauche de a3=8)   → 1 3 6 7 8
    b2=2 : cherche dans [1 3 6]   (à gauche de a2=7)   → 1 2 3 6 7 8
    b4=5 : isolé, pas de partenaire → toute la chaîne → 1 2 3 5 6 7 8   ✓
```

### Pourquoi l'ordre de Jacobsthal ?
Une recherche dichotomique sur une plage de **2ᵏ − 1** éléments prend exactement *k* comparaisons. Sur 2ᵏ éléments, elle en prend *k + 1*. Les nombres de Jacobsthal **1, 3, 5, 11, 21, 43…** (`J(n) = J(n−1) + 2·J(n−2)`) sont choisis pour que la zone de recherche de chaque groupe reste à 2ᵏ − 1 éléments. Insérer d'abord l'indice *le plus élevé* d'un groupe garantit que les éléments insérés ensuite n'agrandissent pas les plages qui restent à parcourir. Insère dans l'ordre simple `b2 b3 b4…` et l'algorithme trie toujours, mais ce n'est plus du Ford-Johnson. Sois prêt à l'expliquer en soutenance.

```cpp
static std::vector<size_t> jacobsthalOrder(size_t m)
{
    std::vector<size_t> order;
    size_t prev = 1;
    size_t a = 1;
    size_t b = 3;

    while (prev < m)
    {
        size_t hi = std::min(b, m);
        for (size_t k = hi; k > prev; --k)
            order.push_back(k);
        prev = hi;
        size_t next = b + 2 * a;
        a = b;
        b = next;
    }
    return order;
}
```

`jacobsthalOrder(12)` donne `b3 b2 b5 b4 b11 b10 b9 b8 b7 b6 b12`. `m` compte tous les b (l'isolé compris), et `b1` est sauté parce qu'il est déjà dans la chaîne.

### L'étape ③ en code : l'insertion dichotomique bornée

```cpp
for (size_t o = 0; o < order.size(); o++)
{
    size_t k = order[o];
    int    value;
    size_t bound;
    if (k > bigs.size())
    {
        value = straggler;
        bound = chain.size();
    }
    else
    {
        value = smalls[k - 1];
        bound = aPos[k];
    }
    std::vector<int>::iterator pos =
        std::upper_bound(chain.begin(), chain.begin() + bound, value);
    size_t q = pos - chain.begin();
    chain.insert(pos, value);
    for (size_t j = 1; j <= bigs.size(); j++)
        if (aPos[j] >= q)
            aPos[j]++;
}
```

- **`std::upper_bound` *est* la recherche dichotomique.** N'écris pas la tienne. Sa plage de recherche s'arrête à `chain.begin() + bound`, c'est-à-dire la position de `a_k`.
- **`aPos[k]`** retient où se trouve actuellement `a_k` dans la chaîne. Chaque insertion à la position `q` décale d'un cran vers la droite les a situés à `q` ou après, donc on met leurs positions à jour. Sans cette comptabilité, tu chercherais au-delà de `a_k` et tu perdrais l'économie de comparaisons.
- **`upper_bound`, pas `lower_bound`,** pour que les valeurs égales se placent après celles déjà présentes. Les doublons sont autorisés et se trient toujours correctement (testé).
- **L'appariement quand il y a des doublons :** après le tri récursif des gagnants, chaque `a` trié doit retrouver son `b`. Garde les paires (par ex. `std::vector<std::pair<int,int> >`) et associe chaque gagnant trié à une paire non utilisée de même valeur. Des gagnants égaux peuvent échanger leurs partenaires sans risque, car le partenaire reste ≤ à son gagnant.

### Mesure du temps (C++98 : pas de `<chrono>`)

```cpp
std::clock_t start = std::clock();
std::vector<int> v = parseInto<std::vector<int> >(ac, av);
fordJohnsonVector(v);
std::clock_t end = std::clock();
double us = 1000000.0 * (end - start) / CLOCKS_PER_SEC;
```

`std::clock()`, de `<ctime>`, mesure le temps processeur. Convertis-le en microsecondes, comme dans l'exemple du sujet.

**Mesuré sur ce Mac** (3000 entiers aléatoires entre 1 et 1000, alors que la commande du sujet utilise 1–100000 ; sans optimisation, même implémentation pour les deux conteneurs) : **vector ≈ 28 ms, deque ≈ 31 ms**, stable d'une exécution à l'autre. Voici comment expliquer l'écart :

| | `std::vector` | `std::deque` |
|---|---|---|
| Mémoire | Un seul bloc contigu | Des morceaux de taille fixe + une table de pointeurs vers ces morceaux |
| `chain.begin() + bound`, `*it` | Simple arithmétique de pointeurs | Deux indirections (table des morceaux, puis morceau) |
| `insert` au milieu | Décalage de la fin, façon `memmove` | Décale vers l'extrémité la plus proche, à travers les morceaux |
| Cache CPU | Excellent : les voisins sont adjacents | Moins bon : sauts entre les morceaux |

La recherche dichotomique et l'insertion au milieu sont exactement les opérations où un bloc contigu l'emporte. Tes chiffres seront différents. Ce qui compte, c'est de savoir les **expliquer**.

### Analyser les arguments

```cpp
for (int i = 1; i < ac; i++)
{
    char *end;
    errno = 0;
    long n = std::strtol(av[i], &end, 10);
    if (*av[i] == '\0' || *end != '\0' || errno == ERANGE || n <= 0 || n > INT_MAX)
        throw std::runtime_error("Error");
    v.push_back(static_cast<int>(n));
}
```

Il faut `<cerrno>` et `<climits>`. Ce code rejette `-1`, `abc`, `3a`, les chaînes vides et les dépassements, mais `strtol` accepte quand même les espaces en tête et le `+`, donc `" +5"` passe. Savoir si `0` compte comme « positif » est ton choix : le test ci-dessus le rejette. Dis-le en soutenance.

### ⚠️ Pièges
- **Les évaluateurs vérifient que l'algorithme est présent pour *chaque* conteneur.** Utiliser `std::sort` sur le second pour « vérifier » le résultat, c'est bien dans un test. Comme implémentation, non.
- Les deux conteneurs doivent ressortir **triés à l'identique**. Compare-les en build de debug.
- La commande de test sur macOS est `./PmergeMe $(jot -r 3000 1 100000 | tr '\n' ' ')`. Sous Linux, c'est `shuf -i 1-100000 -n 3000 | tr "\n" " "`. `jot -r` tire avec remise, donc même cette plage **peut contenir des doublons** (38 lors d'un essai ici) : décide comment tu les gères *avant* la soutenance.
- La fiche teste d'abord 5 à 10 nombres choisis à la main. Si ce petit test échoue, l'évaluation s'arrête.

---

## 6. Ce que vérifie l'évaluateur

D'après la fiche d'évaluation officielle 42evalhub :

> Le dernier chapitre du sujet ajoute que la soutenance **peut demander une petite modification en direct** (changer un affichage, ajuster une structure de données). Connaître ton propre code ligne par ligne fait partie de la note.

**Prérequis**
- [ ] `c++ -Wall -Wextra -Werror`, C++98. Les conteneurs sont autorisés, puisque le module porte sur la STL.
- [ ] Pas de corps de fonction dans un header (templates exceptés). Le Makefile utilise `c++` et les flags.
- [ ] Pas de `*alloc`, `*printf` ni `free`. Pas de bibliothèque externe.
- [ ] Pas de fuites. **Aucun crash ni arrêt inattendu pendant toute la soutenance**, sinon la note est 0.

**ex00 : btc**
- [ ] Un Makefile avec les règles habituelles. Au moins un conteneur, et tu sais **expliquer pourquoi celui-là**.
- [ ] Gestion d'erreurs : un fichier vide, un fichier avec des erreurs, des dates invalides, des valeurs > 1000 ou < 0. **Tout le fichier est traité.**
- [ ] Avec le `input.csv` de l'évaluateur (qu'il peut modifier), les résultats correspondent aux vérifications manuelles, et une date absente utilise la **date inférieure la plus proche**.

**ex01 : RPN**
- [ ] Makefile. Un conteneur, justifié. **Pas un conteneur d'ex00.**
- [ ] Résultats corrects sur les formules de l'évaluateur, y compris les deux supplémentaires du §4. Ni parenthèses ni décimaux exigés.

**ex02 : PmergeMe**
- [ ] Makefile. **Deux conteneurs**, justifiés. **Aucun des deux utilisé dans ex00 ou ex01.**
- [ ] La fusion-insertion (Ford-Johnson) est **présente et utilisée pour chaque conteneur**. Tu en donnes une courte explication. *« En cas de doute, l'évaluation s'arrête ici. »*
- [ ] 5 à 10 entiers positifs choisis à la main ressortent triés.
- [ ] 3000 entiers aléatoires (`jot` / `shuf`) fonctionnent, et tu sais **expliquer la différence de temps** entre les conteneurs.

---

## 7. Aide-mémoire

| Besoin | Écriture C++98 |
|---|---|
| Clé la plus proche ≤ `k` | `it = m.upper_bound(k); if (it != m.begin()) --it;` |
| Lire un fichier ligne par ligne | `std::ifstream f(path); while (std::getline(f, line))` |
| Découper sur les espaces | `std::istringstream in(s); while (in >> tok)` |
| Chaîne → nombre, strictement | `strtod` / `strtol` avec un pointeur `end`, `*end == '\0'`, `errno` |
| Pile avec un conteneur sous-jacent choisi | `std::stack<long, std::list<long> >` (espace dans `> >`) |
| Recherche dichotomique d'un point d'insertion | `std::upper_bound(first, last, value)` |
| Insérer à un itérateur | `c.insert(pos, value)` |
| Temps CPU | `std::clock()` / `CLOCKS_PER_SEC` de `<ctime>` |
| Année bissextile | `(y % 4 == 0 && y % 100 != 0) \|\| y % 400 == 0` |

**Ce que ce module te laisse :** la question « quel conteneur ? » a désormais une réponse qui dépend du *schéma d'accès* : recherche triée → `map`, LIFO → `stack`, accès aléatoire avec insertions qui ménagent le cache → `vector`. C'est ce réflexe que toute la piscine C++ cherchait à construire, et il te suit directement dans webserv.

➡️ Suite : [`../webserv/INDEX.md`](../../../../en/cpp/projets/webserv/INDEX.md) (EN). La STL et le RAII rencontrent les sockets et `poll()`.
