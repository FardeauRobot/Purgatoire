# 🛠️ Outils : Node, npm et `package.json`

> **TL;DR.** **Node** exécute du JavaScript hors du navigateur. **npm** télécharge des bibliothèques dans `node_modules/` et lance des scripts. **`package.json`** décrit le projet (dépendances + commandes) : c'est ton `Makefile`. Pour le TypeScript, `npx tsx fichier.ts` exécute directement.

Liens : [`INDEX`](INDEX.md) · [`PHILOSOPHIE`](PHILOSOPHIE.md) · [`TYPESCRIPT`](TYPESCRIPT.md)

---

## 1. Lancer du code

```sh
node --version          # vérifier que Node est installé
node hello.js           # exécuter un fichier JS
node                    # REPL interactif (Ctrl+D pour sortir) — idéal pour tester une ligne
npx tsx hello.ts        # exécuter un fichier TS sans configuration
```

Dans le navigateur : ouvre les DevTools (`Cmd+Option+I`), onglet **Console**. Tu peux y taper du JS directement — très utile pour inspecter ton frontend.

---

## 2. `package.json` : le Makefile du web

```json
{
  "name": "transcendance-back",
  "type": "module",
  "scripts": {
    "dev": "node ace serve --hmr",
    "build": "node ace build",
    "test": "node ace test"
  },
  "dependencies": {
    "@adonisjs/core": "^6.0.0"
  },
  "devDependencies": {
    "typescript": "^5.0.0"
  }
}
```

| Champ | Rôle | Équivalent connu |
|---|---|---|
| `scripts` | Commandes du projet, lancées avec `npm run dev` | Les règles d'un `Makefile` |
| `dependencies` | Bibliothèques nécessaires à l'exécution | `libft` dans ton projet |
| `devDependencies` | Outils de développement seulement (compilateur, tests…) | `norminette`, `valgrind` |
| `"type": "module"` | Active la syntaxe moderne `import`/`export` | — |

Les scripts exacts dépendent de ton projet : regarde **toujours** le `package.json` avant de demander « comment on lance ? ».

---

## 3. Les commandes npm du quotidien

```sh
npm install                 # installe tout ce que liste package.json (à faire après un git clone / pull)
npm install zod             # ajoute une dépendance
npm install -D vitest       # ajoute une devDependency
npm run dev                 # lance le script "dev"
npx <outil>                 # exécute un outil sans l'installer globalement
```

---

## 4. Les fichiers à connaître

| Fichier | Versionné ? | Pourquoi |
|---|---|---|
| `package.json` | ✅ oui | Déclare le projet. |
| `package-lock.json` | ✅ **oui** | Fige les versions exactes : toute l'équipe (et Docker) installe la même chose. |
| `node_modules/` | ❌ **jamais** | Des centaines de Mo, régénérable avec `npm install`. Dans le `.gitignore`. |
| `tsconfig.json` | ✅ oui | Options du compilateur TypeScript (`"strict": true` !). |
| `.env` | ❌ **jamais** | Secrets (mots de passe BDD, clés API). Le sujet l'exige : un `.env.example` versionné, le vrai `.env` ignoré. |

---

## ⚠️ À retenir

- Après chaque `git pull` qui touche `package.json` : `npm install`.
- Ne commit jamais `node_modules/` ni `.env`.
- En Docker, on copie `package.json` + `package-lock.json` et on lance `npm ci` (installation stricte depuis le lock).
