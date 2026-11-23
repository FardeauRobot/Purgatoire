# 2. Valider les entrées avec VineJS

> **TL;DR.** On décrit ce qu'on attend avec `vine.create({ champ: vine.string().minLength(3)… })` dans `app/validators/`, puis on écrit `await request.validateUsing(monValidateur)` dans le contrôleur. Deux issues possibles :
> - les données sont bonnes : on récupère un objet **typé et nettoyé** (champs inconnus retirés, `trim` appliqué, `"26"` converti en `26`) ;
> - elles ne le sont pas : Adonis répond **422** avec la liste de **toutes** les erreurs, et le code du contrôleur ne s'exécute pas.
>
> `unique` et `exists` interrogent la base. Les messages se traduisent avec un `SimpleMessagesProvider`.

Liens : [`INDEX`](INDEX.md) · [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) · [`AUTH`](AUTH.md) · [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) · [`../react/REFS_FORMULAIRES`](../react/REFS_FORMULAIRES.md) · [doc VineJS](https://vinejs.dev/docs/introduction)

🧪 Labo : aucun. Toutes les réponses viennent de `curl` sur une copie du backend. Le validateur `createGameValidator` a été écrit pour la fiche. Les autres sont ceux de l'équipe.

---

## 1. Le validateur d'inscription de l'équipe

```ts
import vine from '@vinejs/vine'

const email = () => vine.string().email().maxLength(254)
const password = () => vine.string().minLength(8).maxLength(32)

export const signupValidator = vine.create({
  nickname: vine
    .string()
    .trim()
    .minLength(3)
    .maxLength(20)
    .alphaNumeric()
    .unique({ table: 'users', column: 'nickname' }),
  email: email().unique({ table: 'users', column: 'email' }),
  password: password(),
  passwordConfirmation: password().sameAs('password'),

  avatar: vine
    .file({
      size: '12mb',
      extnames: ['jpg', 'png', 'jpeg'],
    })
    .optional(),
})
```

Ligne par ligne :
- `const email = () => vine.string()…` : une **fonction** qui fabrique la règle, pas une règle partagée. Chaque appel donne une chaîne de règles neuve. On peut donc écrire `email().unique(…)` ici sans ajouter `unique` à l'e-mail du login.
- `vine.string()` : le champ est **obligatoire** et doit être une chaîne. Il n'existe pas de « facultatif par défaut ».
- `.trim()` : c'est une **transformation**, pas un contrôle. `"  zed99  "` devient `"zed99"` dans l'objet renvoyé (observé), **avant** que `minLength` et `unique` ne s'appliquent.
- `.alphaNumeric()` : lettres et chiffres seulement. Pas d'espace, pas de `_`.
- `.unique({ table, column })` : une requête `SELECT` dans `users`. La règle ne tourne que si les règles **précédentes** sont passées : on n'interroge pas la base pour un pseudo de 2 lettres.
- `.sameAs('password')` : compare avec un autre champ du même objet.
- `vine.file({...}).optional()` : un fichier multipart, absent autorisé. Le type de sortie devient `MultipartFile | undefined`.

Et dans le contrôleur :

```ts
const { nickname, email, password, avatar } = await request.validateUsing(signupValidator)
```

`nickname` est typé `string`, et `avatar` est typé `MultipartFile | undefined`. Le type vient du validateur, sans rien écrire à la main.

🔗 **Lien C.** C'est le parsing de `cub3D` ou de `so_long` : on **refuse** la carte invalide avant de lancer le jeu, et on liste ce qui ne va pas. La différence : Vine collecte **toutes** les erreurs au lieu de s'arrêter à la première.

---

## 2. Ce que le front reçoit

Observé, un appel par ligne :

```
{} ou rien                                      → 422 {"errors":[{"message":"The nickname field must be defined","rule":"required","field":"nickname"}, …]}
nickname "al ice!", email "pas-un-mail"         → 422 {"errors":[{"message":"The nickname field must contain only letters and numbers","rule":"alphaNumeric","field":"nickname"},
                                                              {"message":"The email field must be a valid email address","rule":"email","field":"email"}]}
password "court"                                → 422 {"message":"The password field must have at least 8 characters","rule":"minLength","field":"password","meta":{"min":8}}
pseudo et e-mail déjà pris                      → 422 {"message":"The nickname has already been taken","rule":"database.unique","field":"nickname"}, …
mots de passe différents                        → 422 {"message":"The passwordConfirmation field and password field must be the same","rule":"sameAs","field":"passwordConfirmation","meta":{"otherField":"password"}}
nickname ""                                     → 422 {"message":"The nickname field must be defined","rule":"required","field":"nickname"}
nickname "  zed99  " et "isAdmin": true en plus → 200, "nickname":"zed99", isAdmin ignoré
```

- **Une entrée par erreur**, tous champs confondus. Le front peut afficher chaque message sous son champ grâce à `field` ([`../react/REFS_FORMULAIRES`](../react/REFS_FORMULAIRES.md)).
- `rule` est stable, `message` ne l'est pas (on peut le traduire, §5). Pour **décider** quelque chose dans le code du front, teste `rule`, pas `message`.
- `nickname: ""` est rapporté comme **absent** (`required`) et non comme trop court. La raison : `convertEmptyStringsToNull: true` dans `config/bodyparser.ts` change `""` en `null` **avant** Vine.
- Les champs non déclarés (`isAdmin`) **n'apparaissent pas** dans l'objet validé. C'est la protection contre l'*assignation de masse* : même avec `User.create(payload)`, un client ne peut pas se donner un rôle.

🚨 **Le login de l'équipe ne valide pas.** `loginValidator` est défini dans `app/validators/user.ts`, mais `AccessTokensController.store` fait `request.only(['email', 'password'])`. Conséquence observée : un corps vide renvoie **400** `{"errors":[{"message":"Invalid user credentials"}]}` au lieu d'un 422 qui nomme les champs. Dans la copie, en remplaçant la ligne par `await request.validateUsing(loginValidator)`, le même appel renvoie **422** `The email field must be defined` / `The password field must be defined`. Le changement a aussi un effet sur le **type** du corps côté front ([`TUYAU`](TUYAU.md), §3).

---

## 3. Un validateur qui interroge la base : `exists`

Écrit pour `GamesController.store` ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §3) :

