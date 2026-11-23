# C++ Module 05 — Guide d'apprentissage

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP05.md)

**Thème du module :** *répétition et exceptions — lancer, attraper,
et la hiérarchie de `std::exception`, autour d'une chaîne de classes
qui grandit (Bureaucrat → Form → Intern).*

Mod 04 t'a appris le polymorphisme. Mod 05 s'appuie sur ces bases,
mais son *vrai* sujet est **la gestion d'erreurs à la manière C++** :
au lieu de renvoyer `-1` en espérant que l'appelant vérifie (l'habitude
du C), tu **lances** (*throw*) une exception et tu la laisses remonter
jusqu'à ce que quelqu'un l'**attrape** (*catch*). Les quatre exercices
racontent une seule histoire continue — tu construis un `Bureaucrat`,
tu lui donnes des `Form` à signer et à exécuter, puis tu embauches un
`Intern` pour remplir les formulaires à ta place. Chaque étape ajoute
une idée nouvelle par-dessus la précédente.

Voir aussi : [`lexique/`](../../../../en/cpp/lexique/INDEX.md) (EN) pour les zooms sur les mots-clés
`throw`/`try`/`catch` et [`oop/ORTHODOX_CANONICAL_FORM.md`](../../notions/oop/ORTHODOX_CANONICAL_FORM.md)
pour les règles de l'OCF que toute classe non-exception de ce module
doit suivre.

---

## La phrase à mémoriser

> Une **exception** est une valeur que tu `throw` à l'endroit où tu
> détectes un problème ; elle déroule la pile, en exécutant les
> destructeurs au passage, jusqu'à ce qu'un `catch` correspondant la
> traite — ce qui découple *l'endroit où l'erreur se produit* de
> *l'endroit où on s'en occupe*.

Image de la vie réelle : une **alarme incendie**. La personne qui sent
la fumée (la fonction de bas niveau) ne décide pas comment évacuer le
bâtiment — elle déclenche simplement l'alarme (`throw`). Chacun, en
sortant, ferme la porte de son bureau derrière lui (les destructeurs
s'exécutent pendant le déroulement de la pile). Le responsable
d'évacuation à la sortie (`catch`) décide de ce qu'on fait vraiment.

---

## Ce que le module t'apprend dans son ensemble

- **`throw` / `try` / `catch`** — les trois mots-clés de la gestion
  d'erreurs en C++.
- **Des classes d'exception personnalisées** dérivées de
  `std::exception`, avec une méthode `what()` — et la bizarrerie du
  spécificateur `throw()` en C++98.
- **La sûreté face aux exceptions** (*exception safety*) : comme les
  destructeurs s'exécutent pendant le déroulement, le RAII (ressources
  possédées par des objets) garantit l'absence de fuite même quand
  tout explose. C'est *pour ça* que le langage s'appuie autant sur les
  constructeurs/destructeurs.
- **Les membres `const`** et ce qu'ils font à ta forme canonique
  orthodoxe (l'opérateur d'affectation par copie devient un problème).
- Des **retrouvailles en douceur avec les classes abstraites** (ex02)
  et un **schéma d'aiguillage propre** pour éviter les échelles de
  `if/else` (ex03).

Pourquoi c'est utile : tout programme C++ non trivial qui touche aux
fichiers, à la mémoire, au réseau ou aux saisies utilisateur a besoin
d'une manière disciplinée de signaler les échecs et de s'en remettre.
`throw`/`catch` est cette discipline, et la STL elle-même lance des
exceptions (`std::out_of_range`, `std::bad_alloc`, …), donc tu dois
savoir les attraper sans hésiter.

---

## Le mécanisme, en un schéma

```
   deep_function()  ── détecte un mauvais grade ──►  throw GradeTooLow();
        │                                          │
        │  (la pile se déroule : locales détruites)│
        ▼                                          ▼
   middle_function()   ── pas de catch qui correspond ──►  continue de remonter
        │                                          │
        ▼                                          ▼
   main() { try { ... } catch (std::exception& e) { e.what(); } }
                                     ▲
                          le premier gestionnaire qui correspond gagne
```

Si **personne** n'attrape, `std::terminate()` est appelé et le
programme s'arrête. Une exception attrapée reprend l'exécution juste
après le bloc `catch` — pas à l'endroit du `throw`.

