# 🪜 Aborder JavaScript et TypeScript

> **TL;DR.** Tu sais déjà programmer : ne réapprends pas les boucles, apprends **ce qui change**. Trois idées font tout le reste : les types sont portés par les **valeurs**, la mémoire est **gérée** pour toi, et il n'y a **qu'un thread** qui ne doit jamais bloquer. Lis les fiches dans l'ordre, lance chaque exemple avec `node` ou `npx tsx`, et passe à TypeScript **dès que** tu sais écrire une fonction `async`. Les prérequis sont surtout du C que tu as déjà : pointeurs, `struct`, pointeurs de fonction, et `poll()`.

Liens : [`INDEX`](INDEX.md) · [`PHILOSOPHIE`](PHILOSOPHIE.md) · [`SYNTHESE`](SYNTHESE.md) · [`lexique/`](lexique/INDEX.md) · [`TYPESCRIPT`](TYPESCRIPT.md)

---

## 1. Ce que tu dois savoir avant

| Prérequis | Pourquoi | Où le revoir |
|---|---|---|
| **Le C** : variables, fonctions, `struct`, tableaux | La syntaxe de JS en descend directement (`if`, `for`, `while`, `{}`) | tes projets du cursus |
| **Pointeurs et références** | En JS, tout objet est manipulé **par référence**. Le piège de la copie, c'est un `memcpy` oublié. | [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) §2 |
| **Pointeurs de fonction** (`qsort`, `signal`) | Les *callbacks* sont la forme normale d'une fonction en JS | [`FONCTIONS`](FONCTIONS.md) §3 |
| **Classes C++98** : constructeur, héritage, `this` | Les classes JS en sont très proches, sans forme canonique | [`CLASSES_MODULES`](CLASSES_MODULES.md) |
| **Templates C++** | Les génériques TypeScript (`Array<T>`) suivent la même idée | [`TYPESCRIPT`](TYPESCRIPT.md) §4 |
| **`poll()` / `select()`** (minishell, webserv) | La boucle d'événements de JS est la même idée, intégrée au langage | [`ASYNCHRONE`](ASYNCHRONE.md) §2 |
| **Le shell et `make`** | `npm run …` remplace `make`, `package.json` remplace le `Makefile` | [`OUTILS`](OUTILS.md) |
| **JSON**, au moins de vue | C'est le format d'échange entre le front et le back | [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) §6 |

**Pas besoin** de connaître HTML, CSS ou le navigateur pour ce dossier : tout se lance avec `node`. Le DOM arrive avec [`react/`](react/INDEX.md).

---

## 2. Le modèle mental à installer

Avant la syntaxe, trois changements de réflexe ([`PHILOSOPHIE`](PHILOSOPHIE.md)) :

```text
  C / C++                                    JavaScript / TypeScript
  ─────────────────────────────────────      ─────────────────────────────────────────────
  la variable a un type                 →    la valeur a un type (TS vérifie AVANT, puis efface)
  malloc / free, new / delete           →    new seulement, le garbage collector libère
  read() bloque, on attend              →    on ne bloque jamais : Promise + await
  un binaire tourne sur la machine      →    un moteur (V8) dans un navigateur OU dans Node
```

Si une seule chose doit rester : **un `await` oublié ne plante pas, il te donne une `Promise` au lieu d'une valeur.** C'est le bug que tu feras le plus.

---

## 3. Comment s'y prendre

