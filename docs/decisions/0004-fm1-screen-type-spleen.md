# 0004: FM1 screen type

Date: 2026-11-13. Status: accepted. Replaces [0003](0003-fm1-bitmap-typography.md).

## Context

0003 kept Terminus for the colour-role work and set the conditions under which the screen type could be
revisited: licensed source fonts, a reviewed bitmap subset, host pixel tests, physical-panel legibility
evidence, and a flash and RAM budget showing the subset fits.

The GUBHU identity review turned up two misfits. Terminus is a console face: crisp and neutral, but it reads
as terminal output rather than as the warm, broad, crafted lettering DESIGN.md asks for, and its lowercase,
seen in the wordmark, is narrow and mechanical. And the large face was the small face's pixels at 2x, so every
stroke in a title was exactly 2 px: no thin stroke anywhere, no drawn hierarchy, which is why the menu's
`ALL KEYS` and `LOW` read as an enlarged caption rather than as large type.

## Decision

1. The FM1's screen type is **Spleen** (BSD-2-Clause): 8x16 for labels, units and status (Latin-1, 32..255),
   and 16x32 for values and titles (digits, signs, capitals, 32..95). Both are **drawn at their own size**;
   neither is an enlargement of the other.
2. The cell and the advances are the ones the layout was built on (8 px and 16 px), so no screen moves and
   `text_w` is unchanged. Only the large face's bitmap width changes, from 24 px to 20 px.
3. Capitals only, as before: `gfx.c` still folds a-z onto A-Z. The four lowercase cells `splash.c` carries
   for the wordmark are re-cut from Spleen, so the boot screen and the rest of the panel are the same face.
4. The web editor's font follows the panel: `tools/gen_webfont.py` builds its TrueType from
   `spleen-8x16.bdf`, so the editor and the screen still show the same letters.
5. The Terminus BDFs and their licence file are removed. Spleen's licence, BSD-2-Clause, is GPL-3.0-compatible
   and is recorded in `LICENSING.md`.

## Evidence

- `tools/gen_font.py`: 5376 B for S and 5120 B for L, both at 1 bit a pixel.
- `tests/font_test.c`: every character 32..255 compares against the BDF's own pixels; the glyph hashes are
  regenerated for both faces; the advances and the 1-bit sizes are asserted; the 4-bit path is checked for S.
- `tests/ui_pages_test.c` and `tests/defaults_test.c` pass, and the PPM frames were reviewed at true size.
- Flash: the image grows from 568,368 B to 573,616 B (+5,248 B), leaving 7,948 B of the 581,564 B XIP window;
  RAM, lowram and pool are unchanged and inside their bounds.

## Open

- **Physical-panel legibility has not been observed.** All evidence here is emulator output. Spleen has a
  single weight, so the small labels are lighter than Terminus Bold was; if that reads worse at arm's length
  on the real panel, the fix is a bolder 8x16 face for labels while keeping the 16x32 large face.
- Spleen carries fewer of the arrows and mathematical symbols Terminus did (409 of the 453 code points the web
  editor keeps). The editor's Latin labels are unaffected, and any symbol it does use outside the Latin blocks
  now falls back to the browser's font.

## What would reverse this

Panel legibility evidence against Spleen's stroke weight at arm's length, or flash pressure that forces the
large face back to the small one's pixels at 2x. Either is a one-line change to `SIZES` in
`tools/gen_font.py`, plus the glyph hashes in `tests/font_test.c`.
