# 3. Lucid : migrations, modèles, relations

> **TL;DR.** Trois couches empilées :
> - Une **migration** crée ou modifie une table. On ne la réécrit jamais une fois jouée : on en ajoute une nouvelle.
> - `migration:run` **génère** `database/schema.ts`, avec une classe par table et ses colonnes (`UserSchema`…).
> - Le **modèle** (`app/models/user.ts`) hérite de cette classe et y ajoute les **relations** et les méthodes.
>
> Chaque relation a sa forme, et une erreur de forme ne se voit qu'à l'exécution :
> - `hasMany` : la clé étrangère est dans **l'autre** table ;
> - `belongsTo` : elle est dans **ma** table ;
> - `manyToMany` : il y a une **table pivot**.
>
> Chez l'équipe, **5 relations sur 11 sont déclarées avec la mauvaise forme** et plantent au premier chargement. Le §5 les détaille avec leur correction testée.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) (§5) · [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) · [`TESTS_JAPA`](TESTS_JAPA.md) · [doc Lucid, relations](https://lucid.adonisjs.com/docs/relationships)

🧪 Labo : aucun. Les erreurs et les résultats viennent de tests Japa lancés sur une copie du backend, avec sa propre base Postgres. Les modèles corrigés n'existent **que** dans cette copie.

---

## 1. Une migration

Celle de l'équipe pour la table qui relie parties et joueurs :

```ts
import { BaseSchema } from '@adonisjs/lucid/schema'

export default class extends BaseSchema {
  protected tableName = 'games_users'

  async up() {
    this.schema.createTable(this.tableName, (table) => {
      table.increments('id')
      table.integer('game_id').unsigned().references('id').inTable('games').onDelete('CASCADE')
      table.integer('user_id').unsigned().references('id').inTable('users').onDelete('CASCADE')
      table.unique(['game_id', 'user_id'])
      table.integer('score').defaultTo(0)
      table.timestamps(true, true)
    })
  }

  async down() {
    this.schema.dropTable(this.tableName)
  }
}
```

- `up()` est joué par `migration:run`, `down()` par `migration:rollback`. Les deux doivent être l'inverse l'un de l'autre.
- `table.increments('id')` : une clé primaire auto-incrémentée (`serial` en Postgres).
- `.references('id').inTable('games').onDelete('CASCADE')` : une **clé étrangère**. Supprimer une partie supprime ses lignes ici. Postgres refuse aussi un `game_id` qui n'existe pas.
- `table.unique(['game_id', 'user_id'])` : un joueur ne peut pas être deux fois dans la même partie. La contrainte est en **base**, donc elle tient même si le code oublie de vérifier.
- `table.timestamps(true, true)` : `created_at` et `updated_at`, avec `now()` par défaut.
- Le nom du fichier commence par un horodatage (`1794910809997_…`). Lucid joue les migrations **dans cet ordre** et note dans la table `adonis_schema` celles qui ont déjà tourné.

🚨 **Ne modifie jamais une migration déjà jouée.** Lucid ne la rejoue pas : ta modification n'atteint pas les bases existantes. L'équipe en a fait l'expérience : `create_users_table` a été modifiée après coup (`full_name` remplacé par `nickname`). Il a fallu une migration de rattrapage, `alter_users_table`, qui teste si la colonne `full_name` existe encore et, si oui, **vide la table `users`** (`TRUNCATE … CASCADE`) avant de la corriger. Ici c'étaient des comptes de test. Sur des vrais comptes, la seule bonne voie est une **nouvelle** migration qui transforme les données.

🔗 **Lien C.** Une migration ressemble à une étape de `git log` : on ne réécrit pas un commit déjà poussé, on en ajoute un qui corrige. `adonis_schema` joue le rôle de l'historique.

---

## 2. Le schéma généré et le modèle

`migration:run` écrit `database/schema.ts` ([`INSTALLATION`](INSTALLATION.md), §5). Extrait :

