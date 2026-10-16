# Level 2 — Two isolated pairs, fix mask + dodge broadcast

> NetPractice randomizes the numbers per student. The **layout** and the **lessons** below are stable; the exact octets you see will differ. Treat the worked figures as one concrete instance, not a password to memorize.

---

## What this level looks like

Two completely separate two-host wires, no router, no switch between them:

```
Computer B ── B1 ─────── A1 ── Computer A     (Goal 1: B must reach A)
Computer D ── D1 ─────── C1 ── Computer C     (Goal 2: D must reach C)
```

Each goal is solved in isolation — the left pair and the right pair never talk to each other. It's two copies of Level 1 stacked together, with two different traps baked in.

The one rule that decides everything (see [`../02_BINARY_AND_MASKS.md`](../02_BINARY_AND_MASKS.md)):

> Two interfaces on the **same wire** communicate **only if** `IP AND mask` gives the **same network address on both**, and the **masks are identical**.

---

## The two concepts to grasp

### 1. A mask must be *legal* (contiguous 1s)
A subnet mask in binary is **all 1s, then all 0s — no gaps.** So each octet can only be one of:

`0, 128, 192, 224, 240, 248, 252, 254, 255`

`255.255.255.223` is **not a mask**: `223 = 1101 1111`. The hole (`0` between `1`s) makes it illegal. The simulator rejects it outright — that's a guaranteed wrong answer, not a "close enough."

### 2. Inside a subnet, two addresses are forbidden as hosts
For any subnet:
- the **network address** (all host bits = 0) — the name of the subnet itself
- the **broadcast address** (all host bits = 1) — "everyone on this subnet"

Neither can be assigned to an interface. This is the trap on the left pair below.

---

## Worked instance (the screenshot)

| Interface | IP (shown) | Mask (shown) | Editable |
|-----------|------------|--------------|----------|
| A1 | `192.168.19.223` | `255.255.255.224` (`/27`) | **IP only** |
| B1 | `192.168.19.222` | `255.255.255.223` ❌ | **mask only** |
| C1 | `127.0.0.1` | `255.255.255.252` (`/30`) | **IP only** |
| D1 | `127.0.0.2` | `/30` | **IP only** |

### Left pair — A1 & B1 (the `/27` block)

B1's mask is locked-editable and currently illegal; A1's mask is fixed at `/27`. The masks **must match**, so set B1 to `/27` too.

Now compute the `/27` block that `192.168.19.222` lives in. `/27` → block size `256 − 224 = 32`. Blocks of the 4th octet: `…, 160, 192, 224, …`. `.222` sits in the **192** block:

```
Network   : 192.168.19.192
First host: 192.168.19.193
Last host : 192.168.19.222   ← B1 lives here (fixed, valid)
Broadcast : 192.168.19.223   ← A1 is sitting ON the broadcast! illegal
```

So **two** fixes on the left:
- **B1 mask** → `255.255.255.224`  (match A1's `/27`; the shown value isn't even a mask)
- **A1 IP** → any host in `.193`–`.221` (`.222` is taken, `.223` is broadcast). e.g. **`192.168.19.221`**

### Right pair — C1 & D1 (the `/30` block)

Both masks are fixed at `/30` and already match. `/30` → block size `4`, exactly **2 usable hosts**. The subnet for `127.0.0.x` is:

```
Network   : 127.0.0.0
Host 1    : 127.0.0.1   ← C1
Host 2    : 127.0.0.2   ← D1
Broadcast : 127.0.0.3
```

`127.0.0.1` and `127.0.0.2` are the only two legal hosts, and both are assigned — this side is **already correct**. If your D1 still shows the original `127.0.0.4` (that's a *different* `/30`: net `.4`, hosts `.5/.6`), change it to **`127.0.0.2`** so both interfaces share `127.0.0.0/30`.

> The `127.x` range is normally loopback, but NetPractice treats it as an ordinary address — don't let it distract you.

---

## Solution summary

| Field | From | To | Why |
|-------|------|----|-----|
| B1 mask | `255.255.255.223` | `255.255.255.224` | illegal mask → make it `/27`, matching A1 |
| A1 IP | `192.168.19.223` | `192.168.19.221` | `.223` is the broadcast of `…192/27`; pick a usable host |
| C1 IP | `127.0.0.1` | *(keep)* | valid host in `127.0.0.0/30` |
| D1 IP | `127.0.0.2` | *(keep, or set from `.4`)* | must be the other usable host of the same `/30` |

---

## Checklist to internalize (defend at eval)

- [ ] A mask is **contiguous 1s** — only `0/128/192/224/240/248/252/254/255` per octet.
- [ ] On one wire, **masks must be identical** and `IP AND mask` must match.
- [ ] An interface IP can be **neither** the network **nor** the broadcast of its subnet.
- [ ] `/27` → 32-block, 30 usable. `/30` → 4-block, 2 usable. (full table in [`../07_EVAL_CHEATSHEET.md`](../07_EVAL_CHEATSHEET.md))
- [ ] When the IP is locked, fix the **mask**; when the mask is locked, fix the **IP**. Always solve the editable field around the fixed one.
