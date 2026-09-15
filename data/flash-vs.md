# VS e Continue — Flash `2159` / `2352`–`2362`

## VS (`pic0`/`pic1`, frame 2159)
- Fundo preto, faixa azul (`pic` frames = `spriteName+1`)
- Nome P1 à esquerda, P2 à direita, **VS** ao centro
- Espera edge de A/B (`_wait` + `keyWasDown`) → `2229` fade → luta
- 3DS: **só ecrã de cima**. Baixo só “Y/X/A fight”.
- Retrato: idle slice a 2.6× até haver `assets/private/pics/*.png`

## Continue (`2352`)
- Retrato do P1 + `CONTINUE?` + dígito **9→0** (~1 s / número, ~25 fps original)
- Dois botões: Continue (`1721` → mesmo fight via `2135`) / Back to menu (`1725` → `1479`)
- Timeout → `2362` **GAME OVER** (retrato verde)
- 3DS: **cima** = arte + contagem; **baixo** = as duas opções
