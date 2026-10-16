# 05 — Level Guide

> **Important:** NetPractice randomizes IPs per student. There are no universal answers. This document covers **what each level teaches** and the **strategy** to solve it. Defending your solution at evaluation requires understanding the *concept*, not memorizing numbers.

The project has 10 levels. They get progressively harder, but the toolkit grows by exactly one idea per level.

---

## Level 1 — "Wire two hosts together"

**Goal:** Make two hosts on the same wire ping each other.

**Concepts introduced:** subnet mask basics, the rule that two interfaces on the same wire must share the same network address (under their mask).

**What's missing:** usually one host's IP or one mask.

**Strategy:**
1. Read both hosts' IPs and masks.
2. Compute `IP AND mask` for both. They must match.
3. If they don't, either change the missing IP to lie in the existing subnet, or correct the mask.

**Trap to avoid:** picking the network address (`.0`) or broadcast (`.255`) as the host IP.

---

## Level 2 — "Two hosts through a switch"

**Goal:** Same as L1 but with a switch in between (cosmetic — a switch is invisible to L3 routing).

**Concepts reinforced:** a switch is transparent. The math is identical to L1.

**Strategy:** ignore the switch. Solve as L1.

**Why this level exists:** to teach you that **switches are not routers**. A switch doesn't change subnet logic — only the wires they carry on.

---

## Level 3 — "Pick a working IP in a given subnet"

**Goal:** Given a fixed subnet, choose a valid host IP.

**Concepts introduced:** the usable-range rule (no network address, no broadcast, no conflicts).

**Strategy:**
1. Compute the subnet's network address and broadcast (see [`03_SUBNETTING.md`](03_SUBNETTING.md)).
2. Pick any IP strictly between them.
3. Avoid IPs already assigned to other interfaces in the level.

**Defensible answer for evaluator:** "I chose `.X` because it's between the network address `.Y` and broadcast `.Z`, and no other interface uses it."

---

## Level 4 — "Add a router"

**Goal:** Connect two LANs via a router. Each LAN is its own subnet; the router has one interface in each.

**Concepts introduced:** routers, default gateways, the rule that "host's gateway must be in host's subnet."

**Strategy:**
1. Identify the two LANs and their subnets.
2. The router's two interfaces must each fall inside their respective LANs.
3. Each host must have its **default gateway** set to the router's IP **on that host's LAN**.

**Trap to avoid:** giving a host a gateway IP from the **other** LAN — unreachable.

---

## Level 5 — "Routing table — first real entry"

**Goal:** A multi-router topology where one router needs an explicit route (not just a default).

**Concepts introduced:** specific (non-default) routing table entries, the "next-hop must be in a directly-connected subnet" rule.

**Strategy:**
1. Trace the path a packet must take from source LAN to destination LAN.
2. For each router along the path, ask: "does this router know how to reach the destination LAN?"
3. If no, add a route: `destination LAN / mask → next-hop IP (the next router's near-side interface)`.
4. Same in reverse for return traffic.

**Defensible answer:** "Router R needs to forward traffic to LAN X. The next router toward X is N, whose interface on R's link is `N_IP`. So the entry is `X.0/mask → N_IP`."

---

## Level 6 — "Tighter masks"

**Goal:** Same as previous levels but with smaller subnets (`/26`, `/28`, `/30`) — less room to maneuver.

**Concepts reinforced:** block-size arithmetic from [`03_SUBNETTING.md`](03_SUBNETTING.md). A `/30` subnet has only 2 usable IPs, both of which are taken by the routers — there's no room for hosts.

**Strategy:**
1. For each subnet, compute network and broadcast precisely.
2. Verify no interface accidentally falls in a different block.
3. Verify gateways still satisfy the in-subnet rule.

**Trap to avoid:** Off-by-one mistakes on block boundaries. `10.0.0.32/27` runs `.32–.63`, not `.33–.62` — but only `.33–.62` are usable hosts.

---

## Level 7 — "Two paths"

