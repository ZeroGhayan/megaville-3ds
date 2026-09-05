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

## Segunda leva (95–303)

| PNG | Pose | Leitura |
|---|---|---|
| 95 | mergulho + anel no chão | **H / divekick** (95–107) |
| 131 | em pé, cabelo ao lado | startup que termina no ar (155 → 62) |
| 180 | agachada + anel | special no chão (vai até 223 → idle) |
| 200 | em pé sobre disco branco | meio desse special |
| 244 | agachada + anel | land / special (270 → air) |
| 303 | em pé + anel | **win loop** (315 volta ao 303) |

O anel no chão se repete: não dá para separar L/H só por ele.
95 é o único claramente “golpe diferente” (corpo na horizontal).

## Slice mínimo para o 3DS

| Ação | Frame a recortar |
|---|---|
| idle | 1 |
| walk | 71 |
| jump | 62 |
| dash | 86 |
| land | 50 |
| L | 108 |
| LL | 121 |
| H | 95 |
| flash | tint no código, não PNG 226 |
| win | 303 (depois) |

Chega de amostrar a Blossom. O mesmo número de frame vale para Bubbles/Buttercup.

