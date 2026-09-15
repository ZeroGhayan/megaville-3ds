# Dexter — clip `DefineSprite_1236_Dexter` (646 PNG, **sem crop**)

Labels do combate (`gotoAndPlay("…")`) × flags no próprio clip.
Arte nativa olha à **esquerda**. `face +1` = flip.

Flash 25 fps. 3DS avança 1 frame de clip a cada 0.04 s.

## Onde cada frame vai

| Label 3DS | PNG (dump) | Loop / fim | Flags | Uso |
|---|---:|---|---|---|
| **idle** | **1–48** | 49 → 1 | | parado |
| **land** | **50–57** | 58, 61 → 50 | | aterragem / poeira |
| **jump** | **62–69** | 70 → 62 | | pulo (e queda se não houver fall) |
| **forward** | **71–84** | 85 → 71 | | corrida |
| **dash** | **86–97** | 98 `stop` | | dash |
| **shield** | **98** | stop | | guarda (1 frame, segura) |
| **combo1** | **99–117** | 117 → idle | 108 `_weakHit` 113 `_comboDone` | Y |
| **combo2** | **118–130** | 130 → idle | 121 `_weakHit` 126 `_comboDone` | Y Y |
| **combo3** | **131–151** | 151 → idle | | Y Y Y / Y X / ar (`combo3`) |
| **ranged** | **152–198** | 198 → idle | 164 `_stateChange` 169 `_weakHit` 171–186 `_hitDone` × | special chão (raio) |
| **air** | **199–222** | 222 → idle | 203–210 `_stateChange` 210 `_hitDone` | air ranged / 2.º hit |
| **downatk** | **223–250** | 250 → **jump** | | X no ar |
| **fall** | **251–257** | 257 `stop` | `_stateDone` | queda |
| **fallen** | **258–267** | 267 → 266 | | KO no chão |
| **recover** | **268–278** | 278 → idle | | levantar |
| **win** | **283–295** | 295 → 283 | 329 também → 283 | vitória |
| **damage** | **319** fim→idle; **330–394** | 394 → idle | | hitstun (confirmar no PNG) |
| **extra** | **395–540** `stop` + **541–625** loop 624 | | | não deitar fora — clip extra / intro |

Não há `teleport` / `frozen` / `upper` com label própria neste clip: double-jump reusa **jump**; freeze reusa **damage**; Dexter não tem upper.

## Copiar (sem editar)

No repo:

```
mkdir -p assets/private/dexter_raw/src
cp assets/raw/dump/sprites/DefineSprite_1236_Dexter/*.png \
   assets/private/dexter_raw/src/
python3 tools/pack_dexter_raw.py
cd platforms/3ds && make
```

O packer **não corta**. Se algum PNG for >1024 px (limite da GPU), só *escala* para caber — não é crop. Relatório no terminal: tamanho médio, nº de folhas, VRAM estimada.

## Teste hardware

Story / Versus, **Dexter vs Dexter** (luta 1 do Story do Dexter já é mirror). Dois clips do mesmo atlas. Se gaguejar: o canvas original é o problema, não a lógica.