**Goal:** A topology where a router has **two possible paths** to reach a destination. Longest-prefix match starts to matter.

**Concepts introduced:** route specificity, longest-prefix match in practice.

**Strategy:**
1. Identify which route should be used (usually the one the level diagram implies).
2. Make sure that route has a **longer prefix** than the default route — automatic, since `0.0.0.0/0` is the shortest possible prefix.
3. If both routes have the same prefix length and point to different next-hops, you have a problem — only one can be "right" per the level.

**Trap to avoid:** adding a route that **overlaps** an existing one but with the wrong next-hop — the longest-prefix match might still pick yours, sending traffic the wrong way.

---

## Level 8 — "Restricted /30 links between routers"

**Goal:** Several routers, each connected by `/30` point-to-point links. The internal layout is dense.

**Concepts reinforced:** `/30` arithmetic (4 addresses total, 2 usable). Routers must agree on subnet for each link.

**Strategy:**
1. For each `/30` link, compute the 4 IPs explicitly. Both router interfaces must be the `.1` and `.2` of that block (whichever block applies).
2. Don't reuse a `/30` subnet across two links — every link is independent.
3. Every router needs routes for every non-adjacent LAN.

**The defining feature of L8 is paperwork** — there are many small subnets to track. Writing each link's `(network, .1, .2, broadcast)` on paper helps.

---

## Level 9 — "Constrained subnets"

**Goal:** You're given a "supernet" (a big subnet like `/22`) and must carve smaller subnets out of it for specific LANs.

**Concepts introduced:** VLSM (Variable Length Subnet Masks), supernetting, hierarchical subnet design.

**Strategy:**
1. List the LANs and how many hosts each needs.
2. For each LAN, pick the smallest mask that fits: a LAN of 50 hosts needs at least `/26` (62 usable); 100 hosts → `/25` (126 usable).
3. Allocate non-overlapping blocks from the supernet, largest LANs first (or in the order specified).
4. Configure each LAN with its block.

**Trap to avoid:** picking a smaller subnet than needed (too few hosts), or overlapping two subnets within the supernet.

---

## Level 10 — "All of the above"

**Goal:** A multi-router, multi-LAN topology with constraints from every previous level combined.

**Concepts:** all of them. This is the validation.

**Strategy:**
1. **Draw the topology** on paper. Label every interface, every subnet, every link.
2. For each LAN, derive: network, broadcast, gateway, host IPs.
3. For each `/30` link, derive the same.
4. For each router, write the **full routing table** (one row per non-directly-connected LAN, plus optional default).
5. For each host, set IP/mask/gateway. Verify the gateway-in-subnet rule.
6. **Trace ping in both directions** for every host-to-host pair tested by the level. Find the first failure and fix it.

**Defensible answer for the whole project:** at the evaluator, walk through one level in detail with your hand-drawn topology and the routing-table reasoning. If you can defend L10, you can defend the project.

---

## General playbook (applies to every level)

```
  1. READ all the values already set on every interface.
  2. DRAW the topology on paper. Mark each subnet's range.
  3. For each red interface, ASK:
       a. Is its IP in the subnet shared by the wire it's on?
       b. Is its mask consistent with the other interfaces on the wire?
       c. If it has a gateway, is the gateway IP in its own subnet?
       d. If it's a router, does it have a route to the destination LAN?
       e. Does the destination LAN have a route back?
  4. FIX one thing at a time. Re-check the topology.
  5. WALK both directions of a ping before declaring success.
```

`★ Insight ─────────────────────────────────────`
The temptation is to "guess and click" — change values until green. **Don't.** You will be evaluated, and "I changed values until it was green" is not a defense. Solve each level on paper first, then enter the values. The 10 minutes you spend on paper save you the embarrassment of not being able to explain your own answers.
`─────────────────────────────────────────────────`

---

## Next

→ [`06_TROUBLESHOOTING.md`](06_TROUBLESHOOTING.md): when the network is red and you don't know why — the decision tree.
