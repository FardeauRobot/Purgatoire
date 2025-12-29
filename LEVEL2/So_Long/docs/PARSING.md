# Parsing Checklist for `so_long`

Use this as a pre-flight list when reading and validating a map. Fail fast with clear error messages; avoid continuing after the first fatal issue.

## Quick checklist
- Args: one `.ber` file, readable, not empty.
- Shape: rectangular, no trailing blank line, min size ok.
- Borders: top/bottom all `1`, sides all `1`.
- Chars: only allowed set; 1 player, 1 exit, >=1 collectible.
- Path: player can reach all collectibles and the exit.
- Size: map fits window/assets; reject oversized maps.
- Cleanup: free map buffers on errors; no MLX init before parse.

## File-level checks
- Wrong args: enforce exactly one map path; reject missing file.
- Extension: require `.ber`; case-sensitive.
- Readability: `open`/`read` errors; empty file; zero-length lines.
- Oversized files: optional guard on max bytes/lines to avoid huge allocations.

## Map shape and layout
- Rectangular: every row same length; no stray `\n`-only rows.
- Minimum size: at least 3x3 (or your rule) so walls can exist.
- No trailing empty line at end of file (common Valgrind trap with GNL).

## Allowed characters
- Accept only the set you support (e.g., `01CEP` or your bonus chars). Reject any other char immediately.
- Count required elements: exactly 1 player, exactly 1 exit, at least 1 collectible (if your rules say so). Detect multiple players/exits.

## Border walls
- First and last rows must be all walls (`1`).
- First and last columns of every row must be `1`.

## Reachability / path validity
- Flood-fill/BFS/DFS from player:
  - All collectibles reachable.
  - Exit reachable after collecting.
- Work on a copy of the map so you do not mutate the original.

## Game-size and rendering constraints
- Map dimension vs. window/tile size: refuse maps that exceed screen or MLX limits before creating the window/images.
- Tile size mismatches: ensure your assets fit the assumed tile size.

## Memory and cleanup
- Free every intermediate buffer from `get_next_line` or your splitter, even on errors.
- On validation failure: free map rows/array and exit gracefully.
- After success: keep ownership clear—who frees the stored map later.

## Error reporting tips
- Print a short, specific message (e.g., "Error: map is not rectangular").
- Avoid partial initialization of MLX before parsing; parse first, then init graphics.

## Suggested validation order
1) Check args and file accessibility.
2) Read file into lines; reject empty file or trailing blank line.
3) Validate rectangle and borders.
4) Validate characters and required counts.
5) Run reachability check on a copied grid.
6) Check size constraints vs. window/tiles.
7) Proceed to MLX init and rendering.
