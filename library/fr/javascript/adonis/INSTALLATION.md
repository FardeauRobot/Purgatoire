# 0. Installer et lancer le backend AdonisJS

> **TL;DR.** Le backend est un **workspace npm** (`apps/backend`) d'un monorepo piloté par `turbo`. Pour le lancer :
> - un `npm ci` **à la racine** ;
> - un `.env` dans `apps/backend` (copié de `.env.example`, avec une `APP_KEY`) ;
> - Postgres via `compose.dev.yaml` ;
> - puis `node ace migration:run` et `node ace serve --hmr`.
>
> Deux dossiers sont **générés** et ne s'écrivent jamais à la main :
> - `.adonisjs/`, régénéré par `node ace codegen` : contrôleurs, registre Tuyau. Il n'est pas commité ;
> - `database/schema.ts`, régénéré par `migration:run`. Il est commité.
>
> Sur un Mac sans `psql`, la première migration échoue sur `spawn psql ENOENT`. Le §5 explique pourquoi.

Liens : [`INDEX`](INDEX.md) · [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) · [`LUCID`](LUCID.md) · [`DEPLOIEMENT`](DEPLOIEMENT.md) · [`../react/INSTALLATION`](../react/INSTALLATION.md) · [doc AdonisJS, installation](https://docs.adonisjs.com/guides/getting-started/installation)

🧪 Labo : aucun. Tout a été rejoué sur une **copie** du dépôt dans un dossier temporaire, avec son propre Postgres sur le port 5499. Le dépôt et la base de l'équipe n'ont pas été touchés.

---

## 1. La forme du dépôt

```
Transcendence/
├── package.json          ← workspaces: ["apps/*"], scripts turbo
├── package-lock.json     ← UN seul lockfile pour tout le monorepo
├── turbo.json
├── compose.dev.yaml      ← Postgres (+ pgAdmin) pour le dev
├── compose.yaml          ← la prod : caddy + backend + db
├── docker_config/        ← Dockerfiles, entrypoint, Caddyfile
└── apps/
    ├── backend/          ← AdonisJS 7
    ├── frontend/         ← React + Vite
    └── types/            ← ws_contract.ts, partagé front/back
```

- **Un seul `node_modules`** à la racine, pour les trois `apps/`. C'est ce que fait npm avec `workspaces`. Chaque `apps/*/package.json` liste ses dépendances, mais l'installation se fait une fois, en haut.
- `apps/types` est un **paquet** (`@api-starter-kit/types`). Le backend l'importe comme une bibliothèque : `from '@api-starter-kit/types/ws_type'`. Le frontend fait pareil avec le backend : `from '@api-starter-kit/backend/registry'` ([`TUYAU`](TUYAU.md)).
- `npm run dev` à la racine lance `turbo dev`, donc le `dev` de **chaque** workspace en parallèle.

🔗 **Lien C.** C'est le Makefile racine d'un projet 42 qui appelle `make -C libft` puis compile le reste : un point d'entrée, plusieurs sous-projets. La différence : ici, les dépendances sont **mises en commun** dans un seul `node_modules`, comme si tous les sous-projets partageaient un même `libft.a`.

---

## 2. Les versions installées

Lues dans `node_modules/*/package.json` après `npm ci`, au 20/11/2026 :

| Paquet | Version | Rôle |
|---|---|---|
| `@adonisjs/core` | 7.6.0 | le framework : routeur, HTTP, ace, conteneur |
| `@adonisjs/lucid` | 22.4.2 | ORM + migrations ([`LUCID`](LUCID.md)) |
| `@adonisjs/auth` | 10.1.0 | guards, jetons d'accès ([`AUTH`](AUTH.md)) |
| `@vinejs/vine` | 4.4.0 | validation ([`VINE`](VINE.md)) |
| `@tuyau/core` | 1.3.0 | client HTTP typé pour le front ([`TUYAU`](TUYAU.md)) |
| `@japa/runner` | 5.4.0 | tests ([`TESTS_JAPA`](TESTS_JAPA.md)) |
| `socket.io` | 4.8.4 | temps réel ([`SOCKET_IO`](SOCKET_IO.md)) |
| `pg` | 8.23.1 | pilote Postgres |
| `typescript` | 6.0.3 | |

Images Docker : `postgres:18-alpine`, `caddy:2.11.9-alpine`.

---

## 3. Le `.env`

```sh
cd apps/backend
cp .env.example .env
node ace generate:key
```

Les variables qui comptent (extrait de `.env.example`) :

```sh
PORT=3333
HOST=localhost
NODE_ENV=development
APP_KEY=
APP_URL=http://${HOST}:${PORT}
MIGRATE=true
SESSION_DRIVER=cookie
POSTGRES_HOST=127.0.0.1
POSTGRES_PORT=5432
POSTGRES_USER=postgres
POSTGRES_PASSWORD=thisissecretpassword
POSTGRES_DB=example
```

- `node ace generate:key` remplit `APP_KEY`. C'est la clé de chiffrement de l'application (cookies signés, `encryption`). **Sans elle, rien ne démarre**, pas même `codegen`.
- `start/env.ts` déclare un **schéma** pour ces variables : `PORT: Env.schema.number()`, `APP_URL: Env.schema.string({ format: 'url', tld: false })`… Une variable absente ou mal formée fait échouer le démarrage avec un message qui la nomme. C'est voulu : mieux vaut planter au lancement qu'au premier appel.
- `${HOST}` dans `APP_URL` est **interpolé**, et avec les variables d'environnement du processus, pas seulement celles du fichier. Observé : lancé avec `HOST=0.0.0.0 PORT=3397`, le backend renvoie des URLs d'avatar en `http://0.0.0.0:3397/…`. En prod, cela a une conséquence ([`DEPLOIEMENT`](DEPLOIEMENT.md), §5).
- `MIGRATE` n'est lu **que** par l'entrypoint Docker ([`DEPLOIEMENT`](DEPLOIEMENT.md)). En dev, il ne fait rien.
- Le `.env` est dans `.gitignore`. Il ne se commite jamais.

🔗 **Lien C.** `Env.schema` joue le rôle des vérifications de `argc` / `argv` au début de `main()` : on refuse tout de suite une entrée invalide plutôt que de la traîner dans le programme.

---

## 4. Postgres en dev

```sh
docker compose -f compose.dev.yaml up -d db
```

- Le service `db` publie `127.0.0.1:5432:5432` : la base n'est joignable **que depuis ta machine**, pas depuis le réseau.
- Il lit `apps/backend/.env` : `POSTGRES_USER`, `POSTGRES_PASSWORD` et `POSTGRES_DB` servent **à la fois** à créer la base (image Postgres) et à s'y connecter (Adonis). Changer le mot de passe **après** la première création ne change rien : le volume `postgres_data` garde l'ancien.
- Le conteneur s'appelle `database_postgres`. Un seul peut exister à ce nom sur la machine.

🚨 **Le port 5432 est déjà pris** si un Postgres local tourne (Homebrew, Postgres.app). Le conteneur refuse de démarrer, ou pire, Adonis se connecte au **mauvais** Postgres et répond `password authentication failed`. Vérifier avec `lsof -iTCP:5432 -sTCP:LISTEN`.

---

## 5. Migrer, générer, lancer

```sh
node ace migration:run       # crée les tables ET régénère database/schema.ts
node ace codegen             # régénère .adonisjs/ (contrôleurs, registre Tuyau)
node ace serve --hmr         # = npm run dev dans apps/backend
```

### Ce que produit `migration:run`

Dans `config/database.ts`, l'équipe a activé `schemaGeneration`. Après chaque migration, Lucid lit les tables et **écrit** `database/schema.ts` : une classe par table (`UserSchema`, `GameSchema`…) avec ses colonnes décorées. Les modèles **héritent** de ces classes ([`LUCID`](LUCID.md), §2).

```ts
/**
 * This file is automatically generated
 * DO NOT EDIT manually
 * Run "node ace migration:run" command to re-generate this file
 */
export class AuthAccessTokenSchema extends BaseModel {
  @column()
  declare abilities: string
  // …
}
```

Ce fichier **est commité**. C'est indispensable : le build Docker ne lance pas de migration, il compile ce qui est dans le dépôt.

### Ce que produit `codegen`

Le dossier `.adonisjs/`, **non commité**, contient :
- `server/controllers.ts` : l'objet `controllers` que `routes.ts` importe via `#generated/controllers` ;
- `client/registry/` : le registre que lit Tuyau côté front.

Ces fichiers viennent des hooks `init` de `adonisrc.ts` (`indexEntities(…)`, `generateRegistry()`). `serve` les régénère à chaque modification. Mais après un `git pull` qui ajoute un contrôleur, `tsc` et l'éditeur se plaignent tant que `codegen` n'a pas tourné.

🚨 **`Property 'Rooms' does not exist on type '{ AccessTokens: () => Promise<…>; … }'`** (message observé) : soit le fichier `app/controllers/rooms_controller.ts` n'existe pas, soit il existe mais `.adonisjs/` est périmé. Dans le second cas, lancer `node ace codegen`.

### Le piège du `psql`

Sur une base **vierge**, la première `migration:run` a échoué ainsi :

```
Error: spawn psql ENOENT
```

Lu dans `@adonisjs/lucid` (`runner.js`, `prepareDatabaseForUp`) : si le fichier `database/schema/pg-schema.sql` **existe**, Lucid considère que c'est un *dump* de schéma et le restaure avec l'outil en ligne de commande `psql` avant de migrer. Le dépôt contient ce fichier, mais **vide** (0 octet). Le Mac n'a pas `psql`, donc `spawn` échoue.

Deux sorties :

```sh
node ace migration:run --schema-path=database/schema/absent.sql   # contournement local, testé
brew install libpq && brew link --force libpq                     # installe psql (non testé ici)
```

Le contournement pointe vers un fichier qui n'existe pas : Lucid ne restaure rien et migre normalement. Ce fichier `.sql` n'est pas copié par `node ace build`, donc la prod n'est pas concernée.

🔗 **Lien C.** C'est un `execve("psql", …)` qui renvoie `ENOENT` : le binaire n'est pas dans le `PATH`. Le message ne dit pas *pourquoi* Lucid voulait `psql`, d'où la lecture du source.

---

## 6. `node ace`, la boîte à outils

| Commande | Effet |
|---|---|
| `node ace list` | toutes les commandes |
| `node ace list:routes` | les routes, leur nom et leurs middlewares ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §1) |
| `node ace make:controller games` | crée `app/controllers/games_controller.ts` |
| `node ace make:migration rooms` | crée une migration horodatée |
| `node ace make:validator game` | crée `app/validators/game.ts` |
| `node ace migration:status` | migrations jouées ou non |
| `node ace migration:rollback` | défait le dernier lot |
| `node ace migration:fresh` | **efface toutes les tables** et rejoue tout |
| `node ace repl` | une console avec l'application démarrée |
| `node ace test` | les tests Japa |

Une commande **à toi** se pose dans `apps/backend/commands/`. Elle est découverte automatiquement, sans l'enregistrer dans `adonisrc.ts`. Exemple testé, qui crée un jeton temporaire de 2FA pour un utilisateur ([`AUTH`](AUTH.md), §4) :

```ts
import { BaseCommand, args } from '@adonisjs/core/ace'
import type { CommandOptions } from '@adonisjs/core/types/ace'

export default class JetonTemp extends BaseCommand {
  static commandName = 'jeton:temp'
  static options: CommandOptions = { startApp: true }
  @args.string() declare id: string

  async run() {
    const { default: User } = await import('#models/user')
    const user = await User.findOrFail(Number(this.id))
    const t = await User.accessTokens.create(user, ['2fa:verify'], { expiresIn: '5 minutes' })
    console.log(t.value!.release())
  }
}
```

- `startApp: true` : la commande démarre l'application (base, providers). Sans cette option, `User` ne peut pas parler à Postgres.
- L'`import()` **dans** `run()` est la convention Adonis : le modèle n'est chargé que si la commande tourne, pas à chaque `node ace list`.

🚨 `migration:fresh` et `db:wipe` vident **la base pointée par le `.env`**. Avant de les taper, relis `POSTGRES_HOST` et `POSTGRES_DB`.

---

## 🧪 À essayer

1. Vide `APP_KEY` dans le `.env` et lance `node ace serve`. Quel message ? Fais pareil avec `PORT=abc`.
2. Supprime `.adonisjs/`, lance `npx tsc --noEmit` dans `apps/backend`, puis `node ace codegen`, puis relance `tsc`.
3. Écris une migration qui ajoute une colonne, lance `migration:run`, puis `git diff database/schema.ts` : qu'est-ce qui a changé tout seul ?
4. Lance `node ace list:routes` et retrouve, pour chaque route de `account`, ses deux middlewares.
