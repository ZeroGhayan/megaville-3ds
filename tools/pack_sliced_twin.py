#!/usr/bin/env python3
"""Twin + flip SÓ das 8 poses (blossom.t3s …). Clip e palco não."""
from __future__ import print_function
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from flash_twin import twin_mul
from char_clips import CHARS

try:
    from PIL import Image
except ImportError:
    sys.exit(0)

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
GFX = os.path.join(ROOT, "platforms", "3ds", "gfx")
KEEP = set(name for name, _folder, _fam in CHARS)


def cleanup_stray():
    if not os.path.isdir(GFX):
        return
    for fn in os.listdir(GFX):
        path = os.path.join(GFX, fn)
        if fn.endswith("_t.t3s") or fn.endswith("_f.t3s"):
            base = fn[:-6]
            if base not in KEEP:
                os.remove(path)
                continue
        if os.path.isdir(path) and (fn.endswith("_t") or fn.endswith("_f")):
            base = fn[:-2]
            if base not in KEEP:
                shutil.rmtree(path, ignore_errors=True)


def main():
    cleanup_stray()
    if not os.path.isdir(GFX):
        return 0
    for name in sorted(KEEP):
        fn = name + ".t3s"
        path = os.path.join(GFX, fn)
        if not os.path.isfile(path):
            continue
        lines = open(path).read().splitlines()
        pngs = [l.strip() for l in lines if l.strip().endswith(".png")]
        if not pngs:
            continue
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
