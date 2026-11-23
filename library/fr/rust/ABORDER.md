# 🪜 Aborder Rust

> **TL;DR.** Rust répond à une question que tu te poses depuis libft : **qui possède cette mémoire, et qui d'autre la regarde en ce moment ?** Le compilateur y répond pour chaque valeur, et refuse le code quand la réponse est mauvaise. Tu gardes la vitesse du C, sans ramasse-miettes et sans use-after-free, double free ni pointeur pendant. Ton bagage C / C++98 est **le** bon prérequis : pointeurs, pile et tas, RAII, références, templates, `virtual`. Ce qui est nouveau, c'est **l'ownership, l'emprunt et les durées de vie**, trois notions sans équivalent en C++. Aborde-les dans cet ordre, avec `cargo check` ouvert en permanence, et lis les erreurs du compilateur au lieu de les contourner.

Liens : [`SYNTHESE`](SYNTHESE.md) · hub [`rust/INDEX`](../../en/rust/INDEX.md) (EN) · [`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) (EN) · [`lexique`](../../en/rust/lexique/INDEX.md) (EN) · [`GLOSSAIRE`](../../en/rust/lexique/GLOSSAIRE.md) (EN)

> 🇬🇧 Toutes les fiches Rust du vault sont en anglais : les liens marqués **(EN)** pointent vers `en/rust/`. Cette fiche-ci n'a pas d'original anglais.

---

## 1. À quel besoin il répond

D'après [`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) §1 (EN), environ **70 % des failles de sécurité graves** des grosses bases C/C++ sont des bugs mémoire (chiffre publié par Microsoft et par l'équipe Chromium de Google). Tu les connais tous :

| Bug | Où tu l'as croisé | Ce que fait Rust |
|---|---|---|
| Use-after-free | `delete p; p->x;` | Ne compile pas : la valeur a été déplacée ou libérée |
| Double free | une classe avec un pointeur brut et sans forme canonique | Une valeur a **un** propriétaire, elle est libérée une fois |
| Référence pendante | renvoyer `&local` | Refusé par la vérification des durées de vie |
| Débordement de tableau | `tab[i]` hors limites | L'indexation est vérifiée et **panique** |
| Data race | Philosophers sans mutex | Refusé par `Send` / `Sync` et les règles d'emprunt |
| Déréférencement de `NULL` | `ptr->x` sur `NULL` | Il n'y a pas de null : `Option<T>`, qu'on doit vérifier |
| Lecture non initialisée | `int x; printf("%d", x);` | Ne compile pas |

Ce que Rust **n'empêche pas** : les bugs de logique, les interblocages, le dépassement d'entier en build release (il boucle), les fuites par cycles `Rc` ou `mem::forget`, et ce que tu écris de faux dans un bloc `unsafe` ([`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) §1).

Le prix : il faut **convaincre le compilateur avant** que le programme existe. C'est un `-Wall -Wextra -Werror` poussé jusqu'au bout ([`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) §2, pilier 4).

À quoi ça sert en pratique : noyaux (Linux, Android, Windows), infrastructure cloud, outils en ligne de commande (`ripgrep`, `fd`, `bat`, `uv`), WebAssembly, embarqué ([`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) §4).

---

## 2. Ce que tu dois savoir avant

Tout le vault Rust est écrit pour quelqu'un qui connaît **le C et le C++98**. Chaque page part de « je sais comment C++ fait ça ; que fait Rust à la place, et pourquoi ? » ([`rust/INDEX`](../../en/rust/INDEX.md)). Ces prérequis-là ne sont donc pas optionnels : c'est la langue dans laquelle les fiches t'expliquent Rust.

### Indispensable : ton C et ton C++98

| Prérequis | Où Rust s'en sert | Où le revoir |
|---|---|---|
| Pile et tas, `malloc` / `free`, `new` / `delete` | l'ownership remplace `delete` ; `String` = en-tête sur la pile + données sur le tas | [`MEMORY`](../../en/cpp/notions/fundamentals/MEMORY.md) (EN) |
| La forme canonique (OCF) et le double free qu'elle évite | le *move* par défaut la rend inutile | [`ORTHODOX_CANONICAL_FORM`](../../en/cpp/notions/oop/ORTHODOX_CANONICAL_FORM.md) (EN) |
| RAII : le destructeur libère en fin de portée | c'est la règle 3 de l'ownership, rendue **obligatoire** | [`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §2 (EN) |
| `const T&` / `T&` | deviennent `&T` / `&mut T`, vérifiés à la compilation | [`REFERENCE`](../../en/cpp/notions/fundamentals/REFERENCE.md) (EN) |
| L'invalidation d'itérateurs de `std::vector` | c'est **l'exemple** de la règle d'emprunt | [`ITERATORS`](../cpp/notions/advanced/ITERATORS.md) |
| `template<typename T>` | les génériques, avec les contraintes écrites dans la signature | [`TEMPLATES`](../../en/cpp/notions/advanced/TEMPLATES.md) (EN) |
| `virtual`, classes abstraites | `dyn Trait` ; pas d'héritage de données | [`POLYMORPHISM`](../../en/cpp/notions/oop/POLYMORPHISM.md) (EN) |
| `try` / `catch`, les exceptions de CPP05 | `Result<T, E>` et `?` | [`ERROR_MANAGEMENT`](../../en/cpp/notions/io-errors/ERROR_MANAGEMENT.md) (EN) |
| `std::string` vs `const char *` | `String` vs `&str` | [`STRING`](../../en/cpp/notions/fundamentals/STRING.md) (EN) |
| Un Makefile, `-Wall -Wextra -Werror` | Cargo et Clippy | [`MAKEFILE_CPP`](../../en/cpp/notions/tooling/MAKEFILE_CPP.md) (EN) |

Le fil commun : **chaque fois qu'un bug C++ t'a coûté une soirée de valgrind**, Rust en a fait une erreur de compilation. Plus tu te souviens de ces soirées, plus les règles te paraîtront évidentes.

### Utile

- **Les threads et mutex de Philosophers** : pour comprendre ce que Rust promet sur les data races ([`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) §7). La fiche `CONCURRENCY` n'est **pas encore écrite** dans le vault.
- **UTF-8** : savoir qu'un caractère fait de 1 à 4 octets explique pourquoi `s[i]` ne compile pas ([`TYPES`](../../en/rust/notions/fundamentals/TYPES.md) §3).
- **Les entiers à largeur fixe** (`int32_t`, `size_t`) : Rust n'a que ça ([`TYPES`](../../en/rust/notions/fundamentals/TYPES.md) §1).
- **Un peu d'anglais technique** : les fiches, le livre officiel et les messages du compilateur sont en anglais.

### Pas nécessaire

Un langage fonctionnel (Haskell, OCaml), même si `match` et `Option` en viennent. La programmation asynchrone (la fiche `ASYNC` est à venir). Les macros (`println!` s'utilise sans savoir l'écrire).

---

## 3. Le modèle mental à installer

```text
  Ownership  → qui libère la valeur                  (un seul propriétaire, drop en fin de portée)
  Emprunt    → qui peut la lire ou la modifier MAINTENANT   (plusieurs &T  OU  un seul &mut T)
  Durées de vie → combien de temps un regard reste valide   (une référence ne survit pas à sa valeur)
```

Ce schéma vient de [`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §6 (EN). Quatre phrases à garder :

1. **`let b = a;` déplace, il ne copie pas.** Après, `a` est mort. Seuls les types « simples bits » (`i32`, `bool`, `char`, `&T`…) sont `Copy` ([`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §3–4).
2. **Aliasing XOR mutation** : plusieurs lecteurs, ou un seul écrivain, jamais les deux à la fois ([`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) §3).
3. **Les durées de vie décrivent, elles ne prescrivent pas** : on ne corrige pas une erreur en ajoutant des `'a` jusqu'à ce qu'elle se taise ([`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §6).
4. **L'échec est dans le type** : `Option<T>` pour l'absence, `Result<T, E>` pour l'erreur ; `panic!` est réservé aux bugs ([`ERROR_HANDLING`](../../en/rust/notions/errors/ERROR_HANDLING.md), TL;DR).

Et le changement de réflexe, en une ligne ([`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) §5) : ne demande plus « qui fait le `delete` ? », demande « **qui possède** ça ? ».

---

## 4. Comment s'y prendre

L'ordre suit celui du hub [`rust/INDEX`](../../en/rust/INDEX.md) (« Suggested reading order »).

1. **La philosophie** ([`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md)). Sans elle, le borrow checker ressemble à un ennemi.
2. **Un bac à sable** ([`CARGO`](../../en/rust/notions/tooling/CARGO.md) §3) : `cargo new sandbox`, puis `cargo run` et surtout **`cargo check`**, qui vérifie tout sans générer de code et répond en une seconde environ.
3. **La syntaxe de tous les jours** : [`TYPES`](../../en/rust/notions/fundamentals/TYPES.md), [`LET_MUT`](../../en/rust/lexique/LET_MUT.md), [`FN`](../../en/rust/lexique/FN.md), [`CONTROL_FLOW`](../../en/rust/lexique/CONTROL_FLOW.md). Deux surprises : la dernière expression **sans point-virgule** est la valeur de retour ([`FN`](../../en/rust/lexique/FN.md) §2), et `for` ne parcourt que des itérateurs ([`CONTROL_FLOW`](../../en/rust/lexique/CONTROL_FLOW.md) §5).
4. **Le cœur, dans l'ordre et sans sauter** : [`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) → [`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) → [`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md). Tape chaque exemple marqué `compile_fail`, **lis l'erreur en entier**, puis `rustc --explain E0382` (ou le code affiché).
5. **Modéliser les données sans classes** : [`STRUCT`](../../en/rust/lexique/STRUCT.md), [`ENUM`](../../en/rust/lexique/ENUM.md), [`MATCH`](../../en/rust/lexique/MATCH.md), [`IMPL_SELF`](../../en/rust/lexique/IMPL_SELF.md).
6. **Templates et `virtual` réunis** : [`TRAITS_GENERICS`](../../en/rust/notions/traits/TRAITS_GENERICS.md), [`TRAIT_DYN`](../../en/rust/lexique/TRAIT_DYN.md).
7. **Les erreurs sans exceptions** : [`ERROR_HANDLING`](../../en/rust/notions/errors/ERROR_HANDLING.md). Réécris CPP05 (le `Bureaucrat`) : la fiche le fait en §3.
8. **Ensuite** : relis [`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) (la fiche le demande elle-même), puis les exercices **Rustlings** et le livre officiel (`rustup doc --book`) ([`CARGO`](../../en/rust/notions/tooling/CARGO.md) §6).

Un mot va te bloquer ? Le [`lexique`](../../en/rust/lexique/INDEX.md) a une page par mot-clé, et le [`GLOSSAIRE`](../../en/rust/lexique/GLOSSAIRE.md) une entrée par concept.

> ⏳ Pas encore couverts dans le vault : pointeurs intelligents (`Box`, `Rc`, `RefCell`), collections, itérateurs et closures en détail, concurrence, macros, async ([`notions/INDEX`](../../en/rust/notions/INDEX.md), « Coming soon »). Pour ces sujets, le livre officiel en attendant.

---

## 5. Les erreurs de débutant à éviter

- **Saupoudrer de `.clone()`** pour faire taire le borrow checker. Ça compile, mais c'est presque toujours un **emprunt** (`&`) que tu voulais ([`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §5).
- **Ajouter des `'a` au hasard** : les durées de vie ne rallongent rien. Fais vivre la donnée plus longtemps, ou renvoie une valeur possédée ([`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §6).
- **Mettre des références dans tes structs dès le début.** En apprenant, fais-les **posséder** leurs données (`String`, pas `&str`) ([`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §4).
- **Chercher l'héritage.** `virtual`, `override` et `final` sont réservés mais ne veulent rien dire : enum si l'ensemble des cas est fermé, trait s'il est ouvert ([`lexique`](../../en/rust/lexique/INDEX.md), [`TRAITS_GENERICS`](../../en/rust/notions/traits/TRAITS_GENERICS.md) §6).
- **`.unwrap()` partout** hors des tests et des prototypes. Utilise `?`, ou `.expect("pourquoi ça ne peut pas échouer")` ([`ERROR_HANDLING`](../../en/rust/notions/errors/ERROR_HANDLING.md) §5).
- **Prendre `&String` ou `&Vec<T>` en paramètre** : `&str` et `&[T]` acceptent plus d'appelants pour le même prix ([`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) §6).
- **Lutter contre l'erreur au lieu de la lire.** Elle a un code, une explication longue, et propose souvent la correction ([`PHILOSOPHY`](../../en/rust/notions/PHILOSOPHY.md) §2, pilier 4).

---

## 6. Tu es prêt pour la suite si…

> [!question]- `let a = String::from("hi"); let b = a; println!("{a}");` : que se passe-t-il, et pourquoi ce n'est pas un double free en puissance ?
> Erreur E0382 (« borrow of moved value »). `let b = a;` a **déplacé** l'en-tête (pointeur, longueur, capacité) ; `a` est marqué mort et aucun destructeur ne tournera pour lui. Un seul propriétaire, donc une seule libération. ([`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §3)

> [!question]- Pourquoi un type ne peut-il pas être à la fois `Copy` et `Drop` ?
> S'il a besoin d'un destructeur, une copie silencieuse bit à bit reproduirait le double free. ([`OWNERSHIP`](../../en/rust/notions/fundamentals/OWNERSHIP.md) §4)

> [!question]- `let first = &v[0]; v.push(4); println!("{first}");` est refusé. Quel bug C++ ce refus évite-t-il ?
> L'invalidation d'itérateurs : `push` peut réallouer le tampon, et `first` pointerait dans de la mémoire libérée. C++98 le compile sans rien dire. ([`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) §3)

> [!question]- Et si on place le `println!` **avant** le `push` ?
> Ça compile : un emprunt se termine à sa **dernière utilisation**, pas à l'accolade. ([`BORROWING`](../../en/rust/notions/fundamentals/BORROWING.md) §4)

> [!question]- `fn pick() -> &String { let local = …; &local }` : quelle erreur, et quelle correction ?
> E0106, « missing lifetime specifier » : la référence n'emprunte à rien de valide, `local` meurt à `}`. On renvoie la valeur possédée : `fn pick() -> String`. ([`LIFETIMES`](../../en/rust/notions/fundamentals/LIFETIMES.md) §1)

> [!question]- `fn square(x: i32) -> i32 { x * x; }` ne compile pas. Pourquoi ?
> Le point-virgule transforme l'expression en instruction : le bloc vaut `()`, pas un `i32` (E0308). On enlève le `;`. ([`FN`](../../en/rust/lexique/FN.md) §2)

> [!question]- Quand utiliser `Result`, et quand `panic!` ?
> `Result` pour un échec **attendu** dont l'appelant peut faire quelque chose (entrée invalide, fichier absent). `panic!` pour un **bug** : un invariant cassé. ([`ERROR_HANDLING`](../../en/rust/notions/errors/ERROR_HANDLING.md) §4)
