# Combate — `frame_1479` + `frame_2258` + `frame_2266`

## Spawn (`2258`)

- P1 `(100, 204)` `_xscale = -100` (olha direita)
- P2 `(300, 204)` `_xscale = 100` (olha esquerda)
- `_wait = true` + clip `readyfight` (nosso 3-2-1)
- `_hp = MAX_HP` (número ainda não lido; barra usa `hp/5` e `hp/10`)

## Input (`2266`)

| Flash | 3DS |
|---|---|
| LEFT/RIGHT | D-pad / Circle Pad |
| UP | pulo |
| DOWN | shield (`STATE_SHIELD`); soltar → idle |
| A | fraco (`KEYINDEX_A`) |
| B | forte (`KEYINDEX_B`) |
| double-tap lado | dash |

`newA` / `newB` = just pressed. Combo usa **buffer**:
`bufferedCombo2` / `bufferedCombo3` = A ou B no meio do hit.

## Cadeia no `_comboDone`

1º hit (A) → espera `_comboDone`
- buffer A → `"upper"`
- buffer B → `"combo2"`
3º hit → `"combo3u"` (A) ou variante B

Isso é L / LL / LLL / LH — o `fight.c` já imita. Labels reais do clip: `idle fall dash upper combo2 combo3u`.

Knockback tem `_atkdirX/Y`: LEFT, RIGHT, UP, DOWN, JUGGLE, VACUUM.

## CPU (`1479`)

Três personalidades, troca no meio da luta:

- `AI_AGGRESSIVE` — aproxima e buffera combo
- `AI_DEFENSIVE` — shield / recua
- `AI_RANGED` — Blossom/Bell atiram; Buttercup evita ranged

`_difficulty` 0–10. `AI_BASE_ERROR` = chance de errar (cai com a dificuldade).
Zim é caso especial. Ainda não portar o bloco inteiro.

## Ainda falta no dump

```
grep -nE '^STATE_|^MAX_HP|^MIN_HP|^MAX_DIFFICULTY|^AI_BASE|^NAME_BLOSSOM' \
  assets/raw/dump/scripts/frame_1479/DoAction.as
```
