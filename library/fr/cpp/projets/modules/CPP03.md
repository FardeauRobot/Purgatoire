# C++ Module 03 — Guide d'apprentissage

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/CPP03.md)

**Thème du module :** *l'héritage — relations « est-un », chaînes
de constructeurs et de destructeurs, et le problème du diamant.*

Ce module martèle une seule question, quatre fois : *quand la classe
B hérite de la classe A, qu'est-ce que B reçoit exactement, et dans
quel ordre tout cela se produit-il ?* Le support est une famille de
robots un peu idiote — `ClapTrap` et ses enfants — choisie pour que
la mécanique reste bien visible (chaque constructeur, destructeur et
action affiche une trace, pour que tu voies la chaîne se déclencher).

Voir aussi : le [`TOPICS.md`](../../../../../LEVEL4/C++1/Mod03/TOPICS.md)
du module pour l'approfondissement exercice par exercice, et
[`oop/INHERITANCE.md`](../../notions/oop/INHERITANCE.md) pour la référence complète
sur l'héritage.

---

## Les quatre règles auxquelles tu reviendras sans cesse

1. **L'héritage public signifie « est-un ».** Chaque `ScavTrap`
   *est-un* `ClapTrap`. Tu peux le passer là où un `ClapTrap&` est
   attendu. Tu peux le stocker dans un `ClapTrap*`.
2. **La construction de la base passe en premier, sa destruction en
   dernier.** Le corps du constructeur dérivé s'exécute une fois le
   sous-objet de base entièrement vivant. Le corps du destructeur
   dérivé s'exécute avant que la base soit démolie.
3. **Les membres privés ne sont pas accessibles depuis les classes
   dérivées.** Utilise `protected` (toujours caché au code extérieur,
   visible des dérivées) ou fournis des accesseurs.
4. **Masquage de nom.** Une méthode de la classe dérivée qui porte le
   même nom qu'une méthode de la base **masque** celle de la base —
   y compris toutes ses surcharges. Fais-les revenir avec
   `using Base::method;`.

---

## Exercice 00 — Aaaaand… OPEN! (`ClapTrap`)

### Ce que tu apprends
**Écrire une classe qui gère des ressources en OCF complète**, quand
ces ressources sont des types primitifs (pas encore de tas). C'est la
base de toutes les classes qui suivent.

### Forme de la classe
- Privé : `std::string name`, `unsigned int hitPoints` (départ
  10), `energyPoints` (départ 10), `attackDamage` (départ 0).
- Public : OCF × 4, plus `attack(std::string const& target)`,
  `takeDamage(unsigned int)`, `beRepaired(unsigned int)`.
- **Chaque** constructeur, destructeur et action **affiche une trace**.

### La liste d'initialisation n'est pas optionnelle

```cpp
ClapTrap::ClapTrap(std::string const& name)
    : name(name), hitPoints(10), energyPoints(10), attackDamage(0) {
    std::cout << "ClapTrap " << name << " constructed" << std::endl;
}
```

- La liste d'initialisation s'exécute avant le corps.
- L'ordre suit **l'ordre de déclaration dans la classe**, pas
  l'ordre dans lequel tu les écris. Écris-les quand même dans l'ordre
  de déclaration — sinon `-Wreorder` → erreur de compilation sous
  `-Werror`.
- Les membres `const` et les membres référence **doivent** être
  initialisés dans la liste d'initialisation (l'ex03 de Mod 01 y
  faisait allusion avec la référence membre de `HumanA`).

### Le dépassement par le bas d'un non-signé — le bug à connaître
`hitPoints -= 100;` quand les PV valent 10 boucle sur `4294967206`,
pas sur 0. Borne :

```cpp
hitPoints = (damage >= hitPoints) ? 0 : hitPoints - damage;
```

Pareil pour tout calcul en `unsigned int` où la valeur soustraite
peut dépasser celle dont on soustrait.

### Gardes des actions — énergie = 0 vs PV = 0

Le sujet est précis :
- **`attack`** — coûte 1 énergie. Refuse si l'énergie ou les PV
  valent 0.
- **`beRepaired`** — coûte 1 énergie. Refuse si l'énergie vaut 0 (les
  PV n'ont pas d'importance ; un ClapTrap à 0 PV est déjà mort en
  esprit, mais le sujet n'exige pas de test sur les PV ici — relis-le).
- **`takeDamage`** — ne coûte PAS d'énergie. Réduit simplement les PV.

