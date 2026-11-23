# 8. Tester le backend avec Japa

> **TL;DR.** `node ace test` démarre l'application en environnement `test`, lance un vrai serveur HTTP, et exécute les fichiers `tests/**/*.spec.ts`. Les outils de base :
> - `client.post(url).json(...)` pour appeler l'API ;
> - `.loginAs(user)` pour se connecter sans passer par le login ;
> - `db.assertHas(table, {...})` pour vérifier la base ;
> - `testUtils.db().withGlobalTransaction()` pour **annuler** tout ce qu'un test a écrit.
>
> L'équipe a la configuration complète (`tests/bootstrap.ts`), mais **aucun test**. Deux pièges avant d'écrire le premier :
> - `.env.test` ne change pas de base : les tests tournent sur la base de **dev**, avec ses données ;
> - le hook de migration de Japa bute sur le même `psql` absent que dans [`INSTALLATION`](INSTALLATION.md).

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) (§5) · [`LUCID`](LUCID.md) (§5) · [`AUTH`](AUTH.md) · [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) · [`SOCKET_IO`](SOCKET_IO.md) · [doc AdonisJS, testing](https://docs.adonisjs.com/guides/testing/introduction)

🧪 Labo : aucun. Les tests ont été écrits et lancés dans une copie du backend, contre un Postgres jetable. La suite complète compte **24 tests verts** : 7 dans `parcours.spec.ts` (§2), 11 dans `audit_relations.spec.ts` (§5), et 6 dans les sondes qui ont servi aux fiches précédentes.

---

## 1. La configuration de l'équipe

`tests/bootstrap.ts`, sans les commentaires :

```ts
import { assert } from '@japa/assert'
import { apiClient } from '@japa/api-client'
import app from '@adonisjs/core/services/app'
import type { Config } from '@japa/runner/types'
import { pluginAdonisJS } from '@japa/plugin-adonisjs'
import { dbAssertions } from '@adonisjs/lucid/plugins/db'
import testUtils from '@adonisjs/core/services/test_utils'
import { authApiClient } from '@adonisjs/auth/plugins/api_client'
import { sessionApiClient } from '@adonisjs/session/plugins/api_client'
import type { Registry } from '../.adonisjs/client/registry/schema.d.ts'

declare module '@japa/api-client/types' {
  interface RoutesRegistry extends Registry {}
}

export const plugins: Config['plugins'] = [
  assert(),
  pluginAdonisJS(app),
  dbAssertions(app),
  apiClient(),
  sessionApiClient(app),
  authApiClient(app),
]

export const runnerHooks: Required<Pick<Config, 'setup' | 'teardown'>> = {
  setup: [],
  teardown: [],
}

export const configureSuite: Config['configureSuite'] = (suite) => {
  if (['browser', 'functional', 'e2e'].includes(suite.name)) {
    return suite.setup(() => testUtils.httpServer().start())
  }
}
```

- Chaque **plugin** ajoute quelque chose au contexte d'un test :
  - `assert()` donne `assert` ;
  - `apiClient()` donne `client` ;
  - `dbAssertions(app)` donne `db` ;
  - `authApiClient(app)` ajoute `.loginAs()` aux requêtes.
- `RoutesRegistry extends Registry` : branche le registre de Tuyau ([`TUYAU`](TUYAU.md)) dans le client de test. C'est pour cela que `.adonisjs/` doit exister avant le premier `tsc`. `node ace test` lance le codegen tout seul (observé : `codegen: created 5 file(s)`).
- `configureSuite` : pour la suite `functional`, démarre un vrai serveur HTTP. `client.get(...)` fait donc une vraie requête réseau, qui traverse **tous** les middlewares ([`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md), §1).
- Les suites sont déclarées dans `adonisrc.ts` : `unit` (`tests/unit/**`, 2 s par test) et `functional` (`tests/functional/**`, 30 s).

🔗 **Lien C.** C'est le testeur maison que tu écris pour un projet 42 (un `main` qui appelle tes fonctions et compare à l'attendu), mais branché sur le vrai programme lancé, comme un script qui fait des `curl` contre `webserv`.

---

## 2. Un fichier de test complet

`tests/functional/parcours.spec.ts`, 7 tests verts :

```ts
import { test } from '@japa/runner'
import testUtils from '@adonisjs/core/services/test_utils'
import User from '#models/user'

const inscription = (nickname: string) => ({
  nickname,
  email: `${nickname}@x.io`,
  password: 'motdepasse',
  passwordConfirmation: 'motdepasse',
})

test.group('auth', (group) => {
  group.each.setup(() => testUtils.db().withGlobalTransaction())

  test('l’inscription renvoie un jeton et cache le mot de passe', async ({ client, assert }) => {
    const r = await client.post('/api/v1/auth/signup').json(inscription('ana'))

    r.assertStatus(200)
    assert.match(r.body().data.token, /^oat_/)
    assert.notProperty(r.body().data.user, 'password')
  })

  test('un pseudo déjà pris donne un 422 sur le bon champ', async ({ client }) => {
    await User.create({ nickname: 'ana', email: 'autre@x.io', password: 'motdepasse' })
    const r = await client.post('/api/v1/auth/signup').json(inscription('ana'))

    r.assertStatus(422)
    r.assertBodyContains({ errors: [{ field: 'nickname', rule: 'database.unique' }] })
  })

  test('le login renvoie toujours la clé token', async ({ client, assert }) => {
    await User.create({ nickname: 'bea', email: 'bea@x.io', password: 'motdepasse' })
    const r = await client.post('/api/v1/auth/login').json({ email: 'bea@x.io', password: 'motdepasse' })

    r.assertStatus(200)
    assert.properties(r.body(), ['token', 'requires2fa'])
  })

  test('un jeton 2fa:verify n’ouvre pas le profil', async ({ client }) => {
    const user = await User.create({ nickname: 'carl', email: 'carl@x.io', password: 'motdepasse' })
    const temp = await User.accessTokens.create(user, ['2fa:verify'])

    const r = await client.get('/api/v1/account/profile').bearerToken(temp.value!.release())
    r.assertStatus(401)
  })
})

test.group('games', (group) => {
  group.each.setup(() => testUtils.db().withGlobalTransaction())

  test('créer une partie inscrit les deux joueurs', async ({ client, db }) => {
    const [ana, bea] = await User.createMany([
      { nickname: 'ana', email: 'ana@x.io', password: 'motdepasse' },
      { nickname: 'bea', email: 'bea@x.io', password: 'motdepasse' },
    ])

    const r = await client.post('/api/v1/games').json({ adversaireId: bea.id }).loginAs(ana)

    r.assertStatus(201)
    await db.assertHas('games_users', { game_id: r.body().id, user_id: ana.id, score: 0 })
    await db.assertHas('games_users', { game_id: r.body().id, user_id: bea.id })
  })

  test('on ne joue pas contre soi-même', async ({ client, db }) => {
    const ana = await User.create({ nickname: 'ana', email: 'ana@x.io', password: 'motdepasse' })

    const r = await client.post('/api/v1/games').json({ adversaireId: ana.id }).loginAs(ana)

    r.assertStatus(400)
    await db.assertEmpty('games')
  })
})

test.group('logout', (group) => {
  group.each.setup(() => testUtils.db().withGlobalTransaction())

  test('le logout supprime le jeton, même sans Socket.IO chargé', async ({ client, db }) => {
    const user = await User.create({ nickname: 'dan', email: 'dan@x.io', password: 'motdepasse' })
    const jeton = await User.accessTokens.create(user)

    const r = await client.post('/api/v1/account/logout').bearerToken(jeton.value!.release())

    r.assertStatus(200)
    await db.assertMissing('auth_access_tokens', { tokenable_id: user.id })
  })
})
```

Les points qui comptent :
- `group.each.setup(() => testUtils.db().withGlobalTransaction())` : avant **chaque** test, ouvre une transaction. La fonction renvoyée la **annule** après le test. Les `ana` et `bea` créés ici disparaissent, donc chaque test peut reprendre les mêmes pseudos sans conflit `unique`.
- `.loginAs(ana)` : crée un jeton pour `ana` et l'ajoute en `Bearer`, sans appeler le login. On teste les parties, pas l'authentification.
- `r.assertBodyContains({...})` : vérifie un **sous-ensemble** du corps. `message` n'est pas comparé, il peut changer (traduction).
- `db.assertHas(table, {...})` : une ligne correspondante existe. `db.assertMissing` vérifie l'inverse, `db.assertEmpty(table)` vérifie que la table n'a aucune ligne.
- `le login renvoie toujours la clé token` ne passe **qu'avec** la correction d'[`AUTH`](AUTH.md) §6. Avec le contrôleur de l'équipe remis en place (observé) :

  ```
  ✔✔✖✔✔✔✔
  ❯ auth / le login renvoie toujours la clé token
  ℹ AssertionError: expected { message: 'Login OK.', …(2) } to contain keys 'token', and 'requires2fa'
   ❯ 36 ┃      assert.properties(r.body(), ['token', 'requires2fa'])
  Tests  6 passed, 1 failed (7)
  ```

  C'est le test qui aurait attrapé le bug des quatre noms de jeton.
- Le test de logout passe alors que `start/ws.ts` n'est **pas** chargé sous Japa (il n'est préchargé que pour l'environnement `web`). `realtime.io` vaut donc `undefined`, et le `?.` du contrôleur évite le plantage ([`SOCKET_IO`](SOCKET_IO.md), §4).

Lancer :

```sh
node ace test                                         # toutes les suites
node ace test functional                              # une suite
node ace test --files=tests/functional/parcours.spec.ts
node ace test --tests="on ne joue pas contre soi-même"
```

(Les trois premières commandes ont été lancées : 24 tests pour les deux premières, 7 pour la troisième. `--tests` filtre par titre d'après la documentation et n'a pas été lancé.)

🚨 **Le serveur de test écoute sur le `PORT` du `.env`.** Si `node ace serve` tourne déjà, observé : `Error: listen EADDRINUSE: address already in use ::1:3399` puis `Tests (0)`. Arrête le serveur de dev, ou lance `PORT=3398 node ace test`. Avec `serve --watch`, le piège est sournois : modifier un fichier relance le serveur de dev, qui reprend le port juste avant les tests.

---

## 3. Les tests tournent sur la base de dev

Premier lancement du fichier ci-dessus, avec la configuration de l'équipe (observé) :

```
Tests  5 passed, 1 failed
ℹ Expected table 'games' to have 0 rows, but found 3
```

(À ce moment-là, le fichier ne contenait que les groupes `auth` et `games`.)

Le test est juste, le contrôleur aussi : les 3 parties sont celles **créées à la main pendant le développement**. Le `.env.test` de l'équipe ne contient que `SESSION_DRIVER=memory`. Les variables `POSTGRES_*` viennent donc du `.env`, et Japa utilise la base de dev.

Conséquences :
- un test qui compte des lignes dépend de ce que tu as fait dans la journée ;
- la transaction globale protège tes données de dev **des tests**, mais pas les tests **de tes données** ;
- un test qui ferait un `TRUNCATE` (ou `db:truncate`, ou `migration:reset`) viderait ta base de dev.

**Correction testée** : une base dédiée aux tests, déclarée dans `.env.test` :

```sh
SESSION_DRIVER=memory
POSTGRES_DB=labo_test
```

Les valeurs de `.env.test` **remplacent** celles de `.env` quand `NODE_ENV=test`. Il faut créer la base une fois (`CREATE DATABASE labo_test;` dans pgAdmin ou `psql`), puis y jouer les migrations avant chaque lancement (§4).

---

## 4. Migrer la base de test au démarrage

La documentation propose `setup: [() => testUtils.db().migrate()]`. Observé, sur ce dépôt :

```
[ info ] Restoring schema dump from "database/schema/pg-schema.sql"
❯ Executed 0 migrations, 1 error, 0 skipped
ℹ Error: spawn psql ENOENT
```

Puis le processus **ne rend pas la main** : il a fallu le tuer. C'est le piège du fichier `pg-schema.sql` vide commité, expliqué dans [`INSTALLATION`](INSTALLATION.md) §5. `testUtils.db().migrate()` n'accepte pas d'option `--schema-path`. On appelle donc la commande nous-mêmes :

```ts
export const runnerHooks: Required<Pick<Config, 'setup' | 'teardown'>> = {
  setup: [
    async () => {
      const ace = await app.container.make('ace')
      const cmd = await ace.exec('migration:run', [
        '--compact-output',
        '--no-schema-generate',
        '--schema-path=database/schema/absent.sql',
      ])
      if (cmd.exitCode) throw cmd.error ?? new Error('migration:run a échoué')
    },
  ],
  teardown: [],
}
```

- `app.container.make('ace')` : le noyau des commandes `ace`, récupéré dans le conteneur d'Adonis. C'est exactement ce que fait `testUtils` en interne (lu dans `@adonisjs/lucid`, `test_utils/database.js`).
- `--no-schema-generate` : ne **réécrit pas** `database/schema.ts` pendant les tests.
- `--schema-path=…absent.sql` : un chemin qui n'existe pas, donc aucun dump à restaurer, donc pas de `psql`.
- `if (cmd.exitCode) throw …` : sans cette ligne, une migration ratée laisserait les tests tourner sur une base vide, avec des erreurs incompréhensibles.

Observé : premier lancement `Executed 13 migrations (120 ms)` puis `Tests 12 passed (12)`, second lancement `Already up to date` puis `PASSED`.

La vraie correction est de **supprimer** `pg-schema.sql` du dépôt (ou d'y mettre un vrai dump). Le contournement ci-dessus n'existe que parce que le dépôt est en lecture seule ici.

---

## 5. Auditer les relations Lucid

[`LUCID`](LUCID.md) §5 a trouvé 5 relations mal déclarées sur 11, et TypeScript n'en voit aucune. Un test par relation les attrape. `tests/functional/audit_relations.spec.ts` :

```ts
import { test } from '@japa/runner'
import testUtils from '@adonisjs/core/services/test_utils'
import User from '#models/user'
import Game from '#models/game'
import Card from '#models/card'
import Room from '#models/room'
import Message from '#models/message'
import Family from '#models/family'

const relations = [
  { nom: 'User.games', charger: () => User.query().preload('games') },
  { nom: 'User.friends', charger: () => User.query().preload('friends') },
  { nom: 'Game.users', charger: () => Game.query().preload('users') },
  { nom: 'Game.room', charger: () => Game.query().preload('room') },
  { nom: 'Room.game', charger: () => Room.query().preload('game') },
  { nom: 'Room.messages', charger: () => Room.query().preload('messages') },
  { nom: 'Message.room', charger: () => Message.query().preload('room') },
  { nom: 'Message.user', charger: () => Message.query().preload('user') },
  { nom: 'Card.family', charger: () => Card.query().preload('family') },
  { nom: 'Card.games', charger: () => Card.query().preload('games') },
  { nom: 'Family.cards', charger: () => Family.query().preload('cards') },
]

test.group('audit des relations', (group) => {
  group.each.setup(() => testUtils.db().withGlobalTransaction())

  test('{nom}')
    .with(relations)
    .run(async (_, { charger }) => {
      await charger()
    })
})
```

- `.with(relations)` : un **dataset**. Japa crée un test par élément du tableau, et `'{nom}'` reprend le champ `nom` dans le titre.
- `.run(async (_, ligne) => …)` : le second argument est l'élément courant. Le premier est le contexte habituel (`client`, `assert`…), inutile ici.
- Pas besoin de données : les tables de la base de test sont **vides**, et Lucid vérifie quand même les clés de la relation au moment du `preload`.

Observé, avec les quatre modèles de l'équipe (`user`, `game`, `room`, `card`) remis en place :

```
✖✖✖✔✖✔✔✔✔✖✔
❯ audit des relations / User.games
ℹ Exception: "games" is not defined as a relationship on "User" model
❯ audit des relations / User.friends
ℹ Exception: Relation "User.friends" expects "userId" to exist on "User" model, but is missing. Did you forget to define the column?
❯ audit des relations / Game.users
ℹ Exception: Relation "Game.users" expects "gameId" to exist on "User" model, but is missing. Did you forget to define the column?
❯ audit des relations / Room.game
ℹ Exception: Relation "Room.game" expects "roomId" to exist on "Game" model, but is missing. Did you forget to define the column?
❯ audit des relations / Card.games
ℹ Exception: Relation "Card.games" expects "cardId" to exist on "Game" model, but is missing. Did you forget to define the column?
Tests  6 passed, 5 failed (11)
```

Avec les modèles corrigés de [`LUCID`](LUCID.md) §5 : `Tests  11 passed (11)`.

Quand on ajoute une relation à un modèle, on ajoute sa ligne ici.

---

## 6. Quoi tester dans Transcendence

Ce qui rapporte le plus, d'après les bugs trouvés dans ces fiches :

| Test | Aurait attrapé |
|---|---|
| Un `preload` par relation (§5) | 5 relations sur 11 mal déclarées |
| `login` renvoie `token` (§2) | les quatre noms de jeton ([`AUTH`](AUTH.md), §3) |
| Un jeton `2fa:verify` refusé partout sauf `login/verify` | une route oubliée sans `requireFullAccess` |
| `GET /games/abc` → 404 | le 500 avec la requête SQL ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §4) |
| `signup` puis `assert.notProperty(user, 'twoFactorSecret')` | la fuite du secret par `return user` |

Ce que Japa **ne** voit **pas** ici :
- **Socket.IO** : `ws.ts` n'est pas chargé en environnement `test`. Pour le tester, il faut un client `socket.io-client` contre un serveur lancé à part, comme les sondes de [`SOCKET_IO`](SOCKET_IO.md).
- **La prod** : `otplib` absent du build ([`DEPLOIEMENT`](DEPLOIEMENT.md), §2) passe tous les tests, puisque les tests tournent avec les dépendances de dev.

---

## 🧪 À essayer

1. Lance `parcours.spec.ts` sur le code de l'équipe (sans la correction d'[`AUTH`](AUTH.md) §6) et lis le message d'échec du test de login.
2. Retire `group.each.setup(...)` du groupe `auth`, lance deux fois, et lis l'erreur du second lancement.
3. Écris le test « `GET /api/v1/games/abc` renvoie 404 », puis retire le matcher `number()` de la route.
4. Écris un test unitaire (`tests/unit/`) pour `messageValidator` ([`SOCKET_IO`](SOCKET_IO.md), §5), avec `await assert.rejects(() => messageValidator.validate({ text: '   ' }))`.
