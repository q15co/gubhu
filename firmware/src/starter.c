/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* Starter content, shared by the boot path and its host render. Included after project.c's
 * capture/apply code; no storage, UI drawing or allocation here. */
/* A fresh unit boots on the intro, not silence. Four bars in A Dorian: the log answers the township
 * accents, i9 | IV9 piano stabs, two 3-3-2 rim figures, open hats off the beat and shaker on every
 * sixteenth. The kick is soft and the whole kit gently low-passed. The four sections are RAM content
 * only: persist_boot has already loaded every saved slot, and one used slot keeps the whole set. No
 * boot-time erase, no starter mixed into a user's song. A saved autosave below still wins whole. */
#if FELUCCA_ARRANGER
UI_SMALL static void starter_scene(project_t *p, uint32_t scene)
{
    uint32_t i;
    proj_capture(p);                               /* the working project is the intro */
    pt_set(&p->t[0], P_MUTE, scene < 2u);          /* A intro / B build: the log waits */
    pt_set(&p->t[TRK_DRUM], P_MUTE, scene == 3u);  /* D break: piano and log only */
    for (i = 0; i < 64u; i++) {
        dstep_t *d = &p->t[TRK_DRUM].dstep[i];
        uint32_t q = i % 16u;
        int kick = scene == 1u ? q % 4u == 0u : scene == 2u && (q == 4u || q == 12u);
        int clap = (scene == 1u || scene == 2u) && (q == 4u || q == 12u);
        if (kick)                                  /* C drop: the kick leaves the log's downbeats */
            dstep_set(d, lane_of_note(36), LV_SOFT, 0);
        else
            dstep_clr(d, lane_of_note(36));
        if (clap)
            dstep_set(d, lane_of_note(39), LV_NORM, 0);
        else
            dstep_clr(d, lane_of_note(39));
    }
    p->sum = proj_sum(p);
}
#endif

UI_SMALL static void starter_amapiano(void)
{
    static const uint8_t LOG[4][6] = {{33, 36, 40, 33, 38, 36}, {33, 38, 40, 43, 40, 38},
                                      {36, 38, 43, 45, 43, 40}, {36, 40, 43, 45, 40, 33}};
    static const uint8_t AT[6] = {0, 3, 5, 8, 11, 13};       /* steps 1, 4, 6, 9, 12, 14 */
    static const uint8_t AM9[4] = {57, 60, 67, 71}, D9[4] = {50, 54, 60, 64};
    track_t *t1 = &trk[0], *t2 = &trk[1];
    uint32_t i;

    song.g[G_BPM] = 112;
    song.g[G_SWING] = 30;
    song.g[G_DRLVL] = 82;
    set_engine_of(t1, 0);
    apply_preset_to(t1, 22);                        /* (ANALOG LOG DEEP: the pitched log drum) */
    set_engine_of(t2, 1);
    apply_preset_to(t2, 5);                         /* (DIGITAL AFRO KEYS: the piano comp) */
    TDRUM->p[P_E0] = DRUM_DEFAULT_KIT;
    TDRUM->p[P_TFLT] = -20;
    TDRUM->p[P_MUTE] = 0;
    t1->p[P_SLEN] = t2->p[P_SLEN] = TDRUM->p[P_SLEN] = 64;
    t1->p[P_MUTE] = 1;                              /* A intro: piano and percussion */
    for (i = 0; i < 64u; i++) {
        step_t *s = &t1->step[i];
        uint32_t k, q = i % 16u;
        memset(s, 0, sizeof *s);
        s->time = ST_REST;
        for (k = 0; k < 6u; k++)
            if (q == AT[k]) {
                s->n = 1;
                s->note[0] = LOG[i / 16u][k];
                s->time = ST_NOTE;
                s->flags = (uint8_t)((q == 0u ? SF_ACCENT : 0u) | (i == 59u ? SF_SLIDE : 0u));
                s->vel = 100;
            }
    }
    t2->p[P_SGATE] = 45;                            /* (stabs, not a held wall) */
    for (i = 0; i < 64u; i++) {
        step_t *s = &t2->step[i];
        const uint8_t *ch = (i / 16u) & 1u ? D9 : AM9;
        uint32_t q = i % 16u;
        memset(s, 0, sizeof *s);
        s->time = ST_REST;
        if (q == 2u || q == 6u || q == 10u || q == 14u) {
            s->n = 4;
            s->note[0] = ch[0], s->note[1] = ch[1], s->note[2] = ch[2], s->note[3] = ch[3];
            s->time = ST_NOTE;
            s->flags = q == 2u ? SF_ACCENT : 0u;
            s->vel = 100;
        }
    }
    for (i = 0; i < 64u; i++) {
        dstep_t *d = &TDRUM->dstep[i];
        uint32_t q = i % 16u;
        memset(d, 0, sizeof *d);
        if (q == 0u || q == 3u || q == 6u || q == 8u || q == 11u || q == 14u)
            dstep_set(d, lane_of_note(37), q == 0u || q == 8u ? LV_HARD : LV_NORM, 0);  /* 3-3-2, twice */
        if (q == 2u || q == 6u || q == 10u || q == 14u)
            dstep_set(d, lane_of_note(46), LV_SOFT, 0);                 /* the open hat on the offbeats */
        dstep_set(d, lane_of_note(70), q % 2u == 0u ? LV_HARD : LV_NORM, 0);            /* the shaker */
    }
#if FELUCCA_ARRANGER
    for (i = 0; i < ARR_SCENES; i++)
        if (proj_ok(&proj_slot[i]))
            return;                                 /* never overwrite or fill around a saved section */
    for (i = 0; i < ARR_SCENES; i++)
        starter_scene(&proj_slot[i], i);
#endif
}
