/* SPDX-License-Identifier: GPL-3.0-only */
/* After an install (a power-on on the flash as it was), track 1 (and the other parts) sound as their sound: the
 * values the boot leaves equal the ones the same sound gets when it is loaded again. And (2.8.4) a working project
 * Clipper left in the flash (SLOOP installed after Clipper): a part on an engine SLOOP does not have (HYBRID, 12 and
 * up) used to wrap round onto ANALOG with HYBRID's values: 808 BOOM without its TRN -24, two octaves high; it gets
 * its default sound now. Exit status: failed checks. */
#include "editor_host.c"
static int fails;
static void check(int c, const char *what) { printf("boot_sound: %-80s %s\n", what, c ? "ok" : "FAIL"); fails += !c; }
static int16_t snapv[NPART][P_COUNT];
static void boot_as_main(void)
{
    uint32_t i;
    persist_boot();
    settings_init();
    panel_init();
    for (i = 0; i < G_COUNT; i++) song.g[i] = GP[i].def;
    fm6_init();
    for (i = 0; i < NTRK; i++) {
        track_t *t = &trk[i];
        track_defaults(t);
        if (i < NPART) { set_engine_of(t, TRK_DEF[i][0]); apply_preset_to(t, TRK_DEF[i][1]); t->engine = t->eng_req; }
        track_defaults_steps(t);
    }
    genre_key_defaults();                          /* as felucca_init: the parts start in the song's key */
    TDRUM->p[P_E0] = DRUM_DEFAULT_KIT;
    song.sel = 0;
    autosave_resume();
    layers_init();
    go_home();
    ui.force = 1;
}
static void compare(const char *when)
{
    uint32_t k, i;
    char b[160];
    for (k = 0; k < NPART; k++) {
        int16_t now[P_COUNT];
        uint32_t e = trk[k].eng_req, pi = trk[k].preset, bad = 0;
        memcpy(now, trk[k].p, sizeof now);
        memcpy(snapv[k], trk[k].p, sizeof now);
        apply_preset_to(&trk[k], pi);                 /* the same sound loaded again */
        for (i = 0; i < P_COUNT; i++)
            if (now[i] != trk[k].p[i]) {
                if (bad++ < 6)
                    printf("boot_sound: %s track %u (%s %s): p[%u] %s %d, loaded again %d\n", when, k + 1, ENGINES[e]->name,
                           ENGINES[e]->presets[pi % ENGINES[e]->npresets].name, i, i < P_E0 ? TP[i].label : "E", now[i], trk[k].p[i]);
            }
        snprintf(b, sizeof b, "%s: track %u plays its sound as loaded (every value)", when, k + 1);
        check(!bad, b);
        memcpy(trk[k].p, now, sizeof now);
    }
}
/* track k's note: its zero-crossing rate over 0.3 .. 0.8 s (Hz, the fundamental of a bass sine) */
static double note_hz(uint32_t k, uint32_t note)
{
    static int32_t o[CTL * 2];
    uint32_t b, i, zc = 0, n = 0;
    int32_t prev = 0;
    for (i = 0; i < NTRK; i++) trk_all_off(&trk[i]);
    for (b = 0; b < 600; b++) mix_block(o, CTL);
    trk_note_on(&trk[k], note, 100);
    for (b = 0; b < 1200; b++) {
        mix_block(o, CTL);
        if (b < 400) continue;
        for (i = 0; i < CTL; i++, n++) { int32_t y = o[2 * i]; if ((prev < 0) != (y < 0)) zc++; prev = y; }
    }
    trk_note_off(&trk[k], note);
    return zc / 2.0 * FS / n;
}
static void pitch_check(const char *when)
{
    uint32_t k;
    char b[160];
    for (k = 0; k < NPART; k++) {
        double a = note_hz(k, 48), c;
        apply_preset_to(&trk[k], trk[k].preset);
        c = note_hz(k, 48);
        printf("boot_sound: %s track %u note C3: %.1f Hz, the sound loaded again %.1f Hz\n", when, k + 1, a, c);
        snprintf(b, sizeof b, "%s: track %u at its pitch (as the sound loaded again)", when, k + 1);
        check(a > c * 0.97 && a < c * 1.03, b);
    }
}
/* the FM-1's key `key` on track k: the note its voice plays */
static int key_note(uint32_t k, uint32_t key)
{
    uint32_t i, f;
    int n = -1;
    song.sel = (uint8_t)k;
    for (f = 0; f < 4; f++) ui_frame();
    fm1_in.notes = 1u << key;
    for (f = 0; f < 6; f++) ui_frame();
    for (i = 0; i < NVOICE; i++) if (trk[k].v[i].active && trk[k].v[i].gate) n = trk[k].v[i].note;
    fm1_in.notes = 0;
    for (f = 0; f < 30; f++) ui_frame();
    return n;
}
static void key_check(const char *when)
{
    uint32_t k;
    char b[160];
    for (k = 0; k < NPART; k++) {
        int a = key_note(k, 7), c;
        printf("boot_sound: %s track %u: octave %d, TRN %d, key C4 plays note %d\n", when, k + 1, song.octave, trk[k].p[P_TRANS], a);
        apply_preset_to(&trk[k], trk[k].preset);
        c = key_note(k, 7);
        printf("boot_sound: %s track %u: the sound loaded again, key C4 plays note %d\n", when, k + 1, c);
        snprintf(b, sizeof b, "%s: track %u, key C4 plays the same note as with the sound loaded again", when, k + 1);
        check(a == c, b);
    }
}
/* C#4 (key 8) under SNAP lands on C4 (key 7): both are in A Dorian, and the scale is what rounds the
 * one into the other. With the quantiser OFF the two keys play different notes -- which is what a fresh
 * instrument did while its parts carried a key the keyboard never applied. */
