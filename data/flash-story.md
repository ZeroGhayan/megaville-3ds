# Story — `frame_2148`, `2248–2310`, `2332`

## Entrada
Story + 1P → dificuldade → select (`_storylevel = 0`).
Primeira confirmação (`_storylevel == 0`) vai a `1624` (intro); as outras, a `2148`.

CPU: `spriteName[1] = SPRITE_OPPONENTS[P1][_storylevel]`

## Rosters (`NUMOF_OPPONENTS = 8`)

Índice 2 é **mirror** (mesmo personagem, tint `twinClrTrans`).

| P1 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---|---|---|---|---|---|---|---|
| Blossom | Bubbles | Buttercup | Blossom | Dexter | Rowdy | Zim | Bell | Shira |
| Bubbles | Buttercup | Blossom | Bubbles | Dexter | Rowdy | Zim | Bell | Shira |
| Buttercup | Blossom | Bubbles | Buttercup | Dexter | Rowdy | Zim | Bell | Shira |
| Bell / Shira | Blossom | Bubbles | Buttercup | Dexter | Rowdy | Zim | Bell | Shira |
| Dexter | Dexter | Bubbles | Buttercup | Blossom | Rowdy | Zim | Bell | Shira |
| Rowdy | Rowdy | Bubbles | Buttercup | Blossom | Dexter | Zim | Bell | Shira |
| Zim | *(sem tabela)* | | | | | | | |

## Comprimento
Vitória: `_storylevel++`. Acaba se:
- `level >= 8` **ou**
- `level >= 7` **e** `_difficulty <= 5`

Diff 1–5: **7 lutas** (para em Bell, sem Shira).
Diff 6–10: **8 lutas** (Shira no fim).

## Entre lutas
- Antes: clip `stories` `"P1-P2"`; se CPU = Shira, label `"shira"`.
- Depois da vitória: `"P1-P2-end"`.
- Derrota: `2352` continue. Continue **não** incrementa level (`_continue = true`, `_numcontinues++`, replay `2135`).

## Finais (`2332`)
Se nunca continuou, diff < 6 e Zim-survival ainda locked → `4644` (unlock).
Senão diff < 6 → `2777`; diff ≥ 6 → `2772` (stats em `2774`).
