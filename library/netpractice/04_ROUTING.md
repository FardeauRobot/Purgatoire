# 04 — Routing

> Hosts on different subnets can't talk directly. They need **routers**, and routers need **rules**. The rules live in the routing table.

---

## 1. What a routing table is

Every host and every router has one. Conceptually:

```
  ┌──────────────────────────────────────────────────────────────┐
  │ DESTINATION  /  PREFIX        NEXT-HOP        INTERFACE       │
  ├──────────────────────────────────────────────────────────────┤
  │ 10.0.0.0     /24              (direct)        eth0            │
  │ 192.168.1.0  /24              (direct)        eth1            │
  │ 172.16.0.0   /16              10.0.0.1        eth0            │
  │ 0.0.0.0      /0               192.168.1.1     eth1            │   ← default route
  └──────────────────────────────────────────────────────────────┘
```

Each row says: *"For destinations matching this network, send the packet to that next-hop via this interface."*

In NetPractice, you edit two fields per row:
- **Destination network** (often `0.0.0.0/0` — the default route)
- **Next-hop IP** (the gateway — must be in a subnet you can reach directly)

---

## 2. How a packet decides where to go

When a host or router has a packet for destination `D`, it walks the routing table and picks **the most specific match** (longest prefix). Algorithm:

```
  for each route in table:
      if (D AND route.mask) == route.network:
          this route matches
  pick the matching route with the longest prefix (most 1-bits in mask)
  send packet via that route's next-hop
```

### Worked example

Routing table:
```
  10.0.0.0   /8    → R2
  10.1.0.0   /16   → R3
  0.0.0.0    /0    → R1   (default route)
```

Destination `10.1.2.3`:
- Matches `10.0.0.0/8`? `10.1.2.3 AND 255.0.0.0 = 10.0.0.0`. ✓ — prefix 8.
- Matches `10.1.0.0/16`? `10.1.2.3 AND 255.255.0.0 = 10.1.0.0`. ✓ — prefix 16.
- Matches `0.0.0.0/0`? Always. — prefix 0.

The longest is `/16` → packet goes to R3. **Longest-prefix match wins.**

---

## 3. The default route — `0.0.0.0/0`

The catch-all. `0.0.0.0/0` matches every IPv4 address (since the mask is zero — no bits to compare). It's the **fallback** when no more specific route applies.

A host's "default gateway" is just the next-hop of the `0.0.0.0/0` route in its table.

In NetPractice, when a host has a `Default Gateway` field, that's syntactic sugar for adding the row:
```
  0.0.0.0   /0   → <that gateway IP>   eth0
```

---

## 4. The "next-hop must be reachable" rule

The next-hop IP in any route **must be in a subnet you can reach directly** — i.e., the same subnet as one of your own interfaces.

```
  Host H:   10.0.0.5  /24
  Default gateway:   10.0.0.1     ✅ (10.0.0.1 is in 10.0.0.0/24, same as H)

  Host H:   10.0.0.5  /24
  Default gateway:   10.0.1.1     ❌ (10.0.1.1 is in a different subnet — unreachable)
```

In the second case, H wants to reach `10.0.1.1`, but to reach `10.0.1.1` it needs a gateway, but its only gateway is `10.0.1.1` — circular. NetPractice marks this red.

**This is the single most common student bug in NetPractice.** When an interface is red, check the gateway-in-subnet rule first.

---

## 5. The router-asymmetry trap

A router can't route what it doesn't have a route for. **Both** sides of a connection need to know how to reach **both** sides.

```
                  10.0.0.0/24                    192.168.1.0/24
   ┌─────┐   ┌────────────────────┐         ┌────────────────────┐   ┌─────┐
   │ H1  ├───┤ R1 .1      R1 .254 ├─────────┤ R2 .1      R2 .254 ├───┤ H2  │
   │ .50 │   └────────────────────┘   /30   └────────────────────┘   │ .50 │
   └─────┘                            link                           └─────┘
```

