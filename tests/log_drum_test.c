/* SPDX-License-Identifier: GPL-3.0-only */
/* ANALOG's LOG DEEP / LOG DUST factory patches: their slots, fields, level trims and low register; a sequencer SLIDE
 * on LOG DEEP glides through intermediate pitches in about 40 ms without restarting the decaying envelope.
 * Exit status: the number of failed checks. */
#define main hostsim_main
#include "hostsim.c"
#undef main

static int fails;
static void check(int ok, const char *what)
{
    printf("log_drum: %-76s %s\n", what, ok ? "ok" : "FAIL");
    fails += !ok;
}

static int extra(const preset_t *p, uint32_t id)
{
    uint32_t i;
    for (i = 0; i + 1u < NELEM(p->x); i += 2u)
        if (p->x[i] == (int8_t)(id + 1u))
            return p->x[i + 1u];
    return 999;
}

static void t_patches(void)
{
    static const char *const name[2] = {"LOG DEEP", "LOG DUST"};
    static const int ed_pit[2] = {26, 36}, trim[2] = {18, 22};
    uint32_t k;
    check(ENGINES[0] == &ENG_ANALOG && ENG_ANALOG.npresets == 24u && PT_MAX == 32u,
          "ANALOG: 24 factory presets (the two log drums appended)");
    for (k = 0; k < 2u; k++) {
        uint32_t pi = 22u + k;
        const preset_t *p = &ENG_ANALOG.presets[pi];
        track_t *t = &trk[0];
        char b[160];
        int fields;
        snprintf(b, sizeof b, "0.%u: %s at its factory-table index", pi, name[k]);
        check(!strcmp(p->name, name[k]), b);
        fields = p->mono == 1u && p->e[0] == 3 && p->env[2] == 0u &&
                 extra(p, P_GLIDE) == 51 && extra(p, P_GLMODE) == 1 &&
                 extra(p, P_ED_PIT) == ed_pit[k] && extra(p, P_TRANS) == -12;
        snprintf(b, sizeof b, "%s: sine, mono, SUS 0, GLD 51 TIME, ENV>PIT %d, TRN -12", name[k], ed_pit[k]);
        check(fields, b);
        host_preset(t, 0, pi);
        snprintf(b, sizeof b, "%s loads those fields onto a track", name[k]);
        check(t->engine == 0u && t->preset == pi && t->p[P_VOICE] == V_LEGATO && t->p[P_E0] == 3 &&
              t->p[P_SUS] == 0 && t->p[P_GLIDE] == 51 && t->p[P_GLMODE] == 1 &&
              t->p[P_ED_PIT] == ed_pit[k] && t->p[P_TRANS] == -12, b);
        snprintf(b, sizeof b, "%s: level trim PRESET_TRIM[0][%u] = %d", name[k], pi, trim[k]);
        check(PRESET_TRIM[0][pi] == trim[k] && t->p[P_ED_FX] == trim[k], b);
    }
}

/* MIDI 45 (A2) through the patch's TRN -12 is A1, 55 Hz. Render 0.3 s and find the strongest Hann-windowed
 * spectral peak from 35 to 150 Hz (the untransposed octave is in the scan too), then interpolate its top. */