```ts
export class UserSchema extends BaseModel {
  @column({ serializeAs: null })
  declare password: string
  @column()
  declare twoFactorSecret: string | null
  // id, nickname, email, avatarUrl, createdAt, updatedAt, isTwoFactorEnabled…
}
```

Le modèle de l'équipe hérite de cette classe :

```ts
export default class User extends compose(UserSchema, withAuthFinder(hash)) {
  static accessTokens = DbAccessTokensProvider.forModel(User)
  declare currentAccessToken?: AccessToken
  // relations, getters…
}
```

- `compose(UserSchema, withAuthFinder(hash))` : un **mixin**. On part de `UserSchema` et on ajoute `User.verifyCredentials(email, password)` et le hachage automatique du mot de passe à la sauvegarde ([`AUTH`](AUTH.md), §2).
- `static accessTokens = …` : le fournisseur de jetons, rangé **sur la classe**.
- Le modèle ne déclare **pas** ses colonnes : elles viennent de `UserSchema`. Ajouter une colonne demande donc une migration, puis `migration:run`, et **rien** à écrire dans le modèle.
- Les noms passent de `snake_case` en base (`avatar_url`) à `camelCase` en TypeScript (`avatarUrl`).

---

## 3. Requêtes courantes

```ts
const u = await User.find(42)                                  // null si absent
const u2 = await User.findOrFail(42)                           // lève une 404 dans une requête HTTP
const u3 = await User.findBy('nickname', 'alice')
const top = await User.query().where('id', '>', 10).orderBy('nickname').limit(20)
const n = await User.create({ nickname: 'bob', email: 'b@x.io', password: 'motdepasse' })
n.nickname = 'robert'
await n.save()
await n.delete()
```

