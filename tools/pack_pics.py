#!/usr/bin/env python3
"""Retratos VS/Continue.

Pasta (local, gitignored):
  assets/private/pics/

Nomes (qualquer capitalização, .png / .jpg):
  blossom.png     bubbles.png     buttercup.png   bell.png
  dexter.png      rowdy.png       zim.png         shira.png

Aceita também 0.png…7.png nessa ordem, ou 'rowdyruff', 'shirabell'.
"""
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
ALIAS = {
    "rowdyruff": "rowdy",
    "rowdy": "rowdy",
    "shirabell": "shira",
    "shira": "shira",
    "ppgbloom": "blossom",
}


def norm(s):
    return s.lower().replace(" ", "").replace("_", "").replace("-", "")


def find(name):
    if not os.path.isdir(SRC):
        return None
    want = norm(name)
    numbered = os.path.join(SRC, "%d.png" % NAMES.index(name))
    if os.path.isfile(numbered):
        return numbered
    hits = []
    for fn in os.listdir(SRC):
        base, ext = os.path.splitext(fn)
        if ext.lower() not in (".png", ".jpg", ".jpeg"):
            continue
        b = norm(base)
        mapped = ALIAS.get(b, b)
        if mapped == want or b == want or b.startswith(want) or want in b:
            hits.append(os.path.join(SRC, fn))
    if not hits:
        return None
    hits.sort(key=lambda p: (len(os.path.basename(p)), os.path.basename(p)))
    return hits[0]


def main():
    print("pics dir:", SRC)
    print("existe:", os.path.isdir(SRC))
    if os.path.isdir(SRC):
        print("ficheiros:", ", ".join(sorted(os.listdir(SRC))) or "(vazio)")
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
            print("pic", i, name, "←", os.path.basename(p), im.size)
            n += 1
        else:
            im = Image.new("RGBA", (8, 8), (0, 0, 0, 0))
            im.save(out)
            print("pic", i, name, "FALTA  (queria", name + ".png)")
        imgs.append(im)
        lines.append("pic/%d.png" % i)
    for i, im in enumerate(imgs):
        tw = twin_mul(im)
        tw.save(os.path.join(GFX, "pic", "%d.png" % (i + 8)))
        lines.append("pic/%d.png" % (i + 8))
    with open(os.path.join(GFX, "pic.t3s"), "w") as f:
        f.write("\n".join(lines) + "\n")
    print("ok", n, "/ 8 + twin →", os.path.join(GFX, "pic.t3s"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
