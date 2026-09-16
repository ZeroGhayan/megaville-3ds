#!/usr/bin/env python3
"""Extrai stats e jump/ataque por personagem do dump AS2.

  python3 tools/dump_combat.py
"""
from __future__ import print_function
import os
import re
import sys
from collections import OrderedDict

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SCR = os.path.join(ROOT, "assets", "raw", "dump", "scripts")
OUT = os.path.join(ROOT, "data", "as2-combat.md")
NAMES = ["BLOSSOM", "BUBBLES", "BUTTERCUP", "BELL",
         "DEXTER", "ROWDY", "ZIM", "SHIRABELL"]
IDS = ["NAME_BLOSSOM", "NAME_BUBBLES", "NAME_BUTTERCUP", "NAME_BELL",
       "NAME_DEXTER", "NAME_ROWDY", "NAME_ZIM", "NAME_SHIRABELL"]


def read(rel):
    p = os.path.join(SCR, rel)
    if not os.path.isfile(p):
        return ""
    return open(p).read()


def grab_arr(text, name):
    return re.findall(r"%s\s*\[[^\]]+\]\s*=\s*([^;]+);" % re.escape(name), text)


def all_sprite_keys(text):
    return sorted(set(re.findall(r"(SPRITE_[A-Z0-9_]+)\s*\[", text)))


def lines_matching(text, pat):
    out = []
    for i, line in enumerate(text.splitlines(), 1):
        if re.search(pat, line, re.I):
            out.append((i, line.strip()))
    return out


def name_blocks(text, ident):
    """Linhas que mencionam NAME_X + 8 linhas seguintes."""
    lines = text.splitlines()
    chunks = []
    for i, line in enumerate(lines):
        if ident in line:
            chunk = lines[i:i + 12]
            chunks.append((i + 1, "\n".join(chunk)))
    return chunks


def main():
    f1479 = read("frame_1479/DoAction.as")
    f2266 = read("frame_2266/DoAction.as")
    f2258 = read("frame_2258/DoAction.as")
    if not f1479:
        print("sem dump em", SCR)
        return 1
    buf = ["# AS2 combate (gerado, sem PNG)\n"]
    buf.append("## SPRITE_* arrays (ordem Blossom→Shira)\n")
    keys = all_sprite_keys(f1479)
    for k in keys:
        vals = grab_arr(f1479, k)
        buf.append("- `%s` (%d) = %s" % (k, len(vals), ", ".join(vals[:8])))
        print(k, vals[:8])
    buf.append("\n## Constantes 1479\n")
    for a, b in re.findall(
            r"^([A-Z][A-Z0-9_]*)\s*=\s*([^;]+);", f1479, re.M):
        if a.startswith("NAME_"):
            continue
        buf.append("- `%s` = %s" % (a, b.strip()))
    buf.append("\n## Jump / dash / hover / teleport\n")
    pat = r"jump|yspd|airborne|hover|teleport|_dashed|_dashes|KEYINDEX_UP|double"
    for label, text in (("1479", f1479), ("2266", f2266), ("2258", f2258)):
        hits = lines_matching(text, pat)
        buf.append("\n### %s (%d linhas)\n" % (label, len(hits)))
        for n, l in hits[:80]:
            buf.append("    %d: %s" % (n, l[:160]))
    buf.append("\n## Por personagem (2266)\n")
    for ident, pretty in zip(IDS, NAMES):
        buf.append("\n### %s\n" % pretty)
        chunks = name_blocks(f2266, ident)
        buf.append("%d blocos.\n" % len(chunks))
        dmg = re.findall(
            r"%s[\s\S]{0,200}?_damage\s*=\s*([^;]+);" % ident, f2266)
        if dmg:
            buf.append("damage: " + ", ".join(dmg[:12]) + "\n")
        states = re.findall(
            r"%s[\s\S]{0,120}?STATE_([A-Z0-9]+)" % ident, f2266)
        if states:
            buf.append("states: " + ", ".join(states[:20]) + "\n")
        for n, ch in chunks[:6]:
            buf.append("```\nL%d:\n%s\n```\n" % (n, ch[:500]))
    with open(OUT, "w") as f:
        f.write("\n".join(buf) + "\n")
    print("escreveu", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
