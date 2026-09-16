# Extração do SWF (na sua máquina)

O dump **não** entra no GitHub. Pasta ignorada: `assets/raw/`.

## 1. Colocar o SWF

```bash
mkdir -p assets/raw
cp /caminho/BattleInMegaville.swf assets/raw/megaville.swf
```

## 2. JPEXS (GUI ou CLI)

[JPEXS FFDec](https://github.com/jindrapetrik/jpexs-decompiler/releases)

GUI: File → Open → Export → export all (image, sprite, sound, script).

CLI:

```bash
java -jar ffdec.jar -export image,sprite,sound,script,frame \
  assets/raw/dump \
  assets/raw/megaville.swf
```

Saída típica:

```
assets/raw/dump/images/
assets/raw/dump/sprites/
assets/raw/dump/sounds/
assets/raw/dump/scripts/
```

## 3. O que fazer com o dump

Não `git add` isso. Em vez disso gere um **inventário** versionado:

```bash
find assets/raw/dump -type f | sort > data/extract-inventory.txt
```

Esse `.txt` (só caminhos e nomes) pode ir no repo. Eu uso os nomes
para ligar `idle / walk / jump / guard / light / heavy / hit`.

## 7. Mapear clips anónimos (obrigatório antes das animações finais)

**Não** subas `sprites/` ao GitHub, nem “temporariamente”. O histórico
fica para sempre (e o dump é IP da Cartoon Network).

O que entra no git é só o relatório:

```bash
python3 tools/map_clips.py
git add data/clip-map.md
```

O script compara cada `DefineSprite_NNNN` anónimo com o pai nomeado
(Blossom, Dexter, …) por hash do conteúdo (ignora canvas preto).

- `EMBEDDED` — a tira já está no pai; ignora a pasta
- `FROZEN` — o pai só fotografou o frame 1; usamos o filho
- `PARTIAL` / `UNUSED` — ver o span e decidir

Se o `.md` for grande demais para colar no chat, faz `git push` **só
desse ficheiro**.

## 4. Áudio

Faixa de luta (*X vs. Zero*) → `romfs/audio/licensed/` (ignorado).
SFX originais também. Placeholder fica no Git.

## 5. Quando os PNG “entram” no jogo

Depois do inventário: você escolhe frames, eu monto o atlas
(`tex3ds`) e o código aponta para `idle_0`, `light_1`, etc.

## 6. Recortar o slice (local)

```bash
pip3 install --user Pillow
python3 tools/crop_slice.py
```

Saída (gitignored):

```
assets/private/sliced/blossom/idle.png
assets/private/sliced/blossom/walk.png
...
assets/private/sliced/bubbles/
assets/private/sliced/buttercup/
```

`--char blossom` faz só uma. Conferir: os PNG devem estar sem faixa preta.

