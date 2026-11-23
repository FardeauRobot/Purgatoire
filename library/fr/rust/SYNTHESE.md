# 📒 Synthèse : Rust

> **TL;DR.** La fiche de révision du dossier [`rust/`](../../en/rust/INDEX.md) (EN). Les trois règles (ownership, emprunt, durées de vie), puis les types, les erreurs, les traits et Cargo, chacun réduit à ce qu'il faut savoir par cœur, avec sa fiche et son paragraphe. Les blocs de code sont **recopiés tels quels** des fiches anglaises, commentaires anglais compris : [`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §2 et §3, [`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) §3, [`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §2 (la fonction seule, sans son `main`) et [`ERROR_HANDLING`](../../en/rust/notions/errors/ERROR_HANDLING.md) §2.

Liens : [`ABORDER`](ABORDER.md) · hub [`rust/INDEX`](../../en/rust/INDEX.md) (EN) · [`lexique`](../../en/rust/lexique/INDEX.md) (EN) · [`GLOSSAIRE`](../../en/rust/lexique/GLOSSAIRE.md) (EN)

Édition : **2024** ([`CARGO`](../../en/rust/notions/tooling/CARGO.md)). Pas encore couverts dans le vault : pointeurs intelligents, collections, itérateurs et closures en détail, concurrence, macros, async ([`notions/INDEX`](../../en/rust/notions/INDEX.md), « Coming soon »).

---

## 1. L'idée en cinq lignes