### Astuces
- Constructeur par défaut `ClapTrap()` : le sujet permet de donner
  par défaut le nom `"default"` ou équivalent. Le constructeur qui
  prend un nom est le principal.
- Le constructeur de copie et l'affectation par copie peuvent s'écrire
  respectivement `*this = src;` et une copie membre à membre. Pour des
  primitifs + `std::string`, les versions par défaut sont correctes —
  mais tu les écris quand même pour les traces.

### Piège
Les messages des constructeurs/destructeurs doivent correspondre au
texte du sujet **caractère pour caractère**, guillemets, points
d'exclamation et éventuel point final compris. La moulinette compare
la sortie avec un diff.

---

## Exercice 01 — Serena, my love! (`ScavTrap`)

### Ce que tu apprends
**L'héritage public, le constructeur de la base dans la liste
d'initialisation, et la chaîne constructeurs/destructeurs.**

```cpp
class ScavTrap : public ClapTrap { ... };
```

### Appeler le constructeur de la base

```cpp
ScavTrap::ScavTrap(std::string const& name) : ClapTrap(name) {
    hitPoints = 100;
    energyPoints = 50;
    attackDamage = 20;
    std::cout << "ScavTrap " << name << " constructed" << std::endl;
}
```

Deux choix possibles :

1. **Réaffecter dans le corps** (ci-dessus) — plus simple, une
   écriture de plus par statistique. Exige que les statistiques
   soient `protected` (ou accessibles via des setters).
2. **Un `ClapTrap(name, hp, ep, atk)` protégé** — plus efficace,
   mais ajoute un constructeur non public. Parfaitement correct ;
   question de goût.

### Ordre des traces constructeurs/destructeurs

```
ClapTrap Steve constructed
ScavTrap Steve constructed
        ← le travail se fait ici
ScavTrap Steve destroyed
ClapTrap Steve destroyed
```

La base d'abord à la construction, la dérivée d'abord à la
destruction.

### Redéfinir `attack()` sans `virtual`

`ScavTrap::attack` masque `ClapTrap::attack`. Un appel via un
`ScavTrap&` obtient la version dérivée. Un appel via un `ClapTrap&`
(qui désigne en réalité un `ScavTrap`) obtient la version de la
**base** — c'est la **liaison statique** (*static dispatch*). Mod 04
introduira `virtual` pour corriger ça.

### Astuces
- `guardGate()` est une méthode propre à `ScavTrap`. À travers un
  `ClapTrap*`, elle est invisible — erreur de compilation.
- Le constructeur de copie transmet à la base : `ScavTrap(ScavTrap const& src) :
  ClapTrap(src) { ... }`. Le découpage (*slicing*) implicite au
  passage du paramètre est sans danger, parce que le constructeur de
  la base n'a besoin que de la vue sur le sous-objet de base.
- N'oublie pas `ScavTrap::operator=`. La forme habituelle :
  ```cpp
  ScavTrap& operator=(ScavTrap const& other) {
      ClapTrap::operator=(other);
      // ... copy ScavTrap's own members, none here
      return *this;
  }
  ```

### Piège
Si tu as laissé les champs de `ClapTrap` en `private` dans ex00 sans
setters, le constructeur de ScavTrap ne peut pas y toucher depuis son
corps. Correction : passe-les en `protected`, ou ajoute des setters.

---

## Exercice 02 — Repetitive work (`FragTrap`)

### Ce que tu apprends
**La répétition pour créer des automatismes.** Même forme que ex01
avec d'autres constantes et une autre méthode signature
(`highFivesGuys()`). Aucune nouvelle notion.

### Astuces
- Copie-colle `ScavTrap.*` → `FragTrap.*`, renomme, ajuste les
  statistiques, mets à jour les messages. 5 minutes de travail.
- **Lis ce que tu écris, ne passe pas en pilote automatique.** ex03
  va te lancer l'héritage multiple dans les jambes, et si le schéma de
  ex02 n'a pas vraiment imprimé, ex03 te semblera magique.

### Piège
Le sujet attend aussi des traces pour le constructeur de copie et
l'affectation par copie. Ne les saute pas parce qu'elles te
« semblent redondantes » — la moulinette teste `FragTrap b(a);` et
compare la sortie.

---

## Exercice 03 — Now it's weird! (`DiamondTrap`, bonus)

### Ce que tu apprends
**Le problème du diamant et l'héritage `virtual`.**

```
        ClapTrap
         /    \
   ScavTrap  FragTrap
         \    /
       DiamondTrap
```

