# 8. Parler à l'API : `fetch`, le jeton, les erreurs, l'URL de base

> **TL;DR.** `fetch` ne lève **pas** d'erreur sur un `404` ou un `401` : il faut tester `r.ok`. On écrit donc **une** fonction `requete()` qui ajoute le jeton, lit le JSON et transforme un statut d'erreur en exception typée ; les composants et les loaders n'appellent qu'elle. L'URL de base doit être **la même origine** que la page en production (Caddy relaie `/api/v1/*`), et en dev le **proxy** de Vite évite CORS. Attention : une variable `VITE_` absente au build ne plante pas, elle tombe sur la valeur par défaut, qui part en production.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) (§5, `VITE_`) · [`EFFETS`](EFFETS.md) (§3) · [`ROUTER`](ROUTER.md) (loaders) · [`TEMPS_REEL`](TEMPS_REEL.md) · [`../ASYNCHRONE`](../ASYNCHRONE.md)

🧪 Labo : aucun. Les observations viennent d'un faux serveur Node (une route `/api/v1/me` protégée par un jeton) derrière le proxy de Vite.

---

## 1. Ce que `fetch` fait, et ne fait pas

Observé :

| Appel | Résultat |
|---|---|
| `/api/v1/me` sans jeton | `r.status === 401`, **pas d'exception** |
| `/api/v1/inconnu` | `r.ok === false`, **pas d'exception** |
| Une autre origine sans en-têtes CORS | exception `TypeError: Failed to fetch` |

- `fetch` ne rejette sa promesse que si la **requête n'a pas pu aboutir** (réseau coupé, CORS refusé, `abort()`). Un serveur qui répond « non », c'est une réponse réussie.
- `r.ok` : vrai pour un statut entre 200 et 299.
- `r.json()` est **aussi** asynchrone : la réponse arrive en deux temps, les en-têtes puis le corps.

🔗 **Lien C.** C'est la différence entre `write()` qui renvoie `-1` (l'appel a échoué) et un serveur qui te répond « refusé » : l'appel système a parfaitement réussi, c'est le contenu qui dit non. Il faut lire le contenu.

---

## 2. Une seule fonction pour toutes les requêtes

```ts
export class ErreurApi extends Error {
    readonly statut: number;
    readonly corps: unknown;

    constructor(statut: number, corps: unknown) {
        super(`HTTP ${statut}`);
        this.statut = statut;
        this.corps = corps;
    }
}

const BASE = import.meta.env.VITE_API_URL ?? "";

export async function requete<T>(chemin: string, init: RequestInit = {}): Promise<T> {
    const headers = new Headers(init.headers);
    headers.set("Accept", "application/json");
    if (init.body && !headers.has("Content-Type")) headers.set("Content-Type", "application/json");

    const jeton = localStorage.getItem("auth_token");
    if (jeton) headers.set("Authorization", `Bearer ${jeton}`);

    const r = await fetch(`${BASE}${chemin}`, { ...init, headers });
    const corps: unknown = r.headers.get("Content-Type")?.includes("json") ? await r.json() : await r.text();

    if (r.status === 401) localStorage.removeItem("auth_token");
    if (!r.ok) throw new ErreurApi(r.status, corps);
    return corps as T;
}
```

Ligne par ligne :
- `ErreurApi` : une classe d'erreur qui garde le **statut**. Un composant peut alors distinguer « pas connecté » (401), « interdit » (403), « introuvable » (404) et « bug serveur » (500).
- `BASE` : vide par défaut, donc `requete("/api/v1/me")` appelle **la même origine** que la page. Voir le §4.
- `new Headers(init.headers)` : on part des en-têtes éventuels de l'appelant, puis on complète.
- `Bearer ${jeton}` : le schéma d'authentification attendu par les jetons opaques d'Adonis. C'est ce que fait `api.ts` de l'équipe dans son `beforeRequest`.
- On lit le JSON **seulement** si la réponse en est : une erreur de Caddy ou une page `404` HTML ferait planter `r.json()`.
- `r.status === 401` : le jeton est expiré ou révoqué. On l'efface pour que l'interface repasse en « non connecté ».
- `corps as T` : TypeScript ne peut pas vérifier ce que renvoie le serveur, c'est une **promesse** qu'on lui fait. Le client Tuyau de l'équipe (§5) supprime ce `as` en tirant les types du code du serveur.

Utilisation, dans un loader de [`ROUTER`](ROUTER.md) :

```ts
import { redirect } from "react-router";
import { ErreurApi, requete } from "./requete";

type Moi = { id: number; username: string };

export async function chargerProfil(): Promise<Moi> {
    try {
        return await requete<Moi>("/api/v1/me");
    } catch (e) {
        if (e instanceof ErreurApi && e.statut === 401) throw redirect("/connexion");
        throw e;
    }
}
```

- Le `401` devient une redirection. Toute autre erreur est relancée et finit dans l'`ErrorBoundary` de la route.
- `return await` dans un `try` : sans le `await`, la promesse serait renvoyée telle quelle, et son échec échapperait au `catch`.

---

## 3. CORS, et le proxy de Vite en dev

En dev, la page est servie par Vite (`localhost:5173`) et l'API par Adonis (`localhost:3333`). Ce sont deux **origines** différentes (le port compte). Observé, sans configuration CORS côté serveur :

```text
Access to fetch at 'http://localhost:4399/api/v1/me' from origin 'http://localhost:4301'
has been blocked by CORS policy: No 'Access-Control-Allow-Origin' header is present …
```

