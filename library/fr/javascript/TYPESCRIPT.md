# 🔷 TypeScript

> **TL;DR.** TypeScript = JavaScript + annotations de type (`let pv: number`). Le compilateur repère les erreurs **avant** l'exécution, puis efface les types. On décrit les données avec `type` / `interface`, on combine avec les **unions** (`"feu" | "eau"`), on réutilise avec les **génériques** (`Array<T>`, comme les templates). Active toujours `"strict": true`. Et retiens : **à l'exécution, plus aucun type n'existe** → toute donnée venant de l'extérieur doit être **validée**.

Liens : [`INDEX`](INDEX.md) · [`PHILOSOPHIE`](PHILOSOPHIE.md) · [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) · [`VERS_TRANSCENDANCE`](VERS_TRANSCENDANCE.md)

---

## 1. Annoter

```ts
let pv: number = 30;
const nom: string = "Aura";
let actif: boolean = true;
const main: string[] = ["feu", "eau"];

function degats(attaque: number, defense: number): number {
    return Math.max(0, attaque - defense);
}

degats(7, "2");   // ❌ erreur de compilation : "2" n'est pas un number
```

La plupart du temps, tu n'as **pas besoin** d'annoter les variables : `let pv = 30` est déduit `number` (*inférence*). Annote surtout les **paramètres de fonctions** et les **structures de données**.

---

## 2. Décrire des objets : `type` et `interface`

```ts
type ElementCarte = "feu" | "eau" | "air" | "terre";   // union de littéraux

interface Carte {
    id: number;
    nom: string;
    element: ElementCarte;
    attaque: number;
    description?: string;     // ? = champ optionnel (string | undefined)
    readonly cout: number;    // ne peut pas être modifié après création
}

const c: Carte = { id: 1, nom: "Aura", element: "feu", attaque: 7, cout: 3 };
c.element = "lave";   // ❌ "lave" ne fait pas partie de ElementCarte
c.cout = 0;           // ❌ readonly
```

- **`interface`** et **`type`** font presque la même chose pour décrire un objet. Convention fréquente : `interface` pour les objets, `type` pour les unions et le reste. Choisis-en une en équipe et tiens-t'y.
- L'union `"feu" | "eau" | …` est l'équivalent d'un `enum` C, mais avec des chaînes lisibles directement dans le JSON.

---

## 3. Unions et *narrowing*

```ts
type Message =
    | { type: "jouer"; carteId: number }
    | { type: "abandon" }
    | { type: "chat"; texte: string };

function traiter(msg: Message) {
    switch (msg.type) {
        case "jouer":
            console.log("carte", msg.carteId);   // ici TS sait que carteId existe
            break;
        case "chat":
            console.log(msg.texte);
            break;
        case "abandon":
            break;
    }
}
```

C'est le **modèle idéal pour les messages WebSocket** de ton jeu : un champ `type` qui dit de quel message il s'agit, et TypeScript **réduit** (*narrows*) le type dans chaque `case`. **Lien :** une `union` C avec un tag, mais vérifiée par le compilateur — c'est l'esprit du `-Wswitch` de tes Makefiles.

Autres façons de réduire un type :

```ts
function afficher(x: string | null) {
    if (x === null) return;        // après ce test, x est string
    console.log(x.toUpperCase());
}
```

---

## 4. Génériques : les templates de TypeScript

```ts
function premier<T>(tab: T[]): T | undefined {
    return tab[0];
}
premier([1, 2, 3]);         // T = number
premier(["a", "b"]);        // T = string

interface Reponse<T> {
    ok: boolean;
    donnees: T;
}
const r: Reponse<Carte[]> = { ok: true, donnees: [] };
```

**Lien CPP07 :** `template <typename T>` → `<T>`. Même idée, mais rien n'est généré : c'est seulement vérifié puis effacé.

---

## 5. `any`, `unknown` et `!`

```ts
let a: any = JSON.parse(texte);       // ❌ désactive TOUTE vérification
let u: unknown = JSON.parse(texte);   // ✅ « je ne sais pas encore » : il faut vérifier avant d'utiliser
const el = document.getElementById("jeu")!;   // ! = « fais-moi confiance, ce n'est pas null »
```

- **`any`** = retour au JavaScript sans filet. À bannir (et `strict` t'aide à le repérer).
- **`unknown`** = le bon type pour une donnée externe : TS t'oblige à la valider.
- **`!`** (*non-null assertion*) : tu mens au compilateur si tu te trompes. Utilise-le avec parcimonie.

---

## 6. Le point crucial : les types disparaissent

```ts
interface Action { carteId: number }

// côté serveur, à la réception d'un message :
const action = JSON.parse(message) as Action;   // ⚠️ « as » ne vérifie RIEN
console.log(action.carteId + 1);                // si le client a envoyé "abc"… → "abc1"
```

`as Action` dit seulement au compilateur « crois-moi ». Si un client (ou un tricheur avec la console du navigateur) envoie n'importe quoi, ton serveur le recevra tel quel.

Le sujet l'exige : **toutes les saisies validées côté frontend ET côté backend**. Côté backend, on passe par une bibliothèque de validation qui vérifie la forme à l'exécution **et** produit le type TypeScript. AdonisJS fournit **VineJS** :

```ts
import vine from "@vinejs/vine";

export const actionValidator = vine.compile(
    vine.object({
        carteId: vine.number().positive(),
    })
);
// dans un contrôleur : const action = await request.validateUsing(actionValidator);
// → action est typé { carteId: number } ET vérifié pour de vrai
```

(`zod` est l'équivalent le plus répandu hors AdonisJS, pratique pour les messages WebSocket.)

---

## 7. `tsconfig.json`

Ton projet en aura un, généré par AdonisJS / Vite. L'option qui compte :

```json
{
  "compilerOptions": {
    "strict": true
  }
}
```

`strict` active notamment la vérification des `null`/`undefined` et interdit les `any` implicites. Sans lui, TypeScript laisse passer la moitié des bugs. **Lien :** c'est ton `-Wall -Wextra -Werror`.

---

## ⚠️ À retenir

- Annote les paramètres et les structures ; laisse l'inférence faire le reste.
- Unions avec un champ `type` pour les messages → `switch` sûr.
- `any` interdit, `unknown` pour l'externe, `as` ne vérifie rien.
- Les types n'existent plus à l'exécution → **valide** toute entrée côté backend.
- `"strict": true`, toujours.
