# 0. Installation : Theatre.js à côté de R3F 9

> **TL;DR.** Trois paquets : `@theatre/core` (le moteur, livré en production), `@theatre/studio` (l'éditeur visuel, **en dev seulement**), `@theatre/r3f` (le pont avec React Three Fiber). Le projet n'a rien publié depuis **mai 2024** : `@theatre/r3f` 0.7.2 réclame encore R3F 8, et `npm install` refuse (`ERESOLVE`). La solution propre est un bloc **`overrides`** dans `package.json`, pas `--legacy-peer-deps`. Le studio se charge par un `import()` derrière `import.meta.env.DEV`, dans un fichier à part.

Liens : [`INDEX`](INDEX.md) · [`CONCEPTS`](CONCEPTS.md) · [`../react-three-fiber/INSTALLATION`](../react-three-fiber/INSTALLATION.md) · [`LIMITES_DEBUG`](LIMITES_DEBUG.md)

🧪 Labo : aucun pour l'instant. Pars d'une copie de `~/Labo/TestThreeFiber/` ou d'un projet `react-ts` neuf.

---

## 1. Les trois paquets

| Paquet | Rôle | En production ? |
|---|---|---|
| `@theatre/core` | Le moteur : projets, feuilles, objets, séquences, lecture des keyframes. | ✅ Oui, il joue les animations. |
| `@theatre/studio` | L'interface d'édition : arbre des objets, panneau de propriétés, timeline, courbes. | ❌ Non, c'est ton outil de travail. |
| `@theatre/r3f` | `<SheetProvider>`, les balises `e.mesh`, `e.group`… et l'extension studio (éditeur de *snapshot* 3D). | ✅ La partie `editable`. L'extension, non. |

🔗 **Lien C.** `core` est la bibliothèque que tu linkes, `studio` est le débogueur que tu lances à côté. Tu ne livres pas `gdb` avec ton binaire. Ici c'est pareil, sauf qu'un oubli ne fait pas planter : il affiche l'éditeur à tous tes joueurs et alourdit le bundle.

---

## 2. Le conflit de versions, et comment le régler

```sh
npm view @theatre/r3f peerDependencies
# → '@react-three/fiber': '^8.13.6', react: '>=17.0.2', three: '>=0.155.0', …
```

Ton labo est en `@react-three/fiber` 9.8.2. Le `^8` exclut la 9, donc :

```text
npm error code ERESOLVE
npm error Could not resolve dependency:
npm error peer @react-three/fiber@"^8.13.6" from @theatre/r3f@0.7.2
```

Trois façons d'en sortir :

| Méthode | Effet | Verdict |
|---|---|---|
| `npm install --legacy-peer-deps` | Ignore **toutes** les `peerDependencies` du projet, pour cette commande. | ❌ Cache aussi les vrais conflits (React ↔ R3F, voir [`../react-three-fiber/INSTALLATION`](../react-three-fiber/INSTALLATION.md)). Et il faut s'en souvenir à chaque `npm install`. |
| `.npmrc` avec `legacy-peer-deps=true` | Pareil, mais permanent et invisible. | ❌ Pire : tout le monde l'hérite sans le savoir. |
| `overrides` dans `package.json` | Dit à npm : « pour `@theatre/r3f`, utilise **ma** version de R3F ». Une seule exception, écrite noir sur blanc. | ✅ À utiliser. |

Le bloc à ajouter au `package.json` (au même niveau que `dependencies`) :

```json
{
  "dependencies": {
    "@react-three/drei": "^10.7.9",
    "@react-three/fiber": "^9.8.2",
    "@theatre/core": "^0.7.2",
    "@theatre/r3f": "^0.7.2",
    "@theatre/studio": "^0.7.2",
    "react": "^19.3.1",
    "react-dom": "^19.3.1",
    "three": "^0.187.0"
  },
  "overrides": {
    "@theatre/r3f": {
      "@react-three/fiber": "$@react-three/fiber"
    }
  }
}
```

- `"$@react-three/fiber"` : le `$` veut dire « la version que **je** déclare dans mes `dependencies` ». Si tu montes R3F, l'override suit tout seul.
- Ensuite, un simple `npm install` passe. `npm ls @react-three/fiber` doit montrer **une seule** copie (les autres en `deduped`).

> ⚠️ **Un override est une promesse que tu fais à la place de l'auteur.** npm ne vérifie plus rien pour ce couple. Ici, la promesse a été testée (voir [`INDEX`](INDEX.md), « Ce qui a été vérifié ») : `editable` n'utilise que l'API publique de R3F (`useThree`, `useFrame`), et l'extension studio embarque **sa propre** copie de R3F 8 pour son éditeur 3D. Mais si une future version de R3F casse quelque chose, c'est à toi de le voir.

🔗 **Lien C++.** C'est un `reinterpret_cast` de dépendances : tu affirmes au compilateur que tu sais mieux que lui. Ça compile, et c'est à toi de prouver que c'est vrai.

---

## 3. Charger le studio en dev seulement

Le studio pèse lourd et affiche son interface par-dessus la page. Il ne doit **jamais** partir en production. Le motif : un fichier dédié, chargé par un `import()` dynamique.

`src/theatreStudio.ts` :

```ts
import studioImport from "@theatre/studio";
import extension from "@theatre/r3f/dist/extension";

type Studio = typeof studioImport;

const studio: Studio =
    "initialize" in studioImport ? studioImport : (studioImport as unknown as { default: Studio }).default;

studio.initialize();
studio.extend(extension);
```

`src/main.tsx` :

```tsx
import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import { Canvas } from "@react-three/fiber";

function App() {
    return (
        <Canvas>
            <mesh>
                <boxGeometry />
                <meshNormalMaterial />
            </mesh>
        </Canvas>
    );
}

async function start() {
    if (import.meta.env.DEV) await import("./theatreStudio");
    createRoot(document.getElementById("root")!).render(
        <StrictMode>
            <App />
        </StrictMode>,
    );
}

start();
```

Ligne par ligne :
- `const studio = "initialize" in studioImport ? … : ….default` : 🚨 sans cette ligne, `npm run dev` plante avec `studio.initialize is not a function` (observé avec Vite 8.3.8). Le paquet est publié en CommonJS, et depuis Vite 8 le serveur de dev livre l'objet `module.exports` entier, dont le studio n'est que le champ `default`. Le build de production, lui, donne directement le studio. La ligne prend le studio là où il est, dans les deux cas (vérifié en dev **et** en build). Les fiches suivantes appellent `studio.…` dans ce fichier, sur cette constante.
- `import.meta.env.DEV` : une constante que Vite remplace par `true` sous `npm run dev` et par `false` sous `npm run build`. En production, le bundler voit `if (false) …`, supprime la branche, et `theatreStudio.ts` n'est même plus empaqueté.
- `await import("./theatreStudio")` : un `import` **dynamique**, qui renvoie une promesse (voir [`../ASYNCHRONE`](../ASYNCHRONE.md)). Le fichier s'exécute une fois, donc `initialize()` est appelé une fois.
- `studio.extend(extension)` ajoute à la barre d'outils le bouton **caméra** : l'éditeur de *snapshot*, une vue 3D où tu sélectionnes et déplaces tes objets à la souris (voir [`STUDIO`](STUDIO.md)).
- On rend l'application **après** le chargement du studio, pour qu'il soit prêt quand les premiers objets se déclarent.

### Pourquoi un fichier à part, et pas `import studio from …` dans `main.tsx`

Deux pièges, observés en testant ces fiches :

1. **Importer sans initialiser bloque tout.** Si le module `@theatre/studio` est chargé mais que `studio.initialize()` n'est jamais appelé, `project.ready` ne se résout **jamais** : rien ne s'anime, et la console affiche `You seem to have imported '@theatre/studio' but haven't initialized it`. Or c'est exactement ce que produit le réflexe d'importer le studio en haut de `main.tsx` (avec la ligne `const studio` ci-dessus), puis d'écrire `if (import.meta.env.DEV) studio.initialize()` : en production, l'import statique reste dans le bundle, l'appel disparaît, et **toutes tes animations sont figées chez les joueurs** alors qu'elles marchaient en dev (observé : studio présent sous `npm run dev`, avertissement et scène figée sous `vite preview`).
2. **`(await import("@theatre/studio")).default` ne marche pas.** Le paquet est publié en CommonJS. Importé dynamiquement, son `default` est un objet module, pas le studio : `e.initialize is not a function`. Le fichier `theatreStudio.ts`, avec son import statique et sa ligne `const studio`, évite le problème.

---

## 4. Lancer, et ce que tu dois voir

```sh
npm run dev
```

- En haut à gauche : deux boutons. Le premier ouvre l'**outline** (l'arbre : projet → feuille → objets), le second (appareil photo) ouvre l'éditeur de snapshot.
- En haut à droite : le panneau des **propriétés** de l'objet sélectionné.
- Tant qu'aucun projet n'est déclaré dans ton code, l'outline ne montre que `Studio` (le projet interne de l'extension). C'est normal : la fiche suivante crée le tien.

Dans la console, une erreur CORS vers `updates.theatrejs.com` : le studio cherche des mises à jour sur un serveur qui ne répond plus. Elle est **sans conséquence**.

```sh
npm run build && npx vite preview
```

En production, plus d'interface. Si tu la vois encore, relis la section 3.

---

## 🧪 À essayer

1. Dans une copie du labo, ajoute les trois paquets **sans** l'override et lance `npm install`. Lis l'erreur `ERESOLVE` en entier : elle nomme le paquet fautif et la version qu'il attend.
2. Ajoute l'override, réinstalle, puis lance `npm ls @react-three/fiber`. Combien de copies ?
3. Lance `npm run build`, puis `du -sh dist/assets` et `grep -c "theatrejs-studio" dist/assets/*.js`. Recommence en retirant le `if (import.meta.env.DEV)` : compare la taille et le résultat du `grep`.
4. Remplace la ligne `const studio = …` par `const studio = studioImport;`, lance `npm run dev`, et lis l'erreur. Puis lance `npm run build && npx vite preview` : pourquoi ça marche là ? Remets la ligne.
5. Commente la ligne `studio.initialize()` dans `theatreStudio.ts`, garde l'import, et regarde la console. Remets-la.
