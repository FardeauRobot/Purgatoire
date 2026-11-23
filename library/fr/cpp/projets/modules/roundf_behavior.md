# `roundf` — Comportement exact et implémentation matérielle

> 🇬🇧 [English version](../../../../en/cpp/projets/modules/roundf_behavior.md)

## Ce que fait vraiment `roundf` (la spécification)

`roundf(float x)` de `<math.h>` arrondit à l'**entier le plus proche, les cas à mi-chemin étant arrondis en s'éloignant de zéro** (*ties away from zero*).

```
roundf( 0.5f) ==  1.0f      roundf(-0.5f) == -1.0f
roundf( 1.5f) ==  2.0f      roundf(-1.5f) == -2.0f
roundf( 2.5f) ==  3.0f      roundf(-2.5f) == -3.0f
roundf( 2.4f) ==  2.0f      roundf(-2.4f) == -2.0f
```

Selon C99/C11, elle doit :

- Arrondir les moitiés en s'éloignant de zéro (PAS vers le pair).
- **Ne pas lever** l'exception flottante `FE_INEXACT`, même quand la valeur a une partie fractionnaire.
- Ignorer le mode d'arrondi flottant courant — son comportement est fixe, quel que soit `fesetround()`.

Le résultat reste un `float`, donc pour `|x| ≥ 2²³` l'entrée est déjà entière et elle est renvoyée telle quelle.

---

## Pourquoi c'est l'intruse

Il y a cinq fonctions « arrondir à l'entier » dans `<math.h>`, et elles diffèrent de façon subtile mais importante :

| Fonction         | Règle d'arrondi                       | Respecte le mode FPU ? | Peut lever inexact ? |
|------------------|---------------------------------------|------------------------|----------------------|
| `roundf`         | plus proche, **moitiés loin de 0**    | non                    | non                  |
| `nearbyintf`     | ce que dit le mode FPU                | oui                    | non                  |
| `rintf`          | ce que dit le mode FPU                | oui                    | oui                  |
| `truncf`         | vers 0                                | non                    | non                  |
| `floorf`/`ceilf` | vers −∞ / +∞                          | non                    | non                  |

Le point agaçant : **la règle de `roundf` ne fait pas partie des quatre modes d'arrondi standard d'IEEE 754.** Les modes IEEE 754 sont :

1. Au plus proche, **moitiés vers le pair** (le défaut — l'« arrondi du banquier », *banker's rounding*)
2. Vers 0 (troncature)
3. Vers +∞
4. Vers −∞

« Moitiés loin de zéro » n'est *pas* dans cette liste. IEEE 754-2008 l'a ajouté comme mode optionnel (`roundTiesToAway`), mais la plupart des processeurs ne l'implémentent pas comme mode réglable.

---

## La réalité matérielle

### x86 / x86-64 (SSE / AVX)

L'instruction `ROUNDSS` (SSE4.1) prend une valeur immédiate sur 8 bits qui sélectionne :

- `0` — au plus proche (moitiés vers le pair)
- `1` — vers −∞
- `2` — vers +∞
- `3` — vers 0
- `4` — utiliser le mode courant de MXCSR

Il n'existe **aucun encodage pour « moitiés loin de zéro »**. Donc `roundf` ne peut pas être une seule instruction sur x86. Les compilateurs et la libm l'implémentent avec une petite séquence : tests + manipulation de bits + troncature.

### ARMv8 (AArch64)

ARM a été plus sympa. AArch64 a ajouté toute une famille d'instructions `FRINT*`, dont celle qu'il nous faut :

- `FRINTA` — **au plus proche, moitiés loin de zéro** ← exactement la sémantique de `roundf`
- `FRINTN` — au plus proche, moitiés vers le pair
- `FRINTM` — vers −∞
- `FRINTP` — vers +∞
- `FRINTZ` — vers zéro
- `FRINTX` — mode courant, *lève* inexact
- `FRINTI` — mode courant, sans inexact

Donc sur AArch64, `roundf(x)` se compile en une seule instruction :

```asm
frinta   s0, s0
ret
```

Alors que le même appel sur une machine x86 antérieure à SSE4.1 peut coûter ~10–15 instructions de bidouillage de bits et un branchement.

---

## Ce que fait vraiment l'implémentation de la libm (x86 / portable)

L'astuce portable, c'est de faire de l'**arithmétique entière sur le motif de bits IEEE 754**, pas sur le float lui-même. Un `float`, c'est :

```
[ signe : 1 ][ exposant : 8 ][ mantisse : 23 ]
```

L'exposant réel débiaisé `e = exp_bits - 127` est la position de la virgule binaire.

L'algorithme (à peu près ce que font glibc/musl) :

1. **Extraire les bits** avec une `union` ou un `memcpy` vers un `uint32_t`.
2. **Sortir l'exposant débiaisé** `e`.
3. **Trois cas :**
   - `e < 0` → `|x| < 1`. Le résultat vaut `±0` si `|x| < 0.5`, sinon `±1`. Facile.
   - `e ≥ 23` → il n'existe aucun bit fractionnaire ; `x` est déjà entier (ou NaN/Inf). On le renvoie tel quel.
   - `0 ≤ e < 23` → il y a une partie fractionnaire à traiter :
     - Calculer un masque pour les bits fractionnaires sous la position `e`.
     - **Les mettre à zéro** pour obtenir une troncature vers zéro.
     - Puis **ajouter conditionnellement 1 ulp à la position entière** si le plus haut bit éliminé valait 1 (le cas « 0,5 ou plus ») — ça donne naturellement « moitiés loin de zéro », parce que c'est la valeur absolue qu'on ajuste avant de réappliquer le signe.
4. Réécrire les bits dans un float.

Pourquoi manipuler des bits plutôt que `(int)(x + copysign(0.5f, x))` ? Parce que cette approche « évidente » est **fausse pour les valeurs proches de `0.5 - ulp`** — l'addition elle-même peut arrondir la valeur vers le haut, ce qui donne une mauvaise réponse sur le dernier bit. La manipulation de bits évite entièrement tout arrondi intermédiaire.

Une version naïve (et subtilement cassée) que certains écrivent :

```c
// Don't do this. Wrong for some inputs near halves due to FP rounding.
float my_roundf(float x) {
    return (x >= 0.0f) ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
```

La vraie version de glibc fait ~20 lignes de bidouillage de bits sans aucune arithmétique flottante.

---

## Bilan des performances

- **AArch64 :** 1 instruction (`frinta`), latence ~3 cycles, entièrement pipelinée.
- **x86 avec SSE4.1+ :** généralement inlinée en une petite séquence (comparaison, masque, ajout prudent de 0,5, `ROUNDSS` vers zéro, etc.), ~10–15 cycles.
- **x86 sans SSE4.1 :** appel de fonction dans la libm, qui exécute la routine de manipulation de bits — des dizaines de cycles, beaucoup de branchements.

C'est pour ça que, dans les chemins critiques, on préfère souvent :

- `(int)x` (troncature) si on veut vraiment une troncature.
- `nearbyintf` si l'arrondi du banquier convient — elle peut se compiler en un seul `ROUNDSS` sur x86.
- `lroundf` si on veut de toute façon un résultat entier (ça évite l'aller-retour par `float`).

---

## TL;DR

**La sémantique de `roundf` est mathématiquement naturelle mais architecturalement maladroite** — elle demande un mode d'arrondi que le matériel IEEE 754 n'a historiquement pas supporté, donc elle coûte presque toujours plus cher que ses cousines.