---

## Exercice 00 — The Bureaucrat

### Ce que tu apprends
**Déclarer, lancer et attraper tes propres classes d'exception**, à
l'aide d'une classe dont l'invariant (le grade doit rester dans
`[1, 150]`) est garanti en lançant une exception.

### Nouveaux termes

- **Exception** — un objet lancé pour signaler une erreur. Ici, il y
  en a deux : `GradeTooHighException`, `GradeTooLowException`.
- **`std::exception`** — la classe de base standard de toutes les
  exceptions. Sa seule interface est `virtual const char* what()
  const throw();` — un message lisible par un humain.
- **Classe imbriquée** (*nested class*) — une classe déclarée
  *à l'intérieur* d'une autre (`Bureaucrat::GradeTooHighException`).
  Elle range l'exception sous son propriétaire, ce qui se lit bien et
  colle au sujet.
- **Invariant** — une règle qui doit *toujours* être vraie pour qu'un
  objet soit valide. Ici : `1 <= grade <= 150`. Le grade **1 est le
  plus haut**, **150 le plus bas** — contre-intuitif, comme les rangs
  aux échecs ou à l'armée, ou « tu es mon numéro 1 ».

### Image de la vie réelle
Pense **hiérarchie d'entreprise ou militaire**. Un `Bureaucrat` a un
nom (fixé à vie — `const`) et un rang. Tu peux le *promouvoir*
(incrémenter → le numéro de grade **descend**, vers 1) ou le
*rétrograder* (décrémenter → le numéro **monte**, vers 150). Essaie de
promouvoir quelqu'un déjà au grade 1, ou de rétrograder quelqu'un à
150, et le système refuse — il **lance une exception**.

### L'extrait central — déclarer une classe d'exception

C'est le schéma que tu réutiliseras dans chaque exercice. Note le
`throw()` après `what()` — en **C++98**, le `what()` du standard porte
une spécification d'exception vide, et pour le redéfinir proprement ta
signature doit correspondre :

```cpp
#include <exception>

class Bureaucrat {
public:
    class GradeTooHighException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
    class GradeTooLowException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
    // ... rest of Bureaucrat
};
```

```cpp
// in the .cpp
const char* Bureaucrat::GradeTooHighException::what() const throw() {
    return "grade is too high (must be >= 1)";
}
```

### Lancer et attraper

```cpp
// throwing — anywhere the invariant would break:
if (grade < 1)
    throw Bureaucrat::GradeTooHighException();

// catching — in main (or any caller):
try {
    Bureaucrat boss("Alice", 0);   // 0 is out of range → throws
}
catch (std::exception& e) {        // catch by reference to the BASE
    std::cerr << e.what() << std::endl;
}
```

Attrape par **`std::exception&`** (la base, par référence) : un seul
gestionnaire couvre à la fois tes exceptions *et* celles de la STL, et
la référence évite le découpage (*slicing*). Le sujet exige
explicitement que tes exceptions puissent être attrapées de cette façon.

