# 11. Atelier : une main de cartes en 3D

> **TL;DR.** On assemble tout ce qui précède en une vraie main de cartes : disposition en **éventail** (hiérarchie : un pivot sous la main, chaque carte tournée d'un angle puis décalée vers le haut), **survol** qui soulève la carte en douceur (`MathUtils.damp` dans `useFrame`), **retournement** (rotation de π autour de Y, avec une face et un dos), et **carte jouée** qui vole jusqu'au centre du plateau. Chaque carte calcule une **cible** à partir de l'état, et `useFrame` l'en rapproche à chaque image : on ne programme jamais une animation « du point A au point B », on change la cible.

Liens : [`INDEX`](INDEX.md) · [`HIERARCHIE`](HIERARCHIE.md) · [`ANIMATION`](ANIMATION.md) · [`INTERACTION`](INTERACTION.md) · [`ETAT_RESEAU`](ETAT_RESEAU.md)

🧪 Labo : à créer. Ce module est autonome : copie-le dans `src/lessons/L8_Cartes.tsx` (ou `.jsx` en retirant les types) et ajoute-le à `index.jsx`. Vérifié au typecheck, pas encore dans un navigateur.

---

## 1. Le principe : état → cible → `damp`

Toutes les animations de cet atelier suivent **le même motif** :

```text
   état React / store          calcul pur              useFrame, 60 fois / s
 ┌─────────────────────┐   ┌──────────────────┐   ┌─────────────────────────────┐
 │ survolée ? jouée ?  │ → │ cible : position │ → │ valeur = damp(valeur, cible) │
 │ face cachée ?       │   │ rotation, échelle│   │ (rapproche un peu la valeur) │
 └─────────────────────┘   └──────────────────┘   └─────────────────────────────┘
```

Pourquoi c'est la bonne méthode pour un jeu :
- **Interruptible** : si on survole puis quitte une carte en plein mouvement, la cible change, et la carte repart **depuis là où elle est**. Pas d'animation à annuler.
- **Indépendant du nombre d'images par seconde** : `damp` utilise `delta` (voir [`ANIMATION`](ANIMATION.md), section 6).
- **Piloté par l'état** : le serveur dit « carte jouée », la cible devient le plateau, la carte y vole. Le réseau n'a pas à connaître l'animation.

---

## 2. La géométrie de l'éventail

```text
                 carte 0   carte 1   carte 2   carte 3   carte 4
                    ╲         │         │         │        ╱
                      ╲   ↖   │   ↑     │    ↗   │     ╱
                        ╲─────┴────●────┴─────╱
                                   pivot, en (0, -R) sous la main
```

Chaque carte est un groupe placé **sur le pivot**, tourné d'un angle autour de Z, et la carte elle-même est décalée de `R` vers le haut **dans** ce groupe. Le décalage tourne avec le groupe : les cartes se répartissent sur un arc de cercle de rayon `R`, et chacune est naturellement inclinée vers l'extérieur. C'est l'orbite de [`HIERARCHIE`](HIERARCHIE.md), section 3, figée à un angle.

Pour `n` cartes et un écart `ecart` entre deux cartes, l'angle de la carte `i` vaut :

```text
angle(i) = (i - (n - 1) / 2) × ecart
```

Le `(n - 1) / 2` centre l'éventail : avec 5 cartes, les indices 0 à 4 donnent -2, -1, 0, 1, 2 fois l'écart. La carte du milieu est droite.

Le signe : une rotation **positive** autour de Z tourne dans le sens **inverse** des aiguilles d'une montre (repère main droite). La carte 0 doit partir **à gauche**, donc on prend l'**opposé** de l'angle.

---

## 3. Le module complet

```tsx
import { Suspense, useRef, useState } from "react";
import { Canvas, useFrame } from "@react-three/fiber";
import { RoundedBox, Text, Environment, useCursor } from "@react-three/drei";
import { MathUtils, type Group } from "three";

type DonneeCarte = { id: string; nom: string; aura: number; couleur: string };

const MAIN: DonneeCarte[] = [
    { id: "c1", nom: "Regard froid", aura: 2, couleur: "#4fc3f7" },
    { id: "c2", nom: "Pose iconique", aura: 4, couleur: "#ba68c8" },
    { id: "c3", nom: "Mèche au vent", aura: 1, couleur: "#81c784" },
    { id: "c4", nom: "Entrée au ralenti", aura: 5, couleur: "#ffb74d" },
    { id: "c5", nom: "Silence gênant", aura: 3, couleur: "#e57373" },
];

const L = 1.0;
const H = 1.4;
const RAYON = 6;
const ECART = 0.09;
const VITESSE = 10;

type Cible = { x: number; y: number; z: number; rz: number; ry: number; echelle: number };

function cibleDeCarte(i: number, n: number, survolee: boolean, jouee: boolean, cachee: boolean): Cible {
    if (jouee) return { x: 0, y: 1.6, z: -1, rz: 0, ry: 0, echelle: 1.3 };

    const angle = -(i - (n - 1) / 2) * ECART;
    const leve = survolee ? 0.45 : 0;
    return {
        x: Math.sin(-angle) * RAYON,
        y: Math.cos(angle) * RAYON - RAYON - 1.6 + leve,
        z: i * 0.01 + (survolee ? 0.3 : 0),
        rz: survolee ? 0 : angle,
        ry: cachee ? Math.PI : 0,
        echelle: survolee ? 1.15 : 1,
    };
}

function Carte({
    donnee,
    cible,
    onSurvol,
    onJouer,
}: {
    donnee: DonneeCarte;
    cible: Cible;
    onSurvol: (dedans: boolean) => void;
    onJouer: () => void;
}) {
    const ref = useRef<Group>(null);
    const [survol, setSurvol] = useState(false);
    useCursor(survol);

    useFrame((_, dt) => {
        const g = ref.current;
        if (!g) return;
        g.position.x = MathUtils.damp(g.position.x, cible.x, VITESSE, dt);
        g.position.y = MathUtils.damp(g.position.y, cible.y, VITESSE, dt);
        g.position.z = MathUtils.damp(g.position.z, cible.z, VITESSE, dt);
        g.rotation.z = MathUtils.damp(g.rotation.z, cible.rz, VITESSE, dt);
        g.rotation.y = MathUtils.damp(g.rotation.y, cible.ry, 6, dt);
        const s = MathUtils.damp(g.scale.x, cible.echelle, VITESSE, dt);
        g.scale.setScalar(s);
    });

    return (
        <group
            ref={ref}
            onPointerOver={(e) => {
                e.stopPropagation();
                setSurvol(true);
                onSurvol(true);
            }}
            onPointerOut={() => {
                setSurvol(false);
                onSurvol(false);
            }}
            onClick={(e) => {
                e.stopPropagation();
                onJouer();
            }}
        >
            <RoundedBox args={[L, H, 0.02]} radius={0.01} smoothness={2}>
                <meshStandardMaterial color="#f3ead7" roughness={0.6} />
            </RoundedBox>

            <mesh position={[0, 0.2, 0.011]}>
                <planeGeometry args={[L * 0.85, H * 0.5]} />
                <meshStandardMaterial color={donnee.couleur} />
            </mesh>
            <Text position={[0, -0.25, 0.012]} fontSize={0.09} color="#222" maxWidth={L * 0.85} textAlign="center">
                {donnee.nom}
            </Text>
            <Text position={[-0.38, 0.58, 0.012]} fontSize={0.14} color="#222">
                {String(donnee.aura)}
            </Text>

            <mesh position={[0, 0, -0.011]} rotation={[0, Math.PI, 0]}>
                <planeGeometry args={[L * 0.95, H * 0.95]} />
                <meshStandardMaterial color="#3b2a5c" />
            </mesh>
        </group>
    );
}

function Main() {
    const [survolee, setSurvolee] = useState<string | null>(null);
    const [jouee, setJouee] = useState<string | null>(null);
    const [cachee, setCachee] = useState(false);
    const enMain = MAIN.filter((c) => c.id !== jouee);

    return (
        <>
            {MAIN.map((c) => {
                const i = enMain.indexOf(c);
                const cible = cibleDeCarte(i, enMain.length, survolee === c.id, jouee === c.id, cachee);
                return (
                    <Carte
                        key={c.id}
                        donnee={c}
                        cible={cible}
                        onSurvol={(dedans) => setSurvolee(dedans ? c.id : null)}
                        onJouer={() => (jouee === c.id ? setJouee(null) : setJouee(c.id))}
                    />
                );
            })}
            <mesh position={[0, 0, -1.5]} onClick={() => setCachee(!cachee)}>
                <planeGeometry args={[20, 12]} />
                <meshStandardMaterial color="#1b1b24" />
            </mesh>
        </>
    );
}

export default function Lecon8() {
    return (
        <Canvas camera={{ position: [0, 0, 5], fov: 50 }}>
            <ambientLight intensity={0.3} />
            <Suspense fallback={null}>
                <Environment preset="studio" />
                <Main />
            </Suspense>
        </Canvas>
    );
}
```

Clique une carte pour la jouer (et la reprendre en recliquant dessus). Clique le fond pour retourner toute la main.

> ⚠️ `Environment preset` et `Text` sans `font` téléchargent leurs fichiers depuis un CDN : d'où le `<Suspense>`, et un écran vide hors ligne. Pour le jeu, héberge-les toi-même (voir [`DREI`](DREI.md), section 4).

---

## 4. Démontage

### La cible, une fonction pure
`cibleDeCarte` ne touche à rien : elle prend l'état et **renvoie des nombres**. On peut la tester à la main, la régler avec Leva (voir [`LEVA`](LEVA.md)), et elle est la **seule** à connaître la disposition. Pour une main en ligne droite plutôt qu'en éventail, on ne change qu'elle.

### Pourquoi `x = sin(-angle) × R` et `y = cos(angle) × R - R`
C'est l'éventail de la section 2, **calculé** au lieu d'être monté avec deux groupes. Un groupe-pivot fonctionnerait aussi pour la main, mais une carte jouée doit **quitter** l'arc et voler vers le plateau : avec des coordonnées calculées, on interpole simplement de l'une à l'autre. Le `- R` ramène la carte du milieu en `y = 0`, et `- 1.6` descend toute la main en bas de l'écran.

### Le survol
- **Carte levée et redressée** : `leve` la monte, `rz: 0` annule son inclinaison, `echelle: 1.15` la grossit. C'est ce qu'on voit dans la plupart des jeux de cartes : la carte survolée se met droite pour être lisible.
- **`z` légèrement avancé** : la carte survolée passe **devant** ses voisines, sinon elle serait en partie cachée. `i * 0.01` empile les cartes au repos de gauche à droite, pour éviter qu'elles se « battent » au même `z` (un clignotement appelé *z-fighting*).
- **`useCursor(survol)`** (drei) passe le curseur en main.
- **`stopPropagation`** dans `onPointerOver` : les cartes se chevauchent, et seule celle du dessus doit réagir (voir [`INTERACTION`](INTERACTION.md), section 5).

### ⚠️ Non vérifié : le scintillement au survol
La zone cliquable **bouge avec la carte**. Une carte survolée monte de 0,45 et grossit : son bord bas remonte d'environ un quart de sa hauteur. Si la souris est dans ce quart, elle « quitte » la carte, qui redescend, revient sous la souris, remonte… Le scintillement classique des mains de cartes. R3F ne relance le rayon qu'au mouvement de la souris : il n'apparaît qu'en bougeant.

Ce module n'a pas été testé dans un navigateur. Si tu observes ce scintillement, la correction habituelle : mettre les handlers sur une **zone de survol fixe**, un plan invisible (`<meshBasicMaterial transparent opacity={0} depthWrite={false} />`) placé à la position **de repos** de la carte, qui ne monte pas avec elle.

### Le retournement
- La face est **devant** (`z = +0.011`), le dos **derrière** (`z = -0.011`), lui-même tourné de π pour regarder vers l'arrière.
- Tourner la carte de π autour de Y montre le dos. `damp` de 0 à π donne une rotation fluide, avec une vitesse plus basse (6) pour qu'on la voie.
- Sans le dos, on verrait la face **à travers** : un `planeGeometry` n'a qu'un côté visible par défaut. Un `side={DoubleSide}` ferait voir le texte à l'envers, en miroir. Un dos séparé est la vraie solution.

### Le `key` stable
`key={c.id}` et **pas** `key={i}` : quand une carte est jouée, les indices des autres changent. Avec l'indice comme clé, React croirait que la carte 3 **est devenue** la carte 2, et réutiliserait son composant : son `ref`, sa position courante, son état de survol. Avec l'`id`, chaque carte garde son composant, et `damp` la fait glisser vers sa nouvelle place. C'est la même règle qu'en React pur, mais en 3D l'erreur se **voit** : les cartes sautent.

### Une carte = un groupe
Le fond, l'illustration, les textes et le dos sont **enfants** du même groupe : on n'anime que le groupe, et tout suit (voir [`HIERARCHIE`](HIERARCHIE.md), section 6).

---

## 5. Brancher sur le jeu réel

Dans ce module, l'état est local (`useState`) pour qu'il tienne en un fichier. Dans Transcendance :
- `MAIN` vient du store, lui-même rempli par le serveur (voir [`ETAT_RESEAU`](ETAT_RESEAU.md)) ;
- `onJouer` **n'écrit pas** `jouee` : il **envoie** `{ type: "jouer", carte: id }` au serveur. La carte ne vole vers le plateau que quand le serveur confirme, en renvoyant un état où elle n'est plus dans la main ;
- pour une interface qui réagit sans attendre le réseau, on peut lever la carte tout de suite (état local « en attente ») et la reposer si le serveur refuse. C'est l'**optimisme côté client** : à n'ajouter qu'une fois la version simple fonctionnelle ;
- `survolee` reste **local** : c'est de l'interface pure, le serveur n'a pas à le savoir.

---

## 6. Aller plus loin

| Idée | Piste |
|---|---|
| Illustration de carte | `useTexture("/cartes/c1.png")` et `<meshStandardMaterial map={texture} />` (voir [`DREI`](DREI.md)). |
| Contour de la carte sélectionnée | `<Outlines>` de drei, à l'intérieur du `RoundedBox`. |
| Aura qui pulse autour d'une carte forte | `<Sparkles>` enfant de la carte, `count` proportionnel à `donnee.aura`. |
| Inspection en grand | Double-clic → cible au centre, `echelle: 2.5`, et `PresentationControls` (voir [`DREI`](DREI.md), section 5). |
| Glisser-déposer au lieu de cliquer | `onPointerDown` → mémoriser, `onPointerMove` → suivre `e.point`, `onPointerUp` → zone de dépôt ? Penser à `setPointerCapture` sur `e.target`. |
| Main de l'adversaire | Même composant, `cachee` à `true`, main placée en haut et tournée de π autour de Z. |

---

## 🧪 À essayer

1. Change `ECART` (0,03 puis 0,2) et `RAYON` (3 puis 20) : l'éventail se resserre, s'ouvre, devient presque plat.
2. Passe `VITESSE` à 2, puis à 30 : les cartes deviennent molles, puis sèches.
3. Remplace `key={c.id}` par l'indice du `map` (`MAIN.map((c, k) => … key={k}`) : ici rien ne change, car `MAIN` garde toujours le même ordre. Puis fais un `map` sur `enMain` au lieu de `MAIN` (en gardant la branche « jouée » à part) : jouer une carte du milieu fait sauter ses voisines. Repasse à `key={c.id}` : elles glissent.
4. Retire le dos de la carte, puis retourne la main : on voit la face par transparence… ou rien, selon l'angle.
5. Ajoute un `useControls` (Leva) pour `ECART`, `RAYON` et `VITESSE`, et trouve tes valeurs préférées.
6. Ajoute une deuxième main (l'adversaire) en haut de l'écran, face cachée.