#define REG_N (FS * 3 / 10)
static int32_t reg_cap[REG_N + CTL];
static double reg_win[REG_N];
static double reg_mag(double hz)
{
    uint32_t i;
    double re = 0, im = 0, w = 2 * M_PI * hz / FS, c = cos(w), s = sin(w), cr = 1, ci = 0;
    for (i = 0; i < REG_N; i++) {
        double x = reg_cap[i] * reg_win[i], t;
        re += x * cr;
        im -= x * ci;
        t = cr * c - ci * s;
        ci = ci * c + cr * s;
        cr = t;
    }
    return re * re + im * im;
}
static double reg_hz(void)
{
    uint32_t k;
    double best = 0, hz = 0, a, b, c, d;
    for (k = 140; k <= 600u; k++) {                 /* 35 .. 150 Hz, quarter-hertz steps */
        double f = k / 4.0, v = reg_mag(f);
        if (v > best)
            best = v, hz = f;
    }
    a = reg_mag(hz - 0.25);
    b = best;
    c = reg_mag(hz + 0.25);
    d = a - 2 * b + c;
    if (d < 0)
        hz += 0.25 * clamp(0.5 * (a - c) / d, -1.0, 1.0);
    return hz;
}
static void fresh_tracks(void)
{
    memset(trk, 0, sizeof trk);
    memset(&song, 0, sizeof song);
    memset(&drums, 0, sizeof drums);
    drums.set = -2;
    transport_req = 0;
    host_tracks_init();
}
static double reg_render(uint32_t pi)
{
    track_t *t = &trk[0];
    int32_t out[CTL * 2];
    uint32_t b, i, k;
    fresh_tracks();
    host_preset(t, 0, pi);
    for (k = 1; k < NTRK; k++)
        trk[k].p[P_LEVEL] = 0;
    song.g[G_DRLVL] = 0;
    trk_note_on(t, 45 + t->p[P_TRANS], 100);
    for (b = 0; b < (REG_N + CTL - 1u) / CTL; b++) {
        mix_block(out, CTL);
        for (i = 0; i < CTL; i++)
            reg_cap[b * CTL + i] = (out[2 * i] + out[2 * i + 1]) / 2;
    }
    trk_note_off(t, 45 + t->p[P_TRANS]);
    return reg_hz();
}
static void t_register(void)
{
    double deep, dust;
    uint32_t i;
    for (i = 0; i < REG_N; i++)
        reg_win[i] = 0.5 - 0.5 * cos(2 * M_PI * i / REG_N);
    deep = reg_render(22);
    dust = reg_render(23);
    printf("log_drum:   MIDI 45 (A2) + TRN -12, 0.30 s: LOG DEEP %.2f Hz, LOG DUST %.2f Hz\n", deep, dust);
    check(deep > 45.0 && deep < 65.0,
          "LOG DEEP: the low note's fundamental is in the 45..65 Hz band (about 55 Hz)");
}

static void t_glide(void)
{
    enum { N0 = 48, N1 = 55 };
    track_t *t = &trk[0];
    voice_t *v = &t->v[0];
    int32_t out[CTL * 2];
    uint32_t b, started = 0, onset = 0, arrived = 0, glide_blocks = 0;
    uint32_t attack = 0, env_rose = 0, nonmono = 0, intermediate = 0, changes = 0;
    int32_t onset_env = 0, env_max = 0, start_q = 0, prev_q = 0;
    const int32_t target_q = N1 * 16 * 256;
    double ms = -1.0;
    uint8_t n;

    fresh_tracks();
    host_preset(t, 0, 22);                              /* LOG DEEP, including MONO's legato voice mode */
    t->p[P_SLEN] = 4;
    t->p[P_SGATE] = 127;
    n = N0;
    put_step(t, 0, 1, &n, ST_NOTE, SF_SLIDE);          /* hold this note into the next one */
    n = N1;
    put_step(t, 1, 1, &n, ST_NOTE, 0);
    transport_req = 1;
    for (b = 0; b < 250u && !arrived; b++) {
        int32_t env_before = v->env;
        int32_t q_before = (v->pitch_cur << 8) + v->pitch_frac;
        uint8_t note_before = v->note;
        mix_block(out, CTL);
        if (v->active && v->gate && v->note == N0)
            started = 1;
        if (!onset && started && note_before != N1 && v->note == N1 && v->pitch16 == N1 * 16) {
            onset = 1;
            onset_env = env_max = env_before;
            start_q = prev_q = q_before;
        }
        if (onset) {
            int32_t q = (v->pitch_cur << 8) + v->pitch_frac;
            glide_blocks++;
            attack |= v->stage == 1u;
            env_rose |= v->env > onset_env;
            if (v->env > env_max)
                env_max = v->env;
            nonmono |= q < prev_q;
            changes += q != prev_q;
            intermediate += q > start_q && q < target_q;
            prev_q = q;
            if (q == target_q)
                arrived = 1;
        }
    }
    if (onset)
        ms = (double)glide_blocks * CTL * 1000.0 / FS;  /* the slide's block through the block that arrives */
    printf("log_drum:   LOG DEEP slide: %.1f ms, %u control blocks, %u intermediate blocks, envelope %d -> max %d\n",
           ms, glide_blocks, intermediate, onset_env, env_max);
    check(onset && arrived && start_q == N0 * 16 * 256 && ms >= 30.0 && ms <= 55.0,
          "LOG DEEP: a sequencer SLIDE arrives in 30..55 ms (about 40 ms)");
    check(onset && arrived && intermediate >= 4u && changes >= 4u && !nonmono,
          "LOG DEEP: the slide passes through intermediate pitches, not a jump");
    check(onset && arrived && !attack && !env_rose,
          "LOG DEEP: no attack stage or envelope rise across the slide (no retrigger)");
    transport_req = 2;
    mix_block(out, CTL);
}

int main(void)
{
    t_patches();
    t_register();
    t_glide();
    printf("log_drum: %s\n", fails ? "FAILED" : "all checks ok");
    return fails;
}