# Getters et setters en C++ — Guide complet

> 🇬🇧 [English version](../../../../en/cpp/notions/oop/GETTERS_SETTERS.md)

> **Zooms sur les mots-clés :** [`PUBLIC/PRIVATE/PROTECTED`](../../../../en/cpp/lexique/PUBLIC_PRIVATE_PROTECTED.md) (EN) · [`CONST`](../../../../en/cpp/lexique/CONST.md) (EN) · [`THIS`](../../../../en/cpp/lexique/THIS.md) (EN) · [`MUTABLE`](../../../../en/cpp/lexique/MUTABLE.md) (EN) · [`EXPLICIT`](../../../../en/cpp/lexique/EXPLICIT.md) (EN)

## Table des matières
1. [Que sont les getters et les setters ?](#Que%20sont%20les%20getters%20et%20les%20setters%20%3F)
2. [Le modèle mental : encapsulation et invariants de classe](#Le%20mod%C3%A8le%20mental%20%3A%20encapsulation%20et%20invariants%20de%20classe)
3. [Exemple minimal](#Exemple%20minimal)
4. [Anatomie d'un bon getter](#Anatomie%20d%27un%20bon%20getter)
   - [Renvoyer par valeur, par référence ou par référence const ?](#Renvoyer%20par%20valeur%2C%20par%20r%C3%A9f%C3%A9rence%20ou%20par%20r%C3%A9f%C3%A9rence%20const%20%3F)
   - [Le `const` final](#Le%20const%20final)
   - [Getters calculés (dérivés)](#Getters%20calcul%C3%A9s%20%28d%C3%A9riv%C3%A9s%29)
5. [Anatomie d'un bon setter](#Anatomie%20d%27un%20bon%20setter)
   - [Passer les paramètres par référence const](#Passer%20les%20param%C3%A8tres%20par%20r%C3%A9f%C3%A9rence%20const)
   - [Stratégies de validation](#Strat%C3%A9gies%20de%20validation)
6. [L'histoire de `const` (en profondeur)](#L%27histoire%20de%20const%20%28en%20profondeur%29)
7. [Quand NE PAS écrire de getter/setter](#Quand%20NE%20PAS%20%C3%A9crire%20de%20getter%2Fsetter)
8. [Pièges courants](#Pi%C3%A8ges%20courants)
9. [Conventions de nommage](#Conventions%20de%20nommage)
10. [Exemple complet — le `Contact` du PhoneBook](#Exemple%20complet%20%E2%80%94%20le%20Contact%20du%20PhoneBook)
11. [Arbre de décision / aide-mémoire](#Arbre%20de%20d%C3%A9cision%20%2F%20aide-m%C3%A9moire)

---

## Que sont les getters et les setters ?

Un **getter** (accesseur) et un **setter** (mutateur) sont des fonctions
membres publiques dont le seul rôle est de lire ou d'écrire une donnée
membre privée. Ce sont les *portes contrôlées* entre le monde extérieur et
l'intérieur d'une classe.

```
Monde extérieur  ──► setter()  ──► membre privé
Monde extérieur  ◄── getter()  ◄── membre privé
```

En C, tu lis et écris directement les champs d'une struct. En C++
idiomatique, les champs sont `private` et on n'y accède que par des
méthodes. Les getters et setters sont le cas le plus simple de ce schéma.

> **En bref :** la classe possède ses données ; les getters et setters
> sont la façon dont elle choisit de les exposer aux appelants.

---

## Le modèle mental : encapsulation et invariants de classe

L'encapsulation, ce n'est pas juste « mettre le champ en privé ». Sa vraie
raison d'être, ce sont les **invariants de classe** — des propriétés de
l'objet qui doivent être vraies à tout moment pour que l'objet soit valide.

Exemples :
- `Date` — le mois doit être dans `1..12`, le jour doit être valide pour ce mois.
- `Account` — le solde doit égaler la somme des dépôts passés moins les retraits.
- `Contact` (PhoneBook) — le numéro de téléphone ne doit contenir que des chiffres.

Si le champ est `public`, *n'importe quel* code, n'importe où, peut casser
l'invariant. La classe ne peut plus rien garantir sur elle-même, puisqu'elle
n'a pas son mot à dire sur qui écrit quoi.

Si le champ est `private` et n'est modifié que via un setter, le setter
**impose** l'invariant à chaque écriture. La classe devient *autodéfensive*.

> **Pourquoi « getter/setter » plutôt que « champ public » ?** Parce que
> « champ public » veut dire « aucun invariant possible ». Le coût d'une
> paire getter/setter est un niveau d'indirection ; le bénéfice, c'est que
> la classe contrôle sa propre validité. Pour les champs non triviaux,
> l'échange en vaut presque toujours la peine.

---

## Exemple minimal

```cpp
class Contact {
private:
    std::string m_name;          // private — outside code cannot touch this

public:
    void               setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }
};
```

```cpp
Contact c;
c.setName("Alice");                     // goes through the setter
std::cout << c.getName() << std::endl;  // goes through the getter
// c.m_name = "Bob";                    // ERROR — private, won't compile
```

Trois petits détails de cet exemple font un vrai travail — tous expliqués
dans les deux sections suivantes :

1. Le setter prend son paramètre en `const std::string&` (référence const).
2. Le getter renvoie un `const std::string&` (référence const).
3. Le getter est marqué `const` après la liste des paramètres.

---

## Anatomie d'un bon getter

### Renvoyer par valeur, par référence ou par référence const ?

C'est la décision la plus importante quand tu écris un getter. La « bonne »
réponse dépend du type du membre.

```cpp
//  A) Return BY VALUE  — caller gets a copy
std::string getName() const { return m_name; }

//  B) Return BY CONST REFERENCE — caller gets a read-only handle, no copy
const std::string& getName() const { return m_name; }

//  C) Return BY NON-CONST REFERENCE — caller can modify the member directly
std::string& getName() { return m_name; }   // (almost always wrong)
```

| Forme | Quand l'utiliser | Coût |
|------|-------------|------|
| `T` (par valeur) | Types primitifs (`int`, `bool`, `char`, …), ou quand la valeur est **calculée** et ne survit pas à l'appel. | Bon marché pour les primitifs, une copie pour les types lourds. |
| `const T&` | Types lourds (`std::string`, conteneurs, classes perso) quand le membre existe déjà dans l'objet. | Zéro copie. La référence est valide tant que l'objet l'est. |
| `T&` | **Presque jamais.** Ça donne à l'appelant une laisse pour modifier ton membre privé sans aucune validation. Ça ruine l'encapsulation. Acceptable seulement si la classe expose délibérément des internes modifiables (par ex. `std::vector::operator[]`). |

> **Règle pratique :** pour `int`/`bool`/`char` → renvoie par valeur.
> Pour `std::string`/les conteneurs → renvoie par `const T&`.
> Ne renvoie par référence non const que si tu veux vraiment dire « ce champ
> est public ; je l'écris juste `obj.foo()` au lieu de `obj.foo` ».

### Le `const` final

```cpp
const std::string& getName() const { return m_name; }
//                            ^^^^^
//   "this method does not modify *this"
```

Ce `const` sur la méthode a deux effets :

1. Le compilateur **vérifie** que la méthode ne modifie aucun champ non
   `mutable`. Si tu écris par erreur `m_name = "x";` dans le corps, ça ne
   compile pas. C'est un contrat vérifié à la compilation.
2. Il rend la méthode **appelable sur des objets `const`** :

   ```cpp
   void print(const Contact& c) {
       std::cout << c.getName();   // works ONLY if getName() is const
   }
   ```

   Si `getName()` n'était pas marquée `const`, l'appel échouerait parce
   que `c` est un `const Contact&` — un objet `const` ne peut appeler que
   des méthodes `const`.

**Marque toujours les getters `const`.** Un getter qui ne l'est pas, soit
ment en prétendant ne pas modifier l'état, soit s'exclut en silence des
objets `const` (ce qui rend les références `const` vers ta classe inutiles).

### Getters calculés (dérivés)

Un getter n'est pas obligé de renvoyer un champ stocké — il peut calculer
la valeur :

```cpp
class Person {
private:
    std::string m_firstName;
    std::string m_lastName;

public:
    std::string getFullName() const {           // ← BY VALUE, not by reference
        return m_firstName + " " + m_lastName;
    }
};
```

> **Piège :** ne renvoie *jamais* un `const T&` vers une valeur calculée
> dans la fonction. Le temporaire qui contient la concaténation est détruit
> à la fin de la fonction, et l'appelant recevrait une référence vers des
> déchets :
>
> ```cpp
> const std::string& getFullName() const {                 // ⚠ DANGLING
>     return m_firstName + " " + m_lastName;               //   temporary dies here
> }
> ```
>
> Les valeurs calculées doivent être renvoyées **par valeur**. Le
> compilateur élide en général la copie (optimisation de la valeur de
> retour), donc le coût est souvent nul.

---

## Anatomie d'un bon setter

### Passer les paramètres par référence const

```cpp
void setName(const std::string& name) { m_name = name; }
//           ^^^^^^^^^^^^^^^^^^^
```

Pour tout type non trivial, prends le paramètre en `const T&` :
- `const` — le setter promet de ne pas modifier l'argument de l'appelant.
- `&` — l'argument n'est pas copié à l'entrée ; la fonction travaille
  directement sur l'objet de l'appelant.

Pour les primitifs (`int`, `bool`, `char`, pointeurs), passe par valeur —
une référence vers une valeur de la taille d'un registre n'est pas plus
rapide que la valeur elle-même.

### Stratégies de validation

C'est là que le style getter/setter se rend utile. Un setter est le goulot
par lequel passent toutes les écritures — c'est l'endroit naturel pour
imposer l'invariant de classe.

Il y a quatre façons courantes de gérer une entrée invalide :

```cpp
// 1. SILENT REJECTION — caller has no idea the call was a no-op.
//    Easy but error-prone; useful for "best-effort" updates.
void setAge(int age) {
    if (age < 0 || age > 150) return;
    m_age = age;
}

// 2. RETURN A BOOL — caller chooses how to react.
//    Common in C++98 code that wants to avoid exceptions.
bool setAge(int age) {
    if (age < 0 || age > 150) return false;
    m_age = age;
    return true;
}

// 3. THROW ON BAD INPUT — caller must handle or propagate.
//    Best when "wrong" is genuinely exceptional; aligns with std-lib style.
void setAge(int age) {
    if (age < 0 || age > 150)
        throw std::invalid_argument("Contact: age out of range");
    m_age = age;
}

// 4. PRECONDITION CONTRACT — no run-time check, doc says "must be in [0,150]".
//    Cheapest; suitable when the caller is always trusted code.
void setAge(int age) {
    m_age = age;   // UB if precondition violated
}
```

Il n'y a pas de réponse unique — choisis-en une et **sois cohérent dans
toute la classe**. Pour les projets de la piscine de 42, les options (1) et
(3) sont toutes deux courantes : rejet silencieux pour une boucle de saisie
utilisateur, par exemple, exception pour les erreurs de programmation.

> **Schéma utile :** quand plusieurs setters partagent la même logique de
> validation, factorise la vérification dans une fonction privée
> (`bool isValidName(const std::string&) const`) et appelle-la à la fois
> depuis le setter et depuis tout constructeur qui accepte le même champ.

---

## L'histoire de `const` (en profondeur)

`const` est partout dans le code de getters/setters C++. Ça vaut le coup de
ralentir.

```cpp
class Foo {
public:
    int                f1() const;            // method is const
    void               f2(const int& x);      // parameter is const reference
    const std::string& f3() const;            // return type is const ref, method is const
private:
    int m_x;
};
```

Trois usages indépendants de `const`, chacun à un « emplacement » différent :

| Emplacement | Signification |
|------|---------|
| Après la liste des paramètres (`f1() const`) | La méthode ne modifie pas `*this`. |
| Avant le type d'un paramètre (`const int& x`) | La méthode ne modifie pas l'argument de l'appelant. |
| Avant le type de retour (`const std::string&`) | L'appelant ne peut pas modifier ce vers quoi pointe la référence renvoyée. |

### Surcharge sur `const`

Tu peux avoir **deux getters du même nom**, l'un `const` et l'autre non :

```cpp
class Buffer {
public:
          int& operator[](size_t i)       { return m_data[i]; }
    const int& operator[](size_t i) const { return m_data[i]; }
private:
    int m_data[100];
};
```

Le compilateur choisit la bonne surcharge selon que l'objet est `const` ou
non. Sur un `Buffer` non const, tu obtiens la référence modifiable ; sur un
`const Buffer`, tu obtiens celle en lecture seule. C'est exactement comme
ça que marche `std::vector::operator[]`.

### `const` logique contre `const` bit à bit

Le `const` d'une méthode est « logique » — il est *vérifié* par le
compilateur au niveau des bits (aucun champ ne peut être affecté), mais tu
peux t'en affranchir avec [`mutable`](../../../../en/cpp/lexique/MUTABLE.md) (EN) pour des
champs comme des caches ou des mutex, qui ne représentent pas l'état
observable de l'objet.

```cpp
class Cache {
public:
    int compute() const {
        if (!m_done) { m_value = expensive(); m_done = true; }
        return m_value;
    }
private:
    mutable int  m_value;   // can be modified inside const methods
    mutable bool m_done;
};
```

> **Attention aux membres pointeurs :** le `const` d'une méthode empêche
> de réaffecter `m_ptr` lui-même, mais **pas** ce vers quoi `m_ptr` pointe.
> Un getter `const` peut toujours faire `*m_ptr = 42;`. Pour l'empêcher, le
> pointeur doit être un `const T*` ou sa cible doit être const.

---

## Quand NE PAS écrire de getter/setter

C'est la partie que la plupart des guides pour débutants sautent, et c'est
la plus importante.

Si chaque membre d'une classe a sa paire `getX`/`setX`, la classe est
**anémique** : c'est un sac de données sans comportement, et la logique qui
*devrait* vivre dans la classe a fui chez ses appelants. Dès que tu vois ça :

```cpp
account.setBalance(account.getBalance() - 50);    // smell
```

…la classe ne fait pas son travail. Le comportement doit être **dans** la
classe :

```cpp
account.withdraw(50);                              // method does the work
```

Le principe, c'est **« dis, ne demande pas »** (*tell, don't ask*) — au lieu
de demander ses données à l'objet, de calculer quelque chose et de le
réécrire, *dis* à l'objet ce que tu veux qu'il fasse.

Une classe saine a en général :
- Une poignée de getters pour les champs que le monde extérieur a vraiment
  besoin de lire.
- **Peu ou pas** de setters bruts ; à la place, des méthodes nommées d'après
  des opérations (`deposit`, `rename`, `validate`, `addContact`) qui
  modifient l'état de façon cohérente avec le domaine.
- Des setters seulement pour les champs qui sont honnêtement « juste des
  données », sans invariant de classe qui les lie à d'autres.

> **Règle pratique avant d'écrire `setX` :** demande-toi « y a-t-il une
> opération de plus haut niveau ici ? ». Si oui, écris cette opération à la
> place. Si vraiment non, le setter convient.

---

## Pièges courants

| # | Piège | Symptôme | Correction |
|---|---------|---------|-----|
| 1 | Renvoyer un `const T&` vers un temporaire calculé dans la fonction | Lecture de mémoire poubelle ; parfois ça marche, parfois ça plante | Renvoyer par valeur les résultats calculés |
| 2 | Renvoyer un `T&` (non const) vers un membre privé | L'appelant contourne la validation ; encapsulation perdue | Renvoyer plutôt un `const T&` |
| 3 | Oublier `const` sur la méthode | L'appel échoue depuis un `const Contact&` ; avertissement du type « this is const » | Ajouter `const` après la liste des paramètres |
| 4 | Passer un `std::string` par valeur à un setter | Une copie inutile à chaque appel | Utiliser `const std::string&` |
| 5 | Valider dans les setters mais pas dans le constructeur | La classe peut être construite dans un état invalide | Appliquer la même vérification dans les deux, factorisée dans une fonction privée |
| 6 | Le setter rejette en silence une entrée invalide, l'appelant croit que ça a marché | Données périmées, bugs déroutants | Renvoyer un `bool`, lancer une exception, ou faire un assert |
| 7 | Champ public « pour la performance » | L'invariant casse dès que quelqu'un d'autre y écrit | Mesurer d'abord ; en pratique, un getter `const T&` ne coûte rien |
| 8 | Écrire `setX/getX` pour *chaque* membre | La classe devient un sac de données sans comportement | « Dis, ne demande pas » — modéliser des opérations, pas des champs |

---

## Conventions de nommage

| Style       | Getter        | Setter        | Notes |
|-------------|---------------|---------------|-------|
| Classique   | `getName()`   | `setName()`   | Le plus universel ; tradition C++/Java |
| Style Qt    | `name()`      | `setName()`   | Apprécié en C++ moderne, où le nom *est* la lecture |
| Style 42    | `F_GetName()` | `F_SetName()` | Héritage des sujets ; tu le croiseras pendant la piscine |

Pour le travail à 42, l'un ou l'autre convient. Au sein d'un même projet,
choisis-en un et **tiens-t'y** — mélanger les styles est la pire option.

Nommage des membres (champs privés) :

```cpp
std::string m_name;     // m_ prefix — common in 42 / older C++
std::string _name;      // leading underscore — also common (but reserved at file scope)
std::string name_;      // trailing underscore — Google style
std::string name;       // no prefix — relies on `this->name` for disambiguation
```

Le préfixe sert à distinguer le champ d'un paramètre ou d'une variable
locale du même nom (`setName(const std::string& name) { m_name = name; }`).

---

## Exemple complet — le `Contact` du PhoneBook

Un `Contact` pour le PhoneBook du module 00, qui montre tous les schémas
ci-dessus en contexte.

### `Contact.hpp`

```cpp
#ifndef CONTACT_HPP
# define CONTACT_HPP

# include <string>

class Contact {
private:
    std::string m_firstName;
    std::string m_lastName;
    std::string m_nickname;
    std::string m_phone;
    std::string m_secret;

    bool isPrintable(const std::string& s) const;

public:
    Contact();
    Contact(const std::string& first, const std::string& last,
            const std::string& nick,  const std::string& phone,
            const std::string& secret);
    Contact(const Contact& other);
    Contact& operator=(const Contact& other);
    ~Contact();

    // --- getters: const refs for std::string, all const methods ---
    const std::string& getFirstName() const;
    const std::string& getLastName()  const;
    const std::string& getNickname()  const;
    const std::string& getPhone()     const;
    const std::string& getSecret()    const;

    // --- setters: const-ref params, validation that throws on bad input ---
    void setFirstName(const std::string& name);
    void setLastName (const std::string& name);
    void setNickname (const std::string& nick);
    void setPhone    (const std::string& phone);
    void setSecret   (const std::string& secret);

    // --- a real operation, not a setter ---
    bool isEmpty() const;
};

#endif
```

### `Contact.cpp` (extraits)

```cpp
#include "Contact.hpp"
#include <stdexcept>
#include <cctype>

bool Contact::isPrintable(const std::string& s) const {
    for (size_t i = 0; i < s.size(); ++i)
        if (!std::isprint(static_cast<unsigned char>(s[i]))) return false;
    return !s.empty();
}

const std::string& Contact::getFirstName() const { return m_firstName; }
const std::string& Contact::getPhone()     const { return m_phone; }
// ... other getters

void Contact::setFirstName(const std::string& name) {
    if (!isPrintable(name))
        throw std::invalid_argument("Contact: first name not printable");
    m_firstName = name;
}

void Contact::setPhone(const std::string& phone) {
    for (size_t i = 0; i < phone.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(phone[i])))
            throw std::invalid_argument("Contact: phone has non-digit");
    m_phone = phone;
}

bool Contact::isEmpty() const {
    return m_firstName.empty() && m_lastName.empty()
        && m_nickname.empty()  && m_phone.empty()
        && m_secret.empty();
}
```

À remarquer :
- Chaque getter est `const` et renvoie un `const std::string&`. Zéro copie.
- Chaque setter prend un `const std::string&`, valide, et lance une
  exception sur une entrée invalide.
- `isPrintable` est `private` et `const` — une fonction d'aide utilisée
  par les setters.
- `isEmpty()` n'est **pas** un getter — c'est une question posée à la
  classe, calculée à partir de plusieurs champs. C'est le genre de méthode
  auquel mène « dis, ne demande pas ».

---

## Arbre de décision / aide-mémoire

```
Le code extérieur a-t-il besoin de LIRE ce champ ?
├── Non → pas de getter
└── Oui → ajoute un getter
         ├── Le champ est primitif (int, bool, char, ...) ?
         │   └── renvoie par valeur :       T    getX() const;
         ├── Le champ est lourd (string, conteneur, classe) ?
         │   └── renvoie par réf. const :   const T& getX() const;
         └── La valeur est CALCULÉE (pas stockée) ?
             └── renvoie par valeur :       T    getX() const;       (JAMAIS const T&)

Le code extérieur a-t-il besoin d'ÉCRIRE ce champ ?
├── Non → pas de setter (le champ est fixé dans le constructeur, ou jamais)
└── Oui → existe-t-il une opération de plus haut niveau qui devrait porter cette écriture ?
         ├── Oui → écris CETTE opération, pas un setter brut
         └── Non → ajoute un setter
                  ├── Paramètre : const T& si lourd, T si primitif
                  ├── Valide la nouvelle valeur (ou documente la précondition)
                  └── Choisis une stratégie d'erreur (silence / bool / throw)
                      et garde la même dans toute la classe
```

> **Résumé en une ligne :** chaque getter est `const`. Les types lourds se
> renvoient par `const T&`. Les valeurs calculées se renvoient par valeur.
> Les setters valident. Si tu te retrouves à écrire `setX/getX` pour chaque
> champ, demande-toi si la classe n'a pas plutôt besoin d'*opérations*.