static void snap_check(const char *when)
{
    uint32_t k;
    char b[160];
    for (k = 0; k < NPART; k++) {
        int c = key_note(k, 7), cs = key_note(k, 8);
        printf("boot_sound: %s track %u: key C4 %d, key C#4 %d (QNT SNAP)\n", when, k + 1, c, cs);
        snprintf(b, sizeof b, "%s: part %u snaps C#4 to C4 (the scale is heard)", when, (unsigned)k + 1u);
        check(cs == c, b);
    }
}
/* what a first power-on on blank flash is. The boot's own ordering is what the emulator shows, so
 * the host asserts it here rather than on the tables alone. */
static void defaults_check(const char *when)
{
    uint32_t k;
    char b[160];
    snprintf(b, sizeof b, "%s: 112 BPM, 30 swing (the genre's)", when);
    check(song.g[G_BPM] == 112 && song.g[G_SWING] == 30, b);
    snprintf(b, sizeof b, "%s: the drum track on the amapiano kit", when);
    check(strcmp(DRUM_KIT_NAMES[TDRUM->p[P_E0]], "AMAPIANO") == 0, b);
    for (k = 0; k < NPART; k++) {
        snprintf(b, sizeof b, "%s: part %u in A Dorian", when, (unsigned)k + 1u);
        check(trk[k].p[P_ROOT] == 9 && trk[k].p[P_SCALE] == 3, b);
        snprintf(b, sizeof b, "%s: part %u snaps to the scale", when, (unsigned)k + 1u);
        check(trk[k].p[P_QUANT] == 1, b);
    }
}

