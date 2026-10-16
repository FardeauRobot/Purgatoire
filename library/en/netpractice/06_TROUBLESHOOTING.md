# 06 — Troubleshooting

> The UI is laconic: green or red, with one error message that's usually generic. This page is the **diagnostic flowchart** you run when a level refuses to go green.

---

## 1. The five questions, in order

When an interface is red, ask these in this exact order. Stop at the first "no."

```
   ┌─── 1. Is the IP a VALID HOST IP for its declared subnet? ─────┐
   │       (not network address, not broadcast, in range)          │
   └─────────────────────────┬─────────────────────────────────────┘
                             │ yes
   ┌─── 2. Do all interfaces on the SAME WIRE share a subnet? ─────┐
   │       (IP AND mask matches for every pair on the wire)        │
   └─────────────────────────┬─────────────────────────────────────┘
                             │ yes
   ┌─── 3. Is the GATEWAY in the SAME SUBNET as the host?  ────────┐
   │       (gateway AND mask == host AND mask)                     │
   └─────────────────────────┬─────────────────────────────────────┘
                             │ yes
   ┌─── 4. Does every router along the path have a ROUTE          ─┐
   │       to the destination LAN, in BOTH directions?             │
   └─────────────────────────┬─────────────────────────────────────┘
                             │ yes
   ┌─── 5. Are there OVERLAPPING SUBNETS or routing LOOPS?  ───────┐
   │       (one router's default points at another that loops back)│
   └───────────────────────────────────────────────────────────────┘
```

90% of red lights are caught at step 1, 2, or 3.

---

## 2. Question 1 — "Is the IP valid?"

Compute the subnet's network address and broadcast (use the recipe in [`03_SUBNETTING.md#8`](03_SUBNETTING.md)).

Reject if:
- IP equals the network address (host bits all 0)
- IP equals the broadcast address (host bits all 1)
- IP is outside the subnet range entirely (different block)
- IP is identical to another interface on the same wire (duplicate)

**Most common bug:** picking `.0` or `.255` as a host IP because they "look round." `.0` is the network address of any `/24` subnet — not assignable.

---

## 3. Question 2 — "Same wire, same subnet?"

For each pair of interfaces visually connected (directly or through a switch — switches are transparent):

```
  Compute  (IP_A AND mask_A)
  Compute  (IP_B AND mask_B)
  They MUST be equal AND the masks MUST be equal.
```

**Mask mismatch is sneaky** — both endpoints can compute the same network address but with different masks, and *one* of them will reject. The masks must match too.

```
  H1: 10.0.0.10 /24    →  network 10.0.0.0    ✅ network ok
  H2: 10.0.0.20 /16    →  network 10.0.0.0    ✅ network ok
                                                    ↑ BUT masks differ → broken
```

In NetPractice, this usually shows up as **one** interface being red, often the one whose mask is "wrong" relative to the rest. Match masks across the wire.

---

## 4. Question 3 — "Gateway in subnet?"

A host or router's "Default Gateway" (or next-hop in any route entry) **must** be in a subnet that the host can reach directly — i.e., the same subnet as one of the host's own interfaces.

```
  Host:    192.168.1.50  /24      → host subnet = 192.168.1.0/24
  Gateway: 192.168.2.1            → gateway subnet = 192.168.2.0/24    ❌
```

The gateway IP `192.168.2.1` is not in `192.168.1.0/24`. The host has no idea how to send a frame to it. Red.

**Fix:** the gateway must be in the **host's own subnet**. Usually that means setting the gateway to one of the **router's interfaces on this LAN**, not on a remote LAN.

---

## 5. Question 4 — "Routes in both directions?"

For each pair (Source LAN, Destination LAN) that the level tests:

```
  For each router on the path source → destination:
      Does this router have a route to the destination LAN?
      If no → add it. Next-hop is the next router toward the destination.

  Repeat for the reverse path destination → source.
```

`★ Insight ─────────────────────────────────────`
Asymmetric routing is the #1 reason a level fails after L5. Ping is a request and a response — if the request reaches the target but the response can't come back (because some intermediate router lacks a return route), the level is still broken. Always trace both directions.
`─────────────────────────────────────────────────`

---

## 6. Question 5 — "Loops or overlaps?"

Two pathologies to check at this stage.

### Routing loops

If router A's default route points to router B, **and** B's default points to A — packets to any non-directly-connected LAN loop forever.

**Diagnosis:** look for a chain of routers where defaults form a cycle.

