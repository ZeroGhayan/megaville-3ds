# Dexter — labels de `DefineSprite_1236_Dexter`

Fonte: 42 `DoAction.as` do clip + `gotoAndPlay("…")` em `frame_2266`.
Lutador = **só este clip**. Filhos anónimos (`1163`, `1235`, …) já vão achatados nas 646 PNG.

Canvas JPEXS: **384×153** (personagem pequeno no palco). Arte olha à **esquerda**.

Flash: o label está no **primeiro** frame; o script está no **último**.

| Label Flash | PNG | Script | O que o AS2 faz |
|---|---:|---|---|
| `idle` | 1–48 | 49 → play(1) | loop parado (6 poses, hold 2) |
| `land` | 50–57 | 58 e 61 → play(50) | aterrar |
| `jump` | 62–69 | 70 → play(62) | pulo |
| `forward` | 71–84 | 85 → play(71) | correr |
| `dash` | 86–97 | (cai em 98 se não cortar) | dash |
| `shield` | 98 | **stop** | guarda (1 frame) |
| `combo1` | 99–117 | 108 `_weakHit` · 113 `_comboDone` · 117 done→idle | Y |
| `combo2` | 118–130 | 121 `_weakHit` · 126 `_comboDone` · 130 done | YY |
| `combo3` | 131–151 | 151 done→idle | YYY **e** light aéreo do Dexter |
| `ranged` | 152–198 | 169–186 hits do projétil · 198 done | special chão |
| `airranged` | 199–222 | 203–210 `_stateChange` · 222 done | 2.º special aéreo |
| `downatk` | 223–250 | 250 done→**jump (62)** | X aéreo; recupera no ar |
| `fall` | 251–257 | 257 done+stop | queda |
| `fallen` | 258–267 | 267 → play(266) | deitado (loop) |
| `recover` | 268–278 | 278 done→idle | levantar |
| `win` | 283–295 | 295 → play(283) | vitória |
| `teleport` | 296–319 | 319 done→idle | **double jump** do Dexter |
| *(ponte)* | 320–329 | 329 done→**win** | transição p/ vitória |
| `damage` | 330–394 | 394 done→idle | hitstun |
| extra | 395–625 | 540 stop · 625 → 624 | `upper` / `combo3u` / `frozen` / `shielddamage` / `vulnerable` — ver visual |

Únicas no pai: **280 / 646**. Holds de 2 frames = 15 fps desenhado em filme a 30.

## Combate (frame_2266) — Dexter

- Light aéreo → `gotoAndPlay("combo3")` (não `airranged`)
- Double jump → `gotoAndPlay("teleport")`
- Special chão → `"ranged"`; no ar → `"airranged"` depois `STATE_AIRRANGED2`
- X aéreo → `"downatk"` (knockdown, `_atkdirY = DOWN`, dmg 12)
- Combo1 Y dmg 5; combo3 dmg 14 (juggle UP); ranged dmg 6

`upper` e `combo3u` são rotas de combo de **todos** os personagens (Y cancel). Estão no bloco extra 395+ até confirmarmos o frame exacto.

## Packer

```
python3 tools/pack_dexter_raw.py
```
