# Build no VSCodium + Lime3DS

Pasta local com o mesmo nome do repo: clone **dentro** dela, ou apague
o conteúdo vazio e clone no lugar.

```bash
cd ~/caminho/pai
rmdir megaville-3ds   # só se a pasta estiver vazia
git clone https://github.com/ZeroGhayan/megaville-3ds.git
cd megaville-3ds
```

Se a pasta já existe e não está vazia:

```bash
cd megaville-3ds
git init
git remote add origin https://github.com/ZeroGhayan/megaville-3ds.git
git fetch origin
git checkout -B main origin/main
```

## Ambiente (uma vez)

devkitPro + pacotes 3DS. No Debian:

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=$DEVKITPRO/devkitARM
export PATH="$DEVKITPRO/tools/bin:$DEVKITARM/bin:$PATH"
```

Pode ir no `~/.bashrc` e no terminal integrado do VSCodium
(Ctrl+`). Confira: `echo $DEVKITARM` não pode sair vazio.

## Compilar

```bash
cd platforms/3ds
make
```

Saída: `platforms/3ds/megaville.3dsx` (+ `.elf` / `.smdh`).

Limpar: `make clean`.

## Lime3DS — hello atual

| Controle | Efeito |
|---|---|
| Circle Pad / D-Pad | mesma coisa: anda, pula, guarda |
| JUMP (cima, ou A se ligado) | caixa sobe |
| GUARD / LIGHT / HEAVY | muda a cor |
| Start (PAUSE) | pausa |
| **Select** | menu de controles (fixo) |
| HOME | sai |

No menu: D-Pad sobe/desce, **A** edita ou liga opção, **B** volta.
Na captura, aperte o botão novo; Select cancela. Grava em
`sdmc:/3ds/megaville/binds.cfg`.


## SWF e áudio privado

Depois do clone:

```bash
mkdir -p assets/raw assets/private romfs/audio/licensed
cp /caminho/do/jogo.swf assets/raw/megaville.swf
```

Esses caminhos já estão no `.gitignore`.

## Próximo código

Hello só prova toolchain + input. Combate do exo-verse entra depois.