int main(void)
{
    uint32_t i, erases0, progs0;
    harness_init();
    memset(nor, 0xFF, sizeof nor);               /* 1. a first install: nothing in the flash */
    memset(proj_slot, 0, sizeof proj_slot);      /* (a cold boot: .noinit has no warm-reset sections) */
    erases0 = fl_erases, progs0 = fl_progs;
    boot_as_main();
    defaults_check("first power-on");
    {
        uint32_t s, i;
        uint32_t kicks[4] = {0};
        static const uint8_t AT[6] = {0, 3, 5, 8, 11, 13};
        check(arrangement_ready() == 15u && arr_valid(&arrangement, arrangement_ready()),
              "first power-on: A, B, C and D are ready for the default song");
        check(song.g[G_BPM] == 112 && song.g[G_SWING] == 30, "first power-on: 112 BPM and the genre's swing");
        check(trk[0].p[P_MUTE] == 1 && trk[1].p[P_MUTE] == 0 && TDRUM->p[P_MUTE] == 0,
              "first power-on: the intro is piano and percussion, log muted");
        for (i = 0; i < 16u; i++)
            check(!dstep_has(&TDRUM->dstep[i], lane_of_note(36)) &&
                  !dstep_has(&TDRUM->dstep[i], lane_of_note(39)),
                  "first power-on: the intro has no kick or clap");
        for (s = 0; s < 4u; s++) {
            const project_t *p = &proj_slot[s];
            uint32_t logs = 0, claps = 0, rims = 0, hats = 0, shaker = 0, low_same = 0, notes = 0, key_ok = 1;
            uint32_t want_kick = s == 1u ? 16u : s == 2u ? 8u : 0u;
            uint32_t want_clap = s == 1u || s == 2u ? 8u : 0u;
            check(pt_get(&p->t[0], P_SLEN) == 64 && pt_get(&p->t[1], P_SLEN) == 64 &&
                  pt_get(&p->t[TRK_DRUM], P_SLEN) == 64, "starter: every scene is four bars of sixteenths");
            check(pt_get(&p->t[0], P_MUTE) == (s < 2u), "starter: the log waits for the drop");
            check(pt_get(&p->t[1], P_MUTE) == 0 && pt_get(&p->t[1], P_SGATE) < 64,
                  "starter: the short-gated piano stays through the break");
            check(pt_get(&p->t[TRK_DRUM], P_MUTE) == (s == 3u) &&
                  pt_get(&p->t[TRK_DRUM], P_TFLT) < 0, "starter: the break drops percussion; the kit is low-passed");
            for (i = 0; i < 64u; i++) {
                const step_t *l = &p->t[0].step[i];
                const dstep_t *d = &p->t[TRK_DRUM].dstep[i];
                uint32_t q = i % 16u, k;
                int accent = 0;
                for (k = 0; k < 6u; k++) accent |= q == AT[k];
                if (l->time == ST_NOTE && l->n) {
                    logs++;
                    notes |= 1u << (l->note[0] % 12u);
                    key_ok &= l->note[0] >= 33u && l->note[0] <= 45u && ((0xAD5u >> (l->note[0] % 12u)) & 1u);
                }
                kicks[s] += dstep_has(d, lane_of_note(36));
                claps += dstep_has(d, lane_of_note(39));
                rims += dstep_has(d, lane_of_note(37));
                hats += dstep_has(d, lane_of_note(46));
                shaker += dstep_has(d, lane_of_note(70));
                low_same += dstep_has(d, lane_of_note(36)) && l->time == ST_NOTE && l->n != 0;
                if (accent && q != 0u && q != 8u)
                    check(l->time == ST_NOTE && l->n == 1u, "starter: log on township accents 4, 6, 12 and 14");
            }
            printf("starter: scene %u log: %u notes, pitch-class mask %03x, in key %u, kick/log together %u\n",
                   s, logs, notes, key_ok, low_same);
            check(logs == 24u && key_ok && (notes & (notes - 1u)),
                  "starter: the four-bar log phrase moves through A Dorian");
            check((p->t[0].step[59].flags & SF_SLIDE) && p->t[0].step[61].time == ST_NOTE,
                  "starter: the last log note glides into the phrase end");
            check(kicks[s] == want_kick && claps == want_clap,
                  "starter: build adds kick and clap; drop halves the kick; break has neither");
            check(low_same * 2u < logs, "starter: log and kick rarely land together");
            if (s < 3u) {
                check(rims == 24u && hats == 16u && shaker == 64u,
                      "starter: rim 3-3-2, open hat offbeats and continuous shaker");
            }
        }
        check(fl_erases == erases0 && fl_progs == progs0, "first power-on: the starter writes no flash sectors");
    }
    snap_check("first power-on");
    compare("first power-on");
    boot_as_main();
    boot_as_main();
    key_check("first power-on");
    for (i = 0; i < 400; i++) ui_frame();         /* (an autosave) */
    persist_flush_now();
    boot_as_main();                               /* 2. an update: the flash as it was */
    compare("power-on after an update");
    boot_as_main();
    boot_as_main();
    key_check("power-on after an update");
    {   /* 3. a working project Clipper wrote: track 1 on engine 12 (HYBRID), its TRN 0, a pattern */
        static project_t q;
        proj_capture(&q);
        q.t[0].engine = NENGINES;                 /* (Clipper's HYBRID: the first engine SLOOP does not have) */
        q.t[0].preset = 0;
        pt_set(&q.t[0], P_TRANS, 0);
        q.t[0].step[0].time = ST_NOTE; q.t[0].step[0].n = 1; q.t[0].step[0].note[0] = 48;
        q.sum = proj_sum(&q);
        st_save(OBJ_AUTOSAVE, &q, sizeof q);
        boot_as_main();
        printf("boot_sound: over Clipper's project: track 1 %s %s, TRN %d\n", ENGINES[trk[0].eng_req]->name,
               ENGINES[trk[0].eng_req]->presets[trk[0].preset].name, trk[0].p[P_TRANS]);
        check(trk[0].eng_req == TRK_DEF[0][0] && trk[0].preset == TRK_DEF[0][1] && trk[0].p[P_TRANS] == -24,
              "over Clipper's working project: track 1 gets its default sound (808 BOOM, TRN -24)");
        check(trk[0].step[0].time == ST_NOTE && trk[0].step[0].note[0] == 48, "... and keeps its pattern");
        key_check("over Clipper's working project");
    }
    {   /* 4. a project of one's own: the defaults are the boot's, never the loader's */
        static project_t q;
        uint32_t k;
        song.g[G_BPM] = 95;
        song.g[G_SWING] = 12;
        TDRUM->p[P_E0] = DRUM_SAMPLED + 3;         /* (80S, not the boot's amapiano kit) */
        for (k = 0; k < NPART; k++) {
            trk[k].p[P_ROOT] = 2;                  /* D */
            trk[k].p[P_SCALE] = 2;                 /* MIN */
            trk[k].p[P_QUANT] = 0;                 /* OFF: the file's choice, not the boot's */
        }
        proj_capture(&q);
        q.sum = proj_sum(&q);
        st_save(OBJ_AUTOSAVE, &q, sizeof q);
        boot_as_main();
        check(song.g[G_BPM] == 95 && song.g[G_SWING] == 12, "a saved project keeps its tempo and swing");
        check(TDRUM->p[P_E0] == DRUM_SAMPLED + 3, "a saved project keeps its kit");
        for (k = 0; k < NPART; k++) {
            char b[160];
            snprintf(b, sizeof b, "a saved project keeps part %u's root, scale and quantiser", (unsigned)k + 1u);
            check(trk[k].p[P_ROOT] == 2 && trk[k].p[P_SCALE] == 2 && trk[k].p[P_QUANT] == 0, b);
        }
    }
    {   /* 5. starter sections never replace a saved working project or a user's scene set */
        static project_t q, keep, now;
        proj_capture(&q);
        memset(nor, 0xFF, sizeof nor);
        memset(proj_slot, 0, sizeof proj_slot);
        st_save(OBJ_AUTOSAVE, &q, sizeof q);
        boot_as_main();
        proj_capture(&now);
        check(!memcmp(&q, &now, sizeof q), "a cold boot resumes the saved working project byte for byte");
        check(arrangement_ready() == 15u, "an autosave without scene slots still gets the starter sections");

        keep = proj_slot[2];                       /* a valid project of one's own, not a format fixture */
        keep.g[G_BPM] = 93;
        pt_set(&keep.t[1], P_LEVEL, 55);
        keep.sum = proj_sum(&keep);
        memset(nor, 0xFF, sizeof nor);
        memset(proj_slot, 0, sizeof proj_slot);
        st_save(OBJ_PROJECT0 + 2u, &keep, sizeof keep);
        boot_as_main();
        check(!memcmp(&keep, &proj_slot[2], sizeof keep), "a saved scene survives the starter byte for byte");
        check(!project_used(0) && !project_used(1) && !project_used(3),
              "one saved scene keeps the starter out of the other slots");
    }
    printf("boot_sound: %d failed\n", fails);
    return fails != 0;
}
