/* SPDX-License-Identifier: GPL-3.0-only */
/* GUBHU boot screen: a beadwork band, the wordmark, the firmware's name (docs/DESIGN.md).
 * Drawn from the active palette, so a unit on MONO gets a monochrome mark and the boot screen
 * costs no flash beyond its code: the fork's own identity, not the SLOOP sailboat. */
#ifndef UI_SMALL                 /* (2.7) compiled for size: not the audio path (gfx.c UI_SMALL); the host tests: nothing */
#define UI_SMALL
#endif

/* The wordmark is a text placeholder (decision 0003) and stays lowercase, which the panel font
 * cannot draw: gfx.c's glyph() folds a-z onto A-Z. The four distinct letters are cut here from
 * the same Tamzen Bold 8x16 cells the font is generated from, 3x on screen. */
static const uint8_t WORD_G[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x66, 0x66, 0x66, 0x3C, 0x60, 0x3E, 0x63, 0x63, 0x3E, 0x00};
static const uint8_t WORD_U[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x00};
static const uint8_t WORD_B[16] = {0x00, 0x00, 0x60, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x00, 0x00, 0x00, 0x00};
static const uint8_t WORD_H[16] = {0x00, 0x00, 0x60, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00};

/* one bead: a stepped diamond, 12 px wide, 4 px steps (Zulu beadwork geometry). The downbeat of
 * each bar is a longer bead, so the strand has the rhythm of the sequencer's 16 steps */
UI_SMALL static void bead(int32_t cx, int32_t y, uint16_t c, uint32_t big)
{
    cv_rect(cx - 2, y, 4, 4, c);
    cv_rect(cx - 4, y + 4, 8, 4, c);
    cv_rect(cx - 6, y + 8, 12, 4, c);
    cv_rect(cx - (big ? 6 : 4), y + 12, big ? 12 : 8, 4, c);
    cv_rect(cx - 2, y + 16, 4, 4, c);
    if (big)
        cv_rect(cx - 2, y + 20, 4, 4, c);
}

/* a wordmark letter: its 8x16 cell at 3x, so the lowercase forms keep their x-height and the
 * 'g' its descender */
UI_SMALL static void word_letter(const uint8_t *rows, int32_t x, int32_t y, uint16_t c)
{
    uint32_t r, b;
    for (r = 0; r < 16u; r++)
        for (b = 0; b < 8u; b++)
            if (rows[r] & (0x80u >> b))
                cv_rect(x + (int32_t)b * 3, y + (int32_t)r * 3, 3, 3, c);
}

/* the boot screen: the band, the wordmark, FM1 FIRMWARE, the version */
UI_SMALL static void gubhu_splash(void)
{
    static const uint8_t *const MARK[5] = {WORD_G, WORD_U, WORD_B, WORD_H, WORD_U};
    const char *v = FELUCCA_VERSION;
    uint32_t i;
    lcd_fill(0, 0, 240, 240, C_BLACK);
    cv_begin(240, 32, C_BLACK);                     /* the band: 16 beads, one per step, a bar to a colour */
    cv_rect(0, 0, 240, 1, C_LINE);
    for (i = 0; i < 16u; i++)
        bead(8 + (int32_t)i * 15, (i % 4u) ? 6 : 4, TE_COL[i / 4u], !(i % 4u));
    cv_rect(0, 30, 240, 1, C_LINE);
    cv_blit(0, 40);
    cv_begin(240, 48, C_BLACK);                     /* the wordmark: five letters at 24 px, 120 px wide */
    for (i = 0; i < 5u; i++)
        word_letter(MARK[i], 60 + (int32_t)i * 24, 0, C_HI);
    cv_blit(0, 88);
    if (v[0] == 'S' && v[1] == 'L' && v[5] == ' ')
        v += 6;                                     /* "SLOOP 1.0" -> "1.0" under the wordmark */
    cv_begin(240, 52, C_BLACK);
    cv_text(120 - text_w(&FONT_S, "FM1 FIRMWARE") / 2, 0, &FONT_S, "FM1 FIRMWARE", C_WHITE);
    cv_text(120 - text_w(&FONT_S, v) / 2, 34, &FONT_S, v, C_GRAY);
    cv_blit(0, 152);
    cv_begin(240, 16, C_BLACK);
    cv_text(120 - text_w(&FONT_S, "BASED ON FELUCCA") / 2, 0, &FONT_S, "BASED ON FELUCCA", C_DIM);
    cv_blit(0, 214);
}
