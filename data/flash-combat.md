# Combate — `frame_1479` + `frame_2258` + `frame_2266`

## Números

```
MAX_DIFFICULTY = 10
MAX_HP = 1000
MIN_HP = -710
AI_BASE_ERROR = 10
NAME_BLOSSOM = 0
STATE_RANGED = 9
```

## Spawn

P1 `(100, 204)` flip; P2 `(300, 204)`.

## Input

DOWN = shield. A = fraco. B = forte. Double-tap = dash.

Cadeia no `_comboDone`: `upper` / `combo2` / `combo3u`.

## Blossom ranged

Bubbles: X = bolha (mais lenta, hitstun, sem gelo).
Buttercup: X = heavy corpo-a-corpo (sem tiro no original).

## CPU

`AI_AGGRESSIVE` no 3DS:
- aproxima a ~52 px
- combo no recovery (65%)
- Blossom longe (>140) = sopro
- dash raro se muito longe
- 18% de hesitar no golpe (eco de `AI_BASE_ERROR`)

DEFENSIVE / RANGED ainda não.
