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
    └── cpp/
        └── notions/
            └── containers/   # les 11 fiches sur les conteneurs de la STL
```

Correspondance : `fr/<chemin>` ↔ `en/<chemin>`. Chaque fiche traduite commence par un lien vers son original anglais.

---

## ✅ Déjà traduit

| Thème | Point d'entrée | Contenu |
|---|---|---|
| Conteneurs STL (C++98) | [`cpp/notions/containers/INDEX.md`](cpp/notions/containers/INDEX.md) | vector, list, deque, stack, queue, priority_queue, set, multiset, map, multimap |

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
