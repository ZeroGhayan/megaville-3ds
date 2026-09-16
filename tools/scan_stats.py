#!/usr/bin/env python3
"""Lê SPRITE_* em frame_1479 e escreve data/char-stats.md."""
from __future__ import print_function
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC = os.path.join(ROOT, "assets", "raw", "dump", "scripts",
                   "frame_1479", "DoAction.as")
OUT = os.path.join(ROOT, "data", "char-stats.md")
NAMES = ["blossom", "bubbles", "buttercup", "bell",
         "dexter", "rowdy", "zim", "shira"]


def grab(text, name):
    vals = []
    pat = re.compile(r"%s\s*\[\s*\w+\s*\]\s*=\s*([^;]+);" % re.escape(name))
    for m in pat.finditer(text):
        vals.append(m.group(1).strip())
    return vals


def main():
    if not os.path.isfile(SRC):
        print("sem", SRC)
        return 0
    text = open(SRC).read()
    keys = [
        "SPRITE_RUNSPEED", "SPRITE_MAXDASHFUEL", "SPRITE_JUMP",
        "SPRITE_HEIGHT", "SPRITE_WIDTH", "SPRITE_WEIGHT",
        "SPRITE_DAMAGE", "_personality",
    ]
    lines = ["# SPRITE_* extraído de frame_1479\n",
             "| campo | " + " | ".join(NAMES) + " |",
             "|---|" + "|".join(["---"] * 8) + "|"]
    for k in keys:
        vals = grab(text, k)
        if not vals:
            # assignments SPRITE_RUNSPEED[n] = 8 inside per-name blocks
            continue
        row = vals[:8] + ["—"] * 8
        lines.append("| `%s` | %s |" % (k, " | ".join(row[:8])))
        print(k, vals[:8])
    # also dump nearby numeric tables
    extra = re.findall(
        r"^((?:SPRITE_|AI_|MAX_|MIN_|BASE_|BUTCH_|BOOMER_)[A-Z0-9_]+)\s*=\s*([^;]+);",
        text, re.M)
    lines.append("\n## Constantes\n")
    for a, b in extra:
        lines.append("- `%s` = %s" % (a, b.strip()))
    with open(OUT, "w") as f:
        f.write("\n".join(lines) + "\n")
    print("escreveu", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
