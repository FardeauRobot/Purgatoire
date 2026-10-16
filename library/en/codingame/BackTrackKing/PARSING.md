# Parsing notes — Back Track King

What the input looks like, how `main.cpp` reads it, and where the referee
(Java, ported in `simulator/sim.py`) differs from what the statement suggests.

---

## Init input, annotated

```
0              myId
23             width
15             height
0 0            cell (0,0): regionId 0, plains
0 0            cell (1,0)
...            width*height lines, row by row (y outer, x inner)
3 1            a river cell of region 3
...
6              townCount
0 12 1 2       town 0 at (12,1), wants town 2
1 21 3 0,3     town 1 at (21,3), wants towns 0 and 3
2 3 6 x        town 2 at (3,6), wants nothing
...
```

The cell index is `y * width + x`, the same order as the turn input.

`desiredConnections` is a single token (no spaces), so `cin >> desired`
reads it whole. Then split it on `,` unless it is `x`:

```cpp
string desired;
cin >> t.id >> t.x >> t.y >> desired;
if (desired != "x") {
    stringstream ss(desired);
    string id;
    while (getline(ss, id, ','))
        t.desired.push_back(stoi(id));
}
```

## Turn input, annotated

```
412            myScore
97             foeScore
-1 0 0 x       cell (0,0): no track, instability 0, not inked, on no path
0 2 0 0-2      cell (1,0): my track (if I'm P0), region at instability 2, on path 0→2
1 0 0 1-0,1-3  on two paths
2 0 0 x        neutral track
-1 4 1 x       inked region
...
```

- `trackOwner` is **absolute** (`0` = player 0), not "me/foe". Compare with
  `myId`; `1 - myId` is the foe.
- `instability` and `inked` are per region, repeated on every cell of the
  region. Reading one cell per region is enough.
- The pairs in `partOfActiveConnections` are `from-to` (directed) and sorted
  **as strings** (`"1-0,10-2,2-3"`). They're a single token too.

Checking whether a connection is active only needs the source town's cell,
since every path starts there:

```cpp
bool isActive(int from, int to) {
    const Town& t = towns[from];
    for (const auto& conn : at(t.x, t.y).connections)
        if (conn.first == from && conn.second == to)
            return true;
    return false;
}
```

## Output

- One line per turn, flushed. `endl` flushes; `"\n"` alone may not, and
  then the referee times out waiting for a line.
- Join actions with `;`. An empty line is **invalid**: send `WAIT` if you
  have nothing to do.
- Debug output goes to `cerr`. It shows up in the viewer's stderr panel.

---

## Referee quirks

Behaviour of the Java referee that the statement doesn't spell out. The local
simulator reproduces all of it.

### Parsing

- Commands are matched **case-insensitively** and must match **exactly**:
  one space between tokens, non-negative integers only. `PLACE_TRACK 1 2`
  (no S), `AUTOPLACE  1 2 3 4` (two spaces) and `PLACE_TRACKS -1 0` are
  invalid, and an invalid command **disqualifies** you.
- The line is split on `;` the way Java's `String.split` does: trailing
  empty parts are dropped, so `WAIT;` is fine, but an empty part in the
  middle (`WAIT;;WAIT`) is invalid.
- Commands before the invalid one still happen on the turn you get
  disqualified.
- Coordinates outside the grid parse fine; the action is just skipped (and
  logged).

### Tracks

- **At most one `AUTOPLACE` per turn.** Any further one is skipped with a
  log line (no disqualification).
- `AUTOPLACE` plans its path on the board **as it was at the start of the
  turn**: it ignores your own `PLACE_TRACKS` from the same line, and so
  do the opponent's placements.
- Placements happen in the order you wrote them, with `AUTOPLACE` expanded
  in place. `PLACE_TRACKS 5 5;AUTOPLACE …` spends paint on (5,5) first.
- `AUTOPLACE` finds the cheapest path in paint, and existing tracks and
  towns cost **0**, whoever owns them. If the target is already linked by
  rail to a cell on the way, it stops there.
- Tracks are placed from the `from` end. Only **running out of paint**
  stops the rest of the `AUTOPLACE`. Other failures (inked region, cell
  taken this turn) skip that one track and carry on.
- Equal-cost paths are broken by direction order N, E, S, W and by the
  layout of Java's `PriorityQueue`. A path that looks just as cheap to you
  can differ from the referee's; to predict it exactly, copy
  `Game.autoplace`.
- A double placement (same cell, same turn) is neutral, and **both players
  pay** for it.

### Scoring and disruption

- Every active connection scores every turn, independently. A track shared
  by three active paths gives its owner 3 points per turn.
- A path can pass through other towns.
- Instability **4** inks a region. The input stub (`config/stub.txt`) says 3; it is wrong.
- A region hit to 4 by both players in the same turn is inked once.
- League 2: the objective checks the **last region you successfully
  disrupted**, on the turn it gets inked while holding at least one enemy
  track. The flag is never reset, so once it's true you've won.

### Game end

- League 3: the game ends at turn 100, or as soon as no town can reach any
  town it wants through cells outside inked regions.
- League 3: a bot that times out or is disqualified ends with a score of -1
  and loses.
