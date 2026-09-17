#!/usr/bin/env python3
"""League 2 boss (and default league 3 sparring partner): AUTOPLACE between two random towns every turn."""
import random

from common import read_init, read_turn, say

my_id, w, h, towns = read_init()
rng = random.Random(1 + my_id)
while True:
    read_turn(w, h)
    a = rng.choice(towns)
    b = rng.choice([t for t in towns if t[0] != a[0]])
    say(f"AUTOPLACE {a[1]} {a[2]} {b[1]} {b[2]}")
