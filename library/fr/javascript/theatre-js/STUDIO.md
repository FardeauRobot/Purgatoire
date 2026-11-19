# 4. Le studio : poser des keyframes à la souris

> **TL;DR.** Sélectionne un objet dans l'**outline**, fais un clic droit sur une prop dans le panneau de droite, puis **Sequence**. La prop passe dans la **timeline** en bas. Place la tête de lecture, change la valeur : une keyframe apparaît. Clique sur le segment entre deux keyframes pour choisir la **courbe** (`linear`, `In Out`, `Back Out`…, ou `Hold` pour un saut). **Espace** lance et arrête la lecture. Tout est sauvegardé dans le `localStorage` **de ce navigateur** tant que tu n'as pas cliqué sur **Export … to JSON**.

Liens : [`INDEX`](INDEX.md) · [`CONCEPTS`](CONCEPTS.md) · [`EDITABLE`](EDITABLE.md) · [`SEQUENCES`](SEQUENCES.md) · [`PRODUCTION`](PRODUCTION.md)

🧪 Labo : aucun pour l'instant. Pars de la scène de [`EDITABLE`](EDITABLE.md), §2.

> ℹ️ Les libellés anglais de cette fiche (« Sequence », « Make static »…) sont recopiés du code du studio 0.7.2. L'enchaînement des clics, lui, n'a été vérifié qu'en partie (voir [`INDEX`](INDEX.md), « Ce qui a été vérifié »).

---

## 1. Les quatre zones

```text
┌──────────┬──────────────────────────────────────────┬──────────────────┐
│ OUTLINE  │                                          │   PROPRIÉTÉS     │
│ Aura     │          ta scène (ou l'éditeur          │   Carte          │
│ └ Intro  │          de snapshot, bouton caméra)     │   position  x 0  │
│   ├ Caméra                                          │             y 0.5│
│   ├ Soleil                                          │   rotation  …    │
│   └ Carte ◄── sélection                             │                  │
├──────────┴──────────────────────────────────────────┴──────────────────┤
│ TIMELINE (séquence de la feuille)        0s    1s    2s    3s          │
│   Carte › position › y                   ◆───────────◆                 │
│                                                ▲ tête de lecture       │
└─────────────────────────────────────────────────────────────────────────┘
```

