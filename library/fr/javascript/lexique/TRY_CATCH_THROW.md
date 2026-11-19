# `try`, `catch`, `finally`, `throw` — les exceptions

> **TL;DR.** Même principe qu'en C++, avec trois différences : **un seul `catch`** (pas de tri par type, on teste avec `instanceof`), un bloc **`finally`** qui s'exécute toujours, et on peut `throw` **n'importe quoi**, mais lance toujours un `Error`, qui porte la pile d'appels. Une promesse rejetée est une exception asynchrone : elle se rattrape avec `await` dans un `try`.

Liens : [`INDEX`](INDEX.md) · [`ASYNC_AWAIT`](ASYNC_AWAIT.md) · [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md) · [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `catch (const std::out_of_range &e)` | `catch (e) { if (e instanceof RangeError) … }` | Un seul `catch`, tri à la main |
| `catch (...)` | `catch (e)` / `catch { }` | Le second ignore l'erreur |
| `class MyErr : public std::exception` | `class MyErr extends Error` | |
| `e.what()` | `e.message` | Plus `e.name` et `e.stack` |
| destructeurs (RAII) | `finally { … }` | Nettoyage garanti, écrit à la main |
| `throw;` (relancer) | `throw e;` | |

---

## 2. Forme complète

```js
function tf() {
    try {
        return "try";
    } finally {
        console.log("finally quand même");   // exécuté AVANT que le return ne parte
    }
}
console.log(tf());
// → finally quand même
// → try

try { null.x; }
catch (e) { console.log(e instanceof TypeError, e.name); }   // → true TypeError

try { JSON.parse("{"); }
catch { console.log("catch sans variable"); }               // quand l'erreur ne sert pas
```

Erreurs natives à connaître : `TypeError` (accès à `undefined.x`, appel d'une non-fonction), `ReferenceError` (variable inconnue), `RangeError`, `SyntaxError` (dont `JSON.parse` invalide).

---

## 3. Ses propres erreurs

```js
class AppError extends Error {
    constructor(msg, code) {
        super(msg);
        this.name = "AppError";
        this.code = code;
    }
}

try { throw new AppError("introuvable", 404); }
catch (e) { console.log(e.name, e.message, e.code, e instanceof Error); }
// → AppError introuvable 404 true

const err = new Error("haut niveau", { cause: new Error("bas niveau") });
console.log(err.cause.message);   // → bas niveau   (chaîner sans perdre l'origine)
```

---

## 4. Lancer autre chose qu'un `Error`

```js
try { throw "juste une chaîne"; }
catch (e) { console.log(typeof e, e.stack); }   // → string undefined   ⚠️ pas de pile
```

C'est permis, mais tu perds la pile d'appels. Lance toujours `new Error(...)` ou une sous-classe.

---

## 5. Pièges

- **Erreur dans un callback** : un `try` autour de `setTimeout(() => { throw … })` ne l'attrape pas, puisque le callback s'exécute plus tard, hors du `try`.
- **Promesse sans `await`** : `try { fetch(url) } catch {}` n'attrape rien. Il faut `await fetch(url)`. Voir [`ASYNC_AWAIT`](ASYNC_AWAIT.md).
- **`return` dans `finally`** écrase le `return` ou l'exception du `try`. N'en mets jamais.
- **Avaler les erreurs** : un `catch {}` vide masque les bugs. Journalise au minimum.
- En TypeScript, `e` est de type `unknown` dans un `catch` : vérifie `e instanceof Error` avant `e.message`.