1. **Installe un terrain d'essai.** Un dossier jetable, `node fichier.js` et `npx tsx fichier.ts` ([`OUTILS`](OUTILS.md) §1). Chaque extrait des fiches qui porte `// → …` a été exécuté : retape-le et compare.
2. **Suis l'ordre de [`INDEX`](INDEX.md).** Il va du changement d'état d'esprit à l'asynchrone, puis aux types. Ne saute pas [`ASYNCHRONE`](ASYNCHRONE.md) : React et Adonis en dépendent à chaque ligne.
3. **Traduis du C vers JS.** Reprends une petite fonction de ta libft (`ft_split`, `ft_itoa`) et réécris-la en JS, puis avec `split` / `map`. Tu verras ce que le langage fait pour toi.
4. **Passe à TypeScript tôt.** Dès [`TYPESCRIPT`](TYPESCRIPT.md), écris tout en `.ts` avec `"strict": true`. Les erreurs du compilateur te montrent les pièges de JS avant l'exécution.
5. **Garde le [`lexique/`](lexique/INDEX.md) ouvert** pour retrouver un mot-clé, pas pour apprendre dans l'ordre.
6. **Arrête-toi à [`VERS_TRANSCENDANCE`](VERS_TRANSCENDANCE.md).** Le but n'est pas de maîtriser JS en entier, c'est d'en savoir assez pour lire React et Adonis. Le reste vient en pratiquant.

---

## 4. Les erreurs de débutant à éviter

- **Apprendre l'ancien JavaScript.** Un tutoriel avec `var`, `function(){}.bind(this)` ou `require` partout date d'avant 2015. Cherche `const`, les fonctions fléchées et `import` ([`lexique/LEGACY`](lexique/LEGACY.md)).
- **Mélanger navigateur et Node.** `document` n'existe pas dans Node, `fs` n'existe pas dans le navigateur ([`PHILOSOPHIE`](PHILOSOPHIE.md) §1).
- **Écrire du C en JS** : des boucles `for` avec index partout. `map` / `filter` / `find` disent l'intention ([`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) §4).
- **Croire que TypeScript protège à l'exécution.** Les types disparaissent : une donnée qui arrive du réseau doit être **validée** ([`TYPESCRIPT`](TYPESCRIPT.md) §6).
- **Se battre avec `this`.** Utilise des fonctions fléchées dans les callbacks, et passe à autre chose ([`FONCTIONS`](FONCTIONS.md) §5).

---

## 5. Tu es prêt pour React et Adonis si…

> [!question]- Que vaut `[1, 10, 2].sort()`, et pourquoi ?
> `[1, 10, 2]`. Sans comparateur, `sort` compare des **chaînes**. Écris `.sort((a, b) => a - b)`.

> [!question]- `const main = []; main.push(3);` : erreur ou pas ?
> Pas d'erreur. `const` interdit de **réaffecter** `main`, pas de modifier le tableau qu'elle désigne.

> [!question]- `const b = a;` avec `a` un objet. On modifie `b.pv`. Que devient `a.pv` ?
> Il change aussi : `b` et `a` désignent **le même** objet. Pour une copie : `{ ...a }`, superficielle (les sous-objets restent partagés).

> [!question]- Dans une fonction `async`, que contient `const r = fetch(url)` sans `await` ?
> Une `Promise`, pas la réponse. Il faut `await fetch(url)`.

> [!question]- Pourquoi un serveur Node qui fait une boucle de calcul de 5 secondes est-il un problème ?
> Un seul thread : pendant ces 5 secondes, **aucune** autre requête n'est traitée, pour aucun joueur.

> [!question]- Un client envoie `{ "attaque": "beaucoup" }` à une route typée `attaque: number`. TypeScript l'arrête-t-il ?
> Non. Les types sont effacés à la compilation. Seule une **validation à l'exécution** (Vine, côté Adonis) l'arrête.

> [!question]- Quelle est la différence entre `unknown` et `any` ?
> `any` désactive la vérification. `unknown` l'impose : il faut **prouver** le type (`typeof`, `instanceof`…) avant d'utiliser la valeur.

> [!question]- À quoi sert `package.json`, en une phrase de C ?
> C'est le `Makefile` du projet, plus la liste de ses bibliothèques : `npm install` les télécharge, `npm run dev` lance une commande.

Si une réponse t'échappe, la fiche entre parenthèses de la [`SYNTHESE`](SYNTHESE.md) te dit où relire.
