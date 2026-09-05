# megaville-3ds

Homebrew **não oficial** para New Nintendo 3DS inspirado no lutador Flash
*PPGD: Battle in Megaville* (arayh, 2006).

Título no menu HOME: **Battle in Megaville 3D**
(o “3D” é o console; estereoscopia vem depois do jogo 2D fechar).

Camada de plataforma copiada de
[ZeroGhayan/exo-verse](https://github.com/ZeroGhayan/exo-verse)
(`engine/` + input/render 3DS).

## Status

Hello 3DS: duas telas, caixa móvel no pad, Y/X/Start no HUD.
Ainda não é o lutador.

Build local: [`docs/BUILD_VSCODIUM.md`](docs/BUILD_VSCODIUM.md).

## O que este repo commita

- Código, Makefile, JSON de moves, atlas **originais ou gerados por nós**
- Áudio **placeholder** em `romfs/audio/placeholder/`

## O que **não** vai para o GitHub

SWF original, dump JPEXS, sprites extraídos, OST da Capcom.
Ver [`docs/PRIVATE_ASSETS.md`](docs/PRIVATE_ASSETS.md).

## Mapa de input (Fase 0)

| Ação | 3DS |
|---|---|
| Andar / pular / agachar | Circle Pad + D-Pad |
| Golpe fraco | **Y** |
| Golpe forte | **X** |
| Pause | **Start** |
| Combos (todos iguais) | Y,Y,Y · Y,Y,X · Y,X |
| Player 2 | CPU no mesmo struct de input |

```bash
cd platforms/3ds
make
```
