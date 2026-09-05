# Assets privados (gitignore)

Sim: dá para ter a build **com** a faixa da Capcom na sua máquina e o
GitHub **sem** ela. O mesmo vale para o `.swf`, dump JPEXS e sprites
extraídos.

## Como funciona

O [`.gitignore`](../.gitignore) da raiz ignora:

| Caminho | O que você coloca lá |
|---|---|
| `assets/raw/` | `.swf`, dumps JPEXS |
| `assets/private/` | qualquer IP de terceiro |
| `romfs/audio/licensed/` | OST / SFX que não podem ir ao remoto |
| `*.swf` | em qualquer pasta |

O Git **não vê** esses arquivos. `git add -A` não os sobe.
Quem clonar o repo público recebe o placeholder em
`romfs/audio/placeholder/`.

## Fluxo local

1. Copie o SWF para `assets/raw/megaville.swf` (ignorado).
2. Extraia com JPEXS para `assets/raw/dump/` (ignorado).
3. Se quiser a faixa original na build: ponha o WAV/MP3 em
   `romfs/audio/licensed/battle.wav` (ignorado).
4. O jogo tenta `licensed/` primeiro; se não existir, cai no placeholder.

## Extra, só nesta máquina

Coisas que você não quer nem no `.gitignore` compartilhado:

```
# .git/info/exclude  (não é commitado)
minhas-notas.txt
```

## O que NÃO fazer

- Não use `git add -f` nesses caminhos.
- Não commite o SWF “só para eu ter backup” — use disco/pendrive.
- Não troque o `.gitignore` para “liberar um arquivo só desta vez”.
- Se um arquivo privado já foi commitado uma vez, ignorar depois **não
  apaga o histórico**. Aí precisa `git rm --cached` + commit. Ainda não
  é o caso deste repo.

## Build pública vs. privada

O loader deve procurar um nome estável (`romfs/audio/battle.wav`).
Na sua cópia você escolhe o ficheiro que vive nesse caminho.
No remoto esse ficheiro não existe; o código usa o placeholder.
