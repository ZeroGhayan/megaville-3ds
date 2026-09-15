#!/usr/bin/env python3
"""Empacota TODAS as PNG do Dexter sem crop, uma folha por label.

  python3 tools/pack_dexter_raw.py
  python3 tools/pack_dexter_raw.py --report-only
"""
from __future__ import print_function

import argparse
import os
import re
import shutil
import sys

try:
    from PIL import Image
except ImportError:
    sys.stderr.write("precisa: pip3 install --user Pillow\n")
    sys.exit(1)

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DUMP_CAND = [
    os.path.join(ROOT, "assets", "raw", "dump", "sprites", "DefineSprite_1236_Dexter"),
    os.path.join(ROOT, "assets", "private", "dexter_raw", "src"),
]
GFX = os.path.join(ROOT, "platforms", "3ds", "gfx")
HDR = os.path.join(ROOT, "platforms", "3ds", "source", "clip_table.h")
MAX_GPU = 1024

RANGES = [
    ("idle",    1, 48),
    ("land",    50, 57),
    ("jump",    62, 69),
    ("forward", 71, 84),
    ("dash",    86, 97),
    ("shield",  98, 98),
    ("combo1",  99, 117),
    ("combo2",  118, 130),
    ("combo3",  131, 151),
    ("ranged",  152, 198),
    ("air",     199, 222),
    ("downatk", 223, 250),
    ("fall",    251, 257),
    ("fallen",  258, 267),
    ("recover", 268, 278),
    ("win",     283, 295),
    ("damage",  319, 394),
    ("extra",   395, 625),
]
ONESHOT = set(("combo1", "combo2", "combo3", "ranged", "air",
               "downatk", "fall", "recover", "damage", "dash"))


def find_dump():
    for p in DUMP_CAND:
        if os.path.isdir(p):
            pngs = [f for f in os.listdir(p) if f.lower().endswith(".png")]
            if pngs:
                return p
    return None


def frame_path(dump, n):
    for name in ("%d.png" % n, "%04d.png" % n, "frame_%d.png" % n):
        p = os.path.join(dump, name)
        if os.path.isfile(p):
            return p
    return None


def fit_gpu(im):
    w, h = im.size
    if w <= MAX_GPU and h <= MAX_GPU:
        return im, False
    s = min(MAX_GPU / float(w), MAX_GPU / float(h))
    nw, nh = max(1, int(w * s)), max(1, int(h * s))
    return im.resize((nw, nh), Image.NEAREST), True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--report-only", action="store_true")
    args = ap.parse_args()

    dump = find_dump()
    if not dump:
        sys.stderr.write("dump Dexter nao encontrado. Copia as PNG para\n")
        sys.stderr.write("  assets/private/dexter_raw/src/\n")
        sys.exit(1)

    files = [f for f in os.listdir(dump) if f.lower().endswith(".png")]
    nums = []
    for f in files:
        m = re.search(r"(\d+)", os.path.splitext(f)[0])
        if m:
            nums.append(int(m.group(1)))
    nums = sorted(set(nums))
    print("dump:", dump)
    print("pngs:", len(files), "min", min(nums) if nums else 0,
          "max", max(nums) if nums else 0)

    sizes = []
    scaled = 0
    missing = []
    for name, a, b in RANGES:
        for n in range(a, b + 1):
            p = frame_path(dump, n)
            if not p:
                missing.append(n)
                continue
            im = Image.open(p)
            sizes.append(im.size)
            if im.size[0] > MAX_GPU or im.size[1] > MAX_GPU:
                scaled += 1

    if sizes:
        aw = sum(s[0] for s in sizes) / float(len(sizes))
        ah = sum(s[1] for s in sizes) / float(len(sizes))
        print("tamanho medio: %.0fx%.0f  (max %dx%d)" % (
            aw, ah, max(s[0] for s in sizes), max(s[1] for s in sizes)))
        vram = aw * ah * 4 * len(sizes) / (1024 * 1024.0)
        print("VRAM bruta estimada (sem compressao): %.1f MiB  (%d frames)" % (
            vram, len(sizes)))
    print("frames >1024 (so scale GPU):", scaled)
    print("faltando no dump:", len(missing), missing[:24])

    if args.report_only:
        return 0

    os.makedirs(GFX, exist_ok=True)
    rows = []
    for name, a, b in RANGES:
        folder = os.path.join(GFX, "dex_" + name)
        if os.path.isdir(folder):
            shutil.rmtree(folder)
        os.makedirs(folder)
        count = 0
        maxw = maxh = 1
        for n in range(a, b + 1):
            p = frame_path(dump, n)
            if not p:
                continue
            im = Image.open(p).convert("RGBA")
            im, _did = fit_gpu(im)
            im.save(os.path.join(folder, "%04d.png" % count))
            if im.size[0] > maxw:
                maxw = im.size[0]
            if im.size[1] > maxh:
                maxh = im.size[1]
            count += 1
        if count == 0:
            print("skip", name)
            rows.append((name, 0, -1, 1))
            continue
        cols = max(1, MAX_GPU // maxw)
        rows_fit = max(1, MAX_GPU // maxh)
        chunk = max(1, min(count, cols * rows_fit))
        if chunk > 16:
            chunk = 16
        nsheet = (count + chunk - 1) // chunk
        for s in range(nsheet):
            t3s = os.path.join(GFX, "dex_%s_%d.t3s" % (name, s))
            lines = ["--atlas -f rgba8888 -z auto"]
            lo = s * chunk
            hi = min(count, lo + chunk)
            for i in range(lo, hi):
                lines.append("dex_%s/%04d.png" % (name, i))
            with open(t3s, "w") as fh:
                fh.write("\n".join(lines) + "\n")
        loop = -1 if name in ONESHOT else (count - 1 if name in (
            "fallen", "win", "shield", "extra") else 0)
        rows.append((name, count, loop, chunk))
        print("folha", name, count, "frames  chunk", chunk, "sheets", nsheet)

    with open(HDR, "w") as fh:
        fh.write("/* gerado por tools/pack_dexter_raw.py */\n")
        fh.write("#ifndef MEG_CLIP_TABLE_H\n#define MEG_CLIP_TABLE_H\n")
        fh.write("#define MEG_DEX_CLIP 1\n")
        fh.write("#define DEX_CLIP_DATA \\\n")
        for i, (name, count, loop, chunk) in enumerate(rows):
            comma = " \\" if i + 1 < len(rows) else ""
            pref = "romfs:/gfx/dex_%s" % name
            fh.write('  { "%s", %d, %d, %d },%s\n' % (
                pref, count, loop, chunk, comma))
        fh.write("#endif\n")
    print("header", HDR)
    print("ok — cd platforms/3ds && make")
    return 0


if __name__ == "__main__":
    sys.exit(main())
