# Dificuldade — não misturar os dois jogos

## Battle in Megaville (este port)

Flash: `_difficulty` **inteiro 1–10**, `MAX_DIFFICULTY = 10`.
O picker 3DS copia isso. CPU usa `AI_BASE_ERROR * (MAX - d)`.

## Jogo autoral de luta (exo-verse / IA de luta)

Escala **Smash-like**, mas **float 0.0–10.0** (não 1–9 inteiro do Smash).

- 0.0 = CPU quase inerte
- 9.0 = nível “alto” estável
- **9.0 → 10.0 custa mais** que os outros degraus (mais reação, menos erro, combo mais longo). Não é +1 linear.

Não reutilizar o picker 1–10 do Megaville nesse título. Slider / 0.5 steps, com o último passo (9→10) mais caro na curva.