**Fix:** in any chain, only the router closest to "the outside" (or the implicit root in NetPractice's diagram) has a default pointing outward. Other routers have **specific** routes pointing inward (toward their leaf LANs).

### Overlapping subnets

Two interfaces with different masks that both claim address ranges that overlap.

```
  R1 interface: 10.0.0.1   /24   →  subnet 10.0.0.0/24    (covers 10.0.0.0–10.0.0.255)
  R2 interface: 10.0.0.130 /25   →  subnet 10.0.0.128/25  (covers 10.0.0.128–10.0.0.255)
```

R1 thinks `10.0.0.200` is on its own LAN. R2 thinks `10.0.0.200` is on its own LAN. Both can't be right. Routing fails ambiguously.

**Fix:** redesign so subnets don't overlap. Usually means narrowing the mask on the bigger subnet.

---

## 7. The "I've checked everything and it's still red" recovery

If you've genuinely walked the five questions and nothing is wrong, try these:

1. **Reload the level.** Sometimes the UI keeps a stale state.
2. **Look at the level's pre-set values.** Sometimes a pre-set IP is not in the subnet you assumed — re-derive the subnet from scratch.
3. **Find the RED interface, not the red label.** NetPractice highlights the *interface* that fails. The fix may be on a neighbouring interface (e.g., the wrong mask is on the *other* end of the wire).
4. **Check for typos.** `10.0.0.10` and `10.0.10.0` look very similar at a glance.
5. **Re-derive everything on paper.** Don't trust your memory of "I already checked this." Compute network and broadcast freshly.

---

## 8. Defensive habits

A few habits that prevent bugs before they happen. Each one comes with a worked example.

### Always write the subnet first, then pick the IP

Decide the network and mask *before* you touch the host octet. If you pick the IP first you end up bending the mask to fit it, which is how overlaps and off-by-one blocks sneak in.

```
  ❌ IP-first:   "I'll type 192.168.1.130 ... now what mask makes that legal?"
                 → you reverse-engineer the mask, guess /25, and hope.

  ✅ subnet-first: subnet = 192.168.1.0/25  → range .1–.126, broadcast .127
                   now pick a host inside it → 192.168.1.60   ✅ provably valid
```

The subnet defines the range; the IP just has to sit inside it. Reverse that order and you're validating blind.

### Never leave a default gateway field blank if the level expects routing across subnets

A host with no gateway can talk to its own LAN and nothing else. The moment a level asks two different subnets to reach each other, an empty gateway field is a guaranteed red.

```
  Host A: 10.0.1.10 /24   gateway: (blank)     ← wants to reach 10.0.2.0/24
                                                 → frame for a remote LAN has
                                                   nowhere to go. Red.

  Fix →   gateway: 10.0.1.1   (the router's interface on A's own LAN)
```

Blank is only correct when everything the host needs is on its own wire (a single-subnet level).

### Sketch the topology before changing anything

Redraw the wires, subnets, and routers on paper first. The in-browser diagram is too cramped to hold in your head, and asymmetric-routing bugs (see §5) are invisible until you can see both directions at once.

```
   [Host A]──┐                       ┌──[Host B]
             ├─[R1]───────[R2]───────┤
   10.0.1.0/24   .1     ?     .1   10.0.2.0/24
                    └── what subnet
                        is THIS wire? ──┘
```

The inter-router link is the wire people forget — sketching it forces you to give it its own subnet instead of leaving it implicit.

### Pick host IPs from the middle of the range

The two failure addresses of any subnet are the network address (host bits all 0) and the broadcast (host bits all 1). Landing on either is an instant reject. Picking from the middle keeps you far from both edges.

```
  Subnet 172.16.5.0/24  →  network .0, broadcast .255, usable .1–.254

  .0    ❌ network address    — not assignable
  .1    ⚠️  legal but often the gateway; easy to collide with it
  .130  ✅ middle of the range — no edge, no gateway clash
  .255  ❌ broadcast          — not assignable
```

Middle IPs also survive a mask change better: if the level later forces a `/25`, `.130` is still a valid host in `172.16.5.128/25`, whereas `.1` might now be stranded in the wrong half.

### Use `.1` for gateways consistently (or `.254` — pick one and stick with it)

Give every router interface the same host number across every LAN. Then "where's the gateway?" is never a question — it's always `.1` (or always `.254`). Mixing conventions is how you point a host at a gateway that isn't there.

```
  10.0.1.0/24  → gateway 10.0.1.1
  10.0.2.0/24  → gateway 10.0.2.1
  10.0.3.0/24  → gateway 10.0.3.1
                          ↑ always .1 — muscle memory, zero lookup
```

If you commit to `.254` instead, the same logic applies (`10.0.1.254`, `10.0.2.254`, …) — the point is *one* convention, not which one.

---

## 9. Common error patterns and their fixes

| Symptom | Likely cause | Fix |
|---|---|---|
| Host can ping its gateway but not beyond | Router lacks a return route | Add specific route on the router pointing back |
| Two hosts on same switch can't ping | Subnet/mask mismatch | Make masks identical, IPs in same subnet |
| Host's interface is red, others are green | Host IP is network or broadcast | Pick a middle IP |
| One direction works, the other doesn't | Asymmetric routing | Trace return path, add missing route |
| Everything green except one router | That router's interface is in the wrong subnet | Re-derive the subnet for that wire |
| All interfaces green but level not validated | A test the UI hides (a deeper host) | Read the level's text again — there might be a host on a far LAN you missed |

---

## Next

→ [`07_EVAL_CHEATSHEET.md`](07_EVAL_CHEATSHEET.md): the one-page summary for evaluation day.
