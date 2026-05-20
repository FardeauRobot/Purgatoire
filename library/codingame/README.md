 # TrollFarm — local test environment

A local sandbox for the CodinGame TrollFarm puzzle. Compile your C++ bot with
`make`, run it against scripted scenarios with `make test`, and watch the
replay in your browser.

The bot reads/writes the exact stdin/stdout protocol described in
[`PRESENTATION.md`](PRESENTATION.md), so anything that works here will work
when you paste your code into the CodinGame IDE.

---

## Quick start

```sh
make            # build ./main from main.cpp
make test       # run your bot against all four league scenarios
make view       # serve the replay viewer at http://localhost:8765/viewer/
```

After `make view` starts, open the URL, pick a league, and step through the
turns with the arrow keys or the play button.

---

## Make targets

| Target         | What it does                                                        |
| -------------- | ------------------------------------------------------------------- |
| `make`         | Compile `main.cpp` to `./main` with `-std=c++17 -O2 -Wall -Wextra`. |
| `make test`    | Compile then run all four league scenarios in sequence.             |
| `make test-l1` | Run only league 1 (also `test-l2`, `test-l3`, `test-l4`).           |
| `make strict`  | Run all leagues with CodinGame-style timing enforced as a loss.     |
| `make view`    | Start a local web server for the viewer on port 8765.               |
| `make clean`   | Remove `./main` and all replay JSONs in `simulator/out/`.           |
| `make help`    | Print the same target list at the terminal.                         |

Override the compiler or its flags from the command line:

```sh
make CXX=clang++ CXXFLAGS="-std=c++17 -O0 -g -Wall"
```

---

## What `make test` actually does

For each league N, the Makefile runs:

```
python3 simulator/sim.py \
    --map simulator/maps/leagueN.json \
    --bot ./main \
    --out simulator/out/replay-lN.json
```

The simulator spawns your bot as a subprocess, feeds it the initial grid +
per-turn game state (matching the puzzle spec exactly), reads the commands
your bot prints, applies the rules, and writes a replay JSON describing
every turn.

The opponent is a static no-op troll — there is no enemy AI. The simulation is
single-player by design, focused on watching what *your* code does.

Direct invocation if you want to override defaults:

```sh
python3 simulator/sim.py \
    --map simulator/maps/league3.json \
    --bot ./main \
    --out simulator/out/replay-l3.json \
    --max-turns 40 \
    --strict-timing
```

`--max-turns N` overrides the per-league turn cap (useful for quick iteration).
`--strict-timing` enforces the puzzle's loss rule: one violation > 100 ms or
three minor violations (50–100 ms) ends the game. Without the flag, time
overruns are logged but not fatal.

---

## The viewer

`make view` runs `python3 -m http.server` from the project root.

Keyboard:

| Key       | Action                       |
| --------- | ---------------------------- |
| `←` / `→` | Previous / next turn         |
| `space`   | Play / pause                 |

UI:

- League dropdown — pick which replay JSON to load.
- Speed dropdown — controls auto-play speed.
- Scrubber bar — jump to any turn.

Side panels show, for the current turn:

