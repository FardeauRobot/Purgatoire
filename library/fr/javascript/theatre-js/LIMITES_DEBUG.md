# 8. Limites, pannes et alternatives

> **TL;DR.** Theatre.js 0.7.2 marche avec R3F 9 et React 19 (vérifié), mais le projet public **n'est plus maintenu** depuis mai 2024 : la suite (1.0) se fait dans un dépôt privé. Utilise-le pour ce qu'il fait de mieux (des cinématiques réglées à la souris), garde-le **isolé** derrière quelques fichiers, et sache par quoi le remplacer. La plupart des pannes ont l'une de ces quatre causes : `ready` pas attendu, studio importé sans être initialisé, `state.json` pas branché ou périmé, nom d'objet changé.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) · [`PRODUCTION`](PRODUCTION.md) · [`../react-three-fiber/PERFORMANCE_DEBUG`](../react-three-fiber/PERFORMANCE_DEBUG.md)

---

## 1. L'état du projet, en clair

| Fait | Source |
|---|---|
| Dernière publication npm : `0.7.2`, le 19 mai 2024, pour les trois paquets. | `npm view` |
| Dernier commit du dépôt public : août 2024. | API GitHub |
| Le README annonce que la 1.0 est développée dans un dépôt **privé**. | README du dépôt |
| `@theatre/r3f` déclare encore R3F 8. Il faut un `override` pour R3F 9. | [`INSTALLATION`](INSTALLATION.md) |

Ce que ça implique :
- **Aucun correctif** à attendre. Un bug trouvé aujourd'hui restera.
- Chaque **montée de version** de React, de R3F ou de three.js est un risque. Tes tests sont ta seule garantie : rejoue au moins la démo et un `vite preview` après chaque mise à jour.
- Le format de `state.json` (`definitionVersion: "0.4.0"`) est figé. Un futur Theatre 1.0 public saura-t-il le relire ? Personne ne peut le dire aujourd'hui.

🔗 **Lien C.** C'est comme dépendre d'une bibliothèque C dont l'auteur a disparu. Tant que tu ne changes rien autour, elle marche. Le jour où ça casse, tu dois pouvoir la remplacer sans tout réécrire.

### Garder Theatre isolé

Le moyen de limiter le risque : que Theatre ne soit importé que par **peu de fichiers**.

```text
src/theatre.ts          ← getProject + state.json   (seul fichier qui connaît le projet)
src/theatreStudio.ts    ← studio, dev seulement
src/scenes/Intro.tsx    ← e.*, SheetProvider        (les cinématiques)
src/cartes/Carte.tsx    ← un seul sheet.object      (l'effet « carte jouée »)
```

Le reste du jeu (le store, le réseau, la main de cartes, `damp`) ne sait **pas** que Theatre existe. Le jour où il faut le retirer, la liste des fichiers à toucher tient en quatre lignes.

---

## 2. Symptôme → cause

| Symptôme | Cause probable | Voir |
|---|---|---|
| `npm install` échoue avec `ERESOLVE … @react-three/fiber@"^8.13.6"` | Le `peerDependencies` de `@theatre/r3f`. | [`INSTALLATION`](INSTALLATION.md), §2 |
| Tout est figé **en production**, alors que tout marchait en dev. Console : `You seem to have imported '@theatre/studio' but haven't initialized it`. | Studio importé statiquement, `initialize()` derrière `if (DEV)`. `project.ready` ne se résout jamais. | [`INSTALLATION`](INSTALLATION.md), §3 |
| `studio.initialize is not a function` sous `npm run dev`, alors que le build marche. | `import studio from "@theatre/studio"` utilisé tel quel : en dev, Vite 8 livre l'objet module CommonJS. | [`INSTALLATION`](INSTALLATION.md), §3 |
| `e.initialize is not a function` | `(await import("@theatre/studio")).default` : interop CommonJS. | [`INSTALLATION`](INSTALLATION.md), §3 |
| Rien n'est animé en production, sans erreur. | `state.json` pas passé à `getProject`, ou pas exporté. | [`PRODUCTION`](PRODUCTION.md), §1 |
| L'animation en production est **différente** de celle du studio. | `state.json` plus vieux que ton brouillon : réexporte. | [`PRODUCTION`](PRODUCTION.md), §6 |
| `play()` renvoie `true`, mais rien n'a bougé. | `play()` appelé avant `project.ready`. | [`SEQUENCES`](SEQUENCES.md), §1 |
| Une animation s'arrête au milieu, sa promesse renvoie `false`. | Un autre `play()` (ou un `pause()`) sur la même feuille. Pour plusieurs lectures simultanées : des instances. | [`INSTANCES_CARTES`](INSTANCES_CARTES.md) |
| Un objet repart de ses valeurs par défaut après un refactor. | Sa clé, ou l'id de sa feuille, a été renommée : ses keyframes sont orphelines dans le JSON. | [`CONCEPTS`](CONCEPTS.md), §2 |
| `Argument config.state in Theatre.getProject(...) is empty` | `state: {}` sans studio. | [`PRODUCTION`](PRODUCTION.md), §5 |
| `object is not a function` en affichant une durée ou une position. | Un *pointer* utilisé comme une valeur : passe par `val()`. | [`SEQUENCES`](SEQUENCES.md), §3 |
| La caméra animée n'est pas celle qui filme, avec un avertissement `Possibly incorrect use of <e.perspectiveCamera />`. | `e.perspectiveCamera` au lieu du `PerspectiveCamera` de `@theatre/r3f` avec `makeDefault`. | [`EDITABLE`](EDITABLE.md), §2 |
| Le modèle glTF n'apparaît pas dans l'éditeur de snapshot. | Snapshot pris avant la fin du chargement : `<RefreshSnapshot />`. | [`EDITABLE`](EDITABLE.md), §6 |
| Ton animation a « disparu » du studio. | Autre origine (`127.0.0.1` vs `localhost`), navigation privée, données du site effacées, autre `persistenceKey`. | [`STUDIO`](STUDIO.md), §5 |
| Erreur CORS vers `updates.theatrejs.com` | Le studio cherche des mises à jour. Sans conséquence. | [`INSTALLATION`](INSTALLATION.md), §4 |

