# Catálogo do dump JPEXS (só nomes)

16 598 arquivos. **Não** use `frames/` (4 652 PNG do filme inteiro).
Fonte útil: `sprites/` nomeados + `scripts/*/frame_N/DoAction.as`.

## Roster (um clip = todas as animações)

| Clip | PNG | Scripts (keyframes) |
|---|---:|---:|
| DefineSprite_471_Blossom | 605 | 32 |
| DefineSprite_563_Bubbles | 585 | 30 |
| DefineSprite_690_Buttercup | 617 | 41 |
| DefineSprite_370_Bell | 617 | 51 |
| DefineSprite_1469_Shira Bell | 617 | 70 |
| DefineSprite_1236_Dexter | 646 | 42 |
| DefineSprite_1334_zim | 1001 | 56 |
| DefineSprite_1399_Rowdyruff | 485 | 36 |

As três meninas e Bell compartilham os mesmos primeiros keyframes:

`49 58 61 70 85 94 108 113 117 121 126 130`

Isso é idle / walk / jump / guard / cadeia L. O resto do clip é special.

## UI / VFX (pegar depois do lutador)

- `751_combo` `738_projectile` `759_hit` `760_hitwin` `778_readyfight`
- `783_winFlash` `788_wins` `1115_hpbar` `1123_hpbarframe` `1102_dashbarframe`
- `1443_howtoplay`

## Deixar de lado no slice

- `1045_stories` (1 201 frames — modo história)
- `frames/*.png` (timeline raiz)
- 59 sons (`*.mp3` / `*.flv`) até existir mapeamento
- 167 sprites anônimos (`DefineSprite_123`)

## Próximo olhar no FFDec

Abrir **só** `scripts/DefineSprite_471_Blossom/frame_49` e `frame_108`.
Lá deve estar `gotoAndPlay`, hitbox e o nome da ação.
Os outros chars copiam a mesma tabela de frames.