| Zone | Rôle |
|---|---|
| **Outline** (bouton en haut à gauche) | L'arbre projet → feuille → objets. Cliquer un objet le sélectionne. |
| **Propriétés** (à droite) | Les props de l'objet sélectionné. Tu glisses un nombre, tu ouvres le sélecteur de couleur. |
| **Timeline** (en bas, apparaît dès qu'une prop est séquencée) | Les pistes de keyframes de la feuille, et la tête de lecture. |
| **Éditeur de snapshot** (bouton appareil photo, avec l'extension R3F) | Une copie de ta scène où tu sélectionnes et déplaces les objets avec des flèches (*gizmo*), comme dans Blender. |

---

## 2. Statique ou séquencée : le clic droit

Une prop est d'abord **statique** : une seule valeur, quel que soit le temps. Ce que tu règles est un *override* : il remplace la valeur par défaut du code.

Clic droit sur le **nom** d'une prop :

| Menu | Effet |
|---|---|
| **Sequence** | La prop passe dans la timeline. Sa valeur actuelle devient la première keyframe. |
| **Sequence all** (sur un groupe : `position`) | Pareil pour `x`, `y` et `z` d'un coup. |
| **Make static** / **Make all static** | L'inverse : supprime les keyframes et garde une seule valeur. |
| **Reset to default** | Oublie ton réglage, revient à la valeur du code. |

🔗 **Lien C.** Une prop statique, c'est une variable. Une prop séquencée, c'est une fonction du temps `f(t)`, représentée par un tableau de points. « Sequence » transforme l'une en l'autre.

---

## 3. Poser des keyframes

1. **Sequence** sur `position › y` de `Carte`. Une keyframe apparaît à 0 s.
2. Clique dans la règle de la timeline à **1 s** : la tête de lecture y saute.
3. Dans le panneau des propriétés, change `y` de `0.5` à `2`. Une deuxième keyframe se crée **à la position de la tête de lecture**.
4. Ramène la tête à 0 s, puis appuie sur **Espace** : la carte monte en une seconde.

La règle à retenir : **sur une prop séquencée, toute modification de valeur écrit une keyframe à l'instant de la tête de lecture**. S'il y en a déjà une à cet instant, elle est modifiée.

Gestes utiles dans la timeline :
- Glisser une keyframe la déplace dans le temps.
- Clic droit sur une keyframe : **Copy**, **Delete**. Sur un emplacement vide : **Paste Keyframes**.
- Avec plusieurs keyframes sélectionnées : **Copy (selection)**, **Delete (selection)**.
- **Sequence length** : la durée totale de la séquence. Une keyframe au-delà de la fin ne sera jamais atteinte par `play()`.
- **Focus range** (clic droit sur la règle du haut) : limite la lecture de l'Espace à un intervalle, pour retravailler un passage sans tout revoir. Sans effet sur `play()` dans ton code.

---

## 4. Les courbes

Clique sur le **segment** qui relie deux keyframes : une liste de courbes apparaît.

| Courbe | Allure | Pour |
|---|---|---|
| `linear` | vitesse constante | une rotation continue, un défilement |
| `In Out` | démarre doucement, freine à l'arrivée (courbe par défaut, d'où le 0.318 de [`CONCEPTS`](CONCEPTS.md), §4) | la plupart des déplacements |
| `Quad Out`, `Cubic Out`… | démarre vite, freine | un objet qui arrive et se pose |
| `Back Out` | dépasse un peu la cible puis revient | un rebond, une carte qui « claque » sur la table |
| `Hold` | aucune transition : la valeur saute à la keyframe suivante | changer de couleur d'un coup, faire apparaître un objet |

Chaque courbe est une **Bézier cubique** à quatre nombres, comme `transition-timing-function: cubic-bezier(…)` en CSS. `Back In Out` a des valeurs négatives et supérieures à 1 : c'est ce qui la fait sortir de l'intervalle.

---

## 5. Où ton travail est enregistré

```text
studio ──(chaque modification)──► localStorage["theatrejs:0.4…"]   ← ce navigateur, cette origine
                                        │
                     clic « Export Aura to JSON »
                                        ▼
                         Aura.theatre-project-state.json   ← dans tes Téléchargements
                                        │
                         tu le déplaces dans src/state.json et tu le commites
```

- Rien ne part sur un serveur : pas de compte, pas de cloud.
- `localhost:5173` et `127.0.0.1:5173` sont deux **origines** différentes, donc deux `localStorage` différents. Ouvre la même URL d'une session à l'autre, sinon ton travail « disparaît ».
- Vider les données du site, ou passer en navigation privée, efface tout ce qui n'a pas été exporté.
- Le bouton d'export est dans le panneau des propriétés quand tu sélectionnes le **projet** (`Aura`) dans l'outline.

> 🚨 **Exporte souvent.** Le `localStorage` n'est pas une sauvegarde : c'est un brouillon. Le seul fichier que Git voit, c'est `src/state.json`.

La suite (relire ce fichier en production, et le conflit entre navigateur et disque) est dans [`PRODUCTION`](PRODUCTION.md).

---

## 6. Piloter le studio depuis le code

Quelques appels utiles, tous dans `theatreStudio.ts` ou derrière `import.meta.env.DEV` :

```ts
import studio from "@theatre/studio";

studio.ui.hide();
studio.ui.restore();

studio.onSelectionChange((selection) => {
    console.log(selection.length, "objet(s) sélectionné(s)");
});
```

- `studio.ui.hide()` / `restore()` : cacher l'interface sans décharger le studio, pour regarder ta scène telle que les joueurs la verront. Les animations continuent de lire le `localStorage`.
- `studio.onSelectionChange` : réagir à la sélection. Par exemple, afficher un repère sur l'objet choisi.
- `studio.createContentOfSaveFile("Aura")` renvoie le même JSON que le bouton d'export, si tu veux l'écrire autrement (voir [`PRODUCTION`](PRODUCTION.md)).

---

## 🧪 À essayer

1. Fais monter `Carte` de `y = 0.5` à `y = 2` en une seconde, puis redescendre à 2 s. Joue-le à l'Espace.
2. Mets `Back Out` sur le premier segment et `Hold` sur le second. Décris ce que tu vois avant de lancer, puis vérifie.
3. Séquence aussi `rotation › y` : un demi-tour (`3.14`) pendant la montée. Les deux pistes partagent la même tête de lecture.
4. Ouvre la même page en `127.0.0.1` au lieu de `localhost`. Où est passée ton animation ?
5. Exporte, ouvre le JSON, et retrouve `Intro`, `Carte` et le chemin `["position","y"]` (voir [`CONCEPTS`](CONCEPTS.md), §2).
