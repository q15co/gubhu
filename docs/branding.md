# gubhu brand

*gubhu* is isiZulu for drum. The name is lowercase everywhere: in prose, in filenames, in the
wordmark.

## Position

**An amapiano instrument, not a preset pack.**

gubhu is a fork of SLOOP for the M-VAVE FM-1. It does not try to be every style. It boots in the
genre's tempo range, it treats the log drum as a playable bass rather than a percussion hit, and its
default project is an arrangement whose drop is the log drum entering.

Everything written about it should be checkable against those three claims.

## Files

| File | Use |
| --- | --- |
| `logo.png` | The lockup: hat, wordmark, `FM1 FIRMWARE` label, step strip. 1536x1024 |
| `logo-mark.png` | The hat alone, square, for icons and avatars |
| `social-preview.png` | 1280x640, for the repository's social preview |
| `palette.png` | The five-stop ramp as swatches |

The ground is part of the logo. Do not place the lockup on white, on a photograph or on a coloured
panel: the artwork is drawn on near-black and assumes it. If a light surface is unavoidable, put the
lockup inside a black plate with generous padding.

## Colour

| Token | Hex | Role |
| --- | --- | --- |
| `ink` | `#010100` | The ground. Everything sits on it |
| `ink-soft` | `#080111` | Raised surfaces, cards, code blocks |
| `cream` | `#FDEBD3` | The wordmark, primary text on ink |
| `violet` | `#A600F3` | Ramp stop 1, left end |
| `magenta` | `#CB02C0` | Ramp stop 2 |
| `rose` | `#FB1778` | Ramp stop 3, the centre of gravity |
| `red` | `#F73550` | Ramp stop 4 |
| `orange` | `#FE5C14` | Ramp stop 5 |
| `amber` | `#F58404` | Ramp stop 6 |
| `gold` | `#F9AD01` | Ramp stop 7, right end |

The ramp runs violet to gold, left to right, and it never runs backwards. Use it for accents, rules
and the step motif, not for body text or large fills. On a page, plan on roughly 90% ink, 8% cream
and 2% ramp.

The palette comes from the artwork, not from taste: every hex above was sampled from the logo.

## Type

The wordmark is a heavy rounded geometric sans, lowercase, with a single-storey `g` and a hooked
descender. It is drawn, not typed: do not set "gubhu" in a different face and call it the logo.

For text that has to match the brand:

- Display and headings: a heavy rounded geometric sans. Poppins SemiBold, Nunito ExtraBold or Baloo 2
  are close enough for documents and slides.
- Body: Inter, or the system UI sans. Do not use the display face for paragraphs.
- The `FM1 FIRMWARE` label is the only uppercase element, and it is always letterspaced.

## Motif

The rounded bars under the wordmark are sequencer steps: a strip of equal-width capsules. They are
the one graphic element that carries over into documents, slides and the installer page. Keep them
equal width, keep the gradient left to right, keep the gaps smaller than the bars.

## Voice

- Present tense, plain verbs, numbers over adjectives. "Boots at 112 BPM with 30 units of swing" beats
  "an authentic groove experience".
- British spelling, as upstream uses: licence, colour, quantise.
- Say what it cannot do. A firmware that runs on one flash bank, without an ADC and without a debug
  header, should not sound confident about anything it has not been flashed and heard.
- Genre words are technical terms here: log drum, vamp, swing percentage, drop. Do not use them as
  decoration.
- A feature is "planned" only with an issue number attached.
- No superlatives. No "revolutionary", "ultimate", "next generation".

## Do not

- Recolour the hat or the wordmark.
- Rotate, squash or outline the lockup.
- Add a drop shadow, glow or bevel.
- Rebuild the hat in vector "in the same style".
- Put the ramp behind text.
- Write the name in capitals. `gubhu`, never `Gubhu` or `GUBHU`.
