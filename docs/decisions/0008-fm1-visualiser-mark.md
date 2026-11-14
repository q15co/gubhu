# 0008: Give the visualiser the fork's own mark

The twelfth visualiser style was `SLOOP`: the upstream logo, drawn from
`tools/gen_logo.py` geometry scaled to 0.86, its sail's four bands lit by the
four tracks, a dial for the master and a hull rocking on the master's wave. The
screens are the instrument's face (`DESIGN.md`), and that mark belongs to the
instrument the fork came from. It is the last place where the upstream brand,
rather than the fork's own, was the picture.

Style 12 is now `BEADS`. Sixteen beads are strung on a thread, one per step of
a bar and four to a track's colour: the bead on the thread is drawn dim, the
beads above it lit, and a track's level decides how far up the thread the beads
reach. A note flashes its track's column and a caret under the thread marks the
step playing. Height carries the level, so the style reads in MONO and in any
palette, not by hue alone.

Alternatives considered:

- **Keep the sail as a homage to SLOOP's authorship.** Rejected: the visualiser
  is the largest surface on the instrument and it is not a credits screen. The
  boot splash already says "BASED ON SLOOP" in words, which is where the
  attribution belongs.
- **Re-skin the sail in the theme's colours.** Rejected: it is the same mark and
  the same problem, and the sail's rotated polygons were the largest part of
  this style's drawing cost.
- **Drop the twelfth style and ship eleven.** Rejected: `vis_style` is kept with
  the settings, so the slot has to keep meaning something.

No new asset. The bead is the boot splash's stepped diamond (`splash.c`), drawn
from the same proportions, so the fork's mark is one geometry in two places
rather than a second drawing. Measured: the image was 570220 B after the
selection-ring change alone and 569316 B with this style, so the style is -904 B
(baseline at `cdce4dc` is 568544 B).

Not verified: appearance on the physical panel, viewing angle and contrast of
the dim bead under `surface` colours. The host renderer writes the same RGB565
pixels the panel receives, but it is not a measurement of the glass.
