# Blossom — mapa do clip 471

Máquina de estados no *timeline*, não em funções.

Flags:

| Flag | Significado |
|---|---|
| `_weakHit` | frame ativo do golpe fraco |
| `_comboDone` | janela de cancel / próximo hit da cadeia |
| `_stateDone` | animação acabou; `gotoAndPlay` diz para onde volta |
| `_stateChange` | troca de pose no meio do estado (dash/air) |
| `_hitDone` | impacto já aplicado |
| `stop()` | segura o último frame (hitstun / KO / win) |

## Faixas

| Frames | Script no fim | Volta para | Leitura |
|---:|---|---|---|
| 1–49 | `gotoAndPlay(1)` | idle | **idle** |
| 50–58 | `gotoAndPlay(50)` | 50 | **walk** |
| 59–61 | `gotoAndPlay(50)` | walk | walk extra / parada |
| 62–70 | `gotoAndPlay(62)` | 62 | **jump / air idle** (golpes aéreos voltam aqui) |
| 71–85 | `gotoAndPlay(71)` | 71 | queda ou guarda |
| 86–94 | `stop()` | — | hitstun ou KO curto |
| 108–117 | weak → comboDone → idle | 1 | **L1** |
| 121–130 | weak → comboDone → idle | 1 | **L2** |
| 155 | done → 62 | air | fim de algo aéreo |
| 156–158 | `_stateChange` | — | transição |
| 179 | done → 62 | air | outro aéreo |
| 223 | done → idle | 1 | recovery |
| 226–231 | weak + `_hitDone` | — | L3 / especial com hit |
| 243 | done → idle | 1 | fim desse golpe |
| 270 | done → 62 | air | |
| 277 | done + `stop` | — | hold |
| 286–287 | loop 286 | — | pose parada |
| 298 | done → idle | 1 | |
| 303–315 | loop 303 | — | win / charge |
| 318–321 | `_stateChange` | — | |
| 336 | done → 62 | air | |
| 354 | done → 303 | win loop | |
| 419 | done → idle | 1 | |
| 565 | `stop()` | — | pose final |

Cadeia no chão, igual ao `fight.c`:

```
L     108–117
LL    108–113  depois  121–130
(o terceiro hit da Blossom não tem o trio weak/combo/done;
 provavelmente 226–243)
```

H e especiais estão nos buracos sem script (95–107, 131–154, 180–222…).
O desenho muda; a flag só aparece no frame de hit.

## Conferir em 30 s

Abra no visualizador, **um** PNG de cada:

`sprites/DefineSprite_471_Blossom/{1,50,62,71,86,108,121,226}.png`

Se 1 = parado, 50 = andando, 62 = no ar, 108 = soco, o mapa está certo.
