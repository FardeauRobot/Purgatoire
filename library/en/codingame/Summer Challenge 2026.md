Rules :

**Two players paint train tracks on a grid to connect towns. Every turn, each active connection pays 1 point per track you own on its shortest path. You can also disrupt regions to wash away enemy tracks. Most points after 100 turns wins.**

Local sandbox -> [[BackTrackKing/README]] (make / make test / make view)

Grid :
	width 21-30 / height 14-20
	Cells :
		type 0 PLAINS -> track costs 1
		type 1 RIVER -> track costs 2
		type 2 MOUNTAIN -> track costs 3
	Regions -> groups of cells with a regionId, can be disrupted

Towns :
	4 to 12, on plains only, max 1 per region, never 2 in neighbouring regions
	desiredConnections -> towns it wants a railway to (ONE WAY : 0 wants 1 != 1 wants 0)

Tracks :
	3 paint points per turn -> lost if unused
	trackOwner : -1 none / 0 / 1 / 2 NEUTRAL (both players placed on same cell same turn -> both pay)
	Not on a town, not on a track, not in an inked region
	Tracks auto-connect to orthogonal neighbours (tracks + towns)

Connections :
	For each A -> B wanted : SHORTEST path of tracks / towns = active connection
	Tie -> directions tried NORTH, EAST, SOUTH, WEST
	End of every turn -> 1 point per own track on the path, for EACH active connection
		-> a track shared by 3 paths = 3 points / turn

Disruption :
	1 disruption point per turn
	DISRUPT region -> instability++
	instability == 4 -> region INKED : tracks wiped, no more building, connections through it cut
	Can't disrupt a town region or an inked region

Turn order :
	1. all PLACE_TRACKS (AUTOPLACE included)
	2. all DISRUPT
	3. ink regions with instability >= 4
	4. connections + scoring

Leagues :
	L1 -> form any connection = instant win (boss WAITs)
	L2 -> ink a region holding an enemy track with your DISRUPT (boss AUTOPLACEs random towns)
	L3 -> full game, most points


INPUT

INIT :

0             myId
23 15         width, height (one per line)

width*height lines -> regionId type (left->right, top->bottom)
0 0
0 0
3 1           region 3, river
...

6             townCount

ID || X || Y || DESIRED
0 12 1 2      town 0 wants town 2
1 21 3 0,3    town 1 wants towns 0 and 3
2 3 6 x       x = wants nothing

EACH TURN :

412           myScore
97            foeScore

width*height lines -> TRACK_OWNER || INSTABILITY || INKED || CONNECTIONS
-1 0 0 x
0 2 0 0-2        my track (if myId 0), region instability 2, on path 0 -> 2
1 0 0 1-0,1-3    on two paths
-1 4 1 x         inked region

Each turn you print ONE line, actions separated by ;

- PLACE_TRACKS x y -> a track on (x, y)
- AUTOPLACE x1 y1 x2 y2 -> cheapest path between the 2 cells, as much as paint allows. MAX 1 PER TURN
- DISRUPT regionId (or DISRUPT x y) -> instability++ of that region
- MESSAGE text -> display in the replay
- WAIT -> do nothing

Timing : 1000 ms first turn, 50 ms after
Invalid command (typo, empty line, PLACE_TRACK without S) -> DISQUALIFIED

Traps -> see BackTrackKing/PARSING.md "Referee quirks"
	AUTOPLACE uses EVERY existing track for free (even enemy's)
	AUTOPLACE out of paint -> rest of it is skipped
	Stub says ink at 3 -> it's 4
