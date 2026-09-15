# Dexter — clip `DefineSprite_1236_Dexter` (646 PNG)

Lutador = **só este clip**. À parte: `738_projectile`, `751_combo`, `759_hit`.
IDs 1254–1320 são pedaços **dentro** do 1236 (já nas PNG se o JPEXS flatten).

Arte olha à **esquerda**. Flash: `_x/_y` = pés (registo), `_xscale ±100` vira nesse ponto.
Palco 400 px = 3DS 1:1. Hitbox 20×36.

| Label | PNG | Uso |
|---|---:|---|
| idle | 1–48 | parado |
| land | 50–57 | aterrar |
| jump | 62–69 | pulo |
| forward | 71–84 | correr |
| dash | 86–97 | dash |
| shield | 98 | guarda |
| combo1 | 99–117 | Y (108 hit) |
| combo2 | 118–130 | YY |
| combo3 | 131–151 | YYY / ar |
| ranged | 152–198 | special chão |
| air | 199–222 | 2.º hit ar |
| downatk | 223–250 | X ar |
| fall / fallen / recover | 251–278 | queda / KO / levantar |
| win | 283–295 | vitória |
| damage | 319–394 | hitstun |
| extra | 395–625 | resto (não preload) |

Packer recorta **só transparente** e guarda (ox,oy) dos pés no canvas do idle — motion baked mantém-se se o canvas for o mesmo tamanho.

Twin Flash: `ra=20 ga=75 ba=100` (multiply), **não** overlay ciano. Game Over usa o mesmo.

```
python3 tools/pack_dexter_raw.py
python3 tools/pack_pics.py
cd platforms/3ds && make
```
