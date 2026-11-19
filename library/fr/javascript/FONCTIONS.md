# ⚙️ Fonctions, closures et callbacks

> **TL;DR.** En JS, une fonction est **une valeur** comme une autre : on la stocke dans une variable, on la passe en argument (*callback*), on la retourne. Les **fonctions fléchées** `(a) => a * 2` sont la syntaxe courte et la plus utilisée. Une fonction **se souvient** des variables qui l'entouraient à sa création : c'est une **closure**.

Liens : [`INDEX`](INDEX.md) · [`VARIABLES_TYPES`](VARIABLES_TYPES.md) · [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) · [`ASYNCHRONE`](ASYNCHRONE.md)

---

## 1. Trois façons d'écrire une fonction

```js
function degats(attaque, defense) {           // 1. déclaration classique
    return Math.max(0, attaque - defense);
}

const soin = function (pv) { return pv + 5; }; // 2. expression de fonction

const double = (x) => x * 2;                  // 3. fonction fléchée, retour implicite
const crit = (a, d) => {                       //    avec un corps : return explicite
    const base = degats(a, d);
    return base * 2;
};
```

- **1** ressemble au C, sans types (ils arrivent avec TypeScript).
- **3** : si le corps est une seule expression, pas besoin de `{}` ni de `return`. Attention : pour retourner un objet, il faut des parenthèses : `() => ({ hp: 10 })`.

Dans le code moderne (et en React), tu verras surtout la forme **3**.

---

## 2. Paramètres : défaut, rest, spread

```js
function attaquer(cible, puissance = 1) {       // valeur par défaut
    return `${cible} prend ${puissance}`;
}
attaquer("boss");                                // → "boss prend 1"

function total(...nombres) {                     // rest : tous les arguments en tableau
    return nombres.reduce((s, n) => s + n, 0);
}
total(1, 2, 3);                                  // → 6

const valeurs = [4, 5, 6];
total(...valeurs);                               // spread : « déplie » le tableau → 15
```

**Lien C :** `...nombres` remplace `va_list`/`va_arg`, en beaucoup plus simple.

---

## 3. Les callbacks : passer une fonction en argument

```js
function pourChaqueCarte(cartes, action) {
    for (const c of cartes) action(c);
}
pourChaqueCarte(["feu", "eau"], (c) => console.log("je joue", c));
```

**Lien C :** c'est un pointeur sur fonction (`void (*action)(char *)`), comme dans `ft_lstiter` ou `qsort`. Sauf qu'en JS tu peux l'écrire directement au moment de l'appel. C'est **partout** : `map`, `filter`, `setTimeout`, les événements, React…

---

## 4. Les closures

```js
function creerCompteur() {
    let n = 0;                 // variable locale…
    return () => {
        n = n + 1;             // …que la fonction retournée continue de voir
        return n;
    };
}
const prochainTour = creerCompteur();
prochainTour();   // → 1
prochainTour();   // → 2
```

En C, `n` serait détruite à la sortie de `creerCompteur` et le pointeur serait invalide. En JS, la fonction fléchée **capture** `n` : tant qu'elle existe, `n` existe. C'est l'équivalent d'une variable `static` locale, mais **une par compteur créé**.

### Le piège classique : `var` dans une boucle

```js
for (var i = 0; i < 3; i++) setTimeout(() => console.log(i));  // → 3, 3, 3
for (let j = 0; j < 3; j++) setTimeout(() => console.log(j));  // → 0, 1, 2
```

Avec `var`, il n'y a **qu'un** `i` partagé ; quand les callbacks s'exécutent, la boucle est finie et `i` vaut 3. Avec `let`, chaque tour a **son propre** `j`. Encore une raison de bannir `var`.

---

## 5. `this` (juste ce qu'il faut savoir)

En C++, `this` désigne toujours l'objet de la méthode. En JS, pour une `function` classique, `this` dépend de **la façon dont on l'appelle**, et peut se perdre :

```js
class Joueur {
    constructor(nom) { this.nom = nom; }
    saluer() { console.log("je suis", this.nom); }
}
const p = new Joueur("Ana");
p.saluer();                     // → je suis Ana
const f = p.saluer;
// f();                         // ❌ TypeError : this est undefined, la méthode a été « détachée »
setTimeout(() => p.saluer());   // ✅ la fléchée appelle bien p.saluer()
```

Les **fonctions fléchées n'ont pas leur propre `this`** : elles reprennent celui de l'endroit où elles sont écrites. Règle pratique : **dans un callback, utilise une fléchée**.

---

## ⚠️ À retenir

- Une fonction est une valeur : on la passe, on la stocke, on la retourne.
- Fléchée par défaut, surtout pour les callbacks.
- Une closure garde vivantes les variables qu'elle utilise.
- Ne passe jamais `obj.methode` seule en callback : enveloppe-la, `() => obj.methode()`.
