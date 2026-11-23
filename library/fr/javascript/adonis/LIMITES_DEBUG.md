# 10. Diagnostiquer, et ce que ce dossier ne couvre pas

> **TL;DR.** Quatre outils suffisent pour la plupart des pannes du backend :
> - `node ace list:routes` : la route existe-t-elle, sous quel nom, avec quels middlewares ?
> - `node ace migration:status` : quelles migrations ont tourné **sur cette base** ?
> - l'écouteur `db:query` : quelles requêtes SQL partent vraiment ?
> - le `request_id` : retrouver dans les logs l'erreur qu'un utilisateur a vue.
>
> Le §2 associe chaque message d'erreur rencontré dans ces fiches à sa cause. Le §3 liste ce qui n'a **pas** été étudié.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) · [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) · [`TESTS_JAPA`](TESTS_JAPA.md) · [`DEPLOIEMENT`](DEPLOIEMENT.md) · [`../react/PERFORMANCE_DEBUG`](../react/PERFORMANCE_DEBUG.md) · [doc AdonisJS](https://docs.adonisjs.com)

🧪 Labo : aucun. Les sorties citées viennent de la copie du backend utilisée pour toutes les fiches.

---

## 1. Les outils

### Les routes

```sh
node ace list:routes
```

Affiche chaque route avec sa méthode, son motif, son **nom** (celui de Tuyau) et ses middlewares ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §1). Premier réflexe devant un 404 ou un 401 inattendu : la route a-t-elle le motif et les middlewares que tu crois ?

### L'état des migrations

```sh
node ace migration:status
```

Observé (début) :

```
│  Name                                                                │  Status     │  Batch  │
│  database/migrations/1761885935168_create_users_table                │  completed  │  1      │
│  database/migrations/1768620764696_create_access_tokens_table        │  completed  │  1      │
```

- `completed` : jouée sur **cette** base, celle du `.env`. Une colonne absente alors que la migration est `completed` veut dire que la migration a été **modifiée après coup** ([`LUCID`](LUCID.md), §1).
- `pending` : pas encore jouée. `node ace migration:run`.

### Les requêtes SQL

