# 📖 GLOSSAIRE — le vocabulaire JS / TS, décodé

> Les **concepts** qui ne sont pas des mots-clés mais reviennent partout : closure, hoisting, prototype, event loop… Chaque entrée donne une **définition en une ligne**, un **exemple court**, le **rapprochement avec le C/C++** et un **lien** vers la page détaillée.
>
> Tu cherches un **mot-clé** (`let`, `this`, `await`…) ? C'est l'[`INDEX`](INDEX.md).

```
            JavaScript  =  un seul thread
                         + des objets reliés par prototypes
                         + des fonctions qui sont des valeurs
                         + une boucle d'événements
            TypeScript  =  JavaScript + des types vérifiés AVANT l'exécution, puis effacés
```

---

## 🗺️ Par famille

| Famille | Entrées |
|---|---|
| Portée & variables | [closure](#closure) · [hoisting](#hoisting) · [TDZ](#TDZ) · [mode strict](#mode%20strict) |
| Valeurs | [référence vs valeur](#référence%20vs%20valeur) · [truthy / falsy](#truthy%20et%20falsy) · [coercition](#coercition) · [copie superficielle / profonde](#copie%20superficielle%20et%20profonde) · [déstructuration](#déstructuration) |
| Objets | [prototype](#prototype) · [garbage collector](#garbage%20collector) · [itérable](#itérable) · [JSON](#JSON) |
| Fonctions | [fonction de première classe](#fonction%20de%20première%20classe) · [callback](#callback) · [fonction pure](#fonction%20pure) |
| Asynchrone | [event loop](#event%20loop) · [promesse](#promesse) · [microtâche](#microtâche) |
| Écosystème | [ESM / CommonJS](#ESM%20et%20CommonJS) · [moteur (V8)](#moteur%20V8) · [bundler](#bundler) · [transpilation](#transpilation) |
| TypeScript | [type erasure](#type%20erasure) · [typage structurel](#typage%20structurel) · [narrowing](#narrowing) · [inférence](#inférence) |

---

## Portée & variables

### closure
Une fonction **emporte avec elle** les variables de l'endroit où elle a été créée, même quand cet endroit n'existe plus.
```js
function makeCounter() { let n = 0; return () => ++n; }
const c = makeCounter(); c(); c();   // → 2 : n survit à makeCounter
```
C/C++ : comme un foncteur qui stockerait ses membres automatiquement, ou une variable `static` locale, mais **une par closure**. → [`FUNCTION_RETURN`](FUNCTION_RETURN.md) · [`VAR_LET_CONST`](VAR_LET_CONST.md) §4

### hoisting
Les déclarations sont « remontées » en haut de leur portée avant l'exécution. `function` est utilisable avant sa ligne ; `var` existe mais vaut `undefined` ; `let`/`const`/`class` existent mais sont inaccessibles (TDZ).
C/C++ : rend inutiles les prototypes de fonction. → [`VAR_LET_CONST`](VAR_LET_CONST.md)

### TDZ
*Temporal Dead Zone* : la zone entre le début du bloc et la ligne `let`/`const`, où toucher la variable lève une `ReferenceError`. → [`VAR_LET_CONST`](VAR_LET_CONST.md) §3

### mode strict
Variante plus sévère du langage (variables non déclarées interdites, `this` nu = `undefined`…). Automatique dans les modules et les classes. → [`LEGACY`](LEGACY.md) §1

---

## Valeurs

### référence vs valeur
Les primitifs sont **copiés** ; les objets, tableaux et fonctions sont manipulés **par référence**. `const b = a` sur un objet crée un second nom, pas une copie.
C/C++ : chaque variable objet se comporte comme un pointeur, mais sans `*` ni `->`, et sans arithmétique. → [`NEW_DELETE`](NEW_DELETE.md)

### truthy et falsy
Toute valeur est convertie en booléen dans une condition. *Falsy* : `false 0 -0 0n "" null undefined NaN`. Tout le reste est *truthy*, y compris `"0"`, `[]` et `{}`. → [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) §2

### coercition
Conversion **implicite** de type : `"5" - 3 → 2`, `"5" + 3 → "53"`. Source de la plupart des bizarreries de JS ; `===` l'évite. → [`OPERATEURS`](OPERATEURS.md) §2

### copie superficielle et profonde
`{ ...obj }` ou `[...arr]` copient le premier niveau, les sous-objets restent **partagés**. `structuredClone(obj)` copie tout.
C/C++ : exactement la *shallow copy* vs *deep copy* du constructeur de copie, sauf qu'ici la copie n'est jamais implicite. → [`OPERATEURS`](OPERATEURS.md) §4

### déstructuration
Extraire des champs dans des variables en une ligne.
```js
const { name, age = 0 } = user;       const [first, ...rest] = arr;
function f({ id, label }) { … }       // très utilisé pour les props React
```
→ [`OPERATEURS`](OPERATEURS.md) §4

---

## Objets

### prototype
Chaque objet a un lien caché vers un autre objet, son prototype. Une propriété absente est cherchée en remontant cette chaîne. Les `class` sont construites par-dessus.
C/C++ : le rôle de la **vtable**, mais résolu dynamiquement et modifiable à l'exécution. → [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) §3

### garbage collector
Le moteur libère automatiquement tout objet **inaccessible**. Ni `delete`, ni destructeur, ni `free`. Les fuites restent possibles via des références oubliées (timers, écouteurs, caches). → [`NEW_DELETE`](NEW_DELETE.md) §4

### itérable
Tout objet qui possède une méthode `[Symbol.iterator]` : tableaux, chaînes, `Map`, `Set`, générateurs. Il fonctionne avec `for…of`, `...` et la déstructuration `[a, b]`.
C/C++ : un conteneur avec `begin()`/`end()`. → [`YIELD_GENERATEURS`](YIELD_GENERATEURS.md) · [`BOUCLES`](BOUCLES.md)

### JSON
Format texte d'échange, calqué sur la syntaxe des objets JS. `JSON.stringify(obj)` sérialise, `JSON.parse(txt)` désérialise. Perd les fonctions, les `undefined`, les `Date` (devenues des chaînes) et les champs `#privés`.

---

## Fonctions

### fonction de première classe
Une fonction est une valeur : on peut la stocker, la passer, la renvoyer.
C/C++ : un pointeur de fonction, avec une syntaxe humaine et la capture en plus. → [`FUNCTION_RETURN`](FUNCTION_RETURN.md)

### callback
Fonction passée à une autre fonction, pour que celle-ci l'appelle plus tard : `arr.map(x => x * 2)`, `button.addEventListener("click", onClick)`. → [`THIS`](THIS.md) §3

### fonction pure
Même entrée → même sortie, sans effet de bord. Notion centrale de React : un composant doit être pur vis-à-vis de ses *props*.

---

## Asynchrone

### event loop
La boucle qui, sur un **seul thread**, exécute le code synchrone puis, quand la pile est vide, prend la tâche suivante (I/O terminée, timer, clic…).
C/C++ : la boucle `poll()` de webserv, mais fournie par le moteur et invisible. → [`ASYNC_AWAIT`](ASYNC_AWAIT.md) §3

### promesse
Objet représentant une valeur **future**. Trois états : `pending`, puis `fulfilled` (valeur) ou `rejected` (erreur). `await` attend son issue. → [`ASYNC_AWAIT`](ASYNC_AWAIT.md)

### microtâche
File prioritaire où vont les suites de promesses (`.then`, reprise après `await`). Elle est vidée **avant** les timers et les I/O. → [`ASYNC_AWAIT`](ASYNC_AWAIT.md) §3

---

## Écosystème

### ESM et CommonJS
Deux systèmes de modules. **ESM** : `import`/`export`, le standard. **CommonJS** : `require`/`module.exports`, l'historique de Node. → [`IMPORT_EXPORT`](IMPORT_EXPORT.md) · [`LEGACY`](LEGACY.md)

### moteur V8
Le programme qui exécute le JS : V8 dans Chrome et Node. Il **compile à la volée** (JIT) les fonctions souvent appelées en code machine.

### bundler
Outil (Vite, Webpack…) qui assemble des centaines de modules en quelques fichiers pour le navigateur. C'est l'équivalent de l'éditeur de liens.

### transpilation
Traduire du code source vers un autre code source : TypeScript → JavaScript, JS récent → JS ancien. Pas de binaire produit. → fiche [`OUTILS`](../OUTILS.md)

---

## TypeScript

### type erasure
Les types TS sont **effacés** à la compilation : il n'en reste rien à l'exécution. Une donnée venue du réseau peut donc mentir à tes types, d'où la validation obligatoire côté backend.
C/C++ : l'inverse des templates, qui génèrent du code par type. → fiche [`TYPESCRIPT`](../TYPESCRIPT.md)

### typage structurel
Deux types sont compatibles s'ils ont **la même forme**, peu importe leur nom. Un objet `{ x, y }` convient partout où `Point { x; y }` est attendu, sans `implements`.
C/C++ : le contraire du typage **nominal** du C++, où deux `struct` identiques restent des types différents.

### narrowing
TypeScript **rétrécit** le type d'une variable après un test : après `if (typeof x === "string")`, `x` est connu comme `string`. Fonctionne avec `typeof`, `instanceof`, `in`, `===` et `?.`. → [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md)

### inférence
TypeScript **devine** les types quand tu ne les écris pas : `const n = 5` est un `number`. Annote les paramètres de fonction et les frontières (API, props) ; laisse inférer le reste.
C/C++ : comme `auto` en C++11, **interdit** dans tes modules C++98.
