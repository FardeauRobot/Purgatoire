"""Minimal input reader shared by the bundled bosses."""
import sys


def read_init():
    my_id = int(input())
    w = int(input())
    h = int(input())
    for _ in range(w * h):
        input()
    towns = []
    for _ in range(int(input())):
        tid, x, y, _desired = input().split()
        towns.append((int(tid), int(x), int(y)))
    return my_id, w, h, towns


def read_turn(w, h):
    input()
    input()
    for _ in range(w * h):
        input()


def say(line):
    print(line, flush=True)


def log(*parts):
    print(*parts, file=sys.stderr, flush=True)
