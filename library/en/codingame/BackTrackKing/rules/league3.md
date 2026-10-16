# League 3 (Bronze and above) — full game

**Win:** more points than the opponent when the game ends.

- End: turn 100, or earlier if no town can reach any town it wants
  through non-inked cells any more.
- Each turn: 3 paint, 1 disruption, at most one `AUTOPLACE`.
- Scoring, every turn: for each active connection, 1 point per track you
  own on its shortest path. Neutral tracks score for nobody.
- Timeout or invalid command: you're out (score -1).
- Locally the opponent is a random autoplacer, so beating it proves little.
  Measure against an older build of your own bot:
  `make series N=40 OPP=./old_main`.
- Run: `make test-l3`, `make selfplay`, `make series`.

Ideas the scoring rewards:

- Tracks on **many** active paths: a trunk line shared by several
  connections pays once per connection per turn.
- Early points: a connection formed on turn 10 scores 90 times.
- Ride the opponent's tracks: AUTOPLACE uses any existing track for free,
  so you can connect towns through their network. The connection scores for
  both of you, but you paid less.
- Shortest-path shifts: one new track of yours can shorten a path and move
  it off the opponent's tracks onto yours.
- Disruption as defence and attack: inking a region wipes its tracks and
  forces paths to reroute. Watch the opponent's instability on your key
  regions.
