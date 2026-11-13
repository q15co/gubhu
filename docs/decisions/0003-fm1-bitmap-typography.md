# 0003: FM1 bitmap typography

Date: 2026-11-13. Status: superseded by [0004](0004-fm1-screen-type-spleen.md), which replaces Terminus
with Spleen after the fork's identity review. The reasoning below stands as the record of why the established
face was kept for the colour-role work, and of the conditions a replacement had to meet.

## Context

The design system names Exo 2 for expressive headings, Inter for body copy and JetBrains Mono for
telemetry. The FM1 is not the companion web UI: it has a 240x240 RGB565 panel, no framebuffer, and a
strip renderer. Its established screen type is generated from bundled Terminus sources by
`tools/gen_font.py`, at 8x16 for labels and 16x32 for large values. The large face can also reuse the
small one's 1-bit glyphs at 2x. Existing tests compare every glyph's pixels and exercise clipping under
random UI use.

The three design-system families cannot simply be selected on the panel. Shipping them requires approved
font files, hand-cut and licensed bitmap subsets, new raster data, and another legibility and flash-budget
pass. Merely substituting an automatically scaled TrueType font would not establish readability at the
FM1's physical size. None of that is necessary for the colour-role work in issue #6.

## Decision

1. **Retain Terminus on the FM1 for all functional text.** Keep the existing 8x16 and 16x32 hierarchy,
   1-bit glyph representation and generator. Do not add a runtime font rasteriser or partially converted
   Exo 2, Inter or JetBrains Mono subsets in this theme change.
2. Exo 2, Inter and JetBrains Mono remain the normative choices for the companion web UI and approved
   larger-canvas artwork. They are not a promise that those font files are embedded in the firmware.
3. **The existing `MONO` palette is the monochrome fallback.** It remains built-in palette index 4 with
   its existing grey ramp. A second token-derived grey palette would spend flash and add a selection
   without making the fallback more monochrome.
4. The custom GUBHU wordmark, when approved artwork is unavailable, remains a text-only placeholder;
   Terminus is not presented as a recreation of that lettering.

## Consequences

- The firmware does not gain Exo 2's brand voice in its labels. Hierarchy comes from the established two
  sizes, spacing, colour roles and non-colour state cues instead.
- The existing glyph goldens, flash layout and localisation behaviour stay stable.
- `DESIGN.md` links here so keeping Terminus is an explicit hardware adaptation, not an omitted font port.

## What would reverse this

Revisit the decision only with licensed source fonts and a reviewed, hand-cut bitmap subset, plus host
pixel tests, physical-panel legibility evidence and a flash/RAM budget showing that the complete subset
fits. A future display with a framebuffer or a different physical size would also reopen it; a preference
for matching desktop typography alone would not.
