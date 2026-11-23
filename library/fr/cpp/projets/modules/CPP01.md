# C++ Module 01 — Guide d'apprentissage

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP01.md)

**Thème du module :** *allocation mémoire, pointeurs sur membres, références et instruction switch.*

C'est la première fois qu'on te demande de réfléchir à **où** vivent tes objets (pile vs tas), **comment** tu y fais référence (valeur, pointeur, référence) et **comment** aiguiller un comportement sans chaîne de `if/else`. Chaque exercice isole l'une de ces idées.

---

## Exercice 00 — Braiiiiiinnnzzz

### Ce que tu apprends
**Allocation sur la pile vs allocation sur le tas, et quand choisir l'une ou l'autre.**

- `Zombie foo("bar");` — **pile** (*stack*). L'objet meurt quand il sort de sa portée. Le destructeur s'exécute automatiquement.
- `Zombie* foo = new Zombie("bar");` — **tas** (*heap*). L'objet vit jusqu'à ce que tu le `delete`. Ce `delete` est de ta responsabilité.

### Règles pratiques
| Situation | Utilise |
|---|---|
| L'objet n'est nécessaire que dans cette fonction | Pile |
| L'objet doit survivre à la fonction qui l'a créé | Tas (`new`) |
| Tu veux un nombre fixe, connu à la compilation | Pile |
| Tu dois décider du nombre à l'exécution | Tas |

### Astuces
- `newZombie` renvoie un pointeur → l'appelant doit le `delete`. Indique-le dans ton code ou documente-le — la propriété de la mémoire est un *contrat*.
- `randomChump` utilise un **objet local sur la pile** ; pas de `new`, pas de `delete`. Le destructeur se déclenche automatiquement à la fin de la fonction.
- Fais afficher quelque chose au destructeur (par ex. `"<name> destroyed"`). Tu *verras* quand chaque zombie meurt — c'est tout l'intérêt de l'exercice.
- Si le message du destructeur s'affiche deux fois pour le même zombie, tu as un double free. Si tu ne le vois jamais, tu as une fuite.

### Piège
C++98 n'a pas de `nullptr`. Utilise `NULL` ou simplement `0` quand un pointeur peut ne pas être initialisé.

---

## Exercice 01 — Moar brainz!

### Ce que tu apprends
**Allocation d'un tableau sur le tas en un seul `new[]`, et le `delete[]` correspondant.**

```cpp
Zombie* horde = new Zombie[N];     // single allocation for N objects
// ...
delete[] horde;                    // MUST use delete[], not delete
```

### Détails pratiques
- `new Zombie[N]` appelle le **constructeur par défaut** N fois. Ta classe `Zombie` **doit** donc avoir un constructeur par défaut — ce qui veut dire qu'il te faut un autre moyen de fixer le nom ensuite (par ex. un setter, ou une affectation à l'attribut public).
- Tu ne peux pas écrire `new Zombie[N]("name")` — cette syntaxe n'existe pas. Tu dois boucler et les nommer après l'allocation.
- Le schéma habituel :
  ```cpp
  Zombie* horde = new Zombie[N];
  for (int i = 0; i < N; i++)
      horde[i].setName(name);
  ```

### Astuces
- `delete horde;` (sans `[]`) sur un tableau = **comportement indéfini**. Sur la plupart des compilateurs, seul le premier élément est détruit et le reste fuit. Associe toujours `new[]` à `delete[]`.
- Lance avec `valgrind ./moar_brainz` pour confirmer zéro fuite / zéro erreur.
- Le sujet de ex01 dit « allouer N Zombies en une **seule** allocation ». Ne fais donc PAS `N` appels individuels à `new Zombie(...)` dans une boucle — ça ferait `N` allocations.

### Piège
Si ton `Zombie` de ex00 n'a pas de constructeur par défaut, tu dois en ajouter un pour ex01. Chaque exercice est évalué indépendamment, donc pas de souci.

---

## Exercice 02 — HI THIS IS BRAIN

### Ce que tu apprends
**Les références ne sont qu'une autre syntaxe pour « manipuler des adresses ».** Cet exercice lève le mystère : une référence contient la même adresse qu'un pointeur ; elle se déréférence simplement de façon implicite.