🚨 **Observé aussi : la requête est arrivée au serveur.** Son journal montre le `GET /api/v1/me`. CORS n'empêche pas la requête de partir, il empêche **le code de la page** de lire la réponse. Ce n'est pas une protection du serveur : un `curl` l'ignore complètement. La protection, c'est l'authentification.

Deux solutions :
1. Configurer CORS sur Adonis pour accepter l'origine de Vite (voir le dossier Adonis).
2. Faire passer l'API **par Vite**, qui la relaie : pour le navigateur, tout vient de la même origine.

```ts
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import tailwindcss from "@tailwindcss/vite";

export default defineConfig({
    plugins: [react(), tailwindcss()],
    server: {
        proxy: {
            "/api": "http://localhost:3333",
            "/socket.io": { target: "ws://localhost:3333", ws: true },
        },
    },
});
```

- `"/api": "http://localhost:3333"` : toute requête dont le chemin commence par `/api` est relayée à Adonis. Observé : `fetch("/api/v1/me")` arrive au serveur, sans erreur CORS, et le jeton passe.
- `ws: true` : relaie aussi les WebSockets (Socket.IO, voir [`TEMPS_REEL`](TEMPS_REEL.md)).
- C'est **la même disposition** qu'en production, où Caddy relaie `/api/v1/*` et `/socket.io/*`. Le code du front n'a plus de différence entre dev et prod : `BASE` reste vide dans les deux cas.

🔗 **Lien C.** Le proxy, c'est un `dup2` : le navigateur croit écrire vers sa propre origine, et Vite redirige le flux vers le bon descripteur.

---

## 4. Le piège de l'URL de base

`api.ts` de l'équipe :

```text
baseUrl: import.meta.env.VITE_API_URL || 'http://localhost:3333',
```

Et dans le dépôt :
- `.dockerignore` exclut `**/.env` et `**/.env.*` (sauf `.env.example`) ;
- le `Dockerfile` de Caddy lance `npm run build` dans `apps/frontend` sans définir `VITE_API_URL` ;
- Vite ne lit pas `.env.example`.

Conséquence, déduite de ces fichiers et du mécanisme vérifié dans [`INSTALLATION`](INSTALLATION.md), §5 : au build de l'image, `VITE_API_URL` vaut `undefined`, le `||` choisit `'http://localhost:3333'`, et cette chaîne est **écrite dans le bundle de production**. Le navigateur d'un joueur appellera alors le port 3333 **de sa propre machine**. Ça « marche » sur le poste du développeur qui fait tourner Adonis en local, et nulle part ailleurs.

Depuis, vérifié par un `vite build` du front de l'équipe sans `.env` ([`../adonis/DEPLOIEMENT`](../adonis/DEPLOIEMENT.md), §3) : tel quel, la chaîne n'est **pas** dans le bundle, parce qu'aucun fichier n'importe encore `api.ts` et que Vite l'élimine. Dès qu'un écran importe `api`, `localhost:3333` apparaît dans `dist/assets/*.js`. Le bug est donc latent : il arrivera avec le premier écran branché sur l'API.

`socket.ts`, lui, fait déjà le bon choix : `import.meta.env.PROD ? undefined : …`, c'est-à-dire la même origine en production.

Pistes de correction, à discuter avec l'équipe :
- Même origine partout : `BASE` vide, et le proxy du §3 en dev. Pour Tuyau, `baseUrl: window.location.origin`.
- Ou passer la valeur au build : `ARG VITE_API_URL` dans le Dockerfile, puis `build.args` dans `compose.yaml`.
- Dans tous les cas, **pas de valeur par défaut** silencieuse vers `localhost` : préfère une erreur claire au démarrage si la variable manque.

🔗 **Lien C.** C'est un `#ifndef PORT / #define PORT 3333 / #endif` dans un header : si personne ne passe `-DPORT`, la valeur de secours compile sans un mot, et le binaire livré pointe vers ta machine de dev.

---

## 5. Tuyau : le client typé de l'équipe

`api.ts` n'utilise pas `fetch` directement mais `createTuyau(...)` (`@tuyau/core` 1.3.0), avec un `registry` **généré à partir du code d'Adonis** (`node ace codegen`, lancé dans le Dockerfile).

- Le client connaît chaque route du serveur, ses paramètres et le type de sa réponse. Si un contrôleur change ce qu'il renvoie, le front ne compile plus : l'erreur arrive au typecheck, pas en production.
- `hooks.beforeRequest` y joue le rôle de l'ajout du jeton du §2.
- Le détail (génération, contenu de `.adonisjs/`, appel d'une route) est dans le dossier Adonis, avec le reste du serveur.

La fonction `requete()` du §2 reste utile pour **comprendre** ce que Tuyau fait pour toi, et pour un appel vers une API externe qu'il ne connaît pas.

---

## 🧪 À essayer

1. Appelle `fetch("/une/route/inexistante")` dans la console du navigateur et regarde la promesse : est-elle rejetée ? Que vaut `ok` ?
2. Sans proxy, appelle l'API depuis la page de Vite et regarde l'onglet **Réseau** : la requête est-elle partie ? Le serveur l'a-t-il reçue ?
3. Ajoute le `proxy` du §3, remplace l'URL complète par `/api/v1/…` et recommence.
4. Lance `npm run build` **sans** `.env`, puis cherche `localhost:3333` dans `dist/assets/*.js` (avec le `||` de l'équipe). Refais-le avec `BASE = import.meta.env.VITE_API_URL ?? ""`.
5. Dans `chargerProfil`, retire le `await` de `return await requete(...)` et fais répondre l'API en 401. La redirection a-t-elle lieu ? Où part l'erreur ?
