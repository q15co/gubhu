---
version: alpha
name: GUBHU
description: Amapiano-inspired firmware identity for the M-VAVE FM1, rooted in contemporary East Rand Kwaito culture and Zulu-inspired beadwork geometry.
colors:
  primary: "#6A00FF"
  secondary: "#D71B8A"
  tertiary: "#FF9A00"
  neutral: "#F8EFE3"
  ink: "#0B0316"
  surface: "#150B22"
  surface-raised: "#24132F"
  surface-hover: "#321844"
  border: "#52365F"
  on-surface: "#F8EFE3"
  muted: "#C7B5CE"
  on-primary: "#FFFFFF"
  on-secondary: "#FFFFFF"
  on-tertiary: "#0B0316"
  fuchsia: "#D71B8A"
  sunset: "#FF544E"
  amber: "#FF9A00"
  maize: "#FFD234"
  turquoise: "#25C5C4"
  success: "#50D3A5"
  warning: "#FFD234"
  error: "#FF6969"
  focus: "#FFD234"
  light-surface: "#F8EFE3"
  light-on-surface: "#0B0316"
  light-muted: "#54445D"
  # FM1 screen variants. The declared primary and secondary do not meet
  # normal-text contrast on Ink; border does not meet the UI-graphics gate.
  # These are 35%, 20% and 20% mixes respectively toward on-surface.
  primary-screen: "#9C54F5"
  secondary-screen: "#DE459C"
  border-screen: "#735B79"
typography:
  display:
    fontFamily: "Exo 2"
    fontSize: 48px
    fontWeight: 800
    lineHeight: 1.05
    letterSpacing: -0.025em
  headline-lg:
    fontFamily: "Exo 2"
    fontSize: 32px
    fontWeight: 700
    lineHeight: 1.15
  headline-md:
    fontFamily: "Exo 2"
    fontSize: 24px
    fontWeight: 700
    lineHeight: 1.2
  body-md:
    fontFamily: Inter
    fontSize: 16px
    fontWeight: 400
    lineHeight: 1.5
  body-sm:
    fontFamily: Inter
    fontSize: 14px
    fontWeight: 400
    lineHeight: 1.45
  label-md:
    fontFamily: Inter
    fontSize: 13px
    fontWeight: 600
    lineHeight: 1.3
  telemetry:
    fontFamily: "JetBrains Mono"
    fontSize: 14px
    fontWeight: 500
    lineHeight: 1.3
  hardware-label:
    fontFamily: "Exo 2"
    fontSize: 12px
    fontWeight: 700
    lineHeight: 1.2
    letterSpacing: 0.08em
rounded:
  none: 0px
  xs: 4px
  sm: 8px
  md: 12px
  lg: 20px
  xl: 28px
  full: 999px
spacing:
  none: 0px
  xs: 4px
  sm: 8px
  md: 16px
  lg: 24px
  xl: 32px
  xxl: 48px
components:
  app-shell:
    backgroundColor: "{colors.ink}"
    textColor: "{colors.on-surface}"
  panel:
    backgroundColor: "{colors.surface}"
    textColor: "{colors.on-surface}"
    borderColor: "{colors.border}"
    rounded: "{rounded.lg}"
  button-primary:
    backgroundColor: "{colors.primary}"
    textColor: "{colors.on-primary}"
    rounded: "{rounded.md}"
  button-secondary:
    backgroundColor: "{colors.surface-raised}"
    textColor: "{colors.on-surface}"
    rounded: "{rounded.md}"
  button-warm:
    backgroundColor: "{colors.maize}"
    textColor: "{colors.ink}"
    rounded: "{rounded.md}"
  active-step:
    backgroundColor: "{colors.maize}"
    textColor: "{colors.ink}"
    rounded: "{rounded.xs}"
  inactive-step:
    backgroundColor: "{colors.surface-raised}"
    textColor: "{colors.on-surface}"
    rounded: "{rounded.xs}"
  status-error:
    backgroundColor: "{colors.error}"
    textColor: "{colors.ink}"
  light-panel:
    backgroundColor: "{colors.light-surface}"
    textColor: "{colors.light-on-surface}"
firmware:
  background: "{colors.ink}"
  accent: "{colors.maize}"
  panel: "{colors.surface}"
  empty: "{colors.surface-raised}"
  empty-outline: "{colors.primary-screen}"
  line: "{colors.border-screen}"
  dim: "{colors.primary-screen}"
  labels: "{colors.muted}"
  secondary: "{colors.maize}"
  values: "{colors.on-surface}"
  track-1: "{colors.primary-screen}"
  track-2: "{colors.turquoise}"
  track-3: "{colors.maize}"
  track-4: "{colors.secondary-screen}"
---

# GUBHU — Design System

## Overview

**GUBHU** is an amapiano-inspired firmware identity for the M-VAVE FM1. Its cultural point of departure is the East Rand of Johannesburg: Kwaito-era streetwear, township ingenuity, contemporary electronic music, and the precise geometry of Zulu beadwork. The feeling is **confident, warm, rhythmic, tactile, inventive, and proudly local without being nostalgic or stereotypical**.

