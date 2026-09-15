#!/usr/bin/env python3
"""Copia assets/private/pics/ → gfx/pic.t3s (0-7 normal, 8-15 twin Flash)."""
from __future__ import print_function
import os
import sys

try:
    from PIL import Image
except ImportError:
    sys.stderr.write("precisa Pillow\n")
    sys.exit(1)

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from flash_twin import twin_mul

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC = os.path.join(ROOT, "assets", "private", "pics")
GFX = os.path.join(ROOT, "platforms", "3ds", "gfx")
NAMES = ["blossom", "bubbles", "buttercup", "bell",
         "dexter", "rowdy", "zim", "shira"]


def find(name):
    if not os.path.isdir(SRC):
        return None
    low = name.lower()
    for fn in os.listdir(SRC):
        base, ext = os.path.splitext(fn)
        if ext.lower() not in (".png", ".jpg", ".jpeg"):
            continue
        b = base.lower().replace(" ", "").replace("_", "")
        if b == low or b.startswith(low) or low in b:
            return os.path.join(SRC, fn)
    return None


def main():
    os.makedirs(os.path.join(GFX, "pic"), exist_ok=True)
    lines = ["--atlas -f rgba8888 -z auto"]
    n = 0
    imgs = []
    for i, name in enumerate(NAMES):
        p = find(name)
        out = os.path.join(GFX, "pic", "%d.png" % i)
        if p:
            im = Image.open(p).convert("RGBA")
            if im.size[1] > 160:
                nw = max(1, im.size[0] * 160 // im.size[1])
                im = im.resize((nw, 160), Image.BICUBIC)
            im.save(out)
            print("pic", name, "←", os.path.basename(p), im.size)
            n += 1
        else:
            im = Image.new("RGBA", (8, 8), (0, 0, 0, 0))
            im.save(out)
            print("pic", name, "FALTA")
        imgs.append(im)
        lines.append("pic/%d.png" % i)
    for i, im in enumerate(imgs):
        tw = twin_mul(im)
        tw.save(os.path.join(GFX, "pic", "%d.png" % (i + 8)))
        lines.append("pic/%d.png" % (i + 8))
    with open(os.path.join(GFX, "pic.t3s"), "w") as f:
        f.write("\n".join(lines) + "\n")
    print("ok", n, "/ 8 + twin")
    return 0 if n else 1


if __name__ == "__main__":
    sys.exit(main())