### Où vivent les throws
- **Constructeur** — valide le grade ; lance une exception s'il est
  hors de `[1,150]`. (Lancer depuis un constructeur ne pose pas de
  problème — l'objet n'est simplement jamais considéré comme construit,
  et aucun destructeur ne s'exécute pour lui.)
- **`increment` / `decrement`** — lancent l'exception *avant* de
  franchir la limite.

### Astuces
- Surcharge `operator<<` (fonction libre, `std::ostream&`) pour
  afficher `<name>, bureaucrat grade <grade>.` — renvoie le flux pour
  que les appels s'enchaînent.
- Donne à `what()` un message réellement utile ; tu l'affiches, alors
  rends-le lisible.
- Teste **à la fois** le chemin qui lance et le chemin normal dans
  `main`, chacun enveloppé dans son propre `try/catch`.

### Piège
**Le nom est `const`** (`const std::string _name`). Dès qu'un membre
est `const`, l'**opérateur d'affectation par copie** généré par le
compilateur est **supprimé dans l'esprit** — tu ne peux pas réaffecter
un membre `const`, donc `a = b;` ne compilera pas si tu essaies
d'affecter `_name`. Options : écrire un `operator=` qui ne copie que
les membres non const (et accepter que le nom ne change pas), ou ne pas
fournir l'affectation si `main` ne l'utilise jamais. Décide en
connaissance de cause — ne te laisse pas surprendre.
*(Attention : ton `ex00/Bureaucrat.hpp` actuel a encore un `_name` non
`const` et pas encore de `_grade` — mets-le en conformité avec le
sujet.)*

---

## Exercice 01 — The Form

### Ce que tu apprends
**Des objets qui interagissent par exceptions et permissions** — une
classe (`Form`) lance, une autre (`Bureaucrat`) attrape et rend compte.

### Nouveaux termes

- **`beSigned(const Bureaucrat&)`** — une méthode de `Form` qui
  compare le grade du bureaucrate au grade *de signature* requis par
  le formulaire. Assez haut → met `signed = true` ; trop bas → lance
  `Form::GradeTooLowException`.
- **Grade requis** — chaque formulaire porte deux grades `const` : un
  pour le **signer**, un pour l'**exécuter** (utilisé dans ex02). Les
  deux sont fixés à la construction, les deux sont validés (exception
  s'ils sont hors de `[1,150]`).

### Image de la vie réelle
Un **bon de commande**. Il attend, non signé. Un responsable dont
l'autorité (le grade) est suffisante peut le signer ; un employé junior
ne peut pas — le formulaire « refuse sa signature ». C'est le
formulaire lui-même qui détient la règle sur *qui a le droit* ;
l'employé se contente d'essayer et de gérer le refus.

### Forme de la classe
- `Form` — `const std::string _name`, `bool _signed` (false à la
  construction), `const int _gradeToSign`, `const int _gradeToExec`.
  Tous **privés** (le sujet est explicite : privés, pas protégés — ça
  compte dans ex02).
- Les deux mêmes classes d'exception imbriquées que `Bureaucrat`, mais
  rangées sous `Form`.
- `Bureaucrat::signForm(Form&)` — appelle `beSigned`, et affiche soit
  `<bureaucrat> signed <form>`, soit
  `<bureaucrat> couldn't sign <form> because <reason>.`

### L'extrait d'interaction

```cpp
void Bureaucrat::signForm(Form& f) {
    try {
        f.beSigned(*this);
        std::cout << _name << " signed " << f.getName() << std::endl;
    }
    catch (std::exception& e) {
        std::cout << _name << " couldn't sign " << f.getName()
                  << " because " << e.what() << "." << std::endl;
    }
}
```

Remarque la répartition des rôles : `Form::beSigned` **lance** ;
`Bureaucrat::signForm` **décide quoi en faire** (afficher une ligne
aimable, ne pas planter). Cette séparation, c'est toute la raison
d'être des exceptions.

### Comparaison de grades — le piège mental
« Assez haut » signifie que le **numéro de grade est assez petit**. Un
bureaucrate peut signer si `bureaucrat.grade <= form.gradeToSign`
(numéro plus petit = plus d'autorité). Écris-le une fois, avec soin, et
commente le `<=` si ça t'aide — c'est le bug logique n°1 du module.

### Astuces
- L'`operator<<` de `Form` doit afficher tout son état (nom, statut de
  signature, les deux grades) — pratique pour déboguer.
- `_signed` commence à `false` ; seul `beSigned` le bascule.
- Les getters sont `const` ; les grades étant des membres `const`
  (comme dans ex00), ton `operator=` ne peut copier que `_signed`.

### Piège
Déclaration anticipée / ordre d'inclusion : `Form` a besoin de
connaître `Bureaucrat` et réciproquement. Utilise une **déclaration
anticipée** (*forward declaration*, `class Bureaucrat;`) dans un
en-tête et inclus la définition complète dans le `.cpp` pour casser le
`#include` circulaire.

---

## Exercice 02 — Formulaire abstrait & formulaires concrets

### Ce que tu apprends
**Combiner exceptions et polymorphisme** : `Form` devient **abstraite**
(`AForm`), et chaque formulaire concret fait sa propre action quand on
l'exécute — mais les *vérifications de permission* vivent une seule
fois, dans la base.

### Nouveaux termes

- **`AForm`** — la base abstraite (`Form` renommée). Ajoute une action
  **virtuelle pure** pour que la classe ne puisse pas être instanciée
  directement :
  ```cpp
  virtual void execute(Bureaucrat const& executor) const = 0;
  ```
  (Ou garde `execute` concrète dans la base pour les vérifications et
  crée une `action()` virtuelle pure protégée — voir le schéma élégant
  plus bas.)
- **Formulaires concrets** — trois sous-classes, chacune avec ses
  grades de signature/exécution et sa propre action :

  | Formulaire | signature | exécution | Action |
  |---|---|---|---|
  | `ShrubberyCreationForm` | 145 | 137 | écrit `<target>_shrubbery` avec des arbres en ASCII |
  | `RobotomyRequestForm` | 72 | 45 | bruits de perceuse ; robotomise la cible **50 %** du temps |
  | `PresidentialPardonForm` | 25 | 5 | annonce que la cible a été graciée par Zaphod Beeblebrox |

- **Nouvelles exceptions** dont tu auras besoin : quelque chose comme
  `AForm::FormNotSignedException`, et une vérification du grade
  d'exécution qui réutilise `GradeTooLowException`.

### Image de la vie réelle
Une **administration avec différents formulaires**. Tous partagent la
même règle au guichet : *« est-il signé ? ton habilitation est-elle
suffisante pour l'exécuter ? »* — cette vérification est identique pour
tous les formulaires, donc elle vit au guichet (la classe de base). Mais
*ce que fait réellement le formulaire* — planter un arbuste, lobotomiser
quelqu'un, accorder une grâce — est propre à chacun (la redéfinition).

### Le schéma élégant (le sujet y fait allusion)
Fais les **vérifications une seule fois** dans la base, puis délègue le
travail spécifique à un point d'accroche virtuel. C'est le patron
*Template Method* (*méthode patron*) :

```cpp
// AForm.cpp — concrete, in the base:
void AForm::execute(Bureaucrat const& executor) const {
    if (!_signed)
        throw AForm::FormNotSignedException();
    if (executor.getGrade() > _gradeToExec)
        throw AForm::GradeTooLowException();
    this->action();        // ← pure virtual, each form overrides
}
```

```cpp
// each concrete form:
protected:
    virtual void action() const;   // does the shrubbery/robotomy/pardon
```

Tu écris la vérification du grade et de la signature **une seule
fois** ; chaque nouveau formulaire ne fournit que son `action()`.
Compare avec le fait de copier-coller les deux mêmes `if` dans trois
`execute()` — la version dans la classe de base, c'est pour ça que le
sujet la dit « plus élégante ».

### Accéder aux membres privés de la base
Les attributs d'`AForm` sont **privés**, mais les formulaires concrets
ont besoin de leurs noms/grades. Fixe-les en passant les valeurs **au
constructeur de la base** dans la liste d'initialisation ; lis-les via
des **getters protégés**. Ne rends pas les membres `protected` — c'est
exactement ce que le sujet interdit.

```cpp
ShrubberyCreationForm::ShrubberyCreationForm(std::string const& target)
    : AForm("ShrubberyCreationForm", 145, 137), _target(target) {}
```

### Astuces
- **Destructeur virtuel sur `AForm`** (réflexe hérité de Mod 04) — tu
  supprimes des formulaires concrets à travers un `AForm*`.
- `ShrubberyCreationForm` utilise `<fstream>` (`std::ofstream`) pour
  écrire le fichier — grâce au RAII, le fichier se ferme même si
  quelque chose lance une exception.
- Les 50 % de `RobotomyRequestForm` — `std::rand()` initialisé une
  fois ; ne sur-conçois pas l'aléatoire pour un exercice de piscine.
- Ajoute `Bureaucrat::executeForm(AForm const&)` sur le modèle de
  `signForm` : essaie `execute`, affiche `<bureaucrat> executed <form>`
  ou l'erreur.

### Piège
Une référence `AForm` peut désigner n'importe quel formulaire concret —
`execute()` aiguille virtuellement vers la bonne `action()`. Mais si tu
as oublié le destructeur virtuel, supprimer via `AForm*` fait fuir la
partie concrète. Valgrind te le dira.

---

## Exercice 03 — The Intern

### Ce que tu apprends
**Un aiguillage propre sans échelle de `if/else if/else`** — construire
un `AForm*` du bon type à partir d'un *nom sous forme de chaîne*.

### Nouveaux termes

- **`Intern`** — une classe sans nom ni grade. Une seule méthode :
  `AForm* makeForm(std::string const& formName, std::string const&
  target);`. Renvoie un formulaire du type correspondant alloué sur le
  tas, ou gère le cas « nom de formulaire inconnu » (afficher une
  erreur / renvoyer `0`). Affiche `Intern creates <form>` en cas de
  succès.

### Image de la vie réelle
Tu vas voir le **stagiaire** et tu lui dis *« sors-moi une demande de
robotomie pour Bender. »* Peu t'importe *comment* il trouve le bon
formulaire dans l'armoire — il te tend le bon, rempli avec ta cible.
`makeForm("robotomy request", "Bender")` → un `RobotomyRequestForm*`.

### L'approche interdite
Le sujet **interdit explicitement** ceci :

```cpp
// ❌ do NOT do this — the subject penalises it:
if (name == "shrubbery creation") return new ShrubberyCreationForm(target);
else if (name == "robotomy request") return new RobotomyRequestForm(target);
else if (name == "presidential pardon") return new PresidentialPardonForm(target);
```

### Le schéma propre — une table de pointeurs de fonctions
Associe chaque nom à une **fonction fabricante**, boucle une fois,
appelle à travers un pointeur. C'est la technique que l'exercice
évalue ; le câblage exact, c'est à toi de l'écrire :

```cpp
// a private static helper per form type — same signature:
//   static AForm* makeShrubbery(std::string const& target)
//       { return new ShrubberyCreationForm(target); }

AForm* Intern::makeForm(std::string const& name, std::string const& target) {
    std::string       names[]  = { "shrubbery creation",
                                   "robotomy request",
                                   "presidential pardon" };
    AForm* (*makers[])(std::string const&) = { /* &makeShrubbery, ... */ };

    for (int i = 0; i < 3; ++i)
        if (names[i] == name)
            return makers[i](target);

    // unknown name: print error, return 0
    return 0;
}
```

Pourquoi c'est mieux : ajouter un quatrième formulaire = ajouter une
ligne à chaque tableau, ne toucher à rien d'autre. Aucune logique de
branchement à relire.

### Astuces
- **Qui supprime le formulaire ?** `makeForm` le crée avec `new` ;
  l'appelant le possède et doit le `delete`. Teste-le dans `main` et
  vérifie avec valgrind.
- Celui qui reçoit l'`AForm*` le signe et l'exécute en général — c'est
  le test de bout en bout de tout le module.
- `AForm* (*makers[])(std::string const&)` est un type effrayant ; un
  `typedef` le rend lisible :
  ```cpp
  typedef AForm* (*FormMaker)(std::string const&);
  ```

### Piège
Sur un **nom de formulaire inconnu**, ne fais aucun `new` et ne fuis
rien — signale simplement et renvoie `0`. Et protège l'appelant : si
`makeForm` peut renvoyer `0`, l'appelant doit le vérifier avant
`delete`/`execute`.

---

## Ce que ce module te laisse

- Tu sais déclarer, lancer et attraper des exceptions personnalisées,
  et tu connais par cœur la signature C++98 `what() const throw()`.
- Tu attrapes par `std::exception&` (la base, par référence) et tu sais
  pourquoi (un seul gestionnaire, pas de découpage).
- Tu comprends *pourquoi* RAII et exceptions racontent la même
  histoire : les destructeurs s'exécutent pendant le déroulement, donc
  les ressources sont en sécurité.
- Tu as senti ce qu'un membre `const` fait à ton OCF, et tu as fait un
  choix délibéré pour `operator=`.
- Tu sais remplacer une échelle d'aiguillage `if/else` sur le type par
  une table de pointeurs de fonctions sans réfléchir.

Après Mod 05, « traiter l'erreur là où ça a du sens, pas là où elle se
produit » est un réflexe — et tu es prêt pour les templates (Mod 06/07),
où la STL va te lancer des exceptions en permanence.
