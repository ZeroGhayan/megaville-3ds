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

## 8. Matrizes PlaceObject (origem Flash)

A qualidade LOW/MEDIUM/HIGH **não** mexe a posição (confirmado no Ruffle).
O (0,0) do lutador está no SWF:

```bash
python3 tools/swf_place.py
git add data/place-map.md && git commit -m "place map" && git push
```

`tx,ty` = offset de cada filho em px. Origem do PNG JPEXS ≈ `(−min tx, −min ty)`
quando o filho se estende para coordenadas negativas (pés no (0,0), corpo em y<0).


## 4. Áudio

Faixa de luta (*X vs. Zero*) → `romfs/audio/licensed/` (ignorado).
SFX originais também. Placeholder fica no Git.

## 5. Quando os PNG “entram” no jogo

O `make` corre `tools/pack_chars.py` (8 lutadores) e `tools/pack_stage.py` (chão, cidade, poste).

```bash
cd platforms/3ds && make
```

Poses → `gfx/<char>.t3s`. Clips → `gfx/<char>_<anim>_N.t3x` (só os 2 da luta são carregados). Ver `data/asset-map.md`.

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

