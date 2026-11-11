# gubhu brand

*gubhu* is isiZulu for drum. The name is lowercase everywhere: in prose, in filenames, in the
wordmark.

**[DESIGN.md](../DESIGN.md) is the source of truth for this brand.** Its YAML frontmatter holds the
normative tokens; `web/tokens.css` is generated from it by `tools/gen_tokens.py`. Do not hand-edit
that CSS, and do not introduce a colour that is not in the tokens.

## Position

**An amapiano instrument, not a preset pack.**

gubhu is a fork of SLOOP for the M-VAVE FM-1. It does not try to be every style. It is being tuned in
the genre's tempo range, it treats the log drum as a playable bass rather than a percussion hit, and
its default project is an arrangement whose drop is the log drum entering.

Everything written about it should be checkable against those three claims.

## Files

| File | Use |
| --- | --- |
| `logo.png` | The lockup: hat, wordmark, `FM1 FIRMWARE` label, step strip. 1536x1024 |
| `logo-web.png` | The same, flattened onto the token ink, for pages and the installer |
| `logo-mark.png` | The hat alone, square, for icons and avatars |
| `social-preview.png` | 1280x640, for the repository's social preview |
| `palette.png` | The five-stop ramp as swatches |

The lockup is a raster master. It is not a font, and re-setting "gubhu" in Exo 2 or any other face
does not make a logo. If the artwork has to change, redraw it and re-approve it; do not synthesise a
substitute.

## Colour

| Token | Hex | Role |
| --- | --- | --- |
| `ink` | `#0B0316` | The ground. Everything sits on it |
| `surface` | `#150B22` | Panels and menu regions |
| `surface-raised` | `#24132F` | Buttons, inactive pads, cards |
| `border` | `#52365F` | 1px edges |
| `on-surface` | `#F8EFE3` | Body copy and labels |
| `muted` | `#C7B5CE` | Metadata and secondary text |
| `primary` | `#6A00FF` | Selected navigation, primary action |
| `secondary` | `#D71B8A` | Musical emphasis |
| `tertiary` | `#FF9A00` | Highlights and artwork |
| `maize` | `#FFD234` | Active steps, focus, beat |
| `sunset` | `#FF544E` | Artwork accent |
| `turquoise` | `#25C5C4` | Artwork accent |

The ramp runs indigo to maize, left to right, and it never runs backwards. On a page, plan on roughly
90% ink, 8% ivory and 2% ramp.

**The artwork predates these tokens and differs slightly.** It was rendered on `#010100` with cream
`#FDEBD3`, a violet `#A600F3` and a gold `#F9AD01`. That is the reference render, not the spec: where
they disagree the tokens win, and the artwork should be re-approved against them rather than the CSS
being bent to the render.

Contrast is not an opinion here: `python3 tools/gen_tokens.py --check` computes every text/background
pairing in the token file and fails below WCAG 2.2 AA. It runs as part of the test suite.

## Type

**Exo 2** for display and interface headings, **Inter** for body, **JetBrains Mono** for BPM, bar and
beat positions, parameter values and file names. Tabular numerals wherever numbers change in place.

The fonts are declared with fallback stacks and are **not yet bundled**: a page without them renders
in the system sans and keeps the hierarchy. Shipping a licensed subset is the next step, not a
silent substitution.

The `FM1 FIRMWARE` label is the only uppercase element in the lockup, and it is always letterspaced.

## Motif

The rounded bars under the wordmark are sequencer steps: a strip of equal-width capsules. They are
the one graphic element that carries over into documents, slides and the installer page. Keep them
equal width, keep the ramp left to right, keep the gaps smaller than the bars.

Beadwork geometry belongs in hero artwork and packaging. It does not belong on every card, and no
invented arrangement may be presented as a canonical Zulu message or ceremonial symbol.

## Voice

- Present tense, plain verbs, numbers over adjectives. "Boots at 112 BPM with 30 units of swing" beats
  "an authentic groove experience".
- British spelling, as upstream uses: licence, colour, quantise.
- Say what it cannot do. A firmware that runs on one flash bank, without an ADC and without a debug
  header, should not sound confident about anything it has not been flashed and heard.
- Genre words are technical terms here: log drum, vamp, swing percentage, drop. Do not use them as
  decoration.
- A feature is "in progress" or "planned" only with an issue number attached.
- No superlatives. No "revolutionary", "ultimate", "next generation".

## Do not

- Recolour the hat or the wordmark.
- Rotate, squash or outline the lockup.
- Add a drop shadow, glow or bevel.
- Rebuild the hat in vector "in the same style".
- Put the ramp behind text.
- Write the name in capitals. `gubhu`, never `Gubhu` or `GUBHU`.
- Ship a colour that is missing from `DESIGN.md`.
