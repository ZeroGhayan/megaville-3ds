# Story — `frame_2148`, `2248–2310`, `2332`

CPU: `spriteName[1] = SPRITE_OPPONENTS[P1][_storylevel]`
Tint ciano **só** se `P1 == CPU` (`twinClrTrans`). Não é “luta 3 = mirror” para todos.

## Lutas 1–8 e self-mirror (P1==CPU)

| P1 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|
| Blossom | Bubbles | Buttercup | **Blossom** | Dexter | Rowdy | Zim | Bell | Shira |
| Bubbles | Buttercup | Blossom | **Bubbles** | Dexter | Rowdy | Zim | Bell | Shira |
| Buttercup | Blossom | Bubbles | **Buttercup** | Dexter | Rowdy | Zim | Bell | Shira |
| Bell | Blossom | Bubbles | Buttercup | Dexter | Rowdy | Zim | **Bell** | Shira |
| Shira | Blossom | Bubbles | Buttercup | Dexter | Rowdy | Zim | Bell | **Shira** |
| Dexter | **Dexter** | Bubbles | Buttercup | Blossom | Rowdy | Zim | Bell | Shira |
| Rowdy | **Rowdy** | Bubbles | Buttercup | Blossom | Dexter | Zim | Bell | Shira |

Excepções vs “sempre luta 3”:
- Rowdy / Dexter: mirror na **1**
- Bell: mirror na **7**; Shira na **8**
- Dexter 2–4 no AS2 são as três meninas, **não** clones dele
- Blossom/Bubbles luta 4 no AS2 é **Dexter**, não segundo clone

Zim: sem tabela no Flash; select de Story no 3DS omite-o.

## Comprimento / continue
Diff ≤ 5: 7 lutas (acaba na Bell). Diff ≥ 6: 8 (Shira).
Vitória: `_storylevel++`, **não** volta à select.
Derrota: CONTINUE. A = mesmo level, `_continue=true`, `_numcontinues++`. B = desiste.
Clear em diff < 6 sem continue → unlock Zim-survival.
