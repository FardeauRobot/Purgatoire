# League 2 — disrupt

**Objective:** ink out a region that holds at least one enemy track, with
`DISRUPT` as the last hit.

- Boss: each turn, `AUTOPLACE` between two random different towns
  (`simulator/bosses/random_autoplace.py`).
- A region is inked when its instability reaches **4**. You have 1
  disruption point per turn, so one region takes 4 turns alone.
- Regions with a town can't be disrupted, and neither can inked ones.
- The win is checked against the **last region you disrupted
  successfully**: don't switch targets on the turn it would ink.
- Loss: objective not done after 100 turns, a timeout, or an invalid command.
- Run: `make test-l2`.

Pick a town-free region the boss's tracks go through (count cells with
`trackOwner == 1 - myId`) and hit it 4 turns in a row. The boss doesn't
rebuild on purpose, but it keeps adding tracks, so a region crossed by many
town pairs is a safe bet.
