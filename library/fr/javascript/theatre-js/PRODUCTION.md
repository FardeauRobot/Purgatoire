# 6. En production : `state.json`, et le conflit navigateur ↔ disque

> **TL;DR.** Exporte depuis le studio, range le fichier en `src/state.json`, commite-le, et passe-le à `getProject("Aura", { state })`. Sans ce `state`, la production joue les **valeurs par défaut** : aucune animation. En dev, le studio a **deux** sources : le `localStorage` (ton brouillon) et `state.json` (le disque). Il garde le brouillon s'il descend de la version du disque, et sinon te demande de choisir (« Browser state is not based on disk state »). Ne passe jamais `{}` comme `state` : sans studio, ça lève une erreur.

Liens : [`INDEX`](INDEX.md) · [`STUDIO`](STUDIO.md) · [`INSTALLATION`](INSTALLATION.md) · [`CONCEPTS`](CONCEPTS.md) · [`LIMITES_DEBUG`](LIMITES_DEBUG.md)

🧪 Labo : aucun pour l'instant.

---

## 1. Brancher `state.json`

`src/theatre.ts`, version finale :

```ts
import { getProject } from "@theatre/core";
import state from "./state.json";

export const project = getProject("Aura", { state });
```

- `import state from "./state.json"` : Vite sait importer un JSON comme un objet. TypeScript l'accepte grâce à `resolveJsonModule`, déjà activé dans le gabarit `react-ts`. Vérifié : ce fichier passe `tsc --strict`.
- Le JSON est **empaqueté** dans le bundle : pas de requête réseau, pas de chargement à attendre.
- L'`id` (`"Aura"`) doit être celui de l'export. Le nom du fichier exporté le rappelle : `Aura.theatre-project-state.json`.

Le circuit complet :

```text
studio ── Export Aura to JSON ──► ~/Téléchargements/Aura.theatre-project-state.json
                                         │  mv … src/state.json
                                         ▼
                                  git add src/state.json
                                         │
                                         ▼
               npm run build ──► getProject("Aura", { state }) ──► les joueurs voient l'animation
```

🔗 **Lien C.** C'est ta map `.ber` de `so_long` : si tu oublies de la livrer avec le binaire, le programme tourne, mais sur un monde vide.

---

## 2. Ce que contient le fichier

Un export minimal, avec une seule prop animée :

```json
{
  "sheetsById": {
    "Intro": {
      "staticOverrides": { "byObject": {} },
      "sequence": {
        "type": "PositionalSequence",
        "length": 2,
        "subUnitsPerUnit": 30,
        "tracksByObject": {
          "Carte": {
            "trackIdByPropPath": { "[\"position\",\"y\"]": "a1" },
            "trackData": {
              "a1": {
                "type": "BasicKeyframedTrack",
                "keyframes": [
                  { "id": "k0", "position": 0, "value": 0.5, "connectedRight": true, "type": "bezier", "handles": [0.5, 1, 0.5, 0] },
                  { "id": "k1", "position": 1, "value": 2, "connectedRight": true, "type": "bezier", "handles": [0.5, 1, 0.5, 0] }
                ]
              }
            }
          }
        }
      }
    }
  },
  "definitionVersion": "0.4.0",
  "revisionHistory": ["Xb3kQ9"]
}
```

| Clé | Contenu |
|---|---|
| `sheetsById.Intro` | une feuille, sous **son id** |
| `staticOverrides.byObject` | les props **statiques** que tu as réglées dans le studio |
| `sequence.length` | la durée, en secondes |
| `subUnitsPerUnit: 30` | la grille d'aimantation de la timeline (30 crans par seconde) |
| `tracksByObject.Carte` | les pistes de l'objet `Carte`, sous **sa clé** |
| `trackIdByPropPath` | le **chemin** de la prop (`["position","y"]`) vers l'id de sa piste |
| `keyframes` | `position` (temps), `value`, et `handles` : la courbe vers la keyframe suivante |
| `revisionHistory` | les identifiants des versions successives, la plus récente en premier (50 au plus). Chaque export en ajoute une. |

Ce JSON a été écrit à la main pour tester ces fiches, et le core l'a joué correctement (1.5 à mi-course). Tu n'as pas à l'écrire toi-même, mais savoir le **lire** sert à deux choses : retrouver pourquoi un objet renommé ne s'anime plus ([`CONCEPTS`](CONCEPTS.md), §2), et relire un diff Git de `state.json`.

---

## 3. En dev : deux sources, une règle

Avec le studio chargé, `getProject("Aura", { state })` reçoit le disque **et** trouve un brouillon dans le `localStorage`. Le core décide ainsi (lu dans son code) :

