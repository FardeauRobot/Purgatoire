# 🧠 Philosophie : ce qui change quand on vient du C++

> **TL;DR.** JavaScript est **dynamique** (les types sont portés par les valeurs, pas par les variables), **géré en mémoire** (un *garbage collector* libère pour toi), et **mono-thread avec une boucle d'événements** (on n'attend jamais en bloquant : on dit « rappelle-moi quand c'est prêt »). TypeScript ajoute des types **vérifiés avant l'exécution**, puis les efface.

Liens : [`INDEX`](INDEX.md) · [`OUTILS`](OUTILS.md) · [`ASYNCHRONE`](ASYNCHRONE.md) · [`TYPESCRIPT`](TYPESCRIPT.md)

---

## 1. Où tourne le code ?

En C++, ton code devient un binaire qui tourne sur la machine. En JavaScript, il y a toujours un **moteur** entre ton code et la machine, embarqué dans un **environnement** :

```
                 ┌──────────── ton code JS / TS ────────────┐
                 ▼                                          ▼
   ┌──────────────────────────┐            ┌──────────────────────────┐
   │       NAVIGATEUR         │            │          NODE.JS          │
   │  moteur V8 + le DOM      │            │  moteur V8 + fichiers,    │
   │  (document, window,      │            │  réseau, process…         │
   │   fetch, WebSocket…)     │            │                           │
   │  → React (frontend)      │            │  → AdonisJS (backend)     │
   └──────────────────────────┘            └──────────────────────────┘
```

Même langage, **API différentes** : `document.getElementById` n'existe pas dans Node, `fs.readFile` n'existe pas dans le navigateur. Quand un exemple trouvé sur internet ne marche pas, demande-toi d'abord *« est-ce du code navigateur ou du code Node ? »*.

---

## 2. Typage dynamique

```js
let x = 42;        // x contient un number
x = "salut";       // maintenant une string — aucune erreur
x = { hp: 10 };    // maintenant un objet
```

- **Ligne 1** : pas de type déclaré. La *valeur* `42` a un type, la *variable* non.
- **Lignes 2-3** : en C++ ce serait refusé à la compilation. Ici, c'est légal.

**Lien C++ :** imagine que chaque variable soit un `void *` qui sait ce qu'il pointe. Pratique pour aller vite, dangereux dès que le projet grossit. C'est **exactement** le problème que TypeScript résout (voir [`TYPESCRIPT`](TYPESCRIPT.md)).

---

## 3. Pas de `delete` : le garbage collector

```js
function creerCarte() {
    const carte = { nom: "Aura", attaque: 7 };   // alloué sur le tas
    return carte;
}
let c = creerCarte();
c = null;   // plus personne ne référence l'objet → il sera libéré un jour
```

Pas de destructeur, pas de forme canonique, pas de fuite au sens valgrind. **Mais** tu peux encore « fuir » : un objet gardé dans un tableau global, un `setInterval` jamais arrêté, un écouteur d'événement jamais retiré. En React, c'est le rôle des fonctions de nettoyage (voir [`VERS_TRANSCENDANCE`](VERS_TRANSCENDANCE.md)).

---

## 4. Un seul thread, jamais bloquer

En C, `read()` bloque le thread jusqu'à ce que les données arrivent. En JS, **il n'y a qu'un thread** : s'il bloque, toute la page gèle (ou tout le serveur arrête de répondre à **tous** les joueurs).

Donc les opérations lentes (réseau, base de données, timers) sont **asynchrones** :

```js
console.log("1. je lance la requête");
fetch("/api/cards").then(() => console.log("3. la réponse est arrivée"));
console.log("2. je continue sans attendre");
```

**Lien :** c'est l'idée de `poll()`/`select()` de webserv ou de minishell, mais intégrée au langage. Tout le détail dans [`ASYNCHRONE`](ASYNCHRONE.md).

---

## 5. TypeScript : un vérificateur, pas un nouveau langage

```
  fichier.ts ──► tsc (vérifie les types) ──► fichier.js ──► exécuté par V8
                   ▲                            ▲
           erreurs ici                  plus aucun type ici
```

TypeScript = JavaScript + annotations de type. Le compilateur vérifie, puis **supprime** les types. Conséquence capitale : **aucune vérification n'a lieu pendant l'exécution**. Si le client envoie `"attaque": "beaucoup"` au lieu d'un nombre, TypeScript ne le verra jamais — c'est à ton code de le valider.

---

## ⚠️ À retenir

- Navigateur ≠ Node : même langage, outils différents.
- Le typage dynamique est un piège à grande échelle → on travaille en TypeScript.
- Ne jamais bloquer le thread : tout ce qui est lent est asynchrone.
- Les types TS sont un filet de sécurité **pour toi**, pas contre l'utilisateur.
