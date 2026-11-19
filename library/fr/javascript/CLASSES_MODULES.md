# 🏛️ Classes et modules

> **TL;DR.** Les classes JS ressemblent beaucoup au C++ : `constructor`, méthodes, `extends`, `super`, champs privés `#`. Pas de forme canonique (pas de destructeur, pas d'opérateur d'affectation). Chaque fichier est un **module** : ce qu'il ne fait pas `export` reste invisible, et on récupère le reste avec `import`.

Liens : [`INDEX`](INDEX.md) · [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) · [`TYPESCRIPT`](TYPESCRIPT.md)

---

## 1. Une classe

```js
class Carte {
    #utilisations = 0;                    // champ privé (le # fait partie du nom)
    static total = 0;                     // champ partagé par toutes les instances

    constructor(nom, attaque) {
        this.nom = nom;                   // champ public, créé à l'affectation
        this.attaque = attaque;
        Carte.total++;
    }

    jouer() {
        this.#utilisations++;
        return `${this.nom} inflige ${this.attaque}`;
    }

    get usee() {                          // getter : s'utilise comme un champ
        return this.#utilisations >= 3;
    }
}

const c = new Carte("Aura", 7);
c.jouer();         // → "Aura inflige 7"
c.usee;            // → false (pas de parenthèses : c'est un getter)
// c.#utilisations // ❌ SyntaxError : vraiment privé
```

| C++98 | JavaScript |
|---|---|
| `Carte(std::string n) : _nom(n) {}` | `constructor(nom) { this.nom = nom; }` |
| `private: int _x;` | `#x` |
| `static int total;` | `static total = 0;` |
| `this->nom` | `this.nom` (le `this.` est **obligatoire**) |
| `~Carte()` | n'existe pas (garbage collector) |
| `operator=`, constructeur de copie | n'existent pas : les objets sont des références |

⚠️ Oublier `this.` est l'erreur n°1 en venant du C++ : `nom` tout seul cherche une variable locale, pas le champ.

---

## 2. Héritage

```js
class CarteSort extends Carte {
    constructor(nom, attaque, cout) {
        super(nom, attaque);              // appel obligatoire avant d'utiliser this
        this.cout = cout;
    }

    jouer() {                             // redéfinition : toujours « virtuelle »
        return super.jouer() + ` pour ${this.cout} mana`;
    }
}
```

**Lien CPP03/CPP04 :** toutes les méthodes sont automatiquement « `virtual` ». `super(...)` remplace la liste d'initialisation `: Carte(nom, attaque)`.

---

## 3. Classes ou objets simples ?

En JS moderne, on utilise **beaucoup moins de classes** qu'en C++ :

- **Données qui voyagent** (requêtes, réponses JSON, état React) → objets simples `{ … }` + un type TypeScript.
- **Comportement avec état interne** (un service, un modèle de base de données, une partie en cours) → classe.

AdonisJS utilise des classes (contrôleurs, modèles) ; React moderne utilise des **fonctions**.

---

## 4. Modules : `import` / `export`

```js
// fichier : regles.js
export const PV_MAX = 30;                       // export nommé
export function degats(a, d) { return Math.max(0, a - d); }
export default class Partie { /* … */ }         // export par défaut (un seul par fichier)
```

```js
// fichier : main.js
import Partie, { PV_MAX, degats } from "./regles.js";
import * as regles from "./regles.js";          // tout, sous un espace de noms
import { useState } from "react";               // depuis une bibliothèque (node_modules)
```

- **Export nommé** : importé entre `{ }`, avec le même nom.
- **Export par défaut** : importé sans `{ }`, avec le nom que tu veux.
- Chemin qui commence par `./` ou `../` = ton fichier ; sinon = un paquet npm.

**Lien C :** un module, c'est un `.c` + son `.h` réunis. Ce qui n'est pas exporté est comme une fonction `static` : invisible de l'extérieur.

> ℹ️ Tu verras aussi l'ancienne syntaxe Node `const x = require("x")` / `module.exports = …` (CommonJS). Les projets récents (AdonisJS v6, Vite/React) utilisent `import`/`export`.

---

## ⚠️ À retenir

- `this.` obligatoire dans les méthodes.
- `#champ` pour du vraiment privé.
- `super(...)` avant tout `this` dans un constructeur dérivé.
- Un fichier = un module ; `export` ce qui doit sortir.
