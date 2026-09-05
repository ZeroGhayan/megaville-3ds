# megaville-3ds

Homebrew **não oficial** para New Nintendo 3DS inspirado no lutador Flash
*PPGD: Battle in Megaville* (arayh, 2006).

Título no menu HOME: **Battle in Megaville 3D**
(o “3D” é o console; estereoscopia vem depois do jogo 2D fechar).

Motor de referência: [ZeroGhayan/exo-verse](https://github.com/ZeroGhayan/exo-verse)
(`engine/` + `platforms/3ds/`).

## O que este repo commita

- Código, Makefile, JSON de moves, atlas **originais ou gerados por nós**
- Áudio **placeholder** em `romfs/audio/placeholder/`

## O que **não** vai para o GitHub

SWF original, dump JPEXS, sprites extraídos, OST da Capcom
(*Mega Man X5 — X vs. Zero*). Isso fica em pastas ignoradas — ver
[`docs/PRIVATE_ASSETS.md`](docs/PRIVATE_ASSETS.md).

## Mapa de input (Fase 0)

| Ação | 3DS |
|---|---|
| Andar / pular / agachar | Circle Pad + D-Pad |
| Golpe fraco | **Y** |
| Golpe forte | **X** |
| Pause | **Start** |
| Combos (todos iguais) | Y,Y,Y · Y,Y,X · Y,X |
| Player 2 | CPU no mesmo struct de input |

## Build (na sua máquina com devkitPro)

```bash
cd platforms/3ds
make
```

Gera `.3dsx` para Homebrew Launcher / Lime3DS.

## Status

Fase 0 travada. Próximo: hello 3DS (duas telas + pad) e extração local do SWF.
