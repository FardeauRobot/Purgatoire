
Rules :

**Each player starts with a shack and a troll. Control your troll to collect fruits and then bring the resources back to your shack. The resources can be used to train more trolls or plant trees. The game is played on a grid, cells have different terrains and trees on them.**

Troll : 
	4 ATTRIBUTES :
		movementSpeed -> how many cells int can MOVE (horizontal / vertical)
		carryCapacity
		harvestPower
		chopPower

Trees :
	ATTIBUTES :
		type (PLUM, LEMON, APPLE, BANANA)
		size
		health
		nbfruits -> Up to 3
		cooldown -> When cooldown == 0 -> size++ until size 4

Cells :
	ATTRIBUTES :
		1 Troll capacity
		GRASS is only surface walkable

Harvesting :
	If Trees have fruit -> can be harvested
	When Troll is on same cell as tree, it can HARVEST given carryCapacity and harvestPower
	If nbTrolls on tree > 1 -> harvest divided by nb of troll. Last fruit can be duplicated

Dropping :
	If Troll close to shack (horizontal / vertical) DROP carried items


INPUT
	width / height of the grid
	height lines -> in wood -> every char is . (== GRASS)

INPUT EXAMPLE :

16 8          (16 = width || 8 = height)

................
................
................
.....0.......... (0 is own SHACK)
..........1..... (1 is opponent SHACK)
................
................
................

0 0 0 0 0 0 (plums, lemons, apples, bananas, (reserved), (reserved)) mine
0 0 1 0 0 0 (plums, lemons, apples, bananas, (reserved), (reserved)) opponent

12 Number of trees

TYPE || X || Y || SIZE || HP || FRUITS || CD
PLUM   1     4       4       12          0          6

PLUM 14 3 4 12 0 6

LEMON 2 2 4 12 3 0
LEMON 13 5 4 12 3 0

APPLE 12 7 4 20 1 3
APPLE 3 0 4 20 2 3
APPLE 8 4 3 17 0 8
APPLE 7 3 3 17 0 8
APPLE 5 1 4 20 0 6
APPLE 10 6 4 20 0 6

BANANA 10 0 4 6 3 1
BANANA 5 7 4 6 3 1

2 NB OF TROLLS

ID || PLAYER (0 == PLAYER | 1 == OPPONENT) || X || Y || MVSPEED || CARRY CAP || HARVEST || RESERVED || C_PLUM || C_LEMON || C_APPLE || C _ BANANA || C_RESERVED || C_RESERVED
0 0 2 4 1 1 1 0 0 0 0 0 0 0
1 1 10 6 1 1 1 0 0 0 0 0 0 0

Each turn you can print any number of commands, separated by ;.

- MOVE id x y Move troll id to cell (x, y).
- HARVEST id Make troll id harvest on its current cell.
- DROP id Make troll id drop all carried items at the shack.
- WAIT to do nothing.
- MSG text to display a message in the replay.