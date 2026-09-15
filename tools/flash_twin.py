"""Flash twinClrTrans: ra=20 ga=75 ba=100, offsets 0."""
from __future__ import print_function

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


def core_feet(im):
    """Pes = pixel mais baixo com alpha alto no terco central (ignora sombra)."""
    im = im.convert("RGBA")
    bb = im.split()[-1].getbbox()
    if not bb:
        return im.size[0] // 2, im.size[1]
    px = im.load()
    x0 = bb[0] + (bb[2] - bb[0]) * 30 // 100
    x1 = bb[2] - (bb[2] - bb[0]) * 30 // 100
    if x1 <= x0:
        x0, x1 = bb[0], bb[2]
    y = bb[3] - 1
    while y >= bb[1]:
        hits = []
        x = x0
        while x < x1:
            if px[x, y][3] >= 200:
                hits.append(x)
            x += 1
        if hits:
            return (hits[0] + hits[-1]) // 2, y
        y -= 1
    return (bb[0] + bb[2]) // 2, bb[3]
