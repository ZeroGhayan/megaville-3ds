#!/usr/bin/env python3
"""Gera gfx/<char>_t.t3s com multiply Flash a partir dos PNG sliced."""
from __future__ import print_function
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from flash_twin import twin_mul

try:
    from PIL import Image
except ImportError:
    sys.exit(0)

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
GFX = os.path.join(ROOT, "platforms", "3ds", "gfx")
SKIP = ("pic.t3s",)


def main():
    if not os.path.isdir(GFX):
        return 0
    for fn in os.listdir(GFX):
        if not fn.endswith(".t3s"):
            continue
        if fn in SKIP or fn.startswith("dex_") or fn.endswith("_t.t3s") or fn.endswith("_f.t3s"):
            continue
        path = os.path.join(GFX, fn)
        lines = open(path).read().splitlines()
        pngs = [l.strip() for l in lines if l.strip().endswith(".png")]
        if not pngs:
            continue
        name = fn[:-4]
        outdir = os.path.join(GFX, name + "_t")
        flipdir = os.path.join(GFX, name + "_f")
        os.makedirs(outdir, exist_ok=True)
        os.makedirs(flipdir, exist_ok=True)
        out = ["--atlas -f rgba8888 -z auto"]
        outf = ["--atlas -f rgba8888 -z auto"]
        for i, rel in enumerate(pngs):
            src = os.path.join(GFX, rel)
            if not os.path.isfile(src):
                continue
            im = Image.open(src).convert("RGBA")
            twin_mul(im).save(os.path.join(outdir, "%02d.png" % i))
            im.transpose(Image.FLIP_LEFT_RIGHT).save(
                os.path.join(flipdir, "%02d.png" % i))
            out.append("%s_t/%02d.png" % (name, i))
            outf.append("%s_f/%02d.png" % (name, i))
        with open(os.path.join(GFX, name + "_t.t3s"), "w") as f:
            f.write("\n".join(out) + "\n")
        with open(os.path.join(GFX, name + "_f.t3s"), "w") as f:
            f.write("\n".join(outf) + "\n")
        print("twin+flip", name)
    return 0


if __name__ == "__main__":
    sys.exit(main())
