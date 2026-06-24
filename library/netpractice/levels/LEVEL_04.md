# Level 4 — First router box (but no routing yet)

> Randomized per student. Representative, self-verified figures below — not your literal screen.

---

## What this level looks like

A router `My_Gate` with **three** interfaces, but only one of them is wired up — through a switch to two hosts:

```
Host A ── A1 ─┐
              ├─ Switch ── R1 ─[ My_Gate ]─ R2  (unconnected)
Host B ── B1 ─┘                          └─ R3  (unconnected)
```

Goals: **A↔B, A↔R, B↔R.**

---

## The two concepts to grasp

### 1. A router is just a host with many feet
Each interface (`R1`, `R2`, `R3`) is an IP in its **own** subnet. Here only `R1` is on a wire, so **only `R1` matters**. `R2` and `R3` lead nowhere — touching them is wasted effort, and the level deliberately dangles them to see if you over-configure.

### 2. A connected router interface obeys the same subnet rule as a host
`A1`, `B1`, and `R1` all hang off one switch → **same subnet** (same mask, same network address). That's it — this is Level 3 with a router as one of the three nodes. **No routing table is needed**, because every goal is *within* this one subnet (reaching "R" just means reaching `R1`, which is local).

> But beware: a router **cannot have two interfaces in the same subnet**. So the subnet you pick for `R1` must not overlap `R2`'s or `R3`'s subnet.

---

## Worked instance (representative numbers)

Base block `80.50.115.x`. Fixed anchors:

| Interface | IP | Mask | Editable | Note |
|-----------|----|------|----------|------|
| A1 | `80.50.115.132` | `255.255.255.240` (`/28`) | mask only | **IP locked** |
| B1 | (default elsewhere) | (default `/16`) | IP + mask | both free |
| R1 | (default `.91`) | (default `/23`) | IP + mask | both free |
| R2 | `80.50.115.1` | `255.255.255.128` (`/25`) | locked | **decoy, .0–.127** |
| R3 | `80.50.115.244` | `255.255.255.192` (`/26`) | locked | **decoy, .192–.255** |

### Pick the subnet around the locked anchor

`A1` is locked at `.132`. Its default mask `/28` gives block size 16 → `.132` lives in:

```
Network   : 80.50.115.128
Hosts     : 80.50.115.129 – .142
Broadcast : 80.50.115.143
```

Crucial check: does `.128/28` (`.128–.143`) overlap the decoys? R2 owns `.0–.127`, R3 owns `.192–.255`. **`.128–.143` is clear of both.** Good — keep `A1` at `/28`.

### Fit `R1` and `B1` into it

- **R1** → IP `80.50.115.129`, mask `255.255.255.240` (`/28`)  *(was `.91 //23` — far too wide, would swallow the decoys)*
- **B1** → IP `80.50.115.130`, mask `255.255.255.240` (`/28`)

All three now on `80.50.115.128/28`, distinct hosts, none `.128`/`.143`. R2/R3 left untouched.

---

## Solution summary

| Field | Action | Why |
|-------|--------|-----|
| A1 mask | keep `/28` | block `.128–.143` avoids the R2/R3 decoy subnets |
| R1 IP/mask | → `80.50.115.129 /28` | router's connected foot joins the switch subnet |
| B1 IP/mask | → `80.50.115.130 /28` | second host joins the switch subnet |
| R2, R3 | **leave alone** | unconnected decoys |

---

## Traps

- **Over-configuring the decoys.** R2/R3 aren't on any wire; they can't help and changing them wastes time.
- **Too-wide a mask on R1.** `/23` would overlap R2/R3 → a router with two interfaces in one subnet = illegal. Match A1's `/28`.
- Same subnet/broadcast rules as before for the host IPs.

---

## Checklist

- [ ] A router = host with many feet; each foot in its **own** subnet.
- [ ] Only configure interfaces that are actually **wired**.
- [ ] Connected router interface follows the **same subnet rule** as a host.
- [ ] Pick the connected subnet so it **doesn't overlap** the router's other interfaces.
- [ ] No routing table needed when every goal is within one subnet.
