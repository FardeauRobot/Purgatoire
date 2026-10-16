# League 1 — connect

**Objective:** form one active connection between two towns. As soon as
you score a point, you win.

- Boss: WAITs every turn (`simulator/bosses/wait.py`).
- Loss: objective not done after 100 turns, a timeout, or an invalid command.
- Useful here: `PLACE_TRACKS`, `AUTOPLACE`, `WAIT`. The input already has
  instability and inked, but nobody disrupts.
- Run: `make test-l1`.

Shortest route to a win: pick the desired pair with the cheapest path and
`AUTOPLACE` between them every turn. 3 paint a turn covers 3 plains cells.