### Le problème sans `virtual`

Déclarer `class DiamondTrap : public FragTrap, public ScavTrap {}`
te donne **deux** sous-objets `ClapTrap` — un dans chaque classe
intermédiaire. Deux `name`, deux `hp`, deux constructeurs qui se
déclenchent, deux destructeurs. Toute référence à `ClapTrap::x` est
ambiguë.

### La correction

Les deux intermédiaires héritent **virtuellement** :

```cpp
class ScavTrap  : virtual public ClapTrap { ... };
class FragTrap  : virtual public ClapTrap { ... };
class DiamondTrap : public ScavTrap, public FragTrap { ... };
```

Il y a maintenant **un seul `ClapTrap` partagé**.

### La règle qui accompagne `virtual`

> La classe la plus dérivée doit appeler directement le constructeur
> de la base virtuelle.

```cpp
DiamondTrap::DiamondTrap(std::string const& name)
    : ClapTrap(name + "_clap_name"),   // virtual base, here
      ScavTrap(name),
      FragTrap(name),
      m_name(name) {
    hitPoints    = FragTrap::hitPoints;     // or just 100
    energyPoints = ScavTrap::energyPoints;  // or just 50
    attackDamage = FragTrap::attackDamage;  // or just 30
    std::cout << "DiamondTrap " << name << " constructed" << std::endl;
}
```

Sans héritage virtuel, l'appel `ClapTrap(name + "_clap_name")` serait
illégal (on ne peut pas appeler directement le constructeur d'un
grand-parent). Avec virtual, il est obligatoire.

### L'astuce du masquage de nom

`DiamondTrap` a son propre `std::string name` privé — même
identifiant que `ClapTrap::name`. Deux membres, même nom, deux
emplacements distincts.

- `DiamondTrap::name` — le nom « humain », par ex. `"Stevie"`.
- `ClapTrap::name` — la désignation du robot, par ex. `"Stevie_clap_name"`.

`whoAmI()` affiche les deux :

```cpp
void DiamondTrap::whoAmI() {
    std::cout << "I am " << m_name
              << " and my ClapTrap name is " << ClapTrap::name
              << std::endl;
}
```

Le nom qualifié (`ClapTrap::name`) passe derrière le masquage.

### Aiguillage de `attack`

Le sujet veut que `DiamondTrap::attack` utilise `ScavTrap::attack` :

```cpp
using ScavTrap::attack;   // bring ScavTrap's attack into scope
```

Ou écris une enveloppe :

```cpp
void DiamondTrap::attack(std::string const& target) {
    ScavTrap::attack(target);
}
```

### Astuces
- **Compte les constructeurs/destructeurs.** Avec l'héritage virtuel,
  le constructeur de `ClapTrap` se déclenche **une seule fois** pendant
  la construction de `DiamondTrap`. C'est le test de fumée de la
  moulinette — si tu le vois deux fois, le `virtual` n'est pas branché.
- **Avertissement -Wshadow.** Le masquage de `name` est voulu ; si ton
  build utilise `-Wshadow`, fais-le taire de façon ciblée ou accepte
  l'avertissement pour ce fichier.
- **OCF sur `DiamondTrap`.** Oui, les quatre. Le constructeur de copie
  et l'affectation par copie transmettent aux deux parents (qui
  partagent tous deux l'unique base virtuelle, donc pas de double
  travail).

### Piège
`ScavTrap(name)` et `FragTrap(name)` essaient chacun d'initialiser
aussi `ClapTrap` avec `name` (ils ne savent pas que `DiamondTrap` s'en
est déjà chargé). Avec `virtual public ClapTrap`, leurs appels
`ClapTrap(name)` sont **ignorés** — seul compte l'appel de
`DiamondTrap`. C'est pour ça que la règle de la classe la plus dérivée
existe.

---

## Ce que ce module te laisse

- Tu sais ce qu'est un **sous-objet** et comment il vit à l'intérieur
  d'un objet dérivé.
- Les chaînes constructeurs/destructeurs deviennent une image mentale
  automatique — tu peux prédire l'ordre des traces sans lancer le code.
- Tu comprends assez bien le **masquage de nom** et la **liaison
  statique** pour que `virtual` (Mod 04) te paraisse une correction,
  pas une surprise.
- Tu as tenu un diamant dans ta main et tu l'as résolu.

Tout Mod 04 suppose ces acquis — en particulier la règle du
destructeur virtuel, qui arrive dès ex01.
