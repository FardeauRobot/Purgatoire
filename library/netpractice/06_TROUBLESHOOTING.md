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

A few habits that prevent bugs before they happen:

- **Always write the subnet first**, then pick the IP. Not the other way around.
- **Never leave a default gateway field blank** if the level expects routing across subnets.
- **Sketch the topology** before changing anything. The visual UI is too small to keep in your head.
- **Pick host IPs from the middle of the range.** It leaves room and avoids accidentally landing on network/broadcast.
- **Use `.1` for gateways consistently** (or `.254` — pick one and stick with it). Reduces "where's the gateway?" cognitive load.

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
