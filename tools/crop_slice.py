#!/usr/bin/env python3
"""Crop the character out of JPEXS sprite frames (huge black canvas).

Output stays in assets/private/sliced/ (gitignored).

  python3 tools/crop_slice.py
  python3 tools/crop_slice.py --char blossom
"""
from __future__ import print_function

import argparse
import os
import sys

try:
    from PIL import Image
except ImportError:
    sys.stderr.write("precisa: pip3 install --user Pillow\n")
    sys.exit(1)

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DUMP = os.path.join(ROOT, "assets", "raw", "dump", "sprites")
OUT = os.path.join(ROOT, "assets", "private", "sliced")

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from char_clips import CHARS as CHAR_LIST, POSES, POSE_NAMES

# nome → pasta dump
CHARS = {name: folder for name, folder, _fam in CHAR_LIST}
FAM = {name: fam for name, _folder, fam in CHAR_LIST}

BLACK = 18
PAD = 2


def bbox(im):
    px = im.convert("RGBA").load()
    w, h = im.size
    x0, y0, x1, y1 = w, h, 0, 0
    found = False
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            if a < 8:
                continue
            if r <= BLACK and g <= BLACK and b <= BLACK:
                continue
            found = True
            if x < x0:
                x0 = x
            if y < y0:
                y0 = y
            if x > x1:
                x1 = x
            if y > y1:
                y1 = y
    if not found:
        return None
    x0 = max(0, x0 - PAD)
    y0 = max(0, y0 - PAD)
    x1 = min(w - 1, x1 + PAD)
    y1 = min(h - 1, y1 + PAD)
    return (x0, y0, x1 + 1, y1 + 1)


def crop_one(src, dst):
    im = Image.open(src)
    box = bbox(im)
    if not box:
        print("vazio:", src)
        return False
    cut = im.crop(box).convert("RGBA")
    # black → transparent
    pix = cut.load()
    w, h = cut.size
    for y in range(h):
        for x in range(w):
            r, g, b, a = pix[x, y]
            if r <= BLACK and g <= BLACK and b <= BLACK:
                pix[x, y] = (0, 0, 0, 0)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    cut.save(dst)
    print("%s  %dx%d" % (os.path.relpath(dst, ROOT), cut.size[0], cut.size[1]))
    return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--char", choices=list(CHARS) + ["all"], default="all")
    args = ap.parse_args()
    names = list(CHARS) if args.char == "all" else [args.char]
    ok = 0
    miss = 0
    for name in names:
        folder = os.path.join(DUMP, CHARS[name])
        fam = FAM.get(name, "ppg")
        for action, frame in POSES[fam]:
            src = os.path.join(folder, "%d.png" % frame)
            if not os.path.isfile(src):
                print("falta:", src)
                miss += 1
                continue
            dst = os.path.join(OUT, name, "%s.png" % action)
            if crop_one(src, dst):
                ok += 1
    print("ok %d  falta %d  → %s" % (ok, miss, os.path.relpath(OUT, ROOT)))
    return 0 if miss == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