```text
brouillon dans le navigateur ?
├── non ──► on prend le disque (ou rien, s'il n'y a pas de state)
└── oui ──► pas de state disque ? ──► on prend le brouillon
            sinon : la version du disque figure-t-elle dans l'historique du brouillon ?
            ├── oui ──► on prend le brouillon          (il « descend » du disque)
            └── non ──► boîte de dialogue :
                        « Browser state is not based on disk state. »
                        [ Use browser's state ]   [ Use disk state ]
```

Ce que ça donne en pratique :

| Situation | Ce que fait le studio |
|---|---|
| Tu exportes, puis tu continues d'éditer. | Garde ton brouillon, sans rien demander. Normal : il descend de l'export. |
| Un coéquipier commite un nouveau `state.json`, tu fais `git pull`. | Boîte de dialogue : ton brouillon ne connaît pas sa version. |
| Tu reviens à un **ancien** `state.json` (`git checkout`). | 🚨 Garde **ton brouillon**, sans rien demander : l'ancienne version est dans son historique. Pour voir vraiment l'ancien fichier, vide le `localStorage` (§4). |

- **Use disk state** : jette ton brouillon et repart du fichier. Ce qui n'était pas exporté est perdu.
- **Use browser's state** : garde ton brouillon, et note la version du disque dans son historique : la question ne reviendra pas pour ce fichier. Le prochain export écrasera le travail du coéquipier, à toi de fusionner à la main.

🔗 **Lien Git.** C'est un *fast-forward* : si ta branche contient le dernier commit distant, pas de conflit. Sinon, il faut choisir. La différence, c'est qu'ici il n'y a pas de fusion automatique : on garde l'un **ou** l'autre.

---

## 4. Repartir de zéro, ou isoler un essai

Le brouillon est rangé sous une clé du `localStorage`, par défaut `theatrejs:0.4`. Deux options de `studio.initialize` :

```ts
import studio from "@theatre/studio";
import extension from "@theatre/r3f/dist/extension";

studio.initialize({ persistenceKey: "aura-essai-camera" });
studio.extend(extension);
```

- `persistenceKey` : une autre clé, donc un **autre** brouillon. Pratique pour un essai que tu ne veux pas mélanger à ton travail principal. Reviens à la clé d'origine, et ton brouillon principal est là.
- `usePersistentStorage: false` : aucun brouillon. Chaque rechargement repart de `state.json`. Utile pour **vérifier** ce que verront les joueurs, sans avoir à builder.

Pour effacer le brouillon à la main : DevTools › *Application* › *Local Storage* › ton origine, et supprime les clés qui commencent par `theatrejs`.

---

## 5. L'erreur du `state` vide

```ts
import { getProject } from "@theatre/core";

export const project = getProject("Aura", { state: {} });
```

Sans le studio, ce code lève (observé) :

```text
Argument config.state in Theatre.getProject("Aura", config) is empty. […]
since @theatre/studio is not loaded, the state of project "Aura" will be empty.
```

Le cas arrive quand on prépare un `state.json` « vide pour l'instant » contenant `{}`. Soit tu n'importes rien et tu ne passes pas de `state` tant que tu n'as rien exporté, soit tu exportes d'abord un projet vierge depuis le studio.

---

## 6. Vérifier un build

```sh
npm run build
grep -c "theatrejs-studio" dist/assets/*.js
npx vite preview
```

- Le `grep` doit répondre `0` pour chaque fichier : le studio n'est pas dans le bundle (voir [`INSTALLATION`](INSTALLATION.md), §3).
- Dans `vite preview`, l'animation doit jouer **exactement** comme dans le studio. Si ce n'est pas le cas, ton `state.json` est plus vieux que ton brouillon : réexporte.

---

## 🧪 À essayer

1. Exporte, branche `state.json`, et lance `vite preview`. Retrouves-tu ton animation ?
2. Modifie une keyframe dans le studio **sans** exporter, puis relance `vite preview`. Qu'est-ce qui a changé ? Pourquoi ?
3. Simule un coéquipier : ouvre la page dans une fenêtre de navigation privée (brouillon vide), modifie l'animation, exporte, remplace `src/state.json`. Reviens dans ta fenêtre normale et recharge : quelle boîte de dialogue ?
4. Passe `usePersistentStorage: false` et recharge plusieurs fois après avoir édité. Que deviennent tes modifications ?
5. Ouvre ton `state.json` dans un diff Git après avoir déplacé une seule keyframe. Combien de lignes changent ?