```ts
import vine, { SimpleMessagesProvider } from '@vinejs/vine'

export const createGameValidator = vine.create({
  adversaireId: vine.number().positive().exists({ table: 'users', column: 'id' }),
})

createGameValidator.messagesProvider = new SimpleMessagesProvider(
  {
    'required': 'Le champ {{ field }} est obligatoire.',
    'number': 'Le champ {{ field }} doit être un nombre.',
    'database.exists': 'Ce joueur n’existe pas.',
  },
  { adversaireId: 'adversaire' }
)
```

- `vine.number()` accepte `26` **et** `"26"` (observé : `{"adversaireId":"26"}` → 201), et renvoie un `number`. Un formulaire HTML envoie toujours des chaînes, donc c'est voulu.
- `.positive()` évite une requête en base pour `-1`.
- `.exists({ table: 'users', column: 'id' })` : le pendant de `unique`. La valeur **doit** se trouver en base.
- Le `SimpleMessagesProvider` prend deux objets : les messages **par règle**, et les noms affichés **par champ** (`{{ field }}` devient `adversaire`).

Observé, avant et après le `messagesProvider` :

```
{}                            → "The adversaireId field must be defined"   →  "Le champ adversaire est obligatoire."
{"adversaireId":"vingt"}      → "The adversaireId field must be a number"   →  "Le champ adversaire doit être un nombre."
{"adversaireId":999999}       → "The selected adversaireId is invalid"      →  "Ce joueur n’existe pas."
```

`rule` et `field` ne changent pas : seul `message` est traduit.

