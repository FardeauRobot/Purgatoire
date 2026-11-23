# 9. Le temps réel : Socket.IO dans React

> **TL;DR.** La socket est créée **une seule fois**, dans un module (`socket.ts`), avec `autoConnect: false`. Les composants s'y **abonnent** dans un effet : `socket.on(…)` à l'entrée, `socket.off(…)` **avec la même fonction** au nettoyage. La connexion elle-même se fait dans un effet qui la referme au nettoyage, ou dans un gestionnaire d'événement. Rejoins le salon dans l'écouteur `connect`, qui se déclenche aussi à chaque reconnexion. Le jeton passe par l'option `auth`, que le serveur vérifie avant d'accepter la connexion. Les messages reçus vont dans un store ([`ZUSTAND`](ZUSTAND.md)) ou dans un état mis à jour avec la forme `set(x => …)`.

Liens : [`INDEX`](INDEX.md) · [`EFFETS`](EFFETS.md) · [`ZUSTAND`](ZUSTAND.md) · [`API`](API.md) (§3, proxy) · [`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md) (§6) · [doc Socket.IO, React](https://socket.io/how-to/use-with-react)

🧪 Labo : aucun. Les observations viennent d'un serveur `socket.io` 4.8.4 minimal, avec un middleware qui vérifie un jeton, derrière le proxy de Vite.

---

## 1. La socket, une seule fois, dans un module

```ts
import { io, type Socket } from "socket.io-client";

export type Message = { id: number; de: string; texte: string; a: number };

export type ServeurVersClient = {
    "chat:message": (m: Message) => void;
    "salon:arrivee": (pseudo: string) => void;
};

export type ClientVersServeur = {
    "salon:rejoindre": (salon: string, ack: (r: { ok: true } | { ok: false; erreur: string }) => void) => void;
    "chat:envoyer": (texte: string, ack: (r: { ok: boolean }) => void) => void;
};

export const socket: Socket<ServeurVersClient, ClientVersServeur> = io({
    autoConnect: false,
    auth: (cb) => cb({ token: localStorage.getItem("auth_token") }),
});
```

- `io()` **sans URL** : même origine que la page. En prod, Caddy relaie `/socket.io/*` vers Adonis ; en dev, le proxy de Vite avec `ws: true` fait de même ([`API`](API.md), §3). `socket.ts` de l'équipe fait l'équivalent avec `import.meta.env.PROD ? undefined : …`.
- `autoConnect: false` : créer la socket n'ouvre rien. On connecte quand un écran en a besoin.
- `Socket<ServeurVersClient, ClientVersServeur>` : les noms d'événements et leurs arguments sont **typés**. L'équipe range ces deux types dans `apps/types/ws_contract.ts`, partagé par le front et le back : si l'un change un événement, l'autre ne compile plus.
- `auth: (cb) => cb({ … })` : une **fonction**, rappelée à chaque tentative de connexion. Elle relit donc le jeton à chaque fois, y compris après une reconnexion. Avec un objet fixe (`auth: { token }`), le jeton serait figé à la création du module.

🔗 **Lien C.** La socket est un descripteur de fichier ouvert une fois et partagé. Créer une socket dans le corps d'un composant reviendrait à faire un `socket()` + `connect()` à chaque rendu : des dizaines de connexions ouvertes, jamais fermées.

---

## 2. S'abonner : `on` à l'entrée, `off` à la sortie

Observé sous `StrictMode`, avec un serveur qui envoie un message de bienvenue à l'arrivée dans un salon :

| Code | Messages affichés |
|---|---|
| `on` sans `off`, connexion dans un effet sans nettoyage | **4** fois le même message |
| `on` + `off`, connexion dans un effet sans nettoyage | **2** fois (le salon a été rejoint deux fois) |
| `on` + `off`, connexion **et** déconnexion dans l'effet (ci-dessous) | **1** fois |

```tsx
import { useEffect, useState } from "react";
import { socket, type Message } from "./socket";

export function Salon({ salon }: { salon: string }) {
    const [messages, setMessages] = useState<Message[]>([]);
    const [erreur, setErreur] = useState<string | null>(null);

    useEffect(() => {
        function surMessage(m: Message) {
            setMessages((liste) => [...liste, m]);
        }
        async function surConnexion() {
            try {
                const r = await socket.timeout(5000).emitWithAck("salon:rejoindre", salon);
                setErreur(r.ok ? null : r.erreur);
            } catch {
                setErreur("Le serveur ne répond pas");
            }
        }
        function surRefus(e: Error) {
            setErreur(`Connexion refusée : ${e.message}`);
        }

        socket.on("chat:message", surMessage);
        socket.on("connect", surConnexion);
        socket.on("connect_error", surRefus);
        socket.connect();

        return () => {
            socket.off("chat:message", surMessage);
            socket.off("connect", surConnexion);
            socket.off("connect_error", surRefus);
            socket.disconnect();
        };
    }, [salon]);

    return (
        <section>
            {erreur && <p role="alert">{erreur}</p>}
            <ul>{messages.map((m) => <li key={m.id}><b>{m.de}</b> {m.texte}</li>)}</ul>
        </section>
    );
}
```

Ligne par ligne :
- Les trois fonctions sont déclarées **dans** l'effet : `off` a besoin de **la même** fonction qu'`on`, comme `removeEventListener` ([`EFFETS`](EFFETS.md), §6). Un `socket.off("chat:message")` sans fonction retirerait **tous** les écouteurs de cet événement, y compris ceux d'autres composants. Observé : deux écouteurs sur `chat:message`, puis `off("chat:message")` → `listeners("chat:message").length` vaut `0`.
- `setMessages((liste) => [...liste, m])` : la forme fonction. `surMessage` est créée une fois et vit longtemps ; avec `setMessages([...messages, m])`, elle verrait toujours le `messages` vide du premier rendu ([`EFFETS`](EFFETS.md), §5).
- Rejoindre le salon dans l'écouteur **`connect`** : il se déclenche à la première connexion **et** à chaque reconnexion. Après une coupure, le serveur a oublié dans quels salons était la socket : il faut les rejoindre de nouveau. L'équipe fait la même chose avec `socket.io.on("reconnect", …)`.
- `[salon]` : changer de salon déconnecte, puis reconnecte et rejoint le nouveau.
- `socket.disconnect()` au nettoyage : sous StrictMode, la première connexion est fermée avant la seconde. Observé : une seule connexion active, un seul « bienvenue ».

🔗 **Lien C.** `on`/`off`, c'est `signal(SIGINT, handler)` puis le retour au gestionnaire par défaut. Un gestionnaire jamais retiré continue d'être appelé, même quand le code qui l'a posé n'existe plus.

---

## 3. Envoyer et attendre l'accusé de réception

```tsx
import { useState, type SubmitEvent } from "react";
import { socket } from "./socket";

export function Saisie() {
    const [envoi, setEnvoi] = useState(false);

    async function surEnvoi(e: SubmitEvent<HTMLFormElement>) {
        e.preventDefault();
        const form = e.currentTarget;
        const texte = String(new FormData(form).get("texte") ?? "").trim();
        if (!texte) return;
        setEnvoi(true);
        try {
            const r = await socket.timeout(5000).emitWithAck("chat:envoyer", texte);
            if (r.ok) form.reset();
        } catch {
            alert("Message non remis : le serveur ne répond pas");
        } finally {
            setEnvoi(false);
        }
    }

    return (
        <form onSubmit={surEnvoi}>
            <input name="texte" disabled={envoi} />
            <button disabled={envoi}>Envoyer</button>
        </form>
    );
}
```

- `emitWithAck` : envoie l'événement et renvoie une **promesse** de la réponse du serveur (le dernier argument `ack` du type `ClientVersServeur`).
- `.timeout(5000)` : sans lui, si le serveur ne répond jamais, la promesse attend **pour toujours**. Observé, avec le délai : `operation has timed out`.
- Le texte n'est effacé qu'**après** confirmation : si le message est perdu, le joueur ne perd pas sa saisie.

---

## 4. L'authentification de la connexion

Côté serveur, un middleware Socket.IO lit `socket.handshake.auth.token` et refuse la connexion si le jeton est faux. Observé côté client avec un mauvais jeton :

```text
connect_error : jeton refusé | socket.active = false
emitWithAck a échoué : operation has timed out
```

- `connect_error` reçoit le message de l'`Error` du middleware serveur.
- `socket.active = false` : un refus du **serveur** arrête les tentatives de reconnexion automatique. (Une coupure **réseau**, elle, laisse `active` à `true` et le client réessaie.) Après une nouvelle connexion du joueur, il faut appeler `socket.connect()` soi-même.
- Les `emit` faits pendant ce temps ne partent jamais : sans `timeout`, ils attendraient sans fin.

🚨 Le chat actuel de l'équipe n'a pas d'authentification : le client envoie son nom dans `room:join`, et le serveur le croit. N'importe qui peut donc parler sous n'importe quel nom. La vérification du jeton se fait côté Adonis, au moment de la connexion. Le nom vient ensuite de l'utilisateur authentifié, pas du message. Voir [`../adonis/SOCKET_IO`](../adonis/SOCKET_IO.md), §3 et §4, où ces corrections sont testées.

🚨 Le client ne fait jamais autorité. Il envoie des **intentions** (« je joue ces cartes »), le serveur vérifie et renvoie l'état ([`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md), §5).

---

## 5. Où ranger ce qui arrive

- Les messages d'**un écran** (le chat d'un salon) : un `useState` dans ce composant, comme au §2.
- Ce que **plusieurs écrans** lisent (l'état de la partie, les notifications, la liste des amis en ligne) : un store. L'écouteur écrit avec `usePartie.getState().appliquer(etat)`, et chaque composant lit son morceau avec un sélecteur ([`ZUSTAND`](ZUSTAND.md)).
- L'écouteur qui alimente un store peut vivre **hors** des composants, posé une fois au démarrage, puisque le store existe lui aussi hors de React. Il n'a alors pas de `off` : il vit aussi longtemps que la page.

---

## 6. La production

- Le Caddyfile de l'équipe relaie déjà `/socket.io/*` vers `backend:3333`. Si un jour la connexion échoue en prod mais pas en dev, c'est la première chose à vérifier.
- Un proxy ou un hébergeur peut couper une connexion inactive au bout de quelques minutes. Socket.IO envoie un *ping* régulier (`pingInterval`, 25 000 ms par défaut dans `engine.io`) qui la garde ouverte. Si tu changes ce réglage côté serveur, garde-le **en dessous** de ce délai de coupure.
- La reconnexion est automatique après une coupure réseau ; ton code doit seulement **rejoindre** à nouveau (§2) et, pour une partie, redemander l'état complet au serveur.

---

## 🧪 À essayer

1. Retire les `socket.off(...)` du nettoyage, garde `StrictMode`, et compte les messages reçus. Puis retire aussi `socket.disconnect()`.
2. Remplace `setMessages((liste) => [...liste, m])` par `setMessages([...messages, m])`. Envoie trois messages : combien en reste-t-il à l'écran ?
3. Coupe le serveur pendant que la page est ouverte, attends, relance-le. Le salon est-il rejoint de nouveau ? Regarde l'onglet **Réseau** → filtre `WS`.
4. Appelle `emitWithAck` vers un événement que le serveur n'écoute pas, sans `.timeout()`. Que devient la promesse ?
5. Mets un mauvais jeton dans `localStorage`, recharge, lis `connect_error`, puis remets le bon jeton. La socket se reconnecte-t-elle seule ? Que faut-il appeler ?
