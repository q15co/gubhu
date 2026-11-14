# 0006: Use the screen-safe purple on the FM-1

DESIGN.md declares `#6A00FF` as the brand primary, but on Ink (`#0B0316`) it
has only about **2.9:1** contrast. That fails the 3:1 meaningful-graphics gate
for focus marks, track identity and the empty-step outline. Normal text needs
4.5:1 as well.

The FM-1 therefore uses the declared `primary-screen` variant, `#9C54F5`, for
track 1, dim labels and empty-step outlines. It is the deliberate 35% mix of
primary toward on-surface already specified by DESIGN.md, not a replacement
for the brand primary in artwork or other surfaces. After RGB565 quantisation,
`tools/gen_theme.py --verify` measures **4.97:1** on Ink and **4.32:1** on the
empty surface; both clear their applicable gates. The other legacy palettes
keep `TE_COL_DEF`, and saved projects retain their own palette and custom theme
track colours.