#!/usr/bin/env python3
"""Lê matrizes PlaceObject2/3 dos clips dos lutadores no SWF.

  python3 tools/swf_place.py
  python3 tools/swf_place.py assets/raw/megaville.swf

Escreve data/place-map.md (sem PNG). Origem Flash (0,0) no PNG JPEXS:
  ox = -xMin_px    oy = -yMin_px
"""
from __future__ import print_function

import os
import struct
import sys
import zlib

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
OUT = os.path.join(ROOT, "data", "place-map.md")
SWF_CAND = [
    os.path.join(ROOT, "assets", "raw", "megaville.swf"),
    os.path.join(ROOT, "assets", "raw", "dump", "megaville.swf"),
]

CHARS = {
    370: "Bell",
    471: "Blossom",
    563: "Bubbles",
    690: "Buttercup",
    1236: "Dexter",
    1334: "zim",
    1399: "Rowdyruff",
    1469: "Shira Bell",
}


class Bits(object):
    def __init__(self, buf, off):
        self.buf = buf
        self.i = off * 8

    def ub(self, n):
        v = 0
        b = self.buf
        i = self.i
        for _ in range(n):
            v = (v << 1) | ((b[i >> 3] >> (7 - (i & 7))) & 1)
            i += 1
        self.i = i
        return v

    def sb(self, n):
        v = self.ub(n)
        if n and v & (1 << (n - 1)):
            v -= 1 << n
        return v

    def fb(self, n):
        return self.sb(n) / 65536.0

    def align(self):
        if self.i & 7:
            self.i += 8 - (self.i & 7)

    def byte(self):
        self.align()
        return self.i >> 3


def u16(b, i):
    return struct.unpack_from("<H", b, i)[0]


def u32(b, i):
    return struct.unpack_from("<I", b, i)[0]


def read_rect(buf, off):
    bits = Bits(buf, off)
    n = bits.ub(5)
    xmin = bits.sb(n)
    xmax = bits.sb(n)
    ymin = bits.sb(n)
    ymax = bits.sb(n)
    bits.align()
    return bits.byte(), xmin, xmax, ymin, ymax


def read_matrix(buf, off):
    bits = Bits(buf, off)
    sx = sy = 1.0
    r0 = r1 = 0.0
    if bits.ub(1):
        n = bits.ub(5)
        sx = bits.fb(n)
        sy = bits.fb(n)
    if bits.ub(1):
        n = bits.ub(5)
        r0 = bits.fb(n)
        r1 = bits.fb(n)
    n = bits.ub(5)
    tx = bits.sb(n)
    ty = bits.sb(n)
    bits.align()
    return bits.byte(), sx, sy, r0, r1, tx, ty


def load_swf(path):
    raw = open(path, "rb").read()
    if len(raw) < 8:
        raise SystemExit("SWF curto")
    sig = raw[:3]
    ver = raw[3]
    if sig == b"CWS":
        body = zlib.decompress(raw[8:])
    elif sig == b"FWS":
        body = raw[8:]
    elif sig == b"ZWS":
        raise SystemExit("SWF LZMA (ZWS) — converte no JPEXS para CWS/FWS")
    else:
        raise SystemExit("não é SWF: %r" % sig)
    return ver, body


def iter_tags(buf, off, end):
    while off + 2 <= end:
        code_len = u16(buf, off)
        off += 2
        code = code_len >> 6
        ln = code_len & 0x3F
        if ln == 0x3F:
            if off + 4 > end:
                break
            ln = u32(buf, off)
            off += 4
        data = buf[off:off + ln]
        yield code, data, off - 6
        off += ln
        if code == 0:
            break


def parse_place(code, data):
    """PlaceObject2 (26) / PlaceObject3 (70). Devolve dict ou None."""
    if code not in (26, 70) or not data:
        return None
    flags = data[0]
    i = 1
    has_actions = flags & 0x80
    has_clip = flags & 0x40
    has_name = flags & 0x20
    has_ratio = flags & 0x10
    has_cxform = flags & 0x08
    has_matrix = flags & 0x04
    has_id = flags & 0x02
    move = flags & 0x01
    if code == 70:
        if i >= len(data):
            return None
        flags2 = data[i]
        i += 1
        # PlaceObject3 extra; skip filter/blend if present later
        has_id = flags & 0x02
    if i + 2 > len(data):
        return None
    depth = u16(data, i)
    i += 2
    cid = None
    if has_id:
        if i + 2 > len(data):
            return None
        cid = u16(data, i)
        i += 2
    sx = sy = 1.0
    tx = ty = 0
    if has_matrix:
        i, sx, sy, _r0, _r1, tx, ty = read_matrix(data, i)
    return {
        "move": bool(move),
        "depth": depth,
        "id": cid,
        "sx": sx,
        "sy": sy,
        "tx": tx / 20.0,
        "ty": ty / 20.0,
        "has_matrix": bool(has_matrix),
    }


