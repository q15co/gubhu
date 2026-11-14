# Issue #6 chrome/state slices

Target: `feat/6-chrome-states`, 240 x 240 FM-1, strip renderer only.
Baseline at `cdce4dc`: application **568544 B**; loader 6920 B; package 610076 B.
Baseline `tests/golden.txt` SHA-256: `94893254d2db78a4f4fb94163bc6cf53e894e3cff6149374710c76347b4739ca`.

The DESIGN.md references for this work are Colors / Contrast (state must not be colour-only), Typography (tracked tiny section labels, monospaced telemetry), Layout (stable transport/pattern/step placement), Elevation & Depth (matte surfaces and 1 px rules), Components / Step sequencer (maize active, purple/fuchsia programmed, surfaced empty, separate selected/playing/triggered cues), and Components / Notifications (plain-language, non-colour feedback).

## Slice 1 — sequencer state geometry and two-palette renderer

Files: `firmware/src/gfx.c`, `firmware/src/ui_draw.c`, `firmware/src/ui_song.c`, `firmware/src/ui_studio.c`, `firmware/src/ui_layers.c`, `tests/ui_pages_test.c`, `SLICE-NOTES.md`.

- Image: **568544 -> 569648 B (+1104 B)**. Loader and package sizes unchanged (6920 B and 610076 B).
- What was wrong: several grids used one-pixel marks or the same fill for different meanings; selected, playing and triggered could collapse into one bright marker. MONO had no empty-pad outline.
- What changed: shared surfaced/outlined empty pads, selected frames, playhead carets/lines and inset trigger marks across the footer, pattern, roll, scale, slicer, arranger, track rows, drum grid and SEQ layer. Active cells use the existing accent token (`C_WHITE`, maize under GUBHU). All marks are drawn into the existing strips; `CV_MAX` and the blit pattern are unchanged.
- Renderer: `ui_pages_test OUTDIR mono|gubhu` selects the initial palette; the old one-argument MONO behaviour is unchanged. Added deterministic PATTERN and SLICER captures and a pixel assertion for selected/playing/triggered/programmed/empty geometry.
- Verified: firmware build; both palette runs each produced **93 PPM pages** in `build/evidence-slice1/{mono,gubhu}` and passed their UI checks plus 20000-frame fuzz. The new state-pixel assertion passes in both palettes. `tests/golden.txt` is unchanged; the host regression reports **185 golden renders, 0 changed, 0 gone**.
- Host suite: `build/host-slice1.log` completed every test with UBSan/ASan enabled. ThreadSanitizer was disabled by a compiler wrapper because its runtime cannot disable ASLR in this container. The script's only failures are `sample_render`, `grain_render`, `drums_mix`, and `fm1_alnk0_irq` target costs. An untouched `cdce4dc` build in `build/baseline-src` fails the same four checks with identical costs (593, 2387, 6811, 268); no budgets were rewritten.
- Hardware unverified: physical legibility, viewing distance and panel timing. The host LCD double checks bounds; it is not a hardware timing measurement.
- ADR: none for the state geometry; it directly implements DESIGN.md / Step sequencer using the existing `pal_step_outline` mechanism.

## Slice 2 — chrome tokens and track colours

Files: `firmware/src/gfx.c`, `firmware/src/ui.c`, `firmware/src/ui_draw.c`, `firmware/src/ui_layers.c`, `firmware/src/ui_menu.c`, `firmware/src/ui_song.c`, `firmware/src/ui_studio.c`, `firmware/src/ui_vis.c`, `tests/ui_pages_test.c`, `docs/decisions/0006-fm1-screen-primary-purple.md`, `SLICE-NOTES.md`.

- Image: **569648 -> 569576 B (-72 B)**; **568544 -> 569576 B (+1032 B)** from the original baseline. Loader/package remain 6920/610076 B.
- What was wrong: panel strips used the shell background, several focus states relied on a colour change, and visualiser effects retained hard-coded legacy track colours.
- What changed: added the semantic `C_PANEL` role (GUBHU Surface, derived custom-theme surface, legacy background), retained Ink for the shell, used existing strips for 1 px maize focus frames, added one-pixel tracking only to short Tamzen section labels, and routed remaining visualiser track colours through the existing `TE_COL`/`TE_MID`/`TE_DIM` mapping. No new fonts, canvas, full-frame fill or DMA pass.
- Verified: firmware build; standalone `font_test`; both MONO and GUBHU UI runs, 93 pages each, state-pixel assertions and 20000-frame fuzz. `C_PANEL` and legacy/GUBHU track roles are asserted by the palette test. ADR 0006 records why `#9C54F5`, not declared `#6A00FF`, is used on the FM-1 (2.9:1 fails the 3:1 graphics gate; RGB565 result 4.97:1 on Ink).
- Full host suite: `build/host-slice2-final.log` completed with the same four baseline target-budget failures documented in slice 1; all other checks passed, including 185/185 unchanged audio goldens. The earlier standalone `gfx.c` dependency found by `font_test` was fixed and retested.
- Hardware unverified: perceived Surface/ink separation, label tracking and focus visibility on the physical panel.
- ADR: `0006-fm1-screen-primary-purple.md`.


## Slice 3 — dialogs and notifications

Files: `firmware/src/gfx.c`, `firmware/src/ui_draw.c`, `firmware/src/ui_layers.c`, `firmware/src/ui_song.c`, `firmware/src/ui_studio.c`, `firmware/src/ui_vis.c`, `tests/ui_pages_test.c`, `SLICE-NOTES.md`.