The signature mark is a **dark Kwaito-style bucket hat with a colourful geometric beadwork band**, positioned over a custom rounded lowercase `gubhu` wordmark. Supporting lockups may say `FM1 FIRMWARE`. The equalizer motif connects the identity to rhythm and sequencing. The hat is the hero; the wordmark is the durable identifier; the equalizer is optional decoration.

The original generated logo and brand-board are **visual references**, not verified vector masters. Do not silently recreate their lettering with a generic font and call it an exact logo. Store approved artwork in versioned assets and prefer those files to AI approximations. The colours below are **proposed implementation tokens based on the concept artwork**, not measured brand-production values.

Brand principles:

- **Contemporary, not costume:** interpret cultural references through deliberate geometry and modern composition, not a collage of generic “African” motifs.
- **Music first:** rhythm, syncopation, stepped sequences and deep bass inspire layouts and motion.
- **Hardware aware:** readable at a glance, high contrast, and usable with buttons, encoders and constrained screens.
- **Rich art, restrained UI:** full beadwork, neon and fabric detail belong in hero artwork and packaging; functional controls remain clean.
- **Respectful specificity:** avoid presenting any invented beadwork arrangement as a canonical Zulu message or ceremonial symbol. Consult cultural practitioners before claiming a particular pattern's meaning.

## Colors

The brand lives primarily on deep violet-black **Ink** (`#0B0316`) with warm **Ivory** (`#F8EFE3`) typography. Electric **Indigo** (`#6A00FF`) drives interaction, **Fuchsia** (`#D71B8A`) provides musical energy, and **Amber** (`#FF9A00`) / **Maize** (`#FFD234`) add the warmth of a Gauteng sunset. **Sunset** (`#FF544E`) and **Turquoise** (`#25C5C4`) are supporting artwork accents, not default control colours.

| Role | Token | Hex | Use |
| --- | --- | --- | --- |
| Main background | `ink` | `#0B0316` | Application shell, logo backdrops |
| Raised surface | `surface` | `#150B22` | Panels, menu regions |
| Raised interactive | `surface-raised` | `#24132F` | Buttons, inactive pads |
| Primary | `primary` | `#6A00FF` | Selected navigation, primary action |
| Secondary | `secondary` | `#D71B8A` | Music emphasis, modulation |
| Warm accent | `tertiary` | `#FF9A00` | Highlights and illustrations |
| Beat accent | `maize` | `#FFD234` | Active steps, focus, beat indicator |
| Foreground | `on-surface` | `#F8EFE3` | Body copy and labels |
| Supporting text | `muted` | `#C7B5CE` | Metadata, secondary descriptions |

The 240x240 RGB565 firmware uses the additional `primary-screen`, `secondary-screen` and `border-screen` variants.
They are not replacements for the brand colours: they are the minimum deliberate lightening steps used here to clear
normal-text (4.5:1) or meaningful-graphics (3:1) contrast after RGB565 quantisation. `tools/gen_theme.py` compiles the
`firmware` role table above and checks the resulting panel values. The existing `MONO` palette remains the mandatory
monochrome rendering; GUBHU does not replace it.

**Gradient:** For artwork and larger canvases only, blend Indigo → Fuchsia → Sunset → Amber → Maize. Avoid placing important text over a gradient without a solid contrast-preserving backing.

**Contrast:** Check actual rendered text/background combinations against WCAG 2.2 AA: 4.5:1 for normal text, 3:1 for large text and meaningful UI graphics. On primary purple use white; on maize/amber use ink. Never rely on colour alone to convey a sequencer state. The declared colour values are targets, not a claim that every combination passes.

**Light mode:** Ivory background, Ink foreground, and dark-purple interactive surfaces. Preserve the brand but do not simply invert glow effects or reuse the dark-mode purple for small text without checking contrast.

## Typography

**Exo 2** is the expressive interface headline font; **Inter** is the utilitarian body font; **JetBrains Mono** is reserved for BPM, bar/beat positions, parameter values, file names and debug information. These are implementation font choices, **not a substitute for the custom logo lettering**.

Use short, direct headings. Prefer sentence case in menus; uppercase tracking is appropriate for tiny section labels only when legibility allows. BPM and numeric telemetry should use tabular numerals when supported. On tiny displays use bundled bitmap/system fonts if necessary, while retaining relative hierarchy and spacing. The FM1 specifically retains its bundled Terminus bitmap fonts; the decision and its reversal triggers are recorded in [0003: FM1 bitmap typography](docs/decisions/0003-fm1-bitmap-typography.md).

Never shrink essential labels below a legible physical size merely to fit a mockup. On hardware, screen resolution, pixel density and viewing distance determine actual font rasterization.

## Layout

Use a **4px base grid**, with 8px, 16px, 24px and 32px increments. For desktop or companion web UI, use a compact left navigation rail, a clear central workspace, and an optional right-side inspector. For touch screens, keep core actions reachable and do not hide critical transport controls behind decorative layers.

The sequencer is the focal element. Prioritize **transport → pattern / track → steps → selected parameter → secondary settings**. Preserve stable placement for BPM, play/stop, pattern name, save state and connectivity indicators.

