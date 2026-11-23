# 📚 Bibliothèque

> 🇬🇧 [English version](../en/INDEX.md)

La version française de la bibliothèque. Elle reproduit l'arborescence de [`../en/`](../en/INDEX.md) — **mêmes dossiers, mêmes noms de fichiers** — et se remplit une fiche à la fois. Tant qu'une fiche n'est pas traduite, c'est la version anglaise qui fait foi : les liens vers elle sont marqués **(EN)**.

---

## 🗂️ Organisation

```
library/
├── en/   # la documentation complète, en anglais
└── fr/   # les traductions — même arbre que en/
    ├── INDEX.md   ← tu es ici
    ├── cpp/
    │   ├── INDEX.md          # les trois portes : projets, notions, lexique
    │   ├── notions/
    │   │   ├── advanced/     # ITERATORS.md
    │   │   ├── containers/   # les 11 fiches sur les conteneurs de la STL
    │   │   └── oop/          # OCF, héritage, polymorphisme, surcharge d'opérateurs, getters/setters
    │   └── projets/
    │       ├── INDEX.md
    │       └── modules/      # CPP00.md → CPP09.md, roundf_behavior.md
    ├── docker/
    │   ├── GLOSSAIRE.md      # tous les termes techniques d'Inception
    │   └── ABORDER.md, SYNTHESE.md   # ✍️ rédigé en français
    ├── javascript/           # ✍️ rédigé en français : bases JS / TS pour Transcendance
    │   └── lexique/          # un mot-clé JS par page + GLOSSAIRE
    ├── projects/
    │   └── LEVEL6/           # ft_transcendence-subject.md
    └── rust/                 # ✍️ rédigé en français : ABORDER.md, SYNTHESE.md
```

Correspondance : `fr/<chemin>` ↔ `en/<chemin>`. Chaque fiche traduite commence par un lien vers son original anglais.

---

## ✅ Déjà traduit

| Thème | Point d'entrée | Contenu |
|---|---|---|
| Conteneurs STL (C++98) | [`cpp/notions/containers/INDEX.md`](cpp/notions/containers/INDEX.md) | vector, list, deque, stack, queue, priority_queue, set, multiset, map, multimap |
| Itérateurs (C++98) | [`cpp/notions/advanced/ITERATORS.md`](cpp/notions/advanced/ITERATORS.md) | catégories, traits, adaptateurs, invalidation, CPP08 |
| C++ — accueil | [`cpp/INDEX.md`](cpp/INDEX.md) · [`cpp/projets/INDEX.md`](cpp/projets/INDEX.md) | les trois portes (projets, notions, lexique) et la liste des parcours guidés |
| Modules CPP00 → CPP08 | [`cpp/projets/modules/CPP00.md`](cpp/projets/modules/CPP00.md) … [`CPP08.md`](cpp/projets/modules/CPP08.md) | guides exercice par exercice : classes, mémoire, OCF, héritage, polymorphisme, exceptions, casts, templates, STL ; + [`roundf_behavior.md`](cpp/projets/modules/roundf_behavior.md) |
| Notions OOP (C++98) | [`cpp/notions/oop/ORTHODOX_CANONICAL_FORM.md`](cpp/notions/oop/ORTHODOX_CANONICAL_FORM.md) · [`INHERITANCE.md`](cpp/notions/oop/INHERITANCE.md) · [`POLYMORPHISM.md`](cpp/notions/oop/POLYMORPHISM.md) · [`OPERATOR_OVERLOADING.md`](cpp/notions/oop/OPERATOR_OVERLOADING.md) · [`GETTERS_SETTERS.md`](cpp/notions/oop/GETTERS_SETTERS.md) | forme canonique, héritage et diamant, vtable et virtuelles, surcharge d'opérateurs, encapsulation |
| Module CPP09 | [`cpp/projets/modules/CPP09.md`](cpp/projets/modules/CPP09.md) | btc (map), RPN (stack), PmergeMe (Ford-Johnson, vector vs deque), grille d'évaluation |
| Glossaire Inception (Docker) | [`docker/GLOSSAIRE.md`](docker/GLOSSAIRE.md) | tous les termes techniques du sujet Inception : définition + usage, par thème, + aide-mémoire des commandes |
| Sujet ft_transcendence | [`projects/LEVEL6/ft_transcendence-subject.md`](projects/LEVEL6/ft_transcendence-subject.md) | sujet v21.1 complet : partie obligatoire, 55 modules + tableau des points, idées de projets, exigences du README, bonus |

---

## ✍️ Rédigé en français

Fiches écrites directement en français, sans original anglais (pour l'instant).

| Thème | Point d'entrée | Contenu |
|---|---|---|
| JavaScript / TypeScript | [`javascript/INDEX.md`](javascript/INDEX.md) | les bases pour un dev C/C++98 : variables, fonctions, objets, classes, asynchrone, TypeScript, le pont vers React / AdonisJS pour Transcendance, et la 3D avec React Three Fiber |
| 🔑 Lexique JS / TS | [`javascript/lexique/INDEX.md`](javascript/lexique/INDEX.md) | une page par mot-clé JavaScript (TypeScript à venir), avec l'équivalent C/C++98, plus le [`GLOSSAIRE`](javascript/lexique/GLOSSAIRE.md) des concepts |
| 🦀 Rust | [`rust/ABORDER.md`](rust/ABORDER.md) · [`rust/SYNTHESE.md`](rust/SYNTHESE.md) | par où commencer depuis C/C++98 (prérequis, modèle ownership / emprunt / durées de vie, ordre de lecture), puis la fiche de révision ; renvoient aux fiches anglaises de [`../en/rust/`](../en/rust/INDEX.md) |
| 🐳 Docker / Inception | [`docker/ABORDER.md`](docker/ABORDER.md) · [`docker/SYNTHESE.md`](docker/SYNTHESE.md) | prérequis C système (minishell, signaux, sockets), les quatre idées, le parcours du ROADMAP, puis la fiche de révision ; renvoient aux fiches anglaises de [`../en/docker/`](../en/docker/INDEX.md) |

---

## 📖 Tout le reste

Voir le hub anglais : [`../en/INDEX.md`](../en/INDEX.md).

---

## ➕ Traduire une nouvelle fiche

1. Crée-la au **même chemin** que l'anglais, sous `library/fr/`.
2. Juste après le titre : `> 🇬🇧 [English version](<chemin relatif vers en/…>)`.
3. Traduis la prose, les titres, les tableaux et les schémas ; laisse les blocs de code tels quels.
4. Les liens vers des fiches pas encore traduites pointent vers `en/` avec le marqueur **(EN)** ; quand la fiche cible est traduite, retire le détour par `en/`.
5. Ajoute-la au tableau « Déjà traduit » ci-dessus.
