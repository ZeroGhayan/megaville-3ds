#!/usr/bin/env python3
"""Aperta os PNG sliced, grava origem (pés) em spr_off.h.

Corre no gfx_stage depois do cp a partir de assets/private/sliced.
"""
from __future__ import print_function

import os
import sys

try:
    from PIL import Image
except ImportError:
    sys.exit(0)

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from flash_twin import knockout_border, trim_alpha, core_feet

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
GFX = os.path.join(ROOT, "platforms", "3ds", "gfx")
HDR = os.path.join(ROOT, "platforms", "3ds", "source", "spr_off.h")
CHARS = ["blossom", "bubbles", "buttercup", "bell",
         "dexter", "rowdy", "zim", "shira"]
FRAMES = ["idle", "land", "jump", "walk", "dash",
          "heavy", "light", "light2", "hit", "ko"]


def main():
    ox = [[0] * len(FRAMES) for _ in CHARS]
    oy = [[0] * len(FRAMES) for _ in CHARS]
    any_ok = False
    for ci, name in enumerate(CHARS):
        folder = os.path.join(GFX, name)
        t3s = os.path.join(GFX, name + ".t3s")
        if not os.path.isdir(folder) or not os.path.isfile(t3s):
            continue
        for fi, fr in enumerate(FRAMES):
            p = os.path.join(folder, fr + ".png")
            if not os.path.isfile(p):
                continue
            im = knockout_border(Image.open(p).convert("RGBA"))
            im, _, _ = trim_alpha(im, 1)
            fx, fy = core_feet(im)
            im.save(p)
            ox[ci][fi] = int(fx)
            oy[ci][fi] = int(fy)
            any_ok = True
            print("off", name, fr, im.size, "origin", fx, fy)
    with open(HDR, "w") as f:
        f.write("/* gerado por tools/pack_sliced_off.py */\n")
        f.write("#ifndef MEG_SPR_OFF_H\n#define MEG_SPR_OFF_H\n")
        f.write("#define MEG_SPR_OFF 1\n")
        f.write("static const int16_t SPR_OX[%d][%d] = {\n" % (
            len(CHARS), len(FRAMES)))
        for row in ox:
            f.write("  { %s },\n" % ", ".join(str(v) for v in row))
        f.write("};\nstatic const int16_t SPR_OY[%d][%d] = {\n" % (
            len(CHARS), len(FRAMES)))
        for row in oy:
            f.write("  { %s },\n" % ", ".join(str(v) for v in row))
        f.write("};\n#endif\n")
    print("header", HDR, "ok" if any_ok else "(vazio)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
