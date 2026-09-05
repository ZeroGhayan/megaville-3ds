# Meninas no `frame_2266`

## Blossom
- `STATE_RANGED` = sopro
- 3DS: gelo + dano

## Bubbles
- Spawn `projectile` em `_x ± 20`, `_y` da sprite
- `_yspd = 0` no chão; `_yspd = 5` (×2 se `_fast`) noutro ramo
Dano Flash:

```
DEFAULT_BASE_DAMAGE = 15   // frame_254
BASE_DAMAGE = 15 * _damagemult
Bubbles  _damage = 3   →  45  (ou 9 se /5)
Butch    0.8           →  12
Boomer   2             →  30
Sopro    thisSprite._damage * 15  (chute: _damage=6 → 90)
```

O ramo `/5` é tick; o impacto usa `* BASE_DAMAGE`.


- Não congela

## Buttercup
- `gotoAndPlay("butch")` no chão (`_y` ≈ 200 + terrain)
- `BUTCH_DAMAGE` (constante ainda não lida)
- 3DS: projétil baixo, verde, hitstun

## Ainda falta
```
grep -n 'BUTCH_DAMAGE\|BOOMER_DAMAGE\|BLOSSOM.*_damage\|projectile._xspd' \
  assets/raw/dump/scripts/frame_1479/DoAction.as \
  assets/raw/dump/scripts/frame_2266/DoAction.as
sed -n '460,580p' assets/raw/dump/scripts/frame_2266/DoAction.as
```