- Image: **569576 -> 569868 B (+292 B)**; **568544 -> 569868 B (+1324 B)** from the original baseline. Loader/package remain 6920/610076 B.
- What was wrong: confirmation, record-arm, saved/write-failure and connectivity feedback relied on coloured indicators in addition to its wording.
- What changed: one shared Tamzen message renderer in `gfx.c` adds a framed non-colour symbol (`?`, `!`, `+`, `R`, `U`, or `i`) without changing message text. Record/arm has an `R` in its open/filled ring, clear/save holds have `!`/`+` in the progress ring, and connected USB has an outlined badge. Layer titles yield to the message so no badge covers wording. The helpers live in `gfx.c` because isolated song tests compile `ui_song.c` without `ui.c`.
- Verified: firmware build; isolated `song_ui_test` and `song_arm_test`; both MONO and GUBHU runs each produced **99 PPM pages** in `build/evidence-slice3/{mono,gubhu}` and passed 20000-frame UI fuzz. Six deterministic `notice-*` captures cover confirmation, write failure, saved state, record arm, save hold and USB. Assertions cover badge kinds, unchanged wording, USB outline geometry and state pixels.
- Audio: `tests/golden.txt` remains at the baseline SHA-256; the final host regression reports **185 golden renders, 0 changed, 0 gone**.
- Full host suite: `build/host-slice3-final.log` completed the full host script. The only failures are the four baseline target costs documented in slice 1 (`sample_render` 593, `grain_render` 2387, `drums_mix` 6811, `fm1_alnk0_irq` 268); all other checks passed. UBSan/ASan ran; ThreadSanitizer was disabled by a compiler wrapper because its runtime cannot disable ASLR in this container. Node-based web tests were skipped because Node is unavailable..
- Hardware unverified: physical symbol legibility and visibility during real writes, recording and USB connect/disconnect transitions. No write hardware or physical FM-1 was available.
- ADR: no new decision; the implementation follows DESIGN.md / Notifications directly.

## Final verification and deliberate exclusions

- Firmware image **569868 B**, **+1324 B** over `cdce4dc`; loader **6920 B**, package **610076 B** unchanged. Both MONO and GUBHU produced **99 PPM pages** and passed all state/notification assertions plus 20000-frame UI fuzz. Isolated song tests passed. The full host regression reports **185/185 golden renders unchanged**, zero health/voice/routing failures and zero crashes. `tests/golden.txt` SHA-256 is unchanged at `94893254d2db78a4f4fb94163bc6cf53e894e3cff6149374710c76347b4739ca`. The complete host script exits 1 only for the four pre-existing target-budget failures; no audio baselines or hashes were changed.
- Deliberately no changes to the audio path, palette order, persisted projects, fonts, generated theme tooling, boot splash or ABOUT work already landed. `params.c` was audited: its short labels already use uppercase Tamzen and values use the same fixed-cell face, so it needed no source change. No hardware flashing, push or PR.

## Follow-up: the three states left open in the merged slices

The branch above was merged as #27 with three items unresolved. All three are now settled, on `fix/6-screen-followups`.

**1. The selection ring shared the accent with the playing fill.** `step_select()` drew its frame in `C_WHITE`, which is maize under GUBHU, and `step_pad()` fills a playing step with `C_WHITE` too. On a track 3 step the ring and the fill were the same pixel; on track 1 they were 2.7:1 apart. Selection now has its own token, `pal_sel`: `values` `#F8EFE3` under GUBHU, unchanged white for the legacy palettes and the custom theme's own accent. Where the palette sets `pal_sel_gap` (GUBHU only) a one-pixel Ink gap is drawn inside a *lit* pad's border before the ring, so the ring never sits directly on a track colour: ivory on Ink measures about 15:1, and the gap on track 3's maize about 14:1. Empty pads keep their outline and take no gap. The three call sites (`graph_steps`, the footer bank, the drum lanes) pass the pad's lit state.

Verified by `tests/ui_pages_test.c`: the frame pixel is asserted to be `C_SEL`, and a new check fails if a palette that draws a gap ever has `pal_sel == pal_hi`, which is the regression itself. Both palettes pass, 99 PPM pages each.

**2. Empty pads use `TE_G1`, not the panel token.** This is not a defect and no code changed. `TE_G1` is the `firmware.empty` role (`#24132F`), which is `surface-raised`; `DESIGN.md` / Step sequencer says "muted surfaces for empty steps" and then names the outline as the signal: `surface-raised` alone is not the empty-state signal. The darker `firmware.panel` `#150B22` fills the strips the pads sit on, so filling a pad with it would leave the pad visible only as its outline and lose the surface the design asks for. The outline (`#9C54F5`, 4.97:1 on Ink, ADR 0006) does the work.

**3. Visualiser style 12 was the upstream SLOOP logo.** Replaced with `BEADS`, the fork's beadwork strand, at 16 beads on a thread, one per step and four to a track's colour, strung up by each track's level. Rationale and alternatives in ADR 0008. The style name in `VIS_NAME` and the header comment changed with it; `vis_style` persists by index, so a saved setting still selects the twelfth style.

### Sizes and tests on this follow-up

- Selection ring alone: **569868 -> 570220 B (+352 B)**.
- With the visualiser change: **570220 -> 569044 B (-1176 B)**; net **-824 B** against #27, **+500 B** against `cdce4dc`.
- Loader **6920 B**, package **610076 B**, `golden.txt` SHA-256 unchanged.
- Both palettes: **99 PPM pages**, all state/notification assertions, 20000-frame fuzz, isolated song tests.
- Full host script: `build/host-followups.log`, only the four pre-existing target-budget overruns.
- Hardware unverified: the gap and the ring on a real panel at arm's length, and the dim bead's contrast under the theme's surface colours.
