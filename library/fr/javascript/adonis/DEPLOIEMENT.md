# 9. Déployer : Docker, Caddy, Postgres

> **TL;DR.** `compose.yaml` lance trois conteneurs :
> - `caddy` (ports 80/443) sert le front **buildé** et relaie `/api/v1/*` et `/socket.io/*` vers le back ;
> - `backend` (Adonis, port 3333 **interne**) ;
> - `db` (Postgres 18), sur un réseau sans accès extérieur.
>
> Le front et l'API partagent donc **la même origine** : pas de CORS en prod.
>
> Six problèmes trouvés en simulant ce déploiement, tous observés :
> - `otplib` et `qrcode` manquent dans l'image : le **login** répond 500 ;
> - le front buildé appellera `http://localhost:3333`, c'est-à-dire le poste du **visiteur** ;
> - Caddy renvoie l'`index.html` à la place des **avatars** ;
> - les URL d'avatar commencent par `http://0.0.0.0:3333` ;
> - le healthcheck est vert même quand la **base est injoignable** ;
> - le `Makefile` commité contient un **conflit git** et ne se lance plus.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) · [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) (§4, §5) · [`TUYAU`](TUYAU.md) (§1) · [`SOCKET_IO`](SOCKET_IO.md) · [`TESTS_JAPA`](TESTS_JAPA.md) · [doc AdonisJS, déploiement](https://docs.adonisjs.com/deployment) · [doc Caddy, `handle`](https://caddyserver.com/docs/caddyfile/directives/handle)

🧪 Labo : aucun. Rien n'a été déployé. La simulation a utilisé :
- un build de prod du backend lancé à la main (`NODE_ENV=production node bin/server.js`) ;
- une installation `npm ci --omit=dev -w apps/backend` faite exactement comme dans le Dockerfile ;
- un conteneur `caddy:2.11.9-alpine` avec le Caddyfile de l'équipe ;
- un `vite build` du front sans `.env`.

Les images de l'équipe n'ont **pas** été construites.

---

## 1. L'architecture

`compose.yaml`, abrégé (le nom du compte GHCR est remplacé par `<compte>`) :

```yaml
services:
  caddy:
    build: { dockerfile: ./docker_config/caddy/Dockerfile, context: . }
    image: ghcr.io/<compte>/caddy_serv
    ports: ["80:80", "443:443"]
    env_file: ".env"
    volumes: [caddy_data:/data, caddy_config:/config]
    networks: [edge]
    depends_on: { backend: { condition: service_healthy } }

  backend:
    build: { dockerfile: ./docker_config/adonis/Dockerfile, context: . }
    image: ghcr.io/<compte>/transcendence_backend
    environment: { HOST: 0.0.0.0 }
    env_file: "apps/backend/.env"
    healthcheck:
      test: ["CMD", "node", "-e", "fetch('http://localhost:3333').then(r => process.exit(r.status === 200 ? 0 : 1)).catch(() => process.exit(1))"]
    expose: ["3333"]
    networks: [edge, backend_net]
    depends_on: { db: { condition: service_healthy } }

  db:
    image: postgres:18-alpine
    volumes: [postgres_data:/var/lib/postgresql]
    networks: [backend_net]
    healthcheck: { test: ["CMD", "pg_isready", "-U", "postgres"] }

networks:
  edge: {}
  backend_net: { internal: true }
```

- `expose: ["3333"]` et pas `ports:` : le back n'est joignable **que** par les autres conteneurs. Depuis Internet, tout passe par Caddy.
- `backend_net: { internal: true }` : la base n'a aucun accès au réseau extérieur, et le réseau extérieur n'y a aucun accès.
- `depends_on … service_healthy` : Caddy attend que le back soit « sain », qui attend que la base le soit. Ce n'est utile que si le healthcheck veut dire quelque chose (§5).
- `HOST: 0.0.0.0` : sans cette ligne, Adonis écouterait sur `localhost` **du conteneur**, injoignable depuis Caddy. Observé pendant la simulation : un back lancé sur `localhost` donne `502` via Caddy ; relancé avec `HOST=0.0.0.0`, la même requête répond `200`.
- `caddy_data` : les certificats HTTPS obtenus par Caddy. Sans ce volume, il les redemanderait à chaque redémarrage.

🔗 **Lien C.** `0.0.0.0` dans un `bind()` veut dire « toutes les interfaces ». `127.0.0.1` n'accepte que les connexions venues de la **même** machine, et chaque conteneur est une machine à part. C'est le même `INADDR_ANY` que dans `webserv` ou `ft_irc`.

---

## 2. L'image du backend : le login répond 500

Le Dockerfile se fait en trois étapes :
- `deps` : `npm ci` complet ;
- `build` : `node ace codegen` puis `node ace build` (TypeScript compilé dans `build/`) ;
- `production` : on repart d'une image vide et on ne garde que l'essentiel.

```dockerfile
FROM base AS production
WORKDIR /app
ENV NODE_ENV=production
COPY package.json package-lock.json ./
COPY apps/backend/package.json apps/backend/
COPY apps/frontend/package.json apps/frontend/
COPY apps/types/package.json apps/types/
RUN npm ci --omit=dev -w /app/apps/backend
COPY --from=build /app/apps/backend/build apps/backend/build
```

- `--omit=dev` : pas de `devDependencies` (TypeScript, Japa, eslint…), l'image est plus légère.
- `-w /app/apps/backend` : n'installe **que** les dépendances du workspace backend.

🚨 **`otplib` et `qrcode` sont déclarés à la racine**, dans le `package.json` du monorepo, pas dans `apps/backend/package.json`. En dev, tout le monorepo est installé, donc ça marche. Dans l'image, `-w apps/backend` ne les installe pas. Observé, en reproduisant exactement cette installation :

```
POST /api/v1/auth/login → 500 {"message":"Cannot find package 'otplib' imported from /…/build/app/controllers/access_tokens_controller.js"}
```

Le serveur **démarre** sans erreur : les contrôleurs sont importés à la première requête (`() => import(...)` dans les routes). Le healthcheck est vert, la page d'accueil s'affiche, et c'est le **premier login** qui casse. Les tests Japa ne le voient pas non plus ([`TESTS_JAPA`](TESTS_JAPA.md), §6).

**Correction testée** : déplacer les deux lignes dans le `package.json` du backend.

```json
{
  "name": "@api-starter-kit/backend",
  "dependencies": {
    "otplib": "^13.6.0",
    "qrcode": "^1.5.4"
  }
}
```

Puis `npm install` à la racine pour mettre à jour `package-lock.json`. Observé, après la même installation `--omit=dev -w` : `otplib` est présent, et le login répond `400 {"errors":[{"message":"Invalid user credentials"}]}`, la réponse attendue pour un compte inconnu.

Règle générale : une dépendance **importée par le code d'un workspace** se déclare **dans ce workspace**. La racine ne garde que l'outillage commun (turbo, prettier).

---

## 3. Le front buildé appelle `localhost:3333`

Le Dockerfile de Caddy builde le front puis copie `dist/` dans `/srv`. Or :
- `.dockerignore` exclut `**/.env` : le build Docker ne voit **aucun** `.env` ;
- `src/lib/api.ts` fait `baseUrl: import.meta.env.VITE_API_URL || 'http://localhost:3333'`.

Vite remplace `import.meta.env.VITE_API_URL` **au build**. Sans variable, c'est le `|| 'http://localhost:3333'` qui reste dans le JavaScript envoyé au navigateur. Et `localhost`, pour le navigateur, c'est la machine du **visiteur**.

Observé, sur un `vite build` du front de l'équipe sans `.env` :
- tel quel, `localhost:3333` n'apparaît **pas** dans le bundle : aucun fichier n'importe encore `lib/api.ts`, donc Vite l'élimine ;
- avec un écran qui importe `api` (simulé par un `import` dans `main.tsx`), on trouve `1 localhost:3333` dans `dist/assets/*.js`.

Le bug est donc **latent** : il apparaîtra avec le premier écran branché sur l'API, et seulement en prod.

`src/lib/socket.ts` fait déjà ce qu'il faut : `import.meta.env.PROD ? undefined : import.meta.env.VITE_API_URL`. `undefined` veut dire « même origine ». **Correction testée** pour `api.ts`, sur le même modèle :

```ts
baseUrl: import.meta.env.PROD ? window.location.origin : import.meta.env.VITE_API_URL,
```

Observé : le bundle contient `baseUrl:window.location.origin` et plus aucun `localhost:3333`, et le typecheck du front passe. **Non testé** dans un navigateur derrière Caddy.

🔗 **Lien C.** `import.meta.env.X` ressemble à un `getenv("X")`, mais se comporte comme un `#define` : la valeur est figée à la **compilation**. Changer la variable sur le serveur après le build ne change rien.

---

## 4. Caddy et les avatars

Le Caddyfile de l'équipe :

```
{$URL} {
  encode

  handle /api/v1/* {
    reverse_proxy backend:3333
  }

  handle /socket.io/* {
    reverse_proxy backend:3333
  }

  handle {
    root * /srv
    try_files {path} /index.html
    file_server
  }
}
```

- `{$URL}` : l'adresse du site, lue dans l'environnement (`URL=:80` dans `.env.example`). Avec un vrai nom de domaine, Caddy obtient le certificat HTTPS tout seul.
- Les `handle` sont **exclusifs** : le premier qui correspond gagne.
- Le dernier `handle` (sans motif) attrape tout le reste. `try_files {path} /index.html` : si le fichier n'existe pas dans `/srv`, on sert `index.html`. C'est ce qui permet à React Router de gérer `/profil` ([`../react/ROUTER`](../react/ROUTER.md)).

🚨 **Les avatars tombent dans ce dernier `handle`.** Adonis sert `public/` (§1 de [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md)) :
- l'avatar par défaut est à `/uploads/avatars/default-avatar.png` ;
- les avatars envoyés sont à `/avatars/<fichier>` (`profile_controller.ts`).

Aucun des deux chemins n'est relayé. Observé, via Caddy :

```
/api/v1/card                         → 200 application/json
/uploads/avatars/default-avatar.png  → 200 text/html     ← l'index.html du front
/avatars/x.png                       → 200 text/html
```

Un `200` qui contient du HTML : le navigateur affiche une image cassée, et aucune erreur n'apparaît dans les logs.

**Correction testée** : un seul `handle` pour tout ce qui va au back.

```
{$URL} {
  encode

  @backend path /api/v1/* /socket.io/* /uploads/* /avatars/*
  handle @backend {
    reverse_proxy backend:3333
  }

  handle {
    root * /srv
    try_files {path} /index.html
    file_server
  }
}
```

- `@backend path …` : un **matcher nommé**, qui accepte plusieurs motifs.

Observé avec ce fichier :

```
/api/v1/card                         → 200 application/json
/uploads/avatars/default-avatar.png  → 200 image/png
/avatars/x.png                       → 404 application/json   ← le back répond : ce fichier n'existe pas
/profil                              → 200 text/html          ← le front gère toujours ses routes
/socket.io/?EIO=4&transport=polling  → 0{"sid":"…","upgrades":["websocket"],…}
```

Plus simple encore à long terme : ranger tous les fichiers servis sous un seul préfixe (`/uploads/avatars/…`) côté back.

---

## 5. Les URL d'avatar et les fichiers perdus

Le transformer fabrique une URL absolue :

```ts
data.avatarUrl = `${appUrl}/uploads/avatars/default-avatar.png`
```

avec `APP_URL=http://${HOST}:${PORT}` dans le `.env`. Or `compose.yaml` force `HOST=0.0.0.0`. Observé sur le build de prod lancé avec `HOST=0.0.0.0` :

```
"avatarUrl":"http://0.0.0.0:3397/uploads/avatars/default-avatar.png"
```

Aucun navigateur ne peut charger cette adresse. Deux corrections possibles (non testées) :
- dans le `.env` de prod, écrire `APP_URL=https://<domaine>` en dur ;
- ou, plus simple, renvoyer un chemin **relatif** (`/uploads/avatars/…`) : le front et l'API ont la même origine (§1).

🚨 **Les avatars envoyés vivent dans le conteneur.** `avatar.move(app.makePath('public/avatars'))` écrit dans `build/public/avatars/`, à l'intérieur de l'image. `compose.yaml` ne monte **aucun** volume à cet endroit. Un `docker compose up` avec une nouvelle image recrée le conteneur, et tous les avatars disparaissent. En base, `avatar_url` pointe alors vers des fichiers absents (lu dans le code, non observé). Correction (non testée) :

```yaml
  backend:
    volumes:
      - avatars:/app/apps/backend/build/public/avatars
```

🔗 **Lien C.** Le système de fichiers d'un conteneur, c'est la pile d'un processus : il disparaît avec lui. Ce qui doit survivre va dans un volume, comme ce qui doit survivre à une fonction va sur le tas ou dans un fichier.

---

## 6. Le healthcheck ne vérifie rien

Le healthcheck appelle `GET /`, qui renvoie `{ hello: 'world' }` sans toucher la base. Observé, avec un back qui pointe vers un Postgres injoignable (`POSTGRES_PORT=1`) :

```
GET /        → 200 {"hello":"world"}
GET /health  → 503 {"status":"db injoignable"}
```

Avec le healthcheck actuel, ce back est « sain ». Caddy démarre, et chaque requête qui touche la base échoue.

**Correction testée** (la seconde ligne ci-dessus) :

```ts
import db from '@adonisjs/lucid/services/db'
import router from '@adonisjs/core/services/router'

router.get('/health', async ({ response }) => {
  try {
    await db.rawQuery('select 1')
    return { status: 'ok' }
  } catch {
    return response.serviceUnavailable({ status: 'db injoignable' })
  }
})
```

Observé avec une base disponible : `200 {"status":"ok"}`. Dans `compose.yaml`, il suffit de remplacer `http://localhost:3333` par `http://localhost:3333/health` : le test exige déjà `r.status === 200`.

---

## 7. Migrations et Makefile

`docker-entrypoint.js` :

```js
if (process.env.MIGRATE === 'true') {
  execSync('node ace migration:run --force', { stdio: 'inherit' })
}
await import('./bin/server.js')
```

- `--force` : en prod, `migration:run` refuse de tourner sans ce drapeau, pour éviter un accident.
- `MIGRATE=true` dans le `.env` du back : chaque démarrage joue les migrations en attente, puis lance le serveur.
- Pas de piège `psql` ici : le dossier `build/database/` produit par `node ace build` ne contient que `migrations/` et `schema.js`, pas de `schema/pg-schema.sql` (observé). La migration au démarrage du conteneur n'a pas été lancée.

🚨 **Une migration modifiée n'est pas rejouée** ([`LUCID`](LUCID.md), §1). Sur un serveur déjà migré, corriger une migration existante ne change rien. Il faut une **nouvelle** migration.

🚨 **Le `Makefile` commité contient un conflit git non résolu** (lu sur la branche courante du dépôt, dernier commit qui le touche : `796dd50`) :

```make
<<<<<<< Updated upstream
local:
  docker compose -f compose.dev.yaml up -d
  npm run dev
=======
COMPOSE_DEV := docker compose -f compose.dev.yaml
>>>>>>> Stashed changes
```

Observé sur une copie : `make` et `make local` répondent `Makefile:1: *** missing separator.  Stop.` Plus aucune cible ne fonctionne. Correction : garder la ligne `COMPOSE_DEV := …` et supprimer les marqueurs et l'ancienne cible `local`, qui est redéfinie plus bas. Ce n'est pas la seule erreur. Observé sur une copie sans les marqueurs : `Makefile:31: *** missing separator.  Stop.`, parce que la recette de `test:` est indentée par des **espaces** au lieu d'une tabulation.

---

## 🧪 À essayer

1. Dans une copie du dépôt, lance `npm ci --omit=dev -w apps/backend`, puis `ls node_modules/otplib`. Déplace la dépendance (§2) et recommence.
2. Builde le front sans `.env`, ajoute un `import { api } from './lib/api'` dans `main.tsx`, rebuilde, puis cherche `localhost` dans `dist/assets/`.
3. Lance `caddy:2.11.9-alpine` avec le Caddyfile de l'équipe et `curl -I` l'avatar par défaut. Lis le `content-type`.
4. Arrête Postgres pendant que le back tourne, puis compare `GET /` et `GET /health`.
5. Après `docker compose up`, envoie un avatar, puis `docker compose up --force-recreate backend`, et recharge le profil.
