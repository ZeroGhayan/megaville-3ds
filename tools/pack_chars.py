#!/usr/bin/env python3
"""Extrai poses + clips de TODOS os lutadores a partir do dump JPEXS.

  python3 tools/pack_chars.py
  python3 tools/pack_chars.py --poses-only
"""
from __future__ import print_function

import argparse
import os
import shutil
import sys

try:
    from PIL import Image
except ImportError:
    sys.stderr.write("precisa: pip3 install --user Pillow\n")
    sys.exit(1)

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from flash_twin import twin_mul, knockout_border, core_feet
from flash_origin import FLASH_PNG_ORIGIN
from char_clips import CHARS, RANGES, POSES, POSE_NAMES, ANIM_NAMES, ONESHOT

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DUMP = os.path.join(ROOT, "assets", "raw", "dump", "sprites")
GFX = os.path.join(ROOT, "platforms", "3ds", "gfx")
SRC = os.path.join(ROOT, "platforms", "3ds", "source")
HDR = os.path.join(SRC, "clip_table.h")
OFF = os.path.join(SRC, "clip_off.h")
TARGET_H = 72
MAX_GPU = 1024
CHUNK = 16


def frame_path(folder, n):
    for name in ("%d.png" % n, "%04d.png" % n):
        p = os.path.join(folder, name)
        if os.path.isfile(p):
            return p
    return None


def prepare(im, origin):
    im = knockout_border(im.convert("RGBA"))
    bb = im.split()[-1].getbbox()
    if not bb:
        return im, im.size[0] // 2, im.size[1]
    l, t, r, b = bb
    l = max(0, l - 1)
    t = max(0, t - 1)
    r = min(im.size[0], r + 1)
    b = min(im.size[1], b + 1)
    trim = im.crop((l, t, r, b))
    if origin:
        ox = origin[0] - l
        oy = origin[1] - t
    else:
        fx, fy = core_feet(im)
        ox, oy = fx - l, fy - t
    w, h = trim.size
    if h > TARGET_H and h > 0:
        s = TARGET_H / float(h)
        nw = max(1, int(w * s))
        nh = max(1, int(h * s))
        trim = trim.resize((nw, nh), Image.NEAREST)
        ox = int(ox * s)
        oy = int(oy * s)
    if trim.size[0] > MAX_GPU or trim.size[1] > MAX_GPU:
        s = min(MAX_GPU / float(trim.size[0]), MAX_GPU / float(trim.size[1]))
        trim = trim.resize((max(1, int(trim.size[0] * s)),
                            max(1, int(trim.size[1] * s))), Image.NEAREST)
        ox = int(ox * s)
        oy = int(oy * s)
    return trim, ox, oy


def write_t3s(rel_pngs, t3s_path):
    lines = ["--atlas -f rgba8888 -z auto"] + list(rel_pngs)
    with open(t3s_path, "w") as f:
        f.write("\n".join(lines) + "\n")


def pack_poses(name, folder, family, origin):
    out = os.path.join(GFX, name)
    os.makedirs(out, exist_ok=True)
    ok = 0
    pose = dict(POSES[family])
    for pn in POSE_NAMES:
        n = pose.get(pn)
        src = frame_path(folder, n) if n else None
        if not src:
            print("pose falta", name, pn)
            continue
        im = Image.open(src)
        trim, ox, oy = prepare(im, origin)
        trim.save(os.path.join(out, pn + ".png"))
        ok += 1
        print("pose", name, pn, trim.size, "off", ox, oy)
    if ok:
        rel = ["%s/%s.png" % (name, pn) for pn in POSE_NAMES
               if os.path.isfile(os.path.join(out, pn + ".png"))]
        write_t3s(rel, os.path.join(GFX, name + ".t3s"))
    return ok


def pack_anims(name, folder, family, origin):
    rows = []
    all_ox = []
    all_oy = []
    for anim, a, b in RANGES[family]:
        dest = os.path.join(GFX, "%s_%s" % (name, anim))
        if os.path.isdir(dest):
            shutil.rmtree(dest)
        os.makedirs(dest)
        count = 0
        maxw = maxh = 1
        for n in range(a, b + 1):
            src = frame_path(folder, n)
            if not src:
                continue
            im = Image.open(src)
            trim, ox, oy = prepare(im, origin)
            trim.save(os.path.join(dest, "%04d.png" % count))
            all_ox.append(ox)
            all_oy.append(oy)
            if trim.size[0] > maxw:
                maxw = trim.size[0]
            if trim.size[1] > maxh:
                maxh = trim.size[1]
            count += 1
        if count == 0:
            rows.append((anim, 0, 0, 1))
            print("anim skip", name, anim)
            continue
        nsheet = (count + CHUNK - 1) // CHUNK
        for s in range(nsheet):
            lo = s * CHUNK
            hi = min(count, lo + CHUNK)
            rel = ["%s_%s/%04d.png" % (name, anim, i) for i in range(lo, hi)]
            write_t3s(rel, os.path.join(GFX, "%s_%s_%d.t3s" % (name, anim, s)))
        loop = -1 if anim in ONESHOT else 0
        if anim in ("fallen", "win", "shield"):
            loop = count - 1
        rows.append((anim, count, loop, CHUNK))
        print("anim", name, anim, count, "%dx%d" % (maxw, maxh))
    return rows, all_ox, all_oy


