# Blossom — mapa do clip 471 (conferido nos PNG)

Export JPEXS = canvas enorme + personagem pequeno à direita.
Antes do atlas: crop no bounding box (pixels ≠ preto).

## O que os 8 frames *são*

| PNG | Pose | Rótulo antigo | Rótulo certo |
|---|---|---|---|
| 1 | em pé, braços baixos | idle | **idle** |
| 50 | agachada + anel de poeira no chão | walk | **land / crouch** |
| 62 | subindo, cabelo para baixo | jump | **jump** |
| 71 | em pé, cabelo para a direita | queda/guarda | **walk** |
| 86 | voo horizontal + rastro branco | hitstun | **dash / fly** |
| 108 | agachada + anel (parecido com 50) | L1 soco | **L1 stomp / land-hit** |
| 121 | agachada compacta | L2 | **L2 baixo** |
| 226 | silhueta branca | L3 | **flash de hit** (não é pose nova) |

## Faixas revisadas

| Frames | Volta | Estado |
|---:|---|---|
| 1–49 | 1 | idle |
| 50–61 | 50 | land / crouch (poeira) |
| 62–70 | 62 | jump / air |
| 71–85 | 71 | walk |
| 86–94 | `stop` | dash (segura o rastro) |
| 108–117 | idle | L1 (flag `_weakHit` no 108) |
| 121–130 | idle | L2 |
| 226–231 | — | flash (`_weakHit` + `_hitDone`) |
| 565 | `stop` | pose final |

H e L3 continuam nos buracos sem script. Candidatos: **95–107, 131–154, 180–222, 244+**.

## Próximos PNG (se quiser fechar H)

`95 131 180 200 244 303`
