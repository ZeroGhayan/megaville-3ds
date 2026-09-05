# Corrigir sprites na mão

Pasta local (gitignored): `assets/private/sliced/<char>/`

Arquivos: `idle.png walk.png jump.png dash.png land.png light.png light2.png heavy.png hit.png ko.png`

1. Edita o PNG (sombra, crop, alinhamento visual).
2. `cd platforms/3ds && make` — o Makefile copia de novo para `gfx/`.

Offsets extra no código (`sprites.c`, `g_offx` / `g_offy`): um par por personagem, em pixels, depois do crop.

O crop automático (`tools/crop_slice.py`) é só o primeiro corte. Não rode de novo em cima do PNG já limpo — ele lê o dump cru.