`config/database.ts` contient `debug: app.inDev`, mais ça ne suffit pas : rien ne s'affiche dans le terminal (observé). Il faut aussi **écouter** l'événement. Trois lignes en haut de `start/routes.ts`, qui est préchargé (liste `preloads` d'`adonisrc.ts`). Un fichier `start/events.ts` à part serait plus propre, à condition de l'**ajouter** à `preloads` (non testé) :

```ts
import db from '@adonisjs/lucid/services/db'
import emitter from '@adonisjs/core/services/emitter'

emitter.on('db:query', db.prettyPrint)
```

Observé dans le terminal de `node ace serve`, après un `GET /health` ([`DEPLOIEMENT`](DEPLOIEMENT.md), §6) :

```
"pg" (952 μs) SELECT 1 []
```

C'est l'outil qui rend visible le **N+1** ([`LUCID`](LUCID.md), §6) : une boucle de `load` affiche une ligne par tour.

🔗 **Lien C.** C'est un `printf` placé dans la fonction qui parle à la base, plutôt qu'à chaque appel. Une seule ligne, et tout passe dedans.

### Le `request_id`

Chaque erreur journalisée porte un `request_id`. Avec le gestionnaire de [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §5, le client reçoit le même identifiant. En prod :

```sh
docker compose logs backend | grep 2e518bf3
```

(Commande non lancée : rien n'a été déployé. Le lien entre réponse et log a été observé en local.)

### Les tests sur un autre port

`PORT=3398 node ace test` quand `node ace serve` tourne déjà ([`TESTS_JAPA`](TESTS_JAPA.md), §2).

---

## 2. Message → cause

Tous les messages ci-dessous ont été observés pendant l'écriture des fiches.

| Message ou symptôme | Cause | Fiche |
|---|---|---|
| `Error: spawn psql ENOENT` à la migration | `database/schema/pg-schema.sql` vide commité, `psql` absent du Mac | [`INSTALLATION`](INSTALLATION.md) §5, [`TESTS_JAPA`](TESTS_JAPA.md) §4 |
| `Property 'Rooms' does not exist on type '{ AccessTokens: … }'` | `.adonisjs/` périmé : relancer `node ace codegen` | [`INSTALLATION`](INSTALLATION.md) §5 |
| `password authentication failed` | Le port 5432 mène à un **autre** Postgres | [`INSTALLATION`](INSTALLATION.md) §4 |
| `TS1206 Decorators are not valid here` dans le front | `experimentalDecorators` retiré du tsconfig du front | [`TUYAU`](TUYAU.md) §1 |
| `TS2339 Property 'token' does not exist on type 'ExtractResponse<…>'` | Les `return` du contrôleur n'ont pas les mêmes clés | [`TUYAU`](TUYAU.md) §3, [`AUTH`](AUTH.md) §6 |
| `relation "game_user" does not exist` | `pivotTable` oublié dans `@manyToMany` | [`LUCID`](LUCID.md) §5 |
| `"games" is not defined as a relationship on "User" model` | Relation absente du modèle | [`LUCID`](LUCID.md) §5, [`TESTS_JAPA`](TESTS_JAPA.md) §5 |
| `Relation "Game.users" expects "gameId" to exist on "User" model` | Mauvaise forme de relation : `hasMany` là où les deux tables passent par un pivot (`manyToMany`) | [`LUCID`](LUCID.md) §4–5 |
| 500 `invalid input syntax for type integer: "abc"` | Paramètre de route sans `matchers.number()` | [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) §4 |
| 400 `Invalid user credentials` sur un corps **vide** | Login sans validateur (`request.only`) | [`VINE`](VINE.md) §2 |
| 401 `Unauthorized access. (meow)` avec un jeton valide | Jeton `2fa:verify` sur une route `requireFullAccess` | [`AUTH`](AUTH.md) §4 |
| `Request failed with status code 400: POST …` côté front | `err.message` de Tuyau : le vrai message est dans `err.response` | [`TUYAU`](TUYAU.md) §4 |
| `connect_error` « jeton refusé » | Jeton absent, inconnu, ou temporaire, à la connexion Socket.IO | [`SOCKET_IO`](SOCKET_IO.md) §3 |
| `disconnect` « io server disconnect », sans reconnexion | Normal après un logout : le serveur a coupé la socket | [`SOCKET_IO`](SOCKET_IO.md) §4 |
| `Join a room first` | `message:send` avant `room:join` | [`SOCKET_IO`](SOCKET_IO.md) §5 |
| `listen EADDRINUSE … :3399` puis `Tests (0)` | `node ace serve` occupe déjà le port | [`TESTS_JAPA`](TESTS_JAPA.md) §2 |
| `Expected table 'games' to have 0 rows, but found 3` | Tests lancés sur la base de **dev** | [`TESTS_JAPA`](TESTS_JAPA.md) §3 |
| 500 `Cannot find package 'otplib'` en prod seulement | Dépendance déclarée à la racine du monorepo | [`DEPLOIEMENT`](DEPLOIEMENT.md) §2 |
| `502` via Caddy | Back lancé sur `HOST=localhost` dans son conteneur | [`DEPLOIEMENT`](DEPLOIEMENT.md) §1 |
| Avatar cassé, `200 text/html` | Chemin non relayé par Caddy : c'est l'`index.html` | [`DEPLOIEMENT`](DEPLOIEMENT.md) §4 |
| `avatarUrl` en `http://0.0.0.0:…` | `APP_URL` construit à partir de `HOST` | [`DEPLOIEMENT`](DEPLOIEMENT.md) §5 |
| `Makefile:1: *** missing separator.  Stop.` | Marqueurs de conflit git commités | [`DEPLOIEMENT`](DEPLOIEMENT.md) §7 |

Quand un message n'est pas dans ce tableau :
1. lis l'**erreur complète** dans le terminal de `node ace serve`, pas seulement la réponse HTTP (en dev, le corps de la réponse contient aussi la stack, [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §4) ;
2. regarde la **première** ligne de la stack qui pointe dans `app/` ou `start/`, pas dans `node_modules/` ;
3. si c'est une erreur SQL, active `db:query` (§1) et relance.

---

## 3. Ce que ce dossier ne couvre pas

Étudié **nulle part** dans ces fiches, donc à vérifier dans la doc avant de t'en servir :
- **Limitation de débit** (`@adonisjs/limiter`, non installé) : ni pour le login, ni pour la 2FA, ni pour Socket.IO ([`AUTH`](AUTH.md) §5, [`SOCKET_IO`](SOCKET_IO.md) §6).
- **Plusieurs instances du backend** : Socket.IO garde ses salons en mémoire, il faudrait un *adapter* ([`SOCKET_IO`](SOCKET_IO.md) §6).
- **Le guard `web`** (sessions par cookie) : déclaré par l'équipe, jamais utilisé ([`AUTH`](AUTH.md) §1).
- **Les e-mails** (mot de passe oublié, vérification d'adresse) et **OAuth** (« se connecter avec 42 »).
- **Les seeders** et les **factories** de Lucid, pour remplir une base de test.
- **Le build Docker réel** des images de l'équipe : [`DEPLOIEMENT`](DEPLOIEMENT.md) en a reproduit les étapes une par une, sans lancer `docker build`.

Une remarque sur les ressources en ligne : beaucoup de tutoriels et de réponses visent **AdonisJS 5**, avec des imports `@ioc:Adonis/Core/…` qui n'existent plus. Le dépôt utilise la version 7 ([`INSTALLATION`](INSTALLATION.md) §2). Vérifie la version annoncée en haut de la page de doc avant de recopier un exemple.

---

## 🧪 À essayer

1. Ajoute l'écouteur `db:query`, ouvre `/api/v1/account/profile` avec un jeton valide, et compte les requêtes. Observé : **trois**, le jeton (`SELECT … auth_access_tokens`), sa date d'usage (`UPDATE … last_used_at`), puis l'utilisateur. Pourtant `silent_auth` **et** `auth()` vérifient tous les deux le jeton ([`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §1). Pourquoi pas six ?
2. Modifie une migration déjà jouée, puis compare la sortie de `migration:status` et le contenu de la table dans pgAdmin.
3. Prends trois lignes du tableau du §2 au hasard, reproduis l'erreur, puis corrige-la sans relire la fiche.
4. Cherche « AdonisJS middleware » sur un moteur de recherche, et classe les cinq premiers résultats par version d'Adonis.
