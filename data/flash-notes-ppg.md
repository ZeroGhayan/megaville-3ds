# Meninas no `frame_2266`

## Blossom
- `STATE_RANGED` = sopro
- 3DS: gelo + dano

## Bubbles
- Spawn `projectile` em `_x ± 20`, `_y` da sprite
- `_yspd = 0` no chão; `_yspd = 5` (×2 se `_fast`) noutro ramo
Dano Flash:

```
BUTCH_DAMAGE = 0.8
BOOMER_DAMAGE = 2
Bubbles projectile._damage = 3
dmg = _damage * BASE_DAMAGE     (ou /5 noutro ramo)
```

3DS assume `BASE_DAMAGE = 30` até ler o valor:
Bubbles 90, Butch 24, sopro 90+gelo.

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
