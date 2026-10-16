# Level 10 — Final: seven goals, mostly-locked routing tables

> Randomized per student. **Method-first** note. The last level is less about clever subnetting and more about **reading mostly-fixed routing tables** and making the few editable fields consistent with them.

---

## What this level looks like

```
H1 ─┐
    ├─ Switch ── R11 [ R1 ] R12 ── I1 [ Internet ]
H2 ─┘                R13
                      │
                     R21 [ R2 ] R22 ── H3
                           R23 ── H4
```

Goals: **H1↔H2, H3↔H4, H1↔Internet, H1↔H4, H2↔H3, H3↔Internet, H4↔Internet** — seven paths.

---

## The character of Level 10

Most routes and many IPs/masks are **locked**. The fixed entries already encode a consistent design (built around `163.172.250.x` for the Internet link, `10.0.0.x` between the routers, and a `[a].[b].[c].x` block split into `/25`/`/26` pieces). Your job is to **read what's fixed, then set the handful of editable fields so they don't contradict it.** Fighting the fixed design is the classic Level-10 mistake.

The editable fields are typically: a couple of host **masks**, one router interface **mask** (`R13`), router R2's host-side **IP/mask** (`R22`), the `8.8.8.8`-facing `R23` IP/mask, and one or two **gateways/routes** (e.g. H3's gateway, R1's `10.0.0.0/8` route, the Internet's return route).

---

## Diagnostic order

1. **Map the fixed anchors first.** Write down every locked IP+mask and locked route. They define the subnets you must conform to — e.g. `R11`'s `/25` foot fixes the H1/H2 switch subnet; `H4`'s fixed IP+mask fixes the R23 subnet; the Internet's `/28` fixes the ISP link.
2. **Switch subnet:** H1, H2, R11 must share R11's subnet/mask. Set H1's/H2's editable masks to match; pick valid distinct hosts.
3. **Each link:** make the editable end match the locked end's subnet and mask (R13↔R21, R22↔H3, R23↔H4).
4. **Host gateways:** each host's default route → its local router foot. Fix the editable one (e.g. H3's gateway → R22's IP).
5. **Router routes:** honour the locked entries; set the editable route/gateway so the router can reach the far LANs and the Internet. Watch **longest-prefix match** — the fixed specific routes win over your default, by design.
6. **Internet return route:** the editable Internet route must cover the inside networks (right network, wide-enough mask) via R1's Internet foot.
7. **Trace all seven goals both directions.**

---

## Why "read the fixed table" is the whole skill

A locked route like `[a].[b].[c].128/26 via [a].[b].[c].253` is telling you: *"the `.128/26` subnet exists and is reached through `.253`."* That pins down (a) a subnet boundary you must respect when you assign editable IPs, and (b) which interface must carry that IP. If you assign an editable interface an address that contradicts a fixed route, the goal fails and the cause is hard to see. So **derive your free choices from the locked constraints**, never the reverse.

---

## Recurring traps

- **Contradicting a fixed route** with an editable IP/mask — the bug hides because the route "looks fine."
- **Switch members' masks** not matching R11's `/25`.
- **A host gateway off its own subnet.**
- **Longest-prefix surprises:** a fixed `/26` or `/27` beats your `/0` default — make sure the specific routes point where they should and your default only catches the rest.
- **Internet return route** too narrow or pointing at the wrong foot.
- Network/broadcast addresses in the small `/25`/`/26` subnets.

---

## Checklist

- [ ] List every **locked** IP, mask, and route first — they are the spec.
- [ ] Switch hosts share R11's subnet + mask.
- [ ] Editable link ends conform to the locked ends.
- [ ] Each host gateway is a local router foot.
- [ ] Editable routes/gateways are consistent with the fixed table; **longest-prefix** respected.
- [ ] Internet return route covers the inside LANs.
- [ ] All seven goals verified **both ways**.
