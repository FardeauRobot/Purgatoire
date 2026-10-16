#!/usr/bin/env python3
"""League 1 boss: skips every turn."""
from common import read_init, read_turn, say

_, w, h, _ = read_init()
while True:
    read_turn(w, h)
    say("WAIT")
