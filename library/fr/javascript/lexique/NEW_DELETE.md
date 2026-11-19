# `new`, `delete` — les faux amis du C++

> **TL;DR.** `new C(...)` crée un objet, le relie à `C.prototype` et appelle le constructeur. **Il n'y a pas de `delete` d'objet** : le *garbage collector* libère tout objet que plus rien ne référence. Le mot-clé `delete` existe, mais il **retire une propriété d'un objet**. Il ne libère pas de mémoire.

Liens : [`INDEX`](INDEX.md) · [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) · [`GLOSSAIRE`](GLOSSAIRE.md) (garbage collector) · fiche [`PHILOSOPHIE`](../PHILOSOPHIE.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `Dog *d = new Dog("Rex");` | `const d = new Dog("Rex");` | Pas de pointeur visible : `d` **est** une référence |
| `delete d;` | `d = null;` (ou sortir de la portée) | Le GC libère quand plus personne ne pointe dessus |
| `new int[10]` / `delete[]` | `new Array(10)` ou `[]` | Taille dynamique, pas de libération |
| `map.erase("a")` | `delete obj.a` | **Ça**, c'est le vrai rôle de `delete` |
| `new` sur la pile vs le tas | — | Tout objet est sur le tas, les primitifs sont copiés |

---

## 2. Ce que fait `new`

```
 new Dog("Rex")
   1. crée un objet vide            {}
   2. le relie au prototype         {} ──► Dog.prototype
   3. appelle constructor avec this = cet objet
   4. renvoie l'objet (sauf si le constructeur renvoie un autre objet)
```

```js
function Old(n) { this.n = n; }   // « classe » d'avant 2015 : une simple fonction
console.log(new Old(3).n);         // → 3
```

Une `class` exige `new` (`Animal("x")` → `TypeError`). Une vieille fonction-constructeur appelée sans `new` écrit dans l'objet global sans prévenir.

---

## 3. `delete` retire une propriété

```js
const p = { a: 1, b: 2 };
console.log(delete p.a, p, "a" in p);   // → true { b: 2 } false

const arr = [1, 2, 3];
delete arr[1];
console.log(arr, arr.length);           // → [ 1, <1 empty item>, 3 ] 3   ⚠️ un trou !
```

Pour un tableau, utilise `arr.splice(1, 1)` ou `arr.filter(...)`. `delete` laisse un **trou**, et la longueur ne change pas.

`delete` sur une variable (`delete x`) est une `SyntaxError` en mode strict : on ne supprime que des **propriétés**.

---

## 4. Et les fuites de mémoire ?

Le GC libère ce qui est **inaccessible**, pas ce qui est **inutile**. Fuites classiques en JS :

- un `setInterval` jamais arrêté, qui garde son callback (et tout ce qu'il capture) en vie ;
- un écouteur d'événement jamais retiré (`removeEventListener`), typique en React sans fonction de nettoyage dans `useEffect` ;
- un cache global (`Map`) qui grossit sans fin. `WeakMap` laisse le GC reprendre les clés.

C'est l'équivalent JS du « still reachable » de valgrind.

---

## 5. Pièges

- **`new Number(5)`, `new String("x")`** créent des objets enveloppes : `typeof new Number(5)` → `"object"`. Utilise `Number("5")` sans `new` pour **convertir**.
- **Pas de destructeur** : ne compte pas sur le GC pour fermer un fichier ou une socket. Appelle `close()` explicitement, ou utilise `finally` ([`TRY_CATCH_THROW`](TRY_CATCH_THROW.md)).
- **Rendre un objet inaccessible = le libérer** : `obj = null` suffit si c'était la dernière référence.
