# 0005: FM1 screen type, Tamzen

Date: 2026-11-13. Status: accepted. Replaces [0004](0004-fm1-screen-type-spleen.md).

## Context

0004 replaced Terminus with Spleen for two reasons: Terminus reads as console output rather than as the warm,
broad lettering DESIGN.md asks for, and its large face was the small one's pixels at 2x.

Spleen answers both, and the review that produced it put Spleen, Tamzen and the existing Terminus side by
side at true panel size. The verdict on that comparison was Tamzen: at 8x16 its bold weight holds the small
labels the way Terminus Bold did, while its rounder, wider letterforms and its lowercase (the `gubhu`
wordmark) carry the warmer character Spleen's single, lighter weight gives up. On a 240 px panel read at arm's
length, the weight of the small text mattered more than a drawn large face.

## Decision

1. The FM1's screen type is **Tamzen Bold 8x16** (a fork of Tamsyn by Scott Fial, extended by Sunaku, under
   the Tamsyn licence: permission to use, copy, modify and distribute, no warranty). Labels, units and status
   draw at 1x from Latin-1, 32..255.
2. Values and titles are **the same glyphs at 2x**. Tamzen has no 16x32 size, so the large face is an
   enlargement again; that was the trade for the weight, and it is the geometry the layout was built on.
3. The cell and the advances are unchanged (8 px advance, 12 px S bitmap, 24 px L bitmap), so no screen moves
   and the layout hash of the Terminus build still holds.
4. Capitals only, as before: `gfx.c` folds a-z onto A-Z. The four lowercase cells `splash.c` carries for the
   wordmark are cut from Tamzen, so the boot screen and the rest of the panel are the same face.
5. The web editor's font follows the panel: `tools/gen_webfont.py` builds its TrueType from
   `tamzen-8x16b.bdf`.
6. The Spleen BDFs and their licence file are removed; `LICENSING.md` records the Tamsyn terms.

## Evidence

- `tools/gen_font.py`: 5376 B for S at 1 bit a pixel; L reuses S's data at 2x, so it costs nothing.
- `tests/font_test.c`: every character 32..255 compares against the BDF's own pixels. The reader now places
  each glyph by its own BBX the way the generator does, because 16 of Tamzen's glyphs are 17 rows tall and
  their topmost row is clipped above the cell. Hashes regenerated; advances and sizes asserted; the 4-bit path
  checked for both faces.
- `tests/ui_pages_test.c` and `tests/defaults_test.c` pass, and the PPM frames were reviewed at true size.
- Flash: the image returns to 568,368 B, because the large face no longer carries its own drawing. The whole
  12.9 KB of XIP headroom is back.

## Open

- **Physical-panel legibility has not been observed.** All evidence is emulator output. The case for Tamzen
  rests on its weight holding up at arm's length, which is exactly what the panel, not the emulator, decides.
- Tamzen covers 175 of the 224 Latin-1 code points the panel addresses. The missing ones are punctuation and
  fractions (16 glyphs: section sign, ordinal indicators, not/registered/macron, plus-minus, superscripts,
  micro, pilcrow, middle dot, fractions). The firmware draws only `Ü` and `ü` outside ASCII, both present, and
  the rest fall back to `?` exactly as an absent glyph always has.
- The editor's inlined font no longer carries arrows, box drawing or music symbols, which Tamzen lacks. Its
  Latin labels are unaffected; a symbol outside the Latin blocks falls back to the browser's font, as the
  Japanese UI text already does.

## What would reverse this

Panel evidence that the 2x large face reads worse than a drawn one, or that Tamzen's rounded letterforms
soften the small labels at arm's length. Either returns 0004's structure: a face with its own 16x32 drawing.