- `User.query()` renvoie un **constructeur de requête** : rien ne part en base avant le `await`.
- `find` renvoie `null`, `findOrFail` **lève** une erreur. Dans un contrôleur, `findOrFail` devient un 404 tout seul ([`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md), §3).
- `create` et `save` déclenchent les hooks du modèle (le hachage du mot de passe, par exemple). Pas `User.query().update(...)`, qui écrit directement en base.

🚨 Une valeur reçue du client passée **sans validation** à `where` peut produire une erreur Postgres renvoyée telle quelle au client : `/games/abc` a donné un 500 contenant la requête SQL ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §4). Lucid paramètre les valeurs (`$1`), donc **pas** d'injection SQL, mais la forme reste à valider.

---

## 4. Les quatre formes de relation

| Forme | La clé étrangère est… | Exemple juste | Déclaration |
|---|---|---|---|
| `belongsTo` | dans **ma** table | `messages.room_id` → un message appartient à un salon | `@belongsTo(() => Room)` sur `Message` |
| `hasMany` | dans **l'autre** table, plusieurs lignes | un salon a plusieurs messages | `@hasMany(() => Message)` sur `Room` |
| `hasOne` | dans **l'autre** table, une ligne | une partie a un salon (`rooms.game_id`) | `@hasOne(() => Room)` sur `Game` |
| `manyToMany` | dans une **table pivot** | parties ↔ joueurs via `games_users` | `@manyToMany(() => User, { pivotTable: 'games_users' })` |

Règle pour choisir : **cherche où est la colonne `xxx_id`**.
- Dans ma table : `belongsTo`.
- Dans la table d'en face : `hasOne` ou `hasMany`.
- Dans une troisième table : `manyToMany`.

🔗 **Lien C.** C'est la question « qui tient le pointeur ? » d'une liste chaînée. Un `t_node` qui a un `parent` est en `belongsTo`. Un parent qui garde un tableau de pointeurs vers ses enfants n'existe pas en SQL : on stocke toujours le pointeur côté enfant, et `hasMany` est la **lecture inverse**.

---

## 5. Les relations de l'équipe, auditées

Test : un `preload` de chaque relation, sur une base vide (tests Japa, voir [`TESTS_JAPA`](TESTS_JAPA.md)). Lucid vérifie la forme de la relation **avant** d'exécuter la requête : l'erreur apparaît même sans aucune ligne.

```
ERREUR User.friends → Relation "User.friends" expects "userId" to exist on "User" model, but is missing. Did you forget to define the column?
ERREUR Game.users   → Relation "Game.users" expects "gameId" to exist on "User" model, …
ERREUR User.game    → Relation "User.game" expects "userId" to exist on "Game" model, …
ERREUR Room.game    → Relation "Room.game" expects "roomId" to exist on "Game" model, …
ERREUR Card.games   → Relation "Card.games" expects "cardId" to exist on "Game" model, …
OK     Game.room · Room.messages · Message.room · Message.user · Card.family · Family.cards
```

Lecture du premier message : `hasMany(() => User)` sur `User` veut dire « chaque `User` ami a une colonne `user_id` qui pointe vers moi ». Cette colonne n'existe pas : les amitiés sont dans la table `friends`.

| Relation | Déclarée | La colonne est… | Correction |
|---|---|---|---|
| `User.friends` | `hasMany(User)` | dans `friends` (pivot) | `manyToMany` sur `friends` |
| `Game.users` | `hasMany(User)` | dans `games_users` (pivot) | `manyToMany` sur `games_users` |
| `User.game` | `hasOne(Game)` | dans `games_users` (pivot) | `manyToMany` → `User.games` |
| `Room.game` | `hasOne(Game)` | `rooms.game_id`, dans **ma** table | `belongsTo(Game)` |
| `Card.games` | `hasMany(Game)` | dans `games_cards` (pivot) | `manyToMany` sur `games_cards` |

Le modèle `User` corrigé, en entier (typechecké, tous les `preload` passent) :

```ts
import { UserSchema } from '#database/schema'
import hash from '@adonisjs/core/services/hash'
import { compose } from '@adonisjs/core/helpers'
import { withAuthFinder } from '@adonisjs/auth/mixins/lucid'
import { type AccessToken, DbAccessTokensProvider } from '@adonisjs/auth/access_tokens'
import { column, manyToMany } from '@adonisjs/lucid/orm'
import type { ManyToMany } from '@adonisjs/lucid/types/relations'
import Game from '#models/game'

export default class User extends compose(UserSchema, withAuthFinder(hash)) {
  static accessTokens = DbAccessTokensProvider.forModel(User)
  declare currentAccessToken?: AccessToken

  @manyToMany(() => Game, {
    pivotTable: 'games_users',
    pivotColumns: ['score'],
    pivotTimestamps: true,
  })
  declare games: ManyToMany<typeof Game>

  @manyToMany(() => User, {
    pivotTable: 'friends',
    pivotForeignKey: 'user_id',
    pivotRelatedForeignKey: 'friend_id',
    pivotTimestamps: true,
  })
  declare friends: ManyToMany<typeof User>

  @column()
  declare twoFactorSecret: string | null

  @column()
  declare isTwoFactorEnabled: boolean

  get initials() {
    return this.nickname
      ? this.nickname.substring(0, 2).toUpperCase()
      : this.email.substring(0, 2).toUpperCase()
  }
}
```

- `pivotTable: 'games_users'` est **obligatoire**. Sans lui, Lucid devine `game_user` (les deux modèles au singulier, par ordre alphabétique). Observé : `relation "game_user" does not exist`.
- `pivotColumns: ['score']` : la colonne de la table pivot est lue avec chaque joueur, dans `user.$extras.pivot_score`.
- `pivotForeignKey` et `pivotRelatedForeignKey` sont nécessaires pour `friends`, parce que les deux côtés sont des `User`. Lucid devinerait `user_id` des deux côtés.
- `pivotTimestamps: true` remplit `created_at` et `updated_at` de la table pivot à chaque `attach`.

Les deux autres corrections, en entier :

```ts
import { RoomSchema } from '#database/schema'
import { belongsTo, hasMany } from '@adonisjs/lucid/orm'
import type { BelongsTo, HasMany } from '@adonisjs/lucid/types/relations'
import Game from '#models/game'
import Message from '#models/message'

export default class Room extends RoomSchema {
  @belongsTo(() => Game)
  declare game: BelongsTo<typeof Game>
  @hasMany(() => Message)
  declare messages: HasMany<typeof Message>
}
```

```ts
import { CardSchema } from '#database/schema'
import { belongsTo, manyToMany } from '@adonisjs/lucid/orm'
import type { BelongsTo, ManyToMany } from '@adonisjs/lucid/types/relations'
import Family from '#models/family'
import Game from '#models/game'

export default class Card extends CardSchema {
  @belongsTo(() => Family)
  declare family: BelongsTo<typeof Family>
  @manyToMany(() => Game, { pivotTable: 'games_cards', pivotTimestamps: true })
  declare games: ManyToMany<typeof Game>
}
```

Et dans `Game` : `@manyToMany(() => User, { pivotTable: 'games_users', pivotColumns: ['score'], pivotTimestamps: true }) declare users: ManyToMany<typeof User>`.

🚨 **TypeScript ne voit rien.** `@hasMany(() => User)` compile parfaitement : c'est une relation valide en soi, juste pas avec ces tables. L'erreur n'arrive qu'au premier `preload`, `related()` ou `load()`. Un test qui charge chaque relation ([`TESTS_JAPA`](TESTS_JAPA.md), §5) l'attrape avant la soutenance.

---

## 6. Travailler avec une table pivot

Extrait d'un test qui passe, avec les modèles corrigés :

```ts
const game = await Game.create({})
await game.related('users').attach([ana.id, bea.id])

await game.related('users').pivotQuery().where('user_id', ana.id).update({ score: 3 })

const relue = await Game.query()
  .where('id', game.id)
  .preload('users', (q) => q.orderBy('pivot_score', 'desc'))
  .firstOrFail()
// relue.users → ana:3 bea:0

const parties = await ana.related('games').query()
// 1

await ana.related('friends').attach([bea.id])
await ana.load('friends')
await bea.load('friends')
// ana.friends → ['bea']   bea.friends → []
```

- `attach([ids])` insère dans la pivot. `attach({ [id]: { score: 7 } })` remplit aussi les colonnes en plus.
- `pivotQuery()` : une requête **sur la table pivot** elle-même, pour modifier un score.
- `preload('users', (q) => q.orderBy('pivot_score', 'desc'))` : le classement trié **par la base**.
- `preload` vs `load` : `preload` dans la requête qui charge les parties, `load` sur un modèle déjà en mémoire.

🚨 **Une amitié n'a qu'un sens.** `ana.related('friends').attach([bea.id])` insère **une** ligne `(user_id=ana, friend_id=bea)`. Observé : `ana.friends` contient `bea`, mais `bea.friends` est **vide**. Pour une amitié réciproque, il faut insérer les deux lignes (dans une transaction), ou chercher dans les deux colonnes. C'est un choix de conception à faire en équipe.

🚨 **N+1.** Une boucle `for (const g of games) await g.load('users')` lance une requête **par partie**. `Game.query().preload('users')` n'en lance que deux au total.

---

## 🧪 À essayer

1. Retire `pivotTable` de `Game.users` et appelle `attach`. Relis le nom de table dans l'erreur.
2. Écris le test d'audit du §5 (un `preload` par relation) sur la version de l'équipe, et compte les erreurs.
3. Ajoute une colonne `elo` à `users` avec une **nouvelle** migration. Lance `migration:run`, puis regarde le `git diff` de `database/schema.ts`.
4. Écris `addFriend(a, b)` qui insère les deux sens dans une transaction (`db.transaction(async (trx) => …)`), puis vérifie `a.friends` et `b.friends`.
