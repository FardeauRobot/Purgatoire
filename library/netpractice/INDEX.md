# 🌐 NetPractice — Support Library

> *"A packet is a stranger looking for a door; the routing table is the doorman."*

Everything you need to **understand, solve, and defend** the 42 NetPractice project. NetPractice has no code: you fix 10 broken networks by editing IPs, subnet masks, and routing entries until every machine can reach every other machine it's supposed to.

The catch — the web interface only tells you *that* the network is broken, not *why*. This library teaches you to diagnose by hand.

---

## 🗺️ Reading order

If you've never touched IP networking before, read these in order. Each builds on the last.

| # | File | What you learn |
|---|---|---|
| 1 | [`01_FOUNDATIONS.md`](01_FOUNDATIONS.md) | What an IP address actually is — dotted-decimal, 32 bits, classes, CIDR notation. The vocabulary you need. |
| 2 | [`02_BINARY_AND_MASKS.md`](02_BINARY_AND_MASKS.md) | Converting between decimal and binary by hand. The bitwise AND that decides "are these two hosts on the same network?" |
| 3 | [`03_SUBNETTING.md`](03_SUBNETTING.md) | Splitting a network into smaller ones. Computing the usable host range, network address, and broadcast for any `a.b.c.d/n`. |
| 4 | [`04_ROUTING.md`](04_ROUTING.md) | Routing tables, the default route, next-hop logic, longest-prefix match. How a packet actually finds its destination. |
| 5 | [`05_LEVEL_GUIDE.md`](05_LEVEL_GUIDE.md) | What each of the 10 levels is testing. Methodology, **not** answers — answers change per student. |
| 6 | [`06_TROUBLESHOOTING.md`](06_TROUBLESHOOTING.md) | Decision tree: "the network is red, what now?" Step-by-step diagnostic. |
| 7 | [`07_EVAL_CHEATSHEET.md`](07_EVAL_CHEATSHEET.md) | The one-pager you skim 15 minutes before evaluation defense. |
| 8 | [`08_FIELD_MANUAL.md`](08_FIELD_MANUAL.md) | **Solve & defend** — the merge of 06 + 07, reorganized around *doing* a level: compute → place → route → trace → defend. The page to keep open while solving. |

---

## 🧩 Per-level worked notes

Concrete walkthroughs of each level I solve, in [`levels/`](levels/). One file per exercise — the layout and lessons are stable, the exact numbers are my instance.

| Level | Note | Teaches |
|---|---|---|
| 1 | [`levels/LEVEL_01.md`](levels/LEVEL_01.md) | Two hosts on a wire; mask width decides which octets must match. |
| 2 | [`levels/LEVEL_02.md`](levels/LEVEL_02.md) | Two isolated host pairs: legal-mask rule + don't sit on the broadcast. |
| 3 | [`levels/LEVEL_03.md`](levels/LEVEL_03.md) | Three hosts on one switch: switch is transparent → all share one subnet. |
| 4 | [`levels/LEVEL_04.md`](levels/LEVEL_04.md) | First router; a router is a host with many feet — only wire the connected one. |
| 5 | [`levels/LEVEL_05.md`](levels/LEVEL_05.md) | First routing: default route, gateway must be on the host's own subnet. |
| 6 | [`levels/LEVEL_06.md`](levels/LEVEL_06.md) | Reaching the Internet — default route + the **return route** trap. |
| 7 | [`levels/LEVEL_07.md`](levels/LEVEL_07.md) | Two routers in series: carve one block into non-overlapping subnets. |
| 8 | [`levels/LEVEL_08.md`](levels/LEVEL_08.md) | Capstone (method): mask mismatches, local gateways, return paths. |
| 9 | [`levels/LEVEL_09.md`](levels/LEVEL_09.md) | Capstone (method): switch + 2 routers + Internet, longest-prefix match. |
| 10 | [`levels/LEVEL_10.md`](levels/LEVEL_10.md) | Capstone (method): read the mostly-locked routing table, conform to it. |

---

## ✅ Validation checklist

You're ready to evaluate when you can:

- [ ] Convert any IP between decimal and binary without a calculator
- [ ] Given `a.b.c.d/n`, compute network address, broadcast, first usable, last usable, host count
- [ ] Read a routing table and predict which interface a packet exits
- [ ] Explain why two interfaces on the **same router** must be in **different subnets**
- [ ] Explain why a host's IP and its gateway must be in the **same subnet**
- [ ] Defend every change you made in the 10 levels (the evaluator *will* ask "why")

If any of these is shaky, re-read the relevant doc.

---

## 🎯 The mental model

Three rules underpin every level. Internalize these and the rest is mechanical:

1. **Same subnet, same wire** — Two interfaces communicate directly only if their `IP & mask` produce the same network address.
2. **Different subnets need a router** — To cross subnets, a host needs a route (often the default route via a gateway) pointing to a router interface **in its own subnet**.
3. **A router is just a host with many feet** — Each interface is an IP in a different subnet. The routing table decides which foot to send a packet out of.

Everything else (longest-prefix match, broadcast, /31 oddities) is detail.

---

## 📌 Notes

- This documentation is **subject-agnostic** — it works for any student's NetPractice instance. Your specific IPs differ from your neighbour's; the *method* is identical.
- For the official subject PDF, see the 42 intranet — this library complements, never replaces, the subject.
- Diagrams are ASCII so they render in any terminal and inside the Obsidian-flavored library mirror at `library/Purgatoire/`.
