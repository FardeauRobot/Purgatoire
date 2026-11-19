# 🚀 Vers Transcendance : où chaque notion réapparaît

> **TL;DR.** **React** (frontend) = des **fonctions** qui reçoivent des données (*props*) et renvoient de l'interface ; l'état change via `useState`, les effets de bord (WebSocket, timers) vivent dans `useEffect`. **AdonisJS** (backend) = des **routes** qui appellent des **méthodes de contrôleurs** `async`, qui **valident** l'entrée puis parlent à la base via l'ORM. Entre les deux : du **JSON** sur HTTPS, et des messages typés en temps réel.

Liens : [`INDEX`](INDEX.md) · [`ASYNCHRONE`](ASYNCHRONE.md) · [`TYPESCRIPT`](TYPESCRIPT.md) · [`React Three Fiber`](react-three-fiber/INDEX.md) · [sujet ft_transcendence](../projects/LEVEL6/ft_transcendence-subject.md)

> ℹ️ Cette fiche reste volontairement **conceptuelle** : la stack de l'équipe (AdonisJS + React) n'est pas encore figée. Pour les détails d'API, la doc officielle de chaque outil fait foi.

---

## 1. Vue d'ensemble

```
  NAVIGATEUR (React + TS)                         SERVEUR (AdonisJS + TS)          BDD
 ┌───────────────────────┐   HTTPS : fetch + JSON  ┌──────────────────────┐   ┌──────────┐
 │ composants (fonctions)│ ──────────────────────► │ routes → contrôleurs │──►│ ORM      │
 │ useState / useEffect  │ ◄────────────────────── │ validation (VineJS)  │◄──│ (Lucid)  │
 └───────────┬───────────┘                         └──────────┬───────────┘   └──────────┘
             │        temps réel : messages JSON typés         │
             └────────────── WebSocket (wss://) ───────────────┘
```

---

## 2. React : les notions JS dont il dépend

```tsx
import { useState } from "react";

interface CarteProps {
    nom: string;
    attaque: number;
    onJouer: (nom: string) => void;
}

function Carte({ nom, attaque, onJouer }: CarteProps) {
    return (
        <button onClick={() => onJouer(nom)}>
            {nom} — {attaque} ⚔️
        </button>
    );
}

function Main() {
    const [cartes, setCartes] = useState<string[]>(["Aura", "Flamme"]);

    const jouer = (nom: string) => {
        setCartes(cartes.filter((c) => c !== nom));   // nouveau tableau, jamais cartes.splice(...)
    };

    return (
        <div>
            {cartes.map((c) => (
                <Carte key={c} nom={c} attaque={7} onJouer={jouer} />
            ))}
        </div>
    );
}
```

| Ce que tu vois | Notion | Fiche |
|---|---|---|
| `function Carte({ nom, attaque, onJouer })` | déstructuration des paramètres | [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) §5 |
| `const [cartes, setCartes] = useState(...)` | déstructuration de tableau | [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) §5 |
| `onClick={() => onJouer(nom)}` | callback + fonction fléchée + closure | [`FONCTIONS`](FONCTIONS.md) |
| `cartes.filter(...)` / `cartes.map(...)` | transformer sans modifier | [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) §4 |
| `interface CarteProps` | décrire les données | [`TYPESCRIPT`](TYPESCRIPT.md) §2 |

**La règle d'or de React :** ne **jamais** modifier l'état en place (`cartes.push(...)`, `joueur.pv = 3`). Crée toujours une nouvelle valeur (`[...cartes, x]`, `{ ...joueur, pv: 3 }`) et passe-la au setter. Sinon React ne voit pas le changement et n'actualise pas l'écran — c'est le piège de la *référence* de [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) §2.

### Les effets et leur nettoyage

```tsx
useEffect(() => {
    const ws = new WebSocket("wss://exemple/partie");
    ws.onmessage = (e) => { /* JSON.parse + validation + setState */ };
    return () => ws.close();          // nettoyage : appelé quand le composant disparaît
}, []);
```

