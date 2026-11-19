# `class`, `extends`, `super`, `constructor` — les classes

> **TL;DR.** Une `class` JS ressemble à une classe C++ mais repose sur des **prototypes** : c'est une fonction spéciale, plus une chaîne d'objets dans laquelle on cherche les méthodes. `extends` = héritage simple, `super(...)` appelle le constructeur parent (**obligatoire avant tout `this`**), `super.methode()` appelle la version parente. Toutes les méthodes sont « virtuelles ».

Liens : [`INDEX`](INDEX.md) · [`STATIC_GET_SET`](STATIC_GET_SET.md) · [`THIS`](THIS.md) · [`NEW_DELETE`](NEW_DELETE.md) · fiche [`CLASSES_MODULES`](../CLASSES_MODULES.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `class Dog : public Animal` | `class Dog extends Animal` | Héritage **simple** uniquement |
| liste d'init `: Animal(name)` | `super(name);` dans `constructor` | Avant toute utilisation de `this` |
| `Animal::speak()` | `super.speak()` | Appel de la version parente |
| `virtual` | (implicite) | **Toute** méthode est redéfinissable, liaison dynamique |
| attributs déclarés dans la classe | `this.x = …` dans le constructeur, ou champ `x = …;` | Créés à l'exécution |
| destructeur `~Dog()` | — | Le *garbage collector* libère, pas de hook fiable |
| Forme canonique (4 membres) | — | Pas de copie implicite : `=` copie la **référence** |

---

## 2. Exemple complet

```js
class Animal {
    constructor(name) { this.name = name; }
    speak() { return `${this.name} fait un bruit`; }
}

class Dog extends Animal {
    constructor(name) {
        super(name);              // appelle Animal.constructor
        this.kind = "chien";
    }
    speak() { return super.speak() + " : Ouaf"; }
}

const d = new Dog("Rex");
console.log(d.speak());           // → Rex fait un bruit : Ouaf
console.log(Object.keys(d));      // → [ 'name', 'kind' ]   (les méthodes n'y sont pas)
console.log(typeof Animal);       // → function
```

---

## 3. Sous le capot : la chaîne de prototypes

```
   d ──────────────► Dog.prototype ────────► Animal.prototype ───► Object.prototype ──► null
 { name, kind }       { speak }               { speak }            { toString, … }
   données            méthodes de Dog         méthodes d'Animal
```

`d.speak()` cherche `speak` dans `d`, puis remonte la chaîne et trouve celle de `Dog.prototype`. C'est l'équivalent dynamique de la **vtable** : au lieu d'un tableau de pointeurs fixé à la compilation, on parcourt une liste d'objets à l'exécution.

```js
Object.getPrototypeOf(d) === Dog.prototype;                     // → true
Object.getPrototypeOf(Dog.prototype) === Animal.prototype;      // → true
```

---

## 4. Les règles que le moteur impose

```js
class B extends Animal {
    constructor() { this.x = 1; super("x"); }
}
new B();   // ❌ ReferenceError: Must call super constructor in derived class
           //    before accessing 'this' or returning from derived constructor

Animal("x"); // ❌ TypeError: Class constructor Animal cannot be invoked without 'new'
```

Une classe dérivée n'a **pas de `this` tant que `super()` n'a pas été appelé** : l'objet est construit par le parent. C'est le même ordre qu'en C++ (base d'abord, dérivée ensuite), mais ici tu l'écris à la main.

---

## 5. Pièges

- **Méthode passée en callback** : `button.onclick = d.speak` perd `this`. Voir [`THIS`](THIS.md).
- **Pas de surcharge de constructeur** : un seul `constructor`, gère les variantes avec des paramètres par défaut.
- **Copie** : `const d2 = d` ne copie rien, c'est un second nom vers le même objet. Il n'y a pas de constructeur de copie.
- Les classes sont en **mode strict** automatiquement : variable non déclarée = `ReferenceError`.
- Pas de `protected` ni de `private` en JS pur, mais les champs `#privés` existent : voir [`STATIC_GET_SET`](STATIC_GET_SET.md). TypeScript ajoute `private`/`protected` (lot TypeScript).
