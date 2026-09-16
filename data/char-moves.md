# Jogabilidade por personagem (AS2)

Cadeia **igual para todos**: `Y Y Y` · `Y Y X` · `Y X`.
Y no idle = `combo1`. X no idle = `STATE_RANGED` (special), não “soco forte”.

`BASE_DAMAGE = 15`. HP 1000. Hitbox lógica 20×36. Spawn 100 / 300, pés y=204.

| | Run | Dash fuel | X (idle) | Aéreo | Dano sofrido |
|---|---:|---:|---|---|---|
| Blossom | 8 | 10 | sopro gelo (`_damage` 6 → 90 + freeze) | — | ×1 |
| Bubbles | 9 | 12 | bolha (`3` → 45, hitstun) | — | ×1 |
| Buttercup | 8 | 10 | Butch chão (`0.8` → 12) | — | ×1 |
| Bell | 8 | 10 | boomer (`2` → 30) | — | ×1 |
| Dexter | 7 | 8 | beam (`6` → 90, sem gelo) | double-jump `teleport`; Y aéreo = combo3; X aéreo = airranged | ×1 |
| Rowdy | 8 | 10 | boomer (`2` → 30) | — | ×1 |
| Zim | 8 | **0** (não dasha) | melee | não joga `"damage"` | ×0.8, armour 5 |
| Shira | **14** | 10 | boomer (`2` → 30) | — | ×2 |

Combo Y: `_damage` 5 → 75. YYY: 14 → 210 (juggle). Dash custo 100 / max 300. Zim não dasha; Dexter não dasha no ar.

CPU: P2 ligado (desliga com dual control).
