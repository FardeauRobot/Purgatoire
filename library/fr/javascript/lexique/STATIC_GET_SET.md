# `static`, `get`, `set`, `#privé` — membres spéciaux de classe

> **TL;DR.** `static` = membre **de la classe**, pas des instances (comme en C++). `get` / `set` = accesseurs qui **ressemblent à des attributs** (`obj.x`) mais exécutent une fonction. `#champ` = champ **vraiment privé**, vérifié par le moteur : y accéder hors de la classe est une erreur de *syntaxe*.

Liens : [`INDEX`](INDEX.md) · [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) · [`THIS`](THIS.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `static int count;` + définition dans le `.cpp` | `static count = 0;` | Pas de définition séparée |
| `Animal::count` | `Animal.count` | Le point remplace `::` |
| `static Animal create(...)` | `static create(...)` | Méthode « usine », classique |
| `private: int _secret;` | `#secret = 42;` | Le `#` fait partie du nom |
| `int getSecret() const` | `get secret()` | Appelé sans parenthèses : `obj.secret` |
| `void setSecret(int v)` | `set secret(v)` | Appelé par une affectation : `obj.secret = 7` |

---

## 2. Exemple

```js
class Animal {
    #secret = 42;
    static count = 0;

    constructor(name) { this.name = name; Animal.count++; }

    get secret() { return this.#secret; }
    set secret(v) {
        if (v < 0) throw new RangeError("négatif");
        this.#secret = v;
    }
    static create(n) { return new Animal(n); }
}

const a = Animal.create("Rex");
console.log(Animal.count, a.secret);  // → 1 42
a.secret = 7;                         // passe par le setter
console.log(a.secret);                // → 7
a.secret = -1;                        // ❌ RangeError: négatif
console.log(a.count);                 // → undefined   (static ≠ instance)
```

---

## 3. `#privé` : une vraie barrière

```js
console.log(a.#secret);
// ❌ SyntaxError: Private field '#secret' must be declared in an enclosing class
```

Ce n'est pas une erreur à l'exécution : **le fichier entier refuse de se charger**. Plus strict que `private` en C++, qu'on contourne avec un `reinterpret_cast`. Le `#secret` n'apparaît ni dans `Object.keys`, ni dans `JSON.stringify`, ni dans `console.log` côté programme.

Avant l'arrivée de `#`, la convention était `_secret` : un simple nom, accessible par tous. Tu la verras encore partout. C'est la même convention que les `_membres` de ce dépôt, sans aucune protection.

---

## 4. `static` est hérité

```js
class Dog extends Animal {}
console.log(Dog.count);   // → 1   (Dog voit le static d'Animal via la chaîne des constructeurs)
```

---

## 5. Pièges

- **Getter sans setter** : `obj.x = 5` est ignoré en silence (ou `TypeError` en mode strict, donc dans une classe).
- **Récursion infinie** : `set name(v) { this.name = v; }` se rappelle lui-même. Stocke dans `#name`.
- **`this` dans une méthode `static`** désigne la **classe**, pas une instance.
- `get`, `set` et `static` ne sont pas des mots réservés : `const get = 1` est valide hors d'une classe.
