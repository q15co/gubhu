/* SPDX-License-Identifier: GPL-3.0-only */
/* Host test of the boot defaults: a fresh unit is an amapiano instrument before the user
 * touches anything. The tempo and the swing are defaults of the parameter tables, the kit is the one
 * DRUM_DEFAULT_KIT names, and the three synth parts start on the song's key rather than each in
 * chromatic C. The last check pins the property that makes the default safe: loading a sound keeps
 * the key, so a preset cannot silently move the song (ui.c param_kept). Build with the same generated
 * headers and flags as hostsim.c; run by tests/run_tests.sh. */
#include <stdio.h>
#include <string.h>
#define main hostsim_main
#include "hostsim.c"
#undef main

static int fails;
static int check(const char *what, int ok)
{
    printf("defaults: %-57s %s\n", what, ok ? "ok" : "FAIL");
    fails += !ok;
    return ok;
}

int main(void)
{
    uint32_t i;
    uint16_t dor = 0;
    /* index the tables only once the bounds are known: a wrong constant must read as a failed check,
     * not as an out-of-bounds read in the test that is supposed to catch it */
    check("GENRE_SCALE inside the scale table", GENRE_SCALE < NSCALES);
    if (GENRE_SCALE < NSCALES)
        dor = SCALE_MASK[GENRE_SCALE];

    /* the tempo band of the genre, and the swing inside it */
    check("BPM default 112", GP[G_BPM].def == 112);
    check("BPM 112 inside its own range", GP[G_BPM].min <= 112 && 112 <= GP[G_BPM].max);
    check("swing default 30 (MPC 57.5 %)", GP[G_SWING].def == 30);
    check("swing 30 inside its own range", GP[G_SWING].min <= 30 && 30 <= GP[G_SWING].max);
    /* one control, not two: the per-track and arp swings stay at zero, or the totals add up */
    check("per-track swing default 0", TP[P_SSWING].def == 0);
    check("arp swing default 0", TP[P_ASWING].def == 0);

    /* the kit: the name at the default index is the genre's kit, not the 808 */
    check("default kit is inside the kit table", DRUM_DEFAULT_KIT < DRUM_KITS);
    check("default kit is AMAPIANO", DRUM_DEFAULT_KIT < DRUM_KITS &&
                                     strcmp(DRUM_KIT_NAMES[DRUM_DEFAULT_KIT], "AMAPIANO") == 0);
    check("default kit is not the 808", DRUM_DEFAULT_KIT != DRUM_SAMPLED);

    /* the key: every synth part, the same root and scale, and the scale is Dorian (minor third, major
     * sixth: the i9 to IV9 vamp's home) */
    check("Dorian mask has the minor third", (dor >> 3) & 1u);
    check("Dorian mask has the major sixth", (dor >> 9) & 1u);
    check("Dorian mask has no minor sixth", !((dor >> 8) & 1u));
    /* the same reset a boot and a NEW project do before the sounds go in (ui.c track_defaults, which
     * needs the panel and so is not in this build): every parameter at its table default */
    for (i = 0; i < NTRK; i++) {
        uint32_t j;
        for (j = 0; j < P_E0; j++)
            trk[i].p[j] = TP[j].def;
    }
    genre_key_defaults();
    for (i = 0; i < NPART; i++) {
        char what[64];
        snprintf(what, sizeof what, "part %u root A (%u)", (unsigned)i + 1u, (unsigned)GENRE_ROOT);
        check(what, trk[i].p[P_ROOT] == GENRE_ROOT);
        snprintf(what, sizeof what, "part %u scale DOR (%u)", (unsigned)i + 1u, (unsigned)GENRE_SCALE);
        check(what, trk[i].p[P_SCALE] == GENRE_SCALE);
        snprintf(what, sizeof what, "part %u QNT SNAP (%u)", (unsigned)i + 1u, (unsigned)GENRE_QUANT);
        check(what, trk[i].p[P_QUANT] == GENRE_QUANT);
    }

    check("root, scale and the quantiser inside their ranges",
          TP[P_ROOT].min <= GENRE_ROOT && GENRE_ROOT <= TP[P_ROOT].max &&
          TP[P_SCALE].min <= GENRE_SCALE && GENRE_SCALE <= TP[P_SCALE].max &&
          TP[P_QUANT].min <= GENRE_QUANT && GENRE_QUANT <= TP[P_QUANT].max);
    /* (a sound loaded after this keeps the key: ui.c param_kept holds ROOT..QUANT, and that path
     * needs the panel, so it is asserted where the panel is: tests/scale_test.c drives it) */

    printf("defaults: %d failed\n", fails);
    return fails != 0;
}