For H1 → H2 traffic to work:
- H1 has default gateway `10.0.0.1` (R1's LAN-side IP). ✓
- R1 needs a route to `192.168.1.0/24` via R2's link-side IP. ✓
- R2 needs a route to `10.0.0.0/24` via R1's link-side IP. ✓
- H2 has default gateway `192.168.1.254` (R2's LAN-side IP). ✓

**Forget any one of these four and the network is broken — but the symptom looks the same on the UI.** Working through every hop, both directions, is the discipline.

`★ Insight ─────────────────────────────────────`
A common student mistake: setting H1's default gateway correctly but forgetting that R1 also needs a route for the return path. In NetPractice, ICMP/ping is bidirectional — even if the request reaches H2, if H2's reply can't get home, the test fails. Always trace **both directions** when debugging.
`─────────────────────────────────────────────────`

---

## 6. Direct vs indirect routes

There are two kinds of routes in any table:

| Type | What it looks like | What it means |
|---|---|---|
| **Direct (connected)** | Destination is one of *your* subnets, next-hop blank/self | "This subnet is on my own wire — I can ARP and deliver directly." |
| **Indirect (static / default)** | Destination is some other subnet, next-hop is a neighbor router | "Hand this to my neighbor; let them deal with it." |

Direct routes appear **automatically** when you configure an interface — you don't add them manually. In NetPractice, you only manage **indirect** routes (and the default).

---

## 7. Two ways to specify a route in NetPractice

NetPractice gives you a few different fields per host/router. They all map to the same routing table:

| Field | Equivalent row |
|---|---|
| Host's "Default Gateway" | `0.0.0.0/0 → <gateway>` |
| Router's "Default Route" or "Route 1 Destination = 0.0.0.0/0" | same |
| Router's "Route 1 Destination = X.Y.Z.0/24, Next-hop = ..." | `X.Y.Z.0/24 → <next-hop>` (specific route) |

You choose between **default routes** (one rule catches everything) and **specific routes** (one rule per remote network).

**Default routes are simpler and almost always sufficient** unless a router has two paths to different parts of the network and the level forces you to discriminate. Even at level 10, defaults work for most edges.

---

## 8. Detecting routing loops by eye

A loop is when packets bounce between routers forever. Common cause in NetPractice: two routers each have a default route pointing at the *other*.

```
   R1's default route → R2
   R2's default route → R1
```

Then a packet for any destination not directly connected to either router ping-pongs forever (in reality, TTL drops to 0 and the packet dies). NetPractice catches this and flags the affected interfaces.

**Rule:** in any chain of routers, only **one** router (usually the one closest to "the outside") should have a default route pointing further out. The others should have **specific** routes pointing inward.

```
      LAN                          /30 link                          LAN
   ┌────────┐                  ┌────────────┐                  ┌────────┐
   │ Host A ├──── R1 ──────────┤            ├──────── R2 ──────┤ Host B │
   └────────┘                  └────────────┘                  └────────┘
       │                           R1 │  R2                          │
       │                              ▼   ▼                          │
       │              R1's default → R2        R2's default → R1     │ ← LOOP if both are defaults
       │                                                             │
       │   Better:                                                   │
       │   R1's route to B's LAN → R2                                │
       │   R2's route to A's LAN → R1                                │
       └─────────────────────────────────────────────────────────────┘
```

In tree topologies (NetPractice favorites): leaves (LAN-edge routers) have **default routes pointing inward**, internal routers have **specific routes per leaf LAN**.

---

## 9. Putting it all together — the packet's journey

End-to-end ping from `H1 (10.0.0.50)` to `H2 (192.168.1.50)`:

```
  1. H1 wants to send to 192.168.1.50.
     H1's subnet is 10.0.0.0/24. Is 192.168.1.50 in it? No.
     → Use default gateway 10.0.0.1 (R1).
     → ARP for 10.0.0.1, encapsulate IP packet in Ethernet frame, send.

  2. R1 receives frame. IP destination = 192.168.1.50.
     R1's directly connected subnets: 10.0.0.0/24, 172.16.0.0/30.
     Routing table: 192.168.1.0/24 via 172.16.0.2 (R2).
     → ARP for 172.16.0.2, re-encapsulate, send out the /30 link.

  3. R2 receives. IP destination = 192.168.1.50.
     R2's directly connected: 172.16.0.0/30, 192.168.1.0/24.
     192.168.1.50 ∈ 192.168.1.0/24 → directly connected.
     → ARP for 192.168.1.50, send.

  4. H2 receives. Replies. Same journey in reverse — relies on every router
     having a route back to 10.0.0.0/24.
```

Every red interface in NetPractice traces back to a broken step in this journey. The discipline is to **walk both directions step by step** and find the first hop that fails the math.

---

## Next

→ [`05_LEVEL_GUIDE.md`](05_LEVEL_GUIDE.md): a per-level summary of what each NetPractice level tests, and the strategy to attack it.
