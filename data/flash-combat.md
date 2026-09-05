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

No 3DS: X no idle/walk = beam (`STATE_RANGED`). Um tiro por vez.
Hit = dano + `STATE_FROZEN` (~70 frames). Guarda não congela.
Bubbles/Buttercup no X = heavy corpo-a-corpo.
Beam atual é retângulo ciano; PNG 211 entra depois.

## CPU

`AI_AGGRESSIVE` no 3DS:
- aproxima a ~52 px
- combo no recovery (65%)
- Blossom longe (>140) = sopro
- dash raro se muito longe
- 18% de hesitar no golpe (eco de `AI_BASE_ERROR`)

DEFENSIVE / RANGED ainda não.
