# 02 — Binary and Masks

> The mask determines who is "near" and who is "far." This is binary arithmetic — let's do it fast.

---

## 1. Decimal ↔ binary, by hand

Every octet (0–255) is 8 bits. The 8 bit-position values from left to right:

```
   bit:   7   6   5   4   3   2   1   0
  value: 128  64  32  16   8   4   2   1
```

Sum the bit-values where the bit is `1`. That's it.

### Examples

- `192` → `128 + 64 = 192` → `11000000`
- `168` → `128 + 32 + 8 = 168` → `10101000`
- `255` → `128+64+32+16+8+4+2+1 = 255` → `11111111`
- `240` → `128+64+32+16 = 240` → `11110000`

**Trick for masks:** since masks are "ones from the left," they're easier:

- `255.255.255.224` — last octet is `224 = 128+64+32` → `11100000` → 3 ones → total mask bits = `8+8+8+3 = /27`. ✅

### Converting `/27` back to dotted

- `/27` = 27 ones = `8 + 8 + 8 + 3`.
- First three octets: `255.255.255.`
- Fourth octet: 3 ones followed by 5 zeros = `11100000` = `128+64+32 = 224`.
- Result: `255.255.255.224`. ✅

`★ Insight ─────────────────────────────────────`
You'll convert between CIDR and dotted-mask **constantly** during NetPractice. Practice until it takes you under 5 seconds. The shortcut: only nine values are ever valid in a mask octet — `{0, 128, 192, 224, 240, 248, 252, 254, 255}`. Their bit-counts are `{0,1,2,3,4,5,6,7,8}` respectively. Pattern-match, don't multiply.
`─────────────────────────────────────────────────`

---

## 2. The bitwise AND — what masks do

Subnet membership is computed by **AND-ing** the IP with the mask, octet by octet.

```
  AND truth table:    a │ b │ a AND b
                     ───┼───┼─────────
                      0 │ 0 │   0
                      0 │ 1 │   0
                      1 │ 0 │   0
                      1 │ 1 │   1
```

A mask of `/24` (`255.255.255.0`) AND'd with `192.168.1.42`:

```
  IP :   192      .  168      .   1       .  42        →  11000000.10101000.00000001.00101010
  mask:  255      .  255      .   255     .   0        →  11111111.11111111.11111111.00000000
  AND :  192      .  168      .   1       .   0        →  11000000.10101000.00000001.00000000
         └──────────── kept ──────────────┘└─ zeroed ──┘
              network bits                  host bits
```

The result, `192.168.1.0`, is the **network address** of the subnet that `192.168.1.42` belongs to.

### Two hosts share a subnet ⇔ their network addresses match

```
  Host A:   10.0.0.5  /24      AND  →  10.0.0.0
  Host B:   10.0.0.99 /24      AND  →  10.0.0.0    ✅ same subnet
  Host C:   10.0.1.5  /24      AND  →  10.0.1.0    ❌ different subnet
```

**NetPractice rule #1:** if two interfaces are on the same wire (same hub/switch in the diagram), their `IP AND mask` must produce the same network address.

**NetPractice rule #2:** if a host's gateway field is set, the gateway IP must be in the host's own subnet (otherwise the host doesn't know how to reach the gateway itself).

---

## 3. AND in non-byte-aligned masks (the tricky case)

`/26` doesn't align to a byte boundary — the split is **inside** the fourth octet.

```
  /26 mask:  255.255.255.192
                            └─ 192 = 11000000 → 2 ones, 6 zeros
```

Consider `10.0.0.70 /26`:

```
  IP   :   10  .  0  .  0  .   70    →  ........  ........  ........  01000110
  mask :  255  . 255 . 255 .  192    →  11111111  11111111  11111111  11000000
  AND  :   10  .  0  .  0  .   64    →  ........  ........  ........  01000000
                                                                       └┬┘└─┬─┘
                                                                     net   host
```

So `10.0.0.70 /26` is in subnet `10.0.0.64 /26`, which spans `10.0.0.64` → `10.0.0.127`.

**Trick — block size:** within an octet, the block size of a non-aligned mask is `256 − octet_value`.
- `/26` last octet = 192 → block size = 256 − 192 = **64**.
- Subnets start at multiples of 64 in the last octet: `0, 64, 128, 192`.
- `70` falls in the `64–127` block. Network address: `10.0.0.64`. Broadcast: `10.0.0.127`.

| Mask octet | Block size | Subnet boundaries in that octet |
|---|---|---|
| 128 | 128 | 0, 128 |
| 192 | 64  | 0, 64, 128, 192 |
| 224 | 32  | 0, 32, 64, 96, ..., 224 |
| 240 | 16  | 0, 16, 32, 48, ..., 240 |
| 248 | 8   | 0, 8, 16, 24, ..., 248 |
| 252 | 4   | 0, 4, 8, 12, ..., 252 |
| 254 | 2   | 0, 2, 4, ..., 254 |

To find the network address of any IP, **round the relevant octet down to the nearest multiple of the block size.**

---

## 4. Quick subnet-membership procedure

Given two IPs and a mask, do they share a subnet?

1. Find the "interesting octet" — the one where the mask has between 1 and 7 ones.
2. Compute the **block size** of that octet (`256 − mask_value`).
3. For each IP's interesting octet, find the **floor to the nearest multiple of block size**.
4. If both floors match (and all earlier octets match), they share a subnet.

### Worked example

> Are `192.168.10.20 /28` and `192.168.10.30 /28` in the same subnet?

- Mask `/28` → last octet = 240 → block size = 16.
- First 3 octets match: `192.168.10`. ✓
- Last octet floors: `20 // 16 = 1 → 16`. `30 // 16 = 1 → 16`. ✓
- Both in `192.168.10.16 /28` (spans `.16` → `.31`).
- **Yes**, same subnet.

> Are `192.168.10.20 /28` and `192.168.10.40 /28` in the same subnet?

- `20` floors to `16` (block `16–31`). `40` floors to `32` (block `32–47`). Different blocks.
- **No**, different subnets.

---

## 5. Why a mask can't be "non-contiguous"

A mask is **defined** as a contiguous prefix of 1s. The kernel uses this assumption — masks like `255.0.255.0` are not just disallowed by NetPractice, they're disallowed by the protocol itself. If you ever see a "valid mask" check fail in NetPractice with a weird value, this is the reason.

The reason is computational: longest-prefix match (the routing algorithm, see [`04_ROUTING.md`](04_ROUTING.md)) requires the mask to be a prefix length, a single integer 0–32. Non-contiguous "masks" break that abstraction.

---

## 6. The `0.0.0.0` and `255.255.255.255` edge cases

- `0.0.0.0/0` — matches **every** IPv4 address. Used in routing tables as the **default route** ("everything not otherwise matched goes here").
- `255.255.255.255` — limited broadcast, never used as an interface IP.
- `0.0.0.0` as a destination — usually means "unspecified," used by DHCP clients before they get an IP.

You'll see `0.0.0.0/0` constantly in NetPractice routing tables. It's not an IP — it's a routing wildcard meaning "fallback."

---

## Next

→ [`03_SUBNETTING.md`](03_SUBNETTING.md): How to split a network in two, compute all the boundaries, and pick valid IPs for each new subnet.