For embedded FM1 layouts, **do not assume a particular display size, colour depth, touch capability, RAM budget or control mapping** until hardware constraints are confirmed. Provide monochrome and reduced-colour fallbacks, deterministic navigation order, and legible focus indicators. Decorative brand elements must never compete with performance-critical feedback.

## Elevation & Depth

The product UI is mostly flat and matte. Create hierarchy with subtle surface contrast, a 1px border, and spacing before resorting to shadows. Purple and sunset-coloured glows are allowed for launch artwork, active musical moments, or hero imagery, but avoid permanent neon halos around every control. The bucket hat may have realistic textile depth in marketing; app chrome should not imitate its fabric texture.

Motion should communicate music and state: a beat pulse, a playhead stepping through a grid, or a short confirmation. Respect reduced-motion preferences in companion applications. Do not animate controls in a way that obscures timing or introduces misleading latency.

## Shapes

The wordmark is **rounded, broad, lowercase and geometric**. UI corners are softly rounded, not pill-shaped by default. Sequencer pads are square-ish rounded rectangles with consistent dimensions and clear spacing.

The signature **hat silhouette** must be recognisable without texture. Develop three production variants: detailed full-colour hat, simplified flat hat, and single-colour silhouette. The beadwork strip uses stepped diamonds, chevrons and repeated triangles with intentional rhythm; it should not be pasted onto every surface.

The **equalizer** uses discrete rounded bars/blocks rather than a continuous sine wave. It can express beat energy, but must not imply actual audio analysis unless wired to real audio data.

## Components

**Brand lockups:**

- **Primary:** bucket hat above `gubhu`, with optional `FM1 FIRMWARE` and equalizer beneath.
- **Compact:** simplified hat beside or above `gubhu`; omit subtitle and equalizer.
- **Icon:** hat silhouette or custom `g` letterform, not both at illegibly small sizes.
- **Monochrome:** single-colour mark for silk-screening, boot logos and limited displays.
- **Clear space:** at least the height of the lowercase `u` around the mark; do not crop the brim.
- **Minimum size:** validate empirically on target output. Do not use the detailed beadwork band where its shapes merge.

**Navigation:** The active item uses a strong purple field and a second non-colour cue (position, border or icon). Navigation labels remain readable in dark and light modes.

**Transport:** Play/stop, record, pattern selection and BPM must be distinguishable by shape, label and state. Destructive actions require explicit confirmation where recoverability is limited.

**Step sequencer:** Use maize for the currently active step, fuchsia or purple for programmed notes, muted surfaces for empty steps, and an additional outline/marker for the playhead. On the FM1, an empty GUBHU step also has a visible rectangular outline: `surface-raised` alone is not the empty-state signal. Distinguish *selected*, *playing*, and *triggered* states. Do not infer that the colour palette itself represents musical velocity.

**Knobs / parameter controls:** Present value, unit and range. Provide keyboard/encoder alternatives and explicit focus. Prefer direct values over skeuomorphic knobs when precision matters.

**Cards and panels:** Matte dark surfaces with restrained borders. Reserve beadwork borders for rare section dividers, splash screens and merch, never every card.

**Notifications:** Brief, plain-language feedback for saved patterns, failed writes, connectivity and firmware updates. Do not rely solely on a colour-coded toast.

**Loading / boot:** Show the simplified hat or wordmark on a solid Ink background. A single restrained stepped equalizer animation is acceptable if boot timing permits. A monochrome fallback is mandatory.

**Accessibility and hardware quality gates:** Test contrast, legibility, focus states, encoder/button operation, low-colour rendering, small-screen clipping, power-on states, reduced-motion behaviour where supported, and state feedback under actual performance conditions.

## Do's and Don'ts

| Do | Don't |
| --- | --- |
| Express the East Rand through modern confidence, rhythm and craft | Use generic safari imagery, animal prints or tourist motifs |
| Keep the bucket hat as the recognisable signature | Turn every UI element into a literal hat or bead pattern |
| Use culturally informed geometric accents sparingly | Claim arbitrary beadwork has a specific traditional meaning |
| Let dark surfaces and ivory type provide calm | Fill every panel with gradients, glow and saturated colour |
| Make the sequencer the functional visual hero | Prioritize branding over live performance controls |
| Ship compact, monochrome and low-resolution marks | Downscale the detailed illustration until it becomes noise |
| Preserve the exact lowercase spelling `gubhu` | Automatically uppercase the wordmark or change its lettering |
| Validate actual hardware constraints before implementation | Assume the FM1 has a colour touch display or GPU effects |

**Agent implementation contract:** Treat the YAML tokens above as normative values and the prose as application guidance. If approved brand assets are unavailable, use a text-only `gubhu` placeholder rather than inventing a replacement logo. Keep design tokens centralized. Check contrast for each component pairing and verify the actual hardware's constraints before shipping firmware UI changes.

**References:** Google Labs `DESIGN.md` alpha specification: https://github.com/google-labs-code/design.md/blob/main/docs/spec.md ; project: https://github.com/google-labs-code/design.md .