### Quoi afficher
```cpp
std::string   str = "HI THIS IS BRAIN";
std::string*  stringPTR = &str;
std::string&  stringREF = str;

std::cout << &str       << std::endl;  // address of str
std::cout << stringPTR  << std::endl;  // same address
std::cout << &stringREF << std::endl;  // same address

std::cout << str        << std::endl;  // value
std::cout << *stringPTR << std::endl;  // same value (dereference)
std::cout << stringREF  << std::endl;  // same value (implicit)
```

### À retenir (à mémoriser)
| | Pointeur | Référence |
|---|---|---|
| Peut être nul | Oui (`NULL`) | **Jamais** |
| Peut être réassigné | Oui | **Jamais** (lié à l'initialisation) |
| Doit être initialisé | Non (mais UB si utilisé non initialisé) | **Oui, à la déclaration** |
| Syntaxe pour accéder à la valeur | `*p` | `r` (implicite) |
| Syntaxe pour obtenir l'adresse | `p` (c'est déjà une adresse) | `&r` |

### Astuce
Quand tu vois `stringREF` *se comporter comme* `str`, tu as compris les références. C'est du sucre syntaxique pour « un pointeur qui ne peut pas être nul et ne peut pas changer ».

---

## Exercice 03 — Unnecessary violence

### Ce que tu apprends
**Quand utiliser une référence membre plutôt qu'un pointeur membre dans une classe.** C'est la suite naturelle de ex02.

### La question de conception
- `HumanA` a **toujours** une arme → le membre est un `Weapon&` (une référence ne peut pas être nulle).
- `HumanB` **peut avoir ou non** une arme → le membre est un `Weapon*` (un pointeur peut valoir `NULL`, et être réassigné avec `setWeapon`).

### Détails pratiques
- `Weapon::getType()` doit renvoyer `const std::string&`, pas `std::string`. Renvoyer par valeur copie la chaîne ; renvoyer par référence const évite la copie et colle au sujet.
- `HumanA` reçoit la `Weapon` dans son **constructeur** — la référence est liée une fois pour toutes.
- `HumanB` n'a **pas d'argument arme** dans son constructeur. Il utilise `setWeapon()` à part. Initialise le pointeur à `NULL` par défaut pour éviter une valeur indéterminée.
- **Propriété :** aucun des deux Human ne *possède* la Weapon. C'est `main` qui la possède. Le destructeur de HumanB ne doit PAS faire `delete` sur le pointeur.

### Le piège de la forme canonique orthodoxe
- `Weapon` : une OCF complète fonctionne sans problème.
- `HumanA` : **impossible** de fournir une OCF complète. Une classe avec une référence membre :
  - Ne peut pas avoir de constructeur par défaut (la référence doit être liée à la construction).
  - Ne peut pas avoir d'`operator=` qui ait du sens (une référence ne peut pas être reliée ailleurs).
  
  La réponse honnête : fournir un constructeur paramétré + un constructeur de copie + un destructeur, et soit omettre `operator=`, soit en faire une version minimale qui ne copie que les membres non-références (comme `name`). Sois prêt à l'expliquer en soutenance — les évaluateurs vérifient que tu as compris le *pourquoi*.