🚨 **`exists` ne vérifie que l'existence.** « L'adversaire existe » ne veut pas dire « je peux jouer contre lui ». Le cas « contre moi-même » est testé **dans le contrôleur**, après la validation, parce qu'il dépend de l'utilisateur connecté. Vine valide la **forme** des données, le contrôleur décide de ce qui est **permis**.

---

## 4. Un validateur qui dépend de l'utilisateur

```ts
export const updateProfileValidator = (userId: number) =>
  vine.create({
    nickname: vine
      .string()
      .trim()
      .minLength(3)
      .maxLength(20)
      .alphaNumeric()
      .unique(async (db, value) => {
        const user = await db
          .from('users')
          .where('nickname', value)
          .whereNot('id', userId)
          .first()
        return !user
      })
      .optional(),
  })
```

Utilisé ainsi : `request.validateUsing(updateProfileValidator(user.id))`.

- Le cas à gérer : quand je garde **mon** pseudo, `unique` le trouve en base (c'est le mien) et refuse. Le `whereNot('id', userId)` m'exclut de la recherche.
- `unique(async (db, value) => …)` : la forme **callback**. On écrit la requête soi-même et on renvoie `true` si la valeur est libre.
- La fonction `(userId) => vine.create(…)` **recompile** le validateur à chaque requête. C'est simple et juste, mais Vine propose aussi un `meta` passé à la validation pour éviter cette recompilation (non testé ici).
- `.optional()` : un `PATCH` peut ne rien changer.

🔗 **Lien C.** C'est une fonction qui **renvoie** une fonction configurée, comme une fabrique qui capture `userId`. En C, on passerait `userId` dans un `void *ctx` à côté du pointeur de fonction.

---

## 5. Quelques règles utiles pour Transcendence

```ts
import vine from '@vinejs/vine'

export const exemples = vine.create({
  pseudo: vine.string().trim().minLength(3).maxLength(20).alphaNumeric(),
  mode: vine.enum(['classique', 'rapide'] as const),
  manche: vine.number().withoutDecimals().range([1, 10]),
  message: vine.string().trim().minLength(1).maxLength(500),
  invites: vine.array(vine.number().positive()).maxLength(3).distinct(),
  public: vine.boolean().optional(),
})
```

- `vine.enum([...] as const)` : le type de sortie est `'classique' | 'rapide'`, pas `string`.
- `vine.boolean()` accepte `true`, `"true"`, `1`, `"on"`… : c'est ce qu'envoie une case à cocher.
- `vine.array(…).distinct()` : pas deux fois le même invité.

Observé avec `exemples.validate(...)` lancé directement sous Node :

```
{pseudo:" bob ", mode:"rapide", manche:"3", message:"yo", invites:[1,2], public:"on"}
  → {"pseudo":"bob","mode":"rapide","manche":3,"message":"yo","invites":[1,2],"public":true}

{pseudo:"bob", mode:"lent", manche:2.5, message:"x", invites:[1,1]}
  → "The selected mode is invalid" (enum, meta.choices), "The manche field must be an integer" (withoutDecimals),
    "The invites field has duplicate values" (distinct)
```

`validate()` s'utilise aussi **hors** d'une requête HTTP. On le retrouvera pour valider ce qui arrive par Socket.IO ([`SOCKET_IO`](SOCKET_IO.md), §5).

---

## 🧪 À essayer

1. Envoie `{"nickname":"abc","email":"a@b.c","password":"12345678","passwordConfirmation":"12345678","avatar":"x"}` en JSON. Quelle règle refuse `avatar` ?
2. Dans `config/bodyparser.ts`, passe `convertEmptyStringsToNull` à `false` et renvoie `nickname: ""`. Quelle règle répond maintenant ?
3. Branche `loginValidator` dans `AccessTokensController.store` (une ligne), puis compare les réponses à un corps vide avant et après.
4. Traduis les messages du `signupValidator` en français avec un `SimpleMessagesProvider`.
