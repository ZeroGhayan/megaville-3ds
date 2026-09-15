"""Flash twinClrTrans + limpeza de canvas preto opaco."""
from __future__ import print_function

try:
    from collections import deque
except ImportError:
    deque = list


def twin_mul(im):
    src = im.convert("RGBA")
    px = src.load()
    w, h = src.size
    out = src.copy()
    op = out.load()
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            op[x, y] = ((r * 20) // 100, (g * 75) // 100, b, a)
    return out


def knockout_border(im, thresh=22):
    """Preto ligado à borda → alpha 0. Não fura pretos interiores (óculos)."""
    im = im.convert("RGBA")
    w, h = im.size
    px = im.load()
    q = deque()
    seen = bytearray(w * h)

    def dark(x, y):
        r, g, b, a = px[x, y]
        return a > 0 and r <= thresh and g <= thresh and b <= thresh

    for x in range(w):
        q.append((x, 0))
        q.append((x, h - 1))
    for y in range(h):
        q.append((0, y))
        q.append((w - 1, y))
    while q:
        x, y = q.popleft() if hasattr(q, "popleft") else q.pop(0)
        if x < 0 or y < 0 or x >= w or y >= h:
            continue
        i = y * w + x
        if seen[i]:
            continue
        seen[i] = 1
        if not dark(x, y):
            continue
        r, g, b, a = px[x, y]
        px[x, y] = (r, g, b, 0)
        q.append((x + 1, y))
        q.append((x - 1, y))
        q.append((x, y + 1))
        q.append((x, y - 1))
    return im


def trim_alpha(im, pad=1):
    bb = im.split()[-1].getbbox()
    if not bb:
        return im, 0, 0
    l, t, r, b = bb
    l = max(0, l - pad)
    t = max(0, t - pad)
    r = min(im.size[0], r + pad)
    b = min(im.size[1], b + pad)
    return im.crop((l, t, r, b)), l, t


def core_feet(im):
    """Pes = fundo do nucleo (ignora elipse de sombra larga)."""
    im = im.convert("RGBA")
    bb = im.split()[-1].getbbox()
    if not bb:
        return im.size[0] // 2, im.size[1]
    px = im.load()
    x0, y0, x1, y1 = bb
    widths = []
    y = y0
    maxw = 1
    while y < y1:
        n = 0
        x = x0
        while x < x1:
            if px[x, y][3] >= 32:
                n += 1
            x += 1
        widths.append(n)
        if n > maxw:
            maxw = n
        y += 1
    cx0 = x0 + (x1 - x0) * 30 // 100
    cx1 = x1 - (x1 - x0) * 30 // 100
    if cx1 <= cx0:
        cx0, cx1 = x0, x1
    y = y1 - 1
    body = y0 + (y1 - y0) * 55 // 100
    while y >= y0:
        roww = widths[y - y0]
        if y > body and roww > max(20, maxw * 55 // 100):
            y -= 1
            continue
        hits = []
        x = cx0
        while x < cx1:
            if px[x, y][3] >= 200:
                hits.append(x)
            x += 1
        if hits:
            return (hits[0] + hits[-1]) // 2, y
        y -= 1
    return (x0 + x1) // 2, y1
