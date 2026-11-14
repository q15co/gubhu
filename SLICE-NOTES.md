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
- Full host suite: pending rerun after fixing the standalone `gfx.c` dependency found by `font_test`; that test and both UI suites pass after the fix.
- Hardware unverified: perceived Surface/ink separation, label tracking and focus visibility on the physical panel.
- ADR: `0006-fm1-screen-primary-purple.md`.
