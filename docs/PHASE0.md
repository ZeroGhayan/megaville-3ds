# Fase 0 — decisões

Travado em 2026-09-04.

| Item | Valor |
|---|---|
| Repo | `ZeroGhayan/megaville-3ds` |
| Título HOME / CIA | Battle in Megaville 3D |
| Alvo | New 3DS, 400×240 cima, HUD baixo |
| Teste | Lime3DS no Linux, depois hardware |
| Motor | exo-verse (`engine/` + `platforms/3ds/`) |
| P1 | direção + Y fraco + X forte + Start pause |
| Combos | YYY / YYX / YX (iguais para o roster) |
| P2 | CPU escreve o slot P2; versus local depois |
| Dificuldade | a do original → delay/agressão da IA |
| Conteúdo | paridade primeiro |
| Stereo 3DS | só depois do 2D jogável |
| Assets públicos | atlas/JSON processados + placeholder de áudio |
| Assets privados | `assets/raw/`, `assets/private/`, `romfs/audio/licensed/` |

## P2

O esqueleto já tem dois slots. Não precisa de segundo pad no MVP:

```
p1 = hid (Circle Pad + Y/X/Start)
p2 = ai_tick(difficulty) → mesmo ExoInput
```

## Fonte de arte

Não há spritesheet público deste jogo. Fonte = SWF local via JPEXS.
Música de luta no original = *X vs. Zero* (Capcom). Não entra no remoto.