La fonction retournée est l'équivalent d'un **destructeur** : sans elle, chaque retour sur la page ouvrirait une connexion de plus (une vraie fuite).

---

## 3. AdonisJS : les notions JS dont il dépend

```ts
// start/routes.ts
import router from "@adonisjs/core/services/router";
const CartesController = () => import("#controllers/cartes_controller");

router.get("/cartes", [CartesController, "index"]);
router.post("/cartes", [CartesController, "store"]);
```

```ts
// app/controllers/cartes_controller.ts
import type { HttpContext } from "@adonisjs/core/http";
import Carte from "#models/carte";
import { creerCarteValidator } from "#validators/carte";

export default class CartesController {
    async index() {
        return Carte.all();                                   // renvoyé en JSON
    }

    async store({ request }: HttpContext) {
        const donnees = await request.validateUsing(creerCarteValidator);
        return Carte.create(donnees);
    }
}
```

| Ce que tu vois | Notion | Fiche |
|---|---|---|
| `export default class …` / `import …` | classes et modules | [`CLASSES_MODULES`](CLASSES_MODULES.md) |
| `async store(...)` / `await request.validateUsing(...)` | asynchrone | [`ASYNCHRONE`](ASYNCHRONE.md) |
| `{ request }: HttpContext` | déstructuration + type | [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md), [`TYPESCRIPT`](TYPESCRIPT.md) |
| `validateUsing(...)` | validation à l'exécution | [`TYPESCRIPT`](TYPESCRIPT.md) §6 |
| `#controllers/...` | alias de chemin défini dans `package.json` | [`OUTILS`](OUTILS.md) |

---

## 4. Le temps réel

Le module « Fonctionnalités temps réel (WebSockets…) » du sujet colle parfaitement à un jeu 1 contre 1. Points à connaître :

- AdonisJS v6 n'a **pas** de WebSocket intégré. Son paquet officiel **Transmit** fait du *Server-Sent Events* (serveur → client uniquement), insuffisant pour un jeu où les deux côtés parlent. Il faudra brancher une bibliothèque WebSocket (par ex. `socket.io` ou `ws`) sur le serveur HTTP d'Adonis — à vérifier dans la doc au moment de choisir.
- **Le serveur fait autorité** : le client envoie une *intention* (« je joue la carte 12 »), le serveur vérifie que c'est légal, met à jour la partie et diffuse le nouvel état aux deux joueurs. Ne fais jamais confiance à un calcul fait côté client.
- **Types partagés** : décris les messages une seule fois (union avec un champ `type`, [`TYPESCRIPT`](TYPESCRIPT.md) §3) dans un fichier commun au front et au back, et **valide** chaque message reçu côté serveur.

---

## 5. Les exigences du sujet, vues côté code

| Exigence | Ce que ça implique en JS/TS |
|---|---|
| Saisies validées frontend **et** backend | Les types TS ne suffisent pas : VineJS / zod côté serveur ([`TYPESCRIPT`](TYPESCRIPT.md) §6). |
| HTTPS pour toute connexion au backend | `https://` et `wss://` (WebSocket chiffré) côté client, jamais `http://` ni `ws://`. |
| Pas de secrets dans le dépôt | `.env` ignoré, `.env.example` versionné ([`OUTILS`](OUTILS.md) §4). |
| Pas d'erreurs ni d'avertissements dans la console du navigateur | Pas de promesse rompue non rattrapée, `key` sur chaque élément de liste React, effets nettoyés. |
| Lancement en une seule commande (Docker) | `npm ci` + scripts du `package.json` dans les Dockerfiles. |

---

## 🧭 Et après ?

1. Lis la doc officielle *getting started* de **React** (react.dev) puis d'**AdonisJS** (docs.adonisjs.com) : avec ces fiches en tête, tu comprendras chaque ligne.
2. Fais un mini-projet jetable : une route Adonis qui renvoie une liste de cartes, et un composant React qui l'affiche avec `fetch`.
3. Reviens ici dès qu'une syntaxe te bloque.
