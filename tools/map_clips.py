#!/usr/bin/env python3
"""Mapeia clips anónimos → personagem pai.

Corre LOCALMENTE (dump não vai ao GitHub):

  python3 tools/map_clips.py

Escreve data/clip-map.md (só hashes/tabelas — isso sim entra no git).
"""
from __future__ import print_function

import collections
import os
import re
import sys

try:
    from PIL import Image
except ImportError:
    sys.stderr.write("precisa Pillow\n")
    sys.exit(1)

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
try:
    from flash_twin import knockout_border, trim_alpha
except ImportError:
    knockout_border = None
    trim_alpha = None

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SPR = os.path.join(ROOT, "assets", "raw", "dump", "sprites")
OUT = os.path.join(ROOT, "data", "clip-map.md")

# contentor nomeado = lutador. filhos anónimos com ID < pai e > pai anterior
CHARS = [
    (370, "Bell"),
    (471, "Blossom"),
    (563, "Bubbles"),
    (690, "Buttercup"),
    (1236, "Dexter"),
    (1334, "zim"),
    (1399, "Rowdyruff"),
    (1469, "Shira Bell"),
]


def sprite_dirs(root):
    out = {}
    if not os.path.isdir(root):
        return out
    for name in os.listdir(root):
        m = re.match(r"DefineSprite_(\d+)(?:_(.*))?$", name)
        if not m:
            continue
        path = os.path.join(root, name)
        if not os.path.isdir(path):
            continue
        out[int(m.group(1))] = (name, path, m.group(2) or "")
    return out


def pngs(folder):
    files = {}
    for fn in os.listdir(folder):
        if not fn.lower().endswith(".png"):
            continue
        m = re.search(r"(\d+)", os.path.splitext(fn)[0])
        if m:
            files[int(m.group(1))] = os.path.join(folder, fn)
    return files


def fp(path):
    im = Image.open(path).convert("RGBA")
    if knockout_border:
        im = knockout_border(im)
        im, _, _ = trim_alpha(im, 0)
    else:
        # fallback: bbox não-preto
        px = im.load()
        w, h = im.size
        xs, ys = [], []
        y = 0
        while y < h:
            x = 0
            while x < w:
                r, g, b, a = px[x, y]
                if a > 20 and (r > 22 or g > 22 or b > 22):
                    xs.append(x)
                    ys.append(y)
                x += 1
            y += 1
        if xs:
            im = im.crop((min(xs), min(ys), max(xs) + 1, max(ys) + 1))
    im = im.convert("L").resize((12, 12), Image.BILINEAR)
    return im.tobytes()


def hamming(a, b):
    n = 0
    i = 0
    while i < len(a):
        if a[i] != b[i]:
            n += 1
        i += 1
    return n


def load_fps(folder):
    files = pngs(folder)
    fps = {}
    for n in sorted(files):
        try:
            fps[n] = fp(files[n])
        except Exception as e:
            sys.stderr.write("skip %s: %s\n" % (files[n], e))
    return fps


def unique_runs(fps):
    nums = sorted(fps)
    if not nums:
        return 0, []
    seen = set(fps[n] for n in nums)
    runs = []
    i = 0
    while i < len(nums):
        j = i
        h = fps[nums[i]]
        while j + 1 < len(nums) and nums[j + 1] == nums[j] + 1 and fps[nums[j + 1]] == h:
            j += 1
        if j > i:
            runs.append((nums[i], nums[j], j - i + 1))
        i = j + 1
    return len(seen), runs