def write_headers(all_rows, all_oxs, all_oys):
    any_clip = any(r[1] > 0 for rows in all_rows for r in rows)
    with open(HDR, "w") as f:
        f.write("/* gerado por tools/pack_chars.py */\n")
        f.write("#ifndef MEG_CLIP_TABLE_H\n#define MEG_CLIP_TABLE_H\n")
        f.write("#define MEG_CLIP %d\n" % (1 if any_clip else 0))
        f.write("#define MEG_DEX_CLIP MEG_CLIP\n")
        f.write("#define CLIP_ANIM_N %d\n" % len(ANIM_NAMES))
        f.write("typedef struct { const char *prefix; int count, loop, chunk; } ClipAnim;\n")
        f.write("static const ClipAnim CLIP[8][CLIP_ANIM_N] = {\n")
        for ci, (name, _folder, _fam) in enumerate(CHARS):
            rows = all_rows[ci] if ci < len(all_rows) else []
            by = {r[0]: r for r in rows}
            f.write("  {\n")
            for i, anim in enumerate(ANIM_NAMES):
                rec = by.get(anim, (anim, 0, 0, 1))
                _n, count, loop, chunk = rec[0], rec[1], rec[2], rec[3]
                pref = "romfs:/gfx/%s_%s" % (name, anim)
                comma = "," if i + 1 < len(ANIM_NAMES) else ""
                f.write('    { "%s", %d, %d, %d }%s\n' % (
                    pref, count, loop, chunk, comma))
            f.write("  },\n")
        f.write("};\n#endif\n")
    with open(OFF, "w") as f:
        f.write("/* ox,oy no recorte; gerado por pack_chars.py */\n")
        f.write("static const int CLIP_BASE[8][CLIP_ANIM_N] = {\n")
        for ci, rows in enumerate(all_rows):
            by = {r[0]: r[1] for r in rows}
            base = 0
            f.write("  {")
            for anim in ANIM_NAMES:
                f.write("%d," % base)
                base += by.get(anim, 0)
            f.write("},\n")
        f.write("};\n")
        for ci, ox in enumerate(all_oxs):
            f.write("static const int16_t CLIP_OX_%d[] = {\n" % ci)
            if not ox:
                f.write("0\n")
            else:
                for i, v in enumerate(ox):
                    f.write("%d,%s" % (v, "\n" if i % 16 == 15 else ""))
                f.write("\n")
            f.write("};\n")
            f.write("static const int16_t CLIP_OY_%d[] = {\n" % ci)
            oy = all_oys[ci]
            if not oy:
                f.write("0\n")
            else:
                for i, v in enumerate(oy):
                    f.write("%d,%s" % (v, "\n" if i % 16 == 15 else ""))
                f.write("\n")
            f.write("};\n")
        f.write("static const int16_t *CLIP_OX[8] = {")
        f.write(",".join("CLIP_OX_%d" % i for i in range(8)))
        f.write("};\nstatic const int16_t *CLIP_OY[8] = {")
        f.write(",".join("CLIP_OY_%d" % i for i in range(8)))
        f.write("};\n")
    print("header", HDR)
    print("off", OFF)


def write_stub():
    with open(HDR, "w") as f:
        f.write("#ifndef MEG_CLIP_TABLE_H\n#define MEG_CLIP_TABLE_H\n")
        f.write("#define MEG_CLIP 0\n#define MEG_DEX_CLIP 0\n")
        f.write("#endif\n")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--poses-only", action="store_true")
    args = ap.parse_args()
    os.makedirs(GFX, exist_ok=True)
    if not os.path.isdir(DUMP):
        print("sem dump:", DUMP)
        write_stub()
        return 0
    all_rows, all_oxs, all_oys = [], [], []
    for name, folder_name, family in CHARS:
        folder = os.path.join(DUMP, folder_name)
        origin = FLASH_PNG_ORIGIN.get(name)
        if not os.path.isdir(folder):
            print("falta pasta", folder)
            all_rows.append([])
            all_oxs.append([])
            all_oys.append([])
            continue
        pack_poses(name, folder, family, origin)
        if args.poses_only:
            all_rows.append([])
            all_oxs.append([])
            all_oys.append([])
            continue
        rows, ox, oy = pack_anims(name, folder, family, origin)
        all_rows.append(rows)
        all_oxs.append(ox)
        all_oys.append(oy)
    write_headers(all_rows, all_oxs, all_oys)
    return 0


if __name__ == "__main__":
    sys.exit(main())