- `HumanB` : une OCF complète fonctionne. Le destructeur ne doit PAS supprimer le pointeur (propriété partagée, c'est `main` qui possède la Weapon).

### Sortie attendue
```
Bob attacks with their crude spiked club
Bob attacks with their some other type of club
Jim attacks with their crude spiked club
Jim attacks with their some other type of club
```
Le fait que la deuxième attaque de Bob montre le type d'arme *mis à jour*, c'est la démonstration : les membres référence/pointeur partagent le même objet sous-jacent — modifier `club` se voit à travers Bob.

### Astuce
L'indice du sujet demande « *quand un pointeur est-il préférable ? quand une référence l'est-elle ?* » — réponse : **une référence quand la relation est obligatoire et permanente ; un pointeur quand elle est optionnelle ou peut changer.**

---

## Exercice 04 — Sed is for losers

### Ce que tu apprends
**La manipulation de `std::string` et les entrées/sorties fichier en C++ (`ifstream`/`ofstream`).** Et aussi : réimplémenter quelque chose que tu ferais normalement avec `sed`, sans utiliser `std::string::replace`.

### L'algorithme (puisque `replace` est interdit)
```cpp
std::string result;
size_t i = 0;
while (i < content.size()) {
    size_t found = content.find(s1, i);
    if (found == std::string::npos) {
        result += content.substr(i);  // append the rest
        break;
    }
    result += content.substr(i, found - i);  // text before the match
    result += s2;                             // the replacement
    i = found + s1.size();                    // skip past the match
}
```

### Détails pratiques
- Lis d'abord tout le fichier dans une seule `std::string` (plus simple que ligne par ligne pour les occurrences qui chevauchent deux lignes) :
  ```cpp
  std::ifstream in(filename.c_str());
  std::stringstream buffer;
  buffer << in.rdbuf();
  std::string content = buffer.str();
  ```
- Fichier de sortie : `<filename>.replace` — c'est-à-dire `argv[1] + ".replace"`.
- Gère les erreurs : fichier absent, `s1` vide (boucle infinie si tu ne fais pas attention !), mauvais argc.

### Astuces
- **Si `s1` est vide**, ta boucle n'avancera jamais — protège-toi explicitement contre ça et, soit renvoie une erreur, soit traite-le comme « aucun remplacement ».
- **N'utilise pas `getline` + un remplacement ligne par ligne** si `s1` peut contenir un retour à la ligne. Lis tout le fichier d'un coup.
- En C++98, `std::ifstream` prend un `const char*`, donc passe `filename.c_str()`, pas directement la `std::string` (ça, c'est du C++11+).
- Ferme tes flux (ou laisse le RAII le faire — ils se ferment à la destruction).

### Piège
Interdits : `std::string::replace`, toutes les fonctions fichier du C (`fopen`, `fread`, `fwrite`, `fclose`, etc.). On attend de toi que tu utilises les flux C++.

---

## Exercice 05 — Harl 2.0

### Ce que tu apprends
**Les pointeurs sur fonctions membres** — la manière correcte en C++ d'aiguiller vers l'une de plusieurs méthodes sans chaîne de `if/else`.

### Syntaxe (c'est tout l'objet de l'exercice)
```cpp
// Declaration of a pointer to a member function of Harl:
void (Harl::*fn)(void);

// Assigning it:
void (Harl::*fn)(void) = &Harl::debug;

// Calling it on an instance:
(this->*fn)();
// or on an object:
(harlInstance.*fn)();
```

### La structure canonique
```cpp
void Harl::complain(std::string level) {
    std::string levels[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    void (Harl::*fns[4])(void) = {
        &Harl::debug, &Harl::info, &Harl::warning, &Harl::error
    };
    for (int i = 0; i < 4; i++) {
        if (level == levels[i]) {
            (this->*fns[i])();
            return;
        }
    }
}
```

### Astuces
- Le sujet **interdit** l'approche en forêt de if/else. Utiliser `switch` ici passe aussi à côté de l'objectif — c'est dans ex06 que `switch` a sa place. Utilise l'approche du tableau de pointeurs sur fonctions membres.
- `debug`, `info`, `warning`, `error` sont **privées**. Seule `complain` est publique. C'est voulu — l'utilisateur de `Harl` passe par des niveaux, pas par des noms de méthodes.
- La syntaxe des pointeurs sur fonctions membres est *vraiment* moche. Prends le temps de la taper plusieurs fois jusqu'à ce qu'elle ne te paraisse plus étrangère : `return_type (ClassName::*varname)(params)`.

### Piège
`Harl::debug` seul est un nom de fonction ; tu dois écrire `&Harl::debug` pour obtenir le pointeur. Contrairement aux fonctions libres (où `foo` et `&foo` sont équivalents), les fonctions membres *exigent* le `&`.

---

## Exercice 06 — Harl filter

### Ce que tu apprends
**L'instruction `switch` avec fall-through volontaire** (*passage au cas suivant*). Cet exercice réutilise la classe `Harl` de ex05.

### L'astuce
Filtrer « affiche ce niveau **et tout ce qui est plus grave** ». C'est taillé sur mesure pour le fall-through :

```cpp
switch (levelIndex) {
    case 0:  // DEBUG
        harl.complain("DEBUG");
        // fall through
    case 1:  // INFO
        harl.complain("INFO");
        // fall through
    case 2:  // WARNING
        harl.complain("WARNING");
        // fall through
    case 3:  // ERROR
        harl.complain("ERROR");
        break;
    default:
        std::cout << "[ Probably complaining about insignificant problems ]" << std::endl;
}
```

### Détails pratiques
- Traduis d'abord l'argument chaîne (`"WARNING"`) en un indice 0–3.
- Envoie les niveaux inconnus dans la branche `default` — la sortie attendue est exactement `[ Probably complaining about insignificant problems ]`.
- L'exécutable doit s'appeler **`harlFilter`** (exigence du sujet — vérifie ton Makefile).

### Astuces
- Ajoute des commentaires `// fall through` au-dessus de chaque fall-through implicite — les compilateurs modernes avertissent à ce sujet (`-Wimplicit-fallthrough`) et tes lecteurs te remercieront.
- La spécification de sortie montre une ligne vide entre les sections (par ex. entre `WARNING` et `ERROR`). Assure-toi que tes `Harl::warning()` etc. se terminent par un retour à la ligne, et vois si tu as besoin d'un saut de ligne supplémentaire entre les niveaux.

### Piège
Ex06 est **facultatif** — tu peux valider le module sans. Mais c'est le seul exercice qui enseigne `switch` dans ce module, alors ne le saute que si tu manques vraiment de temps.

---

## Checklist du module avant la soutenance

- [ ] Compile avec `c++ -Wall -Wextra -Werror -std=c++98` — aucun warning.
- [ ] Zéro fuite mémoire sous `valgrind` (surtout ex00, ex01, ex03).
- [ ] Aucun `using namespace std;` nulle part.
- [ ] Pas de mot-clé `friend`.
- [ ] Pas de conteneurs STL (`vector`, `map`, etc.) — interdits jusqu'au Module 08.
- [ ] Pas de `*printf`, `*alloc`, `free` — c'est -42 immédiatement.
- [ ] Chaque classe dans sa propre paire `.hpp` / `.cpp`, nommée `ClassName.hpp` / `ClassName.cpp`.
- [ ] Chaque `.hpp` a des include guards.
- [ ] Aucune implémentation de fonction dans les en-têtes (sauf templates).
- [ ] Chaque en-tête est **autosuffisant** — il inclut tout ce dont il a besoin, ne dépend pas de l'ordre d'inclusion.
- [ ] À partir de Mod02 (et déjà ici quand ça a du sens), les classes suivent la **forme canonique orthodoxe** — avec l'exception façon HumanA expliquée.

## Correspondance notion → exercice (référence rapide)

| Notion | Où elle apparaît |
|---|---|
| Allocation pile vs tas | ex00 |
| `new[]` / `delete[]` | ex01 |
| Fuites mémoire & `valgrind` | ex00, ex01, ex03 |
| Références vs pointeurs (syntaxe) | ex02 |
| Références vs pointeurs (comme membres de classe) | ex03 |
| Forme canonique orthodoxe avec références | ex03 |
| E/S fichier avec `ifstream`/`ofstream` | ex04 |
| Manipulation de `std::string` | ex04 |
| Pointeurs sur fonctions membres | ex05 |
| Instruction `switch` (avec fall-through) | ex06 |

## À lire / revoir à part

- **Références** — la page de cppreference sur « Reference declaration » (à survoler, pas à mémoriser).
- **Opérateurs d'accès aux membres** — en particulier `.*` et `->*` (utilisés dans ex05).
- **Flux fichier** — les interactions entre `<fstream>`, `<sstream>`, `<iostream>`.
- **Fonctions membres de `std::string`** — voir [STRING_FUNCTIONS.md](../../../../en/cpp/notions/io-errors/STRING_FUNCTIONS.md) (EN).
- **Forme canonique orthodoxe** — voir [ORTHODOX_CANONICAL_FORM.md](../../notions/oop/ORTHODOX_CANONICAL_FORM.md).