def parse_sprite(data):
    if len(data) < 4:
        return None
    sid = u16(data, 0)
    nframes = u16(data, 2)
    frames = []
    cur = []
    frame = 1
    for code, payload, _ in iter_tags(data, 4, len(data)):
        if code in (26, 70):
            p = parse_place(code, payload)
            if p:
                p["frame"] = frame
                cur.append(p)
        elif code == 5:  # RemoveObject2
            pass
        elif code == 1:  # ShowFrame
            frames.append(cur)
            cur = []
            frame += 1
        elif code == 0:
            break
    if cur:
        frames.append(cur)
    return sid, nframes, frames


def find_swf(argv):
    if argv:
        return argv[0]
    for p in SWF_CAND:
        if os.path.isfile(p):
            return p
    return None


def summarize(name, sid, nframes, frames):
    txs, tys = [], []
    nplace = 0
    f1 = frames[0] if frames else []
    for fr in frames:
        for p in fr:
            if p["has_matrix"]:
                txs.append(p["tx"])
                tys.append(p["ty"])
                nplace += 1
    lines = []
    lines.append("## %s  `DefineSprite_%d`" % (name, sid))
    lines.append("")
    lines.append("- frames no tag: **%d**  (ShowFrame %d)" % (nframes, len(frames)))
    lines.append("- PlaceObject com matriz: **%d**" % nplace)
    if txs:
        lines.append("- tx px: min **%.1f**  max **%.1f**" % (min(txs), max(txs)))
        lines.append("- ty px: min **%.1f**  max **%.1f**" % (min(tys), max(tys)))
        # origem no PNG JPEXS (AABB de translates; filhos ainda estendem)
        lines.append("- origem aproximada no PNG (só translates): "
                     "`ox ≈ %.1f`  `oy ≈ %.1f`  *(−min se min<0)*" % (
                         -min(txs) if min(txs) < 0 else 0.0,
                         -min(tys) if min(tys) < 0 else 0.0))
    lines.append("")
    lines.append("Primeiro frame (%d objetos):" % len(f1))
    lines.append("")
    lines.append("| depth | id | tx | ty | sx | sy |")
    lines.append("|---:|---:|---:|---:|---:|---:|")
    for p in f1[:24]:
        lines.append("| %d | %s | %.1f | %.1f | %.2f | %.2f |" % (
            p["depth"], p["id"] if p["id"] is not None else "—",
            p["tx"], p["ty"], p["sx"], p["sy"]))
    if len(f1) > 24:
        lines.append("| … | %d mais | | | | |" % (len(f1) - 24))
    lines.append("")
    return "\n".join(lines)


def main():
    path = find_swf(sys.argv[1:])
    if not path:
        sys.stderr.write("SWF não encontrado. Copia para assets/raw/megaville.swf\n")
        sys.exit(1)
    print("SWF", path)
    ver, body = load_swf(path)
    off, xmin, xmax, ymin, ymax = read_rect(body, 0)
    fps = u16(body, off) / 256.0
    frames = u16(body, off + 2)
    print("SWF v%d  palco %.1fx%.1f px  fps %.2f  frames %d" % (
        ver, (xmax - xmin) / 20.0, (ymax - ymin) / 20.0, fps, frames))
    found = {}
    for code, payload, _ in iter_tags(body, off + 4, len(body)):
        if code != 39:
            continue
        parsed = parse_sprite(payload)
        if not parsed:
            continue
        sid, nfr, frs = parsed
        if sid in CHARS:
            found[sid] = (nfr, frs)
            print("  sprite", sid, CHARS[sid], "frames", nfr, "show", len(frs))
    lines = [
        "# PlaceObject — origem Flash",
        "",
        "Gerado por `python3 tools/swf_place.py`. Sem PNG.",
        "",
        "SWF palco: **%.1f × %.1f** px  (xmin %.1f ymin %.1f)" % (
            (xmax - xmin) / 20.0, (ymax - ymin) / 20.0,
            xmin / 20.0, ymin / 20.0),
        "fps **%.2f**" % fps,
        "",
        "No PNG do JPEXS, o (0,0) do símbolo cai no pixel "
        "`(−xMin, −yMin)` do RECT do *sprite* (não do palco).",
        "Os `tx,ty` abaixo são o offset de cada **filho** em px.",
        "",
    ]
    for sid in sorted(CHARS):
        if sid not in found:
            lines.append("## %s  `%d` — não encontrado\n" % (CHARS[sid], sid))
            continue
        nfr, frs = found[sid]
        lines.append(summarize(CHARS[sid], sid, nfr, frs))
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w") as f:
        f.write("\n".join(lines) + "\n")
    print("escreveu", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
