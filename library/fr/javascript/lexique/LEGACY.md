# Fossiles — `with`, `arguments`, `eval`, `debugger`, `require`, mots réservés

> **TL;DR.** Des morceaux du langage hérités d'avant 2015, que tu croiseras dans de vieux exemples ou sur Stack Overflow. La plupart sont **interdits en mode strict**, donc dans les modules et les classes. Il suffit de les reconnaître pour savoir les remplacer.

Liens : [`INDEX`](INDEX.md) · [`VAR_LET_CONST`](VAR_LET_CONST.md) · [`IMPORT_EXPORT`](IMPORT_EXPORT.md) · [`GLOSSAIRE`](GLOSSAIRE.md) (mode strict)

---

## 1. Le mode strict, la ligne de partage

```js
"use strict";   // en tête de fichier ou de fonction : active le mode strict
```

**Activé automatiquement** dans les modules ESM (`import`/`export`) et dans le corps des classes. En pratique, ton code Transcendance (React, TypeScript, AdonisJS) est **toujours** en mode strict.

| Sans mode strict | Avec |
|---|---|
| `x = 5` sans déclaration → crée une globale | `ReferenceError: x is not defined` |
| écriture dans une propriété en lecture seule → ignorée | `TypeError` |
| `this` d'un appel nu → l'objet global | `undefined` |
| `with`, octal `010`, `delete variable` → acceptés | `SyntaxError` |

---

## 2. Le musée

| Fossile | Ce que ça fait | Remplacement |
|---|---|---|
| `var` | variable à portée de fonction, remontée | `let` / `const` ([`VAR_LET_CONST`](VAR_LET_CONST.md)) |
| `arguments` | pseudo-tableau des arguments (`Array.isArray` → `false`) | paramètre `...rest` |
| `with (obj) { x }` | ajoute les propriétés d'`obj` à la portée | déstructuration `const { x } = obj` |
| `eval("2 + 2")` | exécute une chaîne comme du code → `4` | **rien**. Faille d'injection, à proscrire |
| `new Function("a", "return a")` | idem, sous une autre forme | idem |
| `debugger;` | point d'arrêt si un débogueur est attaché (DevTools, `node inspect`) | toujours valide, retire-le avant de commit |
| `010` | octal implicite → `8` | `0o10` |
| `escape` / `unescape` | encodage d'URL obsolète | `encodeURIComponent` |
| fonction-constructeur `function Old() { this.n … }` | « classe » d'avant 2015 | `class` ([`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md)) |
| `require("x")` / `module.exports` | modules **CommonJS** de Node | `import` / `export` ([`IMPORT_EXPORT`](IMPORT_EXPORT.md)) |

`require` n'est pas un fossile mort : beaucoup de paquets npm et de configurations (`.cjs`) l'utilisent encore. On peut faire `import` d'un module CommonJS. L'inverse est plus délicat.

---

## 3. Mots réservés sans usage

Réservés pour un usage futur, ils ne servent à rien en JS mais tu **ne peux pas** les utiliser comme noms de variables :

- **toujours** : `enum` (`var enum = 1` → `SyntaxError: Unexpected reserved word`) ;
- **en mode strict** : `implements`, `interface`, `package`, `private`, `protected`, `public`.

Clin d'œil : ce sont exactement ceux que **TypeScript** récupère (`enum`, `interface`, `implements`, `private`…). Voir le lot TypeScript.

Anciens réservés **libérés** depuis ES5 : `abstract`, `boolean`, `byte`, `char`, `double`, `final`, `float`, `goto`, `int`, `long`, `native`, `short`, `synchronized`, `throws`, `transient`, `volatile`. Ils venaient de Java.

---

## 4. Mots-clés contextuels

Ce ne sont des mots-clés **qu'à certains endroits** et ils restent valides comme noms ailleurs :

| Mot | Mot-clé dans… |
|---|---|
| `async`, `of`, `get`, `set`, `static`, `from`, `as` | leur position syntaxique uniquement |
| `let`, `yield` | réservés en mode strict |
| `await` | réservé dans les modules et les fonctions `async` |
| `arguments`, `eval` | ne peuvent pas être réaffectés en mode strict |

```js
var yield = 1, await = 2, async = 3, of = 4;   // ✅ valide… en script non strict
```

Valide ne veut pas dire raisonnable.
