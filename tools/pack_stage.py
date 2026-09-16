#!/usr/bin/env python3
"""Palco + HUD a partir do dump: terrain, streetlamp, opaque, hpbar."""
from __future__ import print_function

import os
import sys

try:
    from PIL import Image
except ImportError:
    sys.exit(0)

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from flash_twin import knockout_border, trim_alpha

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DUMP = os.path.join(ROOT, "assets", "raw", "dump", "sprites")
GFX = os.path.join(ROOT, "platforms", "3ds", "gfx")

STAGE = [
    ("terrain", "DefineSprite_1087_terrain0000", 1, (400, 48)),
    ("city", "DefineSprite_1086", 1, (400, 200)),
    ("lamp", "DefineSprite_1446_streetlamp", 1, None),
    ("sky", "DefineSprite_1089_opaque0000", 1, (400, 200)),
    ("hpbar", "DefineSprite_1115_hpbar", 1, None),
    ("hpframe", "DefineSprite_1123_hpbarframe", 1, None),
    ("dashbar", "DefineSprite_1102_dashbarframe", 1, None),
    ("ready", "DefineSprite_778_readyfight", 1, None),
]


def fit(im, box):
    im = knockout_border(im.convert("RGBA"))
    im, _, _ = trim_alpha(im, 0)
    w, h = im.size
    cap = 512
    if w > cap or h > cap:
        s = min(cap / float(max(1, w)), cap / float(max(1, h)))
        im = im.resize((max(1, int(w * s)), max(1, int(h * s))), Image.NEAREST)
        w, h = im.size
    if not box:
        return im
    tw, th = box
    im.thumbnail((tw, th), Image.NEAREST)
    canvas = Image.new("RGBA", (tw, th), (0, 0, 0, 0))
    x = (tw - im.size[0]) // 2
    y = th - im.size[1]
    canvas.paste(im, (x, max(0, y)), im)
    return canvas


def main():
    if not os.path.isdir(DUMP):
        print("pack_stage: sem dump")
        return 0
    os.makedirs(GFX, exist_ok=True)
    ok = 0
    for name, folder, frame, box in STAGE:
        src = os.path.join(DUMP, folder, "%d.png" % frame)
        if not os.path.isfile(src):
            print("falta", src)
            continue
        im = fit(Image.open(src), box)
        outdir = os.path.join(GFX, name)
        os.makedirs(outdir, exist_ok=True)
        im.save(os.path.join(outdir, "00.png"))
        t3s = os.path.join(GFX, name + ".t3s")
        with open(t3s, "w") as f:
            f.write("--atlas -f rgba8888 -z auto\n%s/00.png\n" % name)
        print("stage", name, im.size)
        ok += 1
    print("stage ok", ok)
    return 0


if __name__ == "__main__":
    sys.exit(main())
