# Dash — copiado do AS2 (`frame_2266` / `frame_2258`)

```
_dashes = 300          // 3 cargas
custo   = 100
recarga = +1 por frame Flash (~30/s)
gatilho = double-tap LEFT ou RIGHT
          && !_dashed && _dashes >= 100
          && estado idle/run/shield/recover
_dashFuel = SPRITE_MAXDASHFUEL  // 10 nas meninas
_dashed = true até soltar o eixo
Zim não dasha; Dexter não dasha no ar
```

Barra Flash: 3 pips (`>=100`, `>=200`, `>=300`).
`dashbar._xscale = 100 - _dashes/3` é o recorte invertido do clip; no 3DS os pips bastam.

Cheat `_infdash` existe no original — não portado.