### Trois questions à poser dans la console

```ts
import { val } from "@theatre/core";
import { project } from "./theatre";

const seq = project.sheet("Intro").sequence;
console.log("prêt ?", project.isReady);
console.log("durée", val(seq.pointer.length), "position", seq.position);
console.log("en lecture ?", val(seq.pointer.playing));
```

- `isReady` à `false` en production : le studio est dans le bundle sans être initialisé.
- `durée` à `10` alors que ta séquence fait 2 s : c'est la durée par défaut, donc **aucun** état n'a été chargé pour cette feuille (mauvais `state`, mauvais id de projet ou de feuille).
- `en lecture ?` à `false` juste après `play()` : la lecture a été interrompue, ou n'a jamais démarré.

---

## 3. Quand ne pas utiliser Theatre

- **L'animation dépend de l'état** (survol, suivi, une main qui se réorganise) : `damp` dans `useFrame`. Theatre ne sait pas viser une cible qui bouge.
- **Le mouvement vient d'un modèle animé** (personnage qui marche) : les clips glTF faits dans Blender ([`../react-three-fiber/SQUELETTE_ANIMATIONS`](../react-three-fiber/SQUELETTE_ANIMATIONS.md)).
- **Une seule transition simple** (fondu de 0.3 s) : trois lignes de `damp` coûtent moins qu'un projet, une feuille et un export.
- **Un réglage, pas une animation** (couleur d'ambiance, intensité) : Leva ([`../react-three-fiber/LEVA`](../react-three-fiber/LEVA.md)). Theatre peut le faire, mais oblige à exporter un JSON pour une seule valeur.

Theatre gagne quand **plusieurs props bougent ensemble** selon un timing qu'on règle à l'œil : intro, victoire, coup spécial, caméra de présentation.

---

## 4. Par quoi le remplacer

| Outil | Ce qu'il apporte | Ce qu'il n'a pas |
|---|---|---|
| **Keyframes écrites à la main** + interpolation dans `useFrame` | Zéro dépendance. Tu sais déjà le faire ([`CONCEPTS`](CONCEPTS.md), §4). | Pas d'éditeur : chaque réglage, c'est modifier le code et recharger. |
| **Blender** + clips glTF | Un vrai logiciel d'animation, maintenu. Le clip voyage dans le modèle. | On anime dans Blender, pas dans la scène du jeu. Moins pratique pour une caméra qui dépend de la scène. |
| **GSAP** (timeline en code) | Des timelines puissantes, très utilisées sur le web. | La timeline s'écrit en code, pas à la souris. Vérifie sa licence avant de l'utiliser. |

Non testé ici : ce tableau reste une orientation, à vérifier avant de choisir.

Si un jour Theatre casse, l'isolation du §1 permet de remplacer les quelques fichiers concernés. Garde le `state.json` : c'est un JSON lisible, dont tu sais maintenant extraire les keyframes ([`PRODUCTION`](PRODUCTION.md), §2).

---

## 🧪 À essayer

1. Pour chaque ligne du tableau du §2, reproduis volontairement la panne dans une copie du labo, et lis le message exact. Tu le reconnaîtras le jour où il arrivera pour de vrai.
2. Ouvre `node_modules/@theatre/r3f/package.json` et lis son champ `peerDependencies`. Fais de même pour `@react-three/drei` : quelle version de R3F attend-il ?
3. Écris une fonction de 15 lignes qui prend les `keyframes` d'une piste de `state.json` et un temps `t`, et renvoie la valeur interpolée **linéairement**. Compare avec ce que donne Theatre : la différence, c'est la courbe.