- Inventory in your shack
- Score (yours + opponent's static 0)
- Your troll(s), their stats and what they're carrying
- Commands your bot sent
- The most recent `MSG` from your bot
- Bot stderr captured this turn (everything you `cerr <<`)
- Simulator log (rule violations, warnings)
- Per-turn elapsed milliseconds with the budget

`make view` keeps running until you hit `Ctrl-C`. Re-run `make test` in
another shell and reload the viewer to see the new replay.

---

## Maps and scenarios

Each league has a JSON file in `simulator/maps/`. Edit them to try new
situations — the simulator picks them up on the next run, no rebuild needed.

Schema:

```jsonc
{
  "league": 3,                       // 1..4; controls which actions are accepted
  "name": "L3 — water, iron, rock",  // shown in the viewer summary
  "max_turns": 300,                  // game length
  "width": 16,                       // must equal 2 * height
  "height": 8,                       // 8 for L1/L2, up to 11 for L3/L4
  "grid": [
    "................",              // 1 string per row, exactly width chars long
    "..~~............",              // . grass · ~ water · # rock · + iron
    ".....0..........",              // 0 your shack · 1 opp shack
    "..........1....."
  ],
  "trees": [
    { "type": "PLUM",   "x": 12, "y": 5,
      "size": 4, "health": 12, "fruits": 2, "cooldown": 8 }
  ],
  "trolls": [
    { "player": 0, "x": 5, "y": 3,
      "movement_speed": 1, "carry_capacity": 1,
      "harvest_power": 1, "chop_power": 1 },
    { "player": 1, "x": 10, "y": 3,
      "movement_speed": 1, "carry_capacity": 1,
      "harvest_power": 1, "chop_power": 1 }
  ]
}
```

Tips:

- Every row must be exactly `width` characters. The simulator will assert
  otherwise.
- Trees on water/rock/iron cells will misbehave — trees go on grass.
- Player 0 is yours; player 1 is the static opponent.
- Lower-league maps still need an opponent shack `1` (it's part of the input
  format), it just never does anything.

To add a custom scenario, copy one of the existing JSON files, modify it, and
either replace `simulator/maps/leagueN.json` or pass `--map your_file.json` to
`sim.py` directly.

---

## Strict vs. lenient timing

The puzzle rule is: ≤ 1000 ms for turn 1, ≤ 50 ms after that. One overrun
> 100 ms or three overruns in 50–100 ms loses the game.

- Default (`make test`): overruns are logged in the per-turn `log` panel and
  the JSON, but the game keeps running. You'll see a yellow/red **Timing**
  badge in the viewer.
- Strict (`make strict`): the simulator enforces the loss rule and writes
  `end_reason: "timing loss"` in the replay summary.

If your bot hangs entirely (no newline within ~5 s), the simulator stops
that league with `end_reason: "bot timeout (no output)"` regardless of mode.
The next league still runs.

---

## File layout

```
.
├── Makefile
├── main.cpp                  # your bot (mis-named — it's C++)
├── PRESENTATION.md           # full puzzle statement
├── PARSING.md                # input-parsing notes
├── rules/                    # the four league rule sheets
│   ├── league1.md  league2.md  league3.md  league4.md
├── simulator/
│   ├── sim.py                # Python referee / game engine
│   ├── maps/                 # one JSON scenario per league
│   │   └── league1.json … league4.json
│   └── out/                  # replay JSONs written here
│       └── replay-l1.json …
└── viewer/
    ├── index.html  viewer.js  style.css
```

---

## Common workflows

### Iterate on movement / harvesting

```sh
# Hack on main.cpp …
make test-l1                 # quickly run league 1
# Reload the viewer's league-1 replay (the Load button does this).
```

### Inspect a specific turn

Open the viewer, drag the scrubber to the turn, read the side panels.
Everything your bot printed to stderr that turn shows up under **Bot stderr**.

### Test with a shorter game

```sh
python3 simulator/sim.py \
  --map simulator/maps/league1.json \
  --bot ./main \
  --out simulator/out/replay-l1.json \
  --max-turns 20
```

### Read a replay programmatically

The replay JSON has `turns: [...]` where each entry includes `trees`,
`trolls`, `inventory`, `scores`, `commands`, `stderr`, `timing_ms`, and `log`.
Pipe it through `jq` for ad-hoc analysis:

```sh
jq '.turns[] | {turn, cmds: .commands, score: .scores[0]}' \
   simulator/out/replay-l1.json
```

---

## Limitations

- No opponent bot. Player 1 sits at its shack and never acts.
- Game-rule resolution follows the league 3 phase order
  (move → harvest → plant → chop → pick → train → drop → mine → grow).
  Edge cases when two trolls of the same team contest a cell are handled
  conservatively (first-id wins, second is skipped) — usually irrelevant if
  you don't train extra trolls in the same turn they should collide.
- Trees on freshly-planted cells start at size 1 with cooldown set to the
  table value (water-adjacent or normal). The puzzle statement doesn't
  specify this exactly; the implementation mirrors league 3's growth table.

If something looks off compared to the CodinGame IDE, the simulator's
behavior is in [`simulator/sim.py`](simulator/sim.py) — it's a single file,
roughly 600 lines, and the rule application is in `apply_player_commands`.
