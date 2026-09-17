# Back Track King — local test environment

A local sandbox for the CodinGame **Summer Challenge 2026: Back Track King**.
Compile your C++ bot with `make`, run it against the league bosses with
`make test`, and watch the replay in your browser.

The bot reads and writes the exact stdin/stdout protocol described in
[`PRESENTATION.md`](PRESENTATION.md), so anything that works here will work
when you paste `main.cpp` into the CodinGame IDE.

The referee is a Python port of the official Java one
(<https://github.com/CGjupoulton/SummerChallenge2026>). Turn resolution,
AUTOPLACE pathfinding (tie-breaks included), connections and scoring follow
the Java code; map *generation* is ported too, but uses Python's RNG, so a
seed here is not the same map as that seed on CodinGame.

---

## Quick start

```sh
make            # build ./main from main.cpp
make test       # run your bot in leagues 1, 2 and 3 (seed 1)
make view       # serve the replay viewer at http://localhost:8765/viewer/
```

After `make view` starts, open the URL, pick a replay, and step through the
turns with the arrow keys or the play button.

---

## Make targets

| Target          | What it does                                                          |
| --------------- | --------------------------------------------------------------------- |
| `make`          | Compile `main.cpp` to `./main` with `-std=c++17 -O2 -Wall -Wextra`.   |
| `make test`     | Compile, then play one game in each league (1, 2, 3).                 |
| `make test-l1`  | Only league 1 (also `test-l2`, `test-l3`).                            |
| `make strict`   | Same as `test`, but a turn over the time budget is a timeout loss.    |
| `make selfplay` | League 3, `./main` against `./main` (usually 0–0, see below).         |
| `make series`   | Play `N` games on seeds `SEED..SEED+N-1` and print the win rate.      |
| `make view`     | Start a local web server for the viewer on port 8765.                 |
| `make clean`    | Remove `./main` and every replay JSON in `simulator/out/`.            |
| `make help`     | Print the target list.                                                |

Variables you can override:

```sh
make test SEED=7                    # another generated map
make test-l3 OPP=./old_main         # against a previous build of your bot
make series LEAGUE=3 N=50 OPP=./old_main
make CXX=clang++ CXXFLAGS="-std=c++17 -O0 -g -Wall"
```

---

## What `make test` actually does

For each league N it runs:

```
python3 simulator/sim.py --league N --bot ./main --seed 1 \
    --out simulator/out/replay-lN.json
```

The simulator generates a map from the seed, starts both bots as
subprocesses, sends each one the init input and then one turn input per turn,
reads one line from each, applies the rules and writes a replay JSON with
every turn.

The opponent depends on the league:

| League | Opponent (default)              | You win when…                                                |
| ------ | ------------------------------- | ------------------------------------------------------------ |
| 1      | `bosses/wait.py` — always WAIT  | you form any active connection and score ≥ 1 point           |
| 2      | `bosses/random_autoplace.py`    | a region you disrupted last gets inked while holding an enemy track |
| 3      | `bosses/random_autoplace.py`    | you have more points after 100 turns (or when no connection is possible any more) |

In leagues 1 and 2 you always play as player 0 and lose if the objective
isn't done by turn 100. Those two bosses are the official ones. The real
league 3+ opponents are other players; the random autoplacer is only a sparring
partner. Use `OPP=` to play against something stronger (an older version of
your own bot is the usual choice).

Useful flags when calling `sim.py` directly:

| Flag               | Effect                                                              |
| ------------------ | ------------------------------------------------------------------- |
| `--opponent CMD`   | opponent command (default: the league boss)                         |
| `--seed N`         | map seed                                                            |
| `--map FILE`       | load a map JSON instead of generating one                           |
| `--dump-map FILE`  | write the generated map for `--seed` and exit                       |
| `--max-turns N`    | stop early (quick iteration)                                        |
| `--strict-timing`  | an over-budget turn deactivates the bot, as on CodinGame            |
| `--swap`           | play as player 1 (league 3 only)                                    |
| `--games N`        | a series; in league 3 sides alternate every game                    |

Without `--out`, nothing is written — handy for `--games`.

---

## The viewer

`make view` runs `python3 -m http.server` from the project root.

Keyboard:

| Key       | Action               |
| --------- | -------------------- |
| `←` / `→` | Previous / next turn |
| `space`   | Play / pause         |

UI:

- Replay dropdown: league 1, 2, 3 or self-play. **Load** re-reads the file
  from disk, so you don't need to restart the server after a new `make test`.
- Speed dropdown and scrubber.
- `?replay=../simulator/out/other.json` loads any other replay, and
  `&turn=37` opens it at a given turn.

The map:

- Terrain colour (plains / river / mountain), thick lines for region borders,
  a dark overlay on inked regions.
- In the top-left cell of each region: its id (`r12`) and its instability
  (`⚠2`, red at 3). A coloured square marks a region disrupted this turn.
- Tracks in their owner's colour (red P0, blue P1, grey neutral), linked to
  adjacent tracks and towns. A thin outline marks tracks placed this turn.
- Active connection paths are drawn as a pale ribbon. Click a row in the
  **Connections** panel to show only that path.
- Hover a cell: coordinates, terrain cost, region, instability, owner, town
  and its desired connections, and which connections pass through it.

Side panels for the current turn:

- Score for both players and the final result.
- Commands each bot sent, with the response time (yellow near the budget,
  red over it) and any `MESSAGE`.
- Every desired connection: path length and points each player earned from it
  this turn.
- Referee log: invalid actions, inked regions, connections formed, lost
  or rerouted, slow turns.
- stderr from each bot (everything you `cerr <<`).

---

## Maps and seeds

Maps are generated from `--seed` like the official GridMaker: height 14–20,
width = round(height × 1.5), mountains, then rivers, then regions, then 4–12
towns and their desired connections.

To freeze a map, or hand-edit one:

```sh
python3 simulator/sim.py --seed 7 --dump-map simulator/maps/seed7.json
python3 simulator/sim.py --league 3 --bot ./main --map simulator/maps/seed7.json \
    --out simulator/out/replay-l3.json
```

Schema:

```jsonc
{
  "width": 23,
  "height": 15,
  "types":   ["00012...", ...],        // one string per row: 0 plains, 1 river, 2 mountain
  "regions": [[0, 0, 0, 1, ...], ...], // one list per row: region id of each cell
  "towns": [
    { "id": 0, "x": 12, "y": 1, "desired": [2] }
  ]
}
```

Rules to keep when editing by hand: towns only on plains, at most one town
per region, region ids `0..n-1` and each region connected. The simulator
doesn't check any of these.

`simulator/maps/seed1.json` is a copy of the map `make test` generates for seed 1.
`make test` doesn't read it; pass `--map` to use your edited version.

---

## Strict vs. lenient timing

The rule is ≤ 1000 ms for the first turn and ≤ 50 ms after that.

- Default (`make test`): a slow turn is logged (`slow turn — 63.2 ms > 50 ms
  (lenient)`) and the game continues.
- Strict (`make strict`): a slow turn deactivates that bot, as on CodinGame,
  and `end_reason` is `disqualified / timeout`. In league 3 its score becomes
  -1; in leagues 1–2 you lose unless the objective was already done.

A bot that prints nothing for 5 s, crashes or sends an invalid command is
deactivated in both modes. Measured times include Python's pipe overhead
(~0.2 ms), so leave yourself some margin.

---

## File layout

```
.
├── Makefile
├── main.cpp                    # your bot (C++17 starter)
├── PRESENTATION.md             # the full game statement
├── PARSING.md                  # input format, parsing in C++, referee quirks
├── rules/
│   └── league1.md  league2.md  league3.md
├── simulator/
│   ├── sim.py                  # Python referee (stdlib only)
│   ├── bosses/
│   │   ├── common.py           # input reading shared by the bosses
│   │   ├── wait.py             # league 1 boss
│   │   └── random_autoplace.py # league 2 boss, league 3 default opponent
│   ├── maps/                   # frozen maps (--dump-map / --map)
│   └── out/                    # replay JSONs
└── viewer/
    └── index.html  viewer.js  style.css
```

---

## Common workflows

### Check a change doesn't make the bot worse

```sh
cp main old_main          # keep the current build
# edit main.cpp …
make series N=40 OPP=./old_main
```

In league 3 the series alternates sides, so a first-player advantage can't
skew the result.

### Self-play

`make selfplay` against the very same binary usually ends **0–0**. It isn't
a bug: two identical deterministic bots place the same cells on the same
turn, every track becomes neutral, and neutral tracks score for nobody. For a
real match, play a different build: `make test-l3 OPP=./old_main`.

### Look at one bad game

`make series` prints the seed of each game. Replay the one you lost:

```sh
make test-l3 SEED=13 OPP=./old_main
```

and open league 3 in the viewer.

### Read a replay programmatically

Each entry of `turns` has `tracks` (row strings, `.` empty, `0`/`1`/`2`
owner), `instability`, `inked`, `scores`, `connections`, `events`,
`commands`, `messages`, `stderr`, `timing_ms` and `log`. Turn 0 is the
initial state.

```sh
jq '.turns[] | {turn, cmds: .commands, score: .scores}' simulator/out/replay-l3.json
jq '[.turns[] | .timing_ms[0]] | max' simulator/out/replay-l3.json
```

---

## Limitations

- Seeds don't reproduce CodinGame maps: the generator is a port, but the
  random number generator isn't Java's.
- Only the league 1 and 2 bosses exist; they are the official ones. League 3+
  has no boss to copy, so its default opponent is a random autoplacer.
- The referee follows the Java code, not the statement, where they differ
  (see *Referee quirks* in [`PARSING.md`](PARSING.md)).
- Time is measured on your machine, which is not CodinGame's.

If something doesn't match the CodinGame IDE, everything is in
[`simulator/sim.py`](simulator/sim.py): turn resolution is `Game.update`,
pathfinding is `Game.autoplace` and `Game.train_path`.
