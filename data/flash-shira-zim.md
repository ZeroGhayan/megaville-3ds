# Shira H × Zim no survival — por que o Flash trava

Não é “segunda luta sem variável”. É o **fim de round** no `frame_2266`.

1. KO (`_hp <= MIN_HP`) liga `_gameover` e `gotoAndPlay(2286)`.
2. Só avança para o pose de vitória (`2290`) se:
   - nenhum `projectile._visible`
   - ninguém `_airborne`
   - alguém em IDLE/RUN
3. Zim **não** joga `"damage"` (`if(otherName != NAME_ZIM)`). Vai para o clip filho `zim.gotoAndPlay("damaged")`.
4. H simples da Shira no idle é o **boomer** (`STATE_RANGED`), igual Bell. O hit no Zim **não** marca `_hitDone` / esconde o projétil no mesmo sítio que a bolha (`subside`).
5. Enquanto o boomer existir, a condição 2 falha. O filme fica em `2271` (`winFlash._currentframe != 1` → replay). Timeline + clips = “trava e depois glitch”.

`winClrTrans[NAME_SHIRABELL]` existe — não é índice faltando.

No 3DS: no KO some o tiro; não esperamos clip Flash. Não replica.

Notas de combate:
- Shira leva `dmg * 2` (fora story-CPU)
- Zim leva `dmg * 0.8` e tem `_armour` 5
- `SPRITE_RUNSPEED` Shira = **14** (resto 7–9)
