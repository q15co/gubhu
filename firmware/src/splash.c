/* SPDX-License-Identifier: GPL-3.0-only */
/* GUBHU boot screen: a beadwork band, the wordmark, the firmware's name (docs/DESIGN.md).
 * Drawn from the fork's own colours, never from the active palette: TE_COL[] still holds the
 * upstream blue / green / amber / red until a theme is chosen, so a palette-driven band was
 * SLOOP's mark on every unit that was not on the GUBHU palette. This screen is the brand, not
 * the interface, and it costs no flash beyond its code. */
#ifndef UI_SMALL                 /* (2.7) compiled for size: not the audio path (gfx.c UI_SMALL); the host tests: nothing */
#define UI_SMALL
#endif

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

/* the boot screen: the band, the wordmark, FM1 FIRMWARE, the version. The wordmark itself (a text
 * placeholder, decision 0003, lowercase) is ui_draw.c's gubhu_mark(), so this screen and ABOUT
 * draw the same letters. */
UI_SMALL static void gubhu_splash(void)
{
    static const uint16_t BAND[4] = {GUBHU_TRACK_1, GUBHU_TRACK_2, GUBHU_TRACK_3, GUBHU_TRACK_4};
    const char *v = FELUCCA_VERSION;
    uint32_t i;
    lcd_fill(0, 0, 240, 240, GUBHU_BG);
    cv_begin(240, 32, GUBHU_BG);                    /* the band: 16 beads, one per step, a bar to a colour */
    cv_rect(0, 0, 240, 1, GUBHU_C_LINE);
    for (i = 0; i < 16u; i++)
        bead(8 + (int32_t)i * 15, (i % 4u) ? 6 : 4, BAND[i / 4u], !(i % 4u));
    cv_rect(0, 30, 240, 1, GUBHU_C_LINE);
    cv_blit(0, 40);
    cv_begin(240, 48, GUBHU_BG);                    /* the wordmark: five letters at 24 px, 120 px wide */
    gubhu_mark(60, 0, 3, GUBHU_C_HI);
    cv_blit(0, 88);
    if (v[0] == 'S' && v[1] == 'L' && v[5] == ' ')
        v += 6;                                     /* "SLOOP 1.0" -> "1.0" under the wordmark */
    cv_begin(240, 52, GUBHU_BG);
    cv_text(120 - text_w(&FONT_S, "FM1 FIRMWARE") / 2, 0, &FONT_S, "FM1 FIRMWARE", GUBHU_ACCENT);
    cv_text(120 - text_w(&FONT_S, v) / 2, 34, &FONT_S, v, GUBHU_C_GRAY);
    cv_blit(0, 152);
    cv_begin(240, 16, GUBHU_BG);
    cv_text(120 - text_w(&FONT_S, "BASED ON FELUCCA") / 2, 0, &FONT_S, "BASED ON FELUCCA", GUBHU_C_DIM);
    cv_blit(0, 214);
}