def best_match(child, parent, max_dist=18):
    """Procura a tira do filho como sequência no pai (holds permitidos)."""
    ck = sorted(child)
    pk = sorted(parent)
    if not ck or not pk:
        return None
    # unique ordered child frames (skip holds inside child)
    cuniq = []
    prev = None
    for n in ck:
        if child[n] != prev:
            cuniq.append((n, child[n]))
            prev = child[n]
    # slide unique child over parent unique
    puniq = []
    prev = None
    for n in pk:
        if parent[n] != prev:
            puniq.append((n, parent[n]))
            prev = parent[n]
    best = None
    for i, (pn, ph) in enumerate(puniq):
        if hamming(ph, cuniq[0][1]) > max_dist:
            continue
        matched = 1
        j = i + 1
        k = 1
        while k < len(cuniq) and j < len(puniq):
            if hamming(puniq[j][1], cuniq[k][1]) <= max_dist:
                matched += 1
                k += 1
                j += 1
            else:
                break
        score = matched / float(len(cuniq))
        span_a = pn
        span_b = puniq[min(j, len(puniq) - 1)][0]
        rec = (score, matched, len(cuniq), span_a, span_b)
        if best is None or rec[0] > best[0] or (rec[0] == best[0] and rec[1] > best[1]):
            best = rec
    return best


def classify(best, nchild):
    if not best:
        return "UNUSED"
    score, matched, nuniq, a, b = best
    if score >= 0.75 and matched >= min(3, nuniq):
        return "EMBEDDED"
    if matched <= 1:
        return "FROZEN"
    if score >= 0.4:
        return "PARTIAL"
    return "UNUSED"


def main():
    dirs = sprite_dirs(SPR)
    if not dirs:
        sys.stderr.write("dump não encontrado: %s\n" % SPR)
        sys.stderr.write("copia o dump JPEXS para assets/raw/dump/sprites/\n")
        sys.exit(1)

    lines = [
        "# Mapa clips anónimos → lutador",
        "",
        "Gerado por `python3 tools/map_clips.py`. Sem PNG.",
        "",
        "| Tag | Significado |",
        "|---|---|",
        "| EMBEDDED | tira já está nas PNG do pai — ignora a pasta |",
        "| FROZEN | pai só tem o frame 1 do filho — usar a pasta do filho |",
        "| PARTIAL | mistura; ver span |",
        "| UNUSED | não casa com o pai |",
        "",
    ]

    char_ids = [c[0] for c in CHARS]
    for idx, (cid, cname) in enumerate(CHARS):
        if cid not in dirs:
            lines.append("## %s (`%d`) — pasta em falta\n" % (cname, cid))
            continue
        _, ppath, _ = dirs[cid]
        print("hash pai", cname, "...")
        sys.stdout.flush()
        parent = load_fps(ppath)
        pu, pruns = unique_runs(parent)
        hold2 = sum(1 for a, b, n in pruns if n == 2)
        lines.append("## %s  `DefineSprite_%d`" % (cname, cid))
        lines.append("")
        lines.append("- PNG pai: **%d**  únicas: **%d**  holds de 2 frames: %d" %
                     (len(parent), pu, hold2))
        lo = 0 if idx == 0 else char_ids[idx - 1]
        kids = sorted(i for i in dirs if lo < i < cid and not dirs[i][2])
        if not kids:
            lines.append("- (sem anónimos neste intervalo)")
            lines.append("")
            continue
        lines.append("")
        lines.append("| Filho | PNG | Únicas | Tag | Span no pai | overlap |")
        lines.append("|---|---:|---:|---|---|---:|")
        for kid in kids:
            _, kpath, _ = dirs[kid]
            print("  filho", kid, "...")
            sys.stdout.flush()
            child = load_fps(kpath)
            cu, _ = unique_runs(child)
            best = best_match(child, parent)
            tag = classify(best, len(child))
            if best:
                score, matched, nuniq, a, b = best
                span = "%d–%d" % (a, b)
                ov = "%d/%d (%.0f%%)" % (matched, nuniq, score * 100)
            else:
                span, ov = "—", "—"
            lines.append("| `%d` | %d | %d | **%s** | %s | %s |" %
                         (kid, len(child), cu, tag, span, ov))
        lines.append("")

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w") as f:
        f.write("\n".join(lines) + "\n")
    print("escreveu", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
