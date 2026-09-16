# Mapa de assets Flash → 3DS

Fonte: dump JPEXS (não vai ao GitHub). Packer: `python3 tools/pack_chars.py`.

## Lutadores (um clip = todas as animações)

| CH | Pasta | PNG | Labels | Origem PNG |
|---|---|---:|---|---|
| Blossom | `DefineSprite_471_Blossom` | 605 | PPG (idle 1–48 …) | 296, 47 |
| Bubbles | `DefineSprite_563_Bubbles` | 585 | PPG | 296, 44 |
| Buttercup | `DefineSprite_690_Buttercup` | 617 | PPG | 327, 45 |
| Bell | `DefineSprite_370_Bell` | 617 | PPG | 367, 335 |
| Dexter | `DefineSprite_1236_Dexter` | 646 | conferido | 296, 132 |
| Rowdy | `DefineSprite_1399_Rowdyruff` | 485 | PPG | 306, 147 |
| Zim | `DefineSprite_1334_zim` | 1001 | Dexter-like + extra | 447, 405 |
| Shira | `DefineSprite_1469_Shira Bell` | 617 | PPG | 400, 337 |

`id 248` = sombra (−37, 13.2) em todos. Corpo no depth 4.

Labels iguais ao AS2 (`gotoAndPlay("idle")` …). Família PPG partilha os keyframes `49 58 61 70 85 94 108 113 117 121 126 130`.

**Poses** (atlas `gfx/<char>.t3s`, 10 frames): idle land jump walk dash heavy light light2 hit ko.

**Clip** (lazy, só P1/P2): `gfx/<char>_<anim>_N.t3x` gerado no `make`.

Filhos anónimos (1163, 102, …) já vêm achatados no PNG do pai — ignorar.

## Palco / HUD

| Pasta | Uso |
|---|---|
| `DefineSprite_1087_terrain0000` | chão (9 frames) |
| `DefineSprite_1086` | cidade / céu (370 PNG — frame 1) |
| `DefineSprite_1089_opaque0000` | fundo opaco |
| `DefineSprite_1446_streetlamp` | poste |
| `DefineSprite_1115_hpbar` + `1123_hpbarframe` | vida |
| `DefineSprite_1102_dashbarframe` | dash (4) |
| `DefineSprite_778_readyfight` | GET READY / FIGHT |
| `DefineSprite_751_combo` | combo popup |
| `DefineSprite_738_projectile` | projéteis (327) |
| `DefineSprite_759_hit` / `760_hitwin` | hit spark |
| `DefineSprite_1045_stories` | **não** (1201, modo história) |

## VRAM

Altura alvo 72 px. Clip só carrega os 2 lutadores da luta. Twin do clip = tint em runtime (sliced ainda tem folha `_t`).
