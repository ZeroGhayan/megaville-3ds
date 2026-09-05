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

Anote no inventário, se vir:

- nome do personagem no símbolo
- quantos frames por animação
- tamanho aproximado do sprite (px)
- se o palco é bitmap ou vector

## 4. Áudio

Faixa de luta (*X vs. Zero*) → `romfs/audio/licensed/` (ignorado).
SFX originais também. Placeholder fica no Git.

## 5. Quando os PNG “entram” no jogo

Depois do inventário: você escolhe frames, eu monto o atlas
(`tex3ds`) e o código aponta para `idle_0`, `light_1`, etc.

Até lá o hello desenha caixas com o nome da ação.
