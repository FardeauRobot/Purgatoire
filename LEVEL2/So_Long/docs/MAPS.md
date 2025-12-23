# Maps

Expected file extension: `.ber`

## Allowed tiles
- `0` — floor
- `1` — wall
- `P` — player start (exactly 1)
- `E` — exit (exactly 1)
- `C` — collectible (>=1)

## Shape rules
- Rectangular grid (all rows same length)
- Fully enclosed by walls (`1` on outer border)

## Validity checks
- Only the allowed characters above
- One reachable path where the player can collect all `C` and reach `E`
- No empty lines; map not empty

## Tips
- Keep maps small enough for your window size
- Group collectibles away from spawn to test pathfinding
