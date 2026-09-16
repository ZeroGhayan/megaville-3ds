# Por que “imitar o script” não alinha a arte

Os `.as` que temos (**2266**, **1479**, **2258**, DoAction dos clips) são o **jogo**.
A **pose dentro do PNG** não está lá.

## O que os scripts dizem (e já usamos)

| Flash | Valor | No port |
|---|---|---|
| palco | 400 × 250 (`SCREENSIZE_HALF*`) | 400 × 240 |
| spawn | `_x=100/300`, `_y=204` | x=100/300 como **canto** da hitbox |
| `_xscale` P1/P2 | −100 / +100 | `face` + `scaleX` |
| corpo lógico | `SPRITE_WIDTH=20` `HEIGHT=36` | 28×40 |
| gravidade | `_yspd++` por frame (30 fps) | 900 px/s² contínuo |
| `_quality` | `"HIGH"` (1479:952) | — |

`_x,_y` no Flash é o **ponto de transformação** do movieclip (pés/origem do símbolo), não o canto do PNG.

## O que **não** está nos scripts

1. **Matriz PlaceObject** de cada filho, em cada frame — offset de braço, rastro, sombra. Vive no SWF. O flatten do JPEXS é que deveria cozer isto no PNG 384×153.
2. **Labels** `upper`, `combo3u`, `frozen`, `vulnerable`, `shielddamage` — o combate chama-os; no clip do Dexter caem no buraco 395–625.
3. **`sprite.atk`** — hitbox é um clip filho com `hitTest`, não o contorno da arte.

## Porque a animação “parece errada” mesmo com o mapa de frames

- PNG já traz holds de 2 frames (filme a 30, desenho a ~15). Nós avançamos a 25 fps → holds **em cima** de holds.
- Origem partilhada = `core_feet` no idle, não a origem Flash no canvas 384×153. Se o (0,0) do símbolo não for esse pé, **todos** os frames saem deslocados.
- `scaleX = -1` em atlas citro2d já partiu o P2 no VS; nas folhas `dex_*` o P1 (vira à direita) pode estar a amostrar UV errado.
- Só o Dexter usa o clip de 646 PNG. Os outros 7 ainda são 8 fatias.

## Qualidade — descartada

Ruffle LOW / MEDIUM / HIGH: Blossom e Dexter nos **mesmos** píxeis.
`_quality = HIGH` só muda anti-alias. Próximo: `tools/swf_place.py`.