- **Le pari** : la plupart des bugs mémoire du C/C++ viennent de « qui possède cette mémoire, et qui la regarde ? ». Le compilateur y répond pour chaque valeur ([`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md), TL;DR).
- **Ownership** → qui libère ; **emprunt** → qui regarde ou modifie maintenant ; **durées de vie** → combien de temps un regard est valide ([`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §6).
- **Pas de null, pas d'exceptions** : `Option<T>` et `Result<T, E>`, que le type oblige à traiter.
- **Pas d'héritage** : des traits pour les interfaces, des enums pour les ensembles fermés, la composition pour les données.
- **Le compilateur est un mentor** : erreurs longues, codées (`rustc --explain E0382`), qui proposent souvent la correction ([`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) §2, pilier 4).

---

## 2. Ownership ([`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md))

```
 1. Each value has exactly one owner (a variable, a field, a Vec slot…).
 2. There can only be one owner at a time.
 3. When the owner goes out of scope, the value is dropped.
```

```rust,compile_fail
fn main() {
    let a = String::from("hi");
    let b = a;              // ownership moves from a to b
    println!("{a}");        // error[E0382]: borrow of moved value: `a`
}
```

| À savoir | § |
|---|---|
| Un *move* copie l'**en-tête de 3 mots** (pointeur, longueur, capacité) ; le tas n'est pas touché, la source est marquée morte | 3 |
| Les moves ont lieu à **chaque** passage par valeur : argument, retour, insertion dans un `Vec` | 3 |
| `Copy` : types « simples bits » (`i32`, `bool`, `char`, `&T`, une struct dont tous les champs sont `Copy`) ; `let b = a;` duplique | 4 |
| **`Copy` et `Drop` sont incompatibles** : copier un type à destructeur referait le double free | 4 |
| `.clone()` = constructeur de copie, mais **appelé par son nom** ; en saupoudrer = un emprunt manqué | 5 |
| Ordre de drop : variables locales en ordre **inverse** de déclaration, comme en C++ ; `drop(x)` libère tout de suite | 7 |
| valgrind rarement utile (FFI, `unsafe`) ; Miri pour le code `unsafe` | 8 |

### La forme canonique, côte à côte ([`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §6)

| OCF (C++98) | Rust |
|---|---|
| Constructeur par défaut | `fn new() -> Self` ou `#[derive(Default)]` |
| Constructeur de copie | `#[derive(Clone)]` / `impl Clone`, appelé explicitement |
| `operator=` | `b = a.clone();` (l'ancien `b` est libéré d'abord) |
| Destructeur | `impl Drop`, seulement pour les ressources autres que la mémoire |
| « Ai-je oublié un des quatre ? » | Impossible : par défaut, *move* et *drop*, toujours corrects |

---

## 3. Emprunt ([`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md))

**Aliasing XOR mutation** : à tout instant, plusieurs `&T` **ou** un seul `&mut T`, jamais les deux (§3).

```rust,compile_fail
fn main() {
    let mut v = vec![1, 2, 3];
    let first = &v[0];          // shared borrow of v...
    v.push(4);                  // error[E0502]: cannot borrow `v` as mutable because it is also borrowed as immutable
    println!("{first}");        // ...still in use here
}
```

C'est l'**invalidation d'itérateurs** de `std::vector` : `push` peut réallouer, et C++98 compile sans rien dire (§3).

| À savoir | § |
|---|---|
| **NLL** : un emprunt finit à sa **dernière utilisation**, pas à l'accolade ; déplacer une ligne suffit souvent | 4 |
| E0502 : finir de lire avant d'écrire, ou copier la valeur (`let x = v[0];`) | 5 |
| E0499 (deux `&mut`) : `split_at_mut`, ou emprunter des **champs différents** (autorisé) | 5 |
| E0507 (sortir une valeur d'un `&`) : cloner, `std::mem::take`, ou changer la signature | 5 |
| E0597 (« does not live long enough ») : le propriétaire meurt avant | 5 |
| Un *slice* (`&[T]`, `&str`) = pointeur + longueur, vérifié ; en paramètre, **`&str` et `&[T]`**, pas `&String` / `&Vec<T>` | 6 |
| La même règle interdit les data races (« fearless concurrency ») ; le détail est sur la page `CONCURRENCY`, à venir | 7 |

---

## 4. Durées de vie ([`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md))

```rust
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() >= y.len() { x } else { y }
}
```

Le résultat est valide **tant que les deux entrées le sont** : `'a` est le **chevauchement** des deux emprunts (§2).

| À savoir | § |
|---|---|
| Renvoyer `&local` : **E0106** (« missing lifetime specifier ») ; on renvoie la valeur possédée | 1 |
| Élision, règle 1 : chaque paramètre référence a sa propre durée de vie | 3 |
| Règle 2 : **une seule** durée de vie en entrée → la sortie la prend | 3 |
| Règle 3 : un `&self` / `&mut self` → la sortie prend celle de `self` | 3 |
| `longest` a besoin de `'a` : deux références en entrée, pas de `self` | 3 |
| Une struct qui tient une référence : `struct Parser<'a>` ; en apprenant, fais **posséder** (`String`) | 4 |
| `&'static str` : valide tout le programme (littéraux, `static`, mémoire volontairement fuitée) | 5 |
| `T: 'static` : T ne contient **aucun emprunt non statique** ; `String` le satisfait. D'où le `move` vers `thread::spawn` | 5 |
| Les durées de vie **décrivent**, elles ne prescrivent pas : on ne corrige pas en ajoutant des `'a` | 6 |

---

## 5. Types et syntaxe ([`TYPES`](../../en/rust/notions/fundamentals/TYPES.md), [`lexique`](../../en/rust/lexique/INDEX.md))

| À savoir | Fiche § |
|---|---|
| `i32` et `f64` par défaut ; **`usize` pour les index et longueurs** ; `u8` est un octet, pas un caractère ; `char` fait **4 octets** ; `()` ≈ `void` | [`TYPES`](../../en/rust/notions/fundamentals/TYPES.md) §1 |
| Dépassement : **panique en debug, boucle en release** ; au choix `checked_`, `wrapping_`, `saturating_`, `overflowing_` | §2 |
| `String` possède (tas), `&str` est une vue empruntée ; `s[i]` ne compile pas (E0277, UTF-8 de 1 à 4 octets) : `s.chars().nth(i)` | §3 |
| `[T; N]` taille fixe / `Vec<T>` extensible / `&[T]` vue ; `v[i]` **panique** hors limites, `v.get(i)` renvoie `Option` | §4 |
| Immuable par défaut : `let mut` pour modifier | [`LET_MUT`](../../en/rust/lexique/LET_MUT.md) |
| La dernière expression **sans `;`** est la valeur ; avec `;` le bloc vaut `()` (E0308) | [`FN`](../../en/rust/lexique/FN.md) §2 |
| `if` est une expression ; `for` ne parcourt **que des itérateurs** | [`CONTROL_FLOW`](../../en/rust/lexique/CONTROL_FLOW.md) §2, §5 |
| `match` doit être **exhaustif** (E0004) ; éviter le `_` réflexe sur ses propres enums | [`MATCH`](../../en/rust/lexique/MATCH.md) §3 |
| Un seul cas : `if let` ; boucle : `while let` ; sortie anticipée : `let … else` ; un booléen : `matches!` | [`MATCH`](../../en/rust/lexique/MATCH.md) §4 |
| `Option<T>` remplace `NULL` | [`ENUM`](../../en/rust/lexique/ENUM.md) §3 |
| `&self` lit, `&mut self` modifie, `self` **consomme** l'objet ; `self.x` toujours écrit | [`IMPL_SELF`](../../en/rust/lexique/IMPL_SELF.md) §1 |

---

## 6. Erreurs ([`ERROR_HANDLING`](../../en/rust/notions/errors/ERROR_HANDLING.md))

```
  let a = expr?;
      ≡
  let a = match expr {
      Ok(v)  => v,
      Err(e) => return Err(e.into()),   // .into() converts to the function's error type
  };
```

| À savoir | § |
|---|---|
| `?` marche aussi sur `Option` dans une fonction qui renvoie `Option` | 2 |
| Une hiérarchie d'exceptions devient un **enum** d'erreurs + `impl Display` + `impl std::error::Error` (le `Bureaucrat` de CPP05) | 3 |
| `Result` : échec **attendu**, l'appelant peut réagir. `panic!` : **bug**, invariant cassé | 4 |
| Paniquent : index hors limites, dépassement en debug, `unwrap()` sur `None`, `panic!` / `unreachable!` / `todo!` | 4 |
| `.unwrap()` : tests et prototypes ; `.expect("pourquoi")` ; `.unwrap_or(défaut)` ; `?` pour propager | 5 |
| `fn main() -> Result<(), Box<dyn std::error::Error>>` ; `Err` → message et code de sortie 1 ; `anyhow` (applications) / `thiserror` (bibliothèques) | 6 |

---

## 7. Traits et génériques ([`TRAITS_GENERICS`](../../en/rust/notions/traits/TRAITS_GENERICS.md))

| À savoir | § |
|---|---|
| `fn max<T: PartialOrd>(a: T, b: T) -> T` : la contrainte est **dans la signature** ; l'erreur tombe à l'appel, en une ligne | 1 |
| Générique = une copie par type, appel direct (≈ template) ; `dyn Trait` = une copie, vtable (≈ `Base*` + `virtual`). Choisi **à chaque usage** | 3 |
| `derive` : `Debug`, `Clone`, `Copy`, `Default`, `PartialEq`, `PartialOrd`, `Hash`… ; à écrire : `Display`, `Drop`, `Add`, `Index`, `From`, `Iterator` | 4 |
| `Fixed` de CPP02 = une ligne de `derive` et deux petits `impl` (`Add`, `Display`) | 4 |
| `Iterator` a un **type associé** (`type Item`) | 5 |
| Pas d'héritage : interface → trait, code commun → méthodes par défaut, données → composition, ensemble fermé → enum. Pas de diamant (CPP03) | 6 |
| Règle orpheline (« only traits defined in the current crate… ») : envelopper le type étranger dans un *newtype* | 7 |

Choisir ([`TRAIT_DYN`](../../en/rust/lexique/TRAIT_DYN.md) §5) : type connu à la compilation → générique / `impl Trait` ; collection hétérogène → `Box<dyn Trait>` ; ensemble fermé que tu contrôles → souvent un **enum**.

---

## 8. Cargo ([`CARGO`](../../en/rust/notions/tooling/CARGO.md))

| Makefile 42 | Cargo |
|---|---|
| copier un template | `cargo new my_app` |
| `make` | `cargo build` (→ `target/debug/`) |
| `make && ./my_app` | `cargo run -- arg1 arg2` |
| `-O2` | `cargo build --release` |
| `make fclean` | `cargo clean` |
| `-Wall -Wextra -Werror` | `cargo clippy -- -D warnings` |
| `norminette` | `cargo fmt` (reformate au lieu de se plaindre) |
| libft en sous-module | `cargo add some_crate` (verrouillé dans `Cargo.lock`) |
| un `main.cpp` de test | `cargo test` (tests à côté du code) |

La commande la plus utilisée : **`cargo check`**, vérification complète sans génération de code, environ une seconde (§3). Pour apprendre ensuite : `rustup doc --book`, Rustlings (§6).

---

## 🧠 Questions de révision

> [!question]- Que copie exactement `let b = a;` quand `a` est une `String` ?
> L'en-tête de 3 mots (pointeur, longueur, capacité). Les données du tas ne bougent pas ; `a` est marqué mort et aucun destructeur ne tourne pour lui. ([`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §3)

> [!question]- `let _a = Tag("a"); let _b = Tag("b"); let moved = Tag("c"); drop(moved);` puis fin de `main`. Dans quel ordre s'affichent les `drop` ?
> `drop c` au moment de `drop(moved)`, puis `drop b`, puis `drop a` : ordre inverse de déclaration. ([`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §7)

> [!question]- Peut-on prendre deux `&mut` en même temps sur `p.hp` et `p.mana` ?
> Oui : ce sont des champs différents. Pour deux moitiés d'un tableau, `split_at_mut`. ([`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) §5)

> [!question]- Pourquoi `fn sum(xs: &[i32])` plutôt que `fn sum(xs: &Vec<i32>)` ?
> `&[i32]` accepte un `Vec`, un tableau et une sous-plage (`&v[1..3]`), pour le même coût. ([`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) §6)

> [!question]- `fn first_word(s: &str) -> &str` compile sans `'a`. Pourquoi, alors que `longest` en a besoin ?
> Règle d'élision 2 : une seule durée de vie en entrée, la sortie la prend. `longest` a deux références en entrée et pas de `self`. ([`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §3)

> [!question]- `String` satisfait-elle la borne `T: 'static` ?
> Oui : elle possède ses données et ne contient aucun emprunt. `'static` en borne ne veut pas dire « vit toujours ». ([`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §5)

> [!question]- `let x: u8 = 250; x + 10` : que se passe-t-il en debug, et en release ?
> Debug : panique « attempt to add with overflow ». Release : boucle à 4. Pour choisir, `checked_add`, `wrapping_add`, `saturating_add`. ([`TYPES`](../../en/rust/notions/fundamentals/TYPES.md) §2)

> [!question]- Que fait `?` sur un `Err(e)` ?
> Il renvoie `Err(e.into())` tout de suite : `.into()` convertit vers le type d'erreur de la fonction. ([`ERROR_HANDLING`](../../en/rust/notions/errors/ERROR_HANDLING.md) §2)

> [!question]- Tu ajoutes une variante à un enum. Pourquoi est-ce sans danger ?
> Chaque `match` qui ne la traite pas devient une erreur E0004, à condition de ne pas avoir mis de `_` par réflexe. ([`MATCH`](../../en/rust/lexique/MATCH.md) §3)

> [!question]- Un `Vec` qui mélange cercles et carrés : générique ou `dyn` ?
> `Vec<Box<dyn Shape>>` : un générique ne mélange pas les types dans un même `Vec`. ([`TRAITS_GENERICS`](../../en/rust/notions/traits/TRAITS_GENERICS.md) §3)
