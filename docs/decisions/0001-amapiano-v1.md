# 0001: amapiano v1 scope

Date: 2026-11-11. Status: accepted.

## Context

The FM-1 firmware needs to be an amapiano instrument, not SLOOP with a preset pack. Amapiano is built
on four things: 108-115 BPM (112 is the consensus), 20-30% sixteenth swing, a tuned percussive log
drum that plays melodic content between the kick hits, and vamps of two to four extended chords,
usually a Dorian i9 to IV9. Energy changes by adding or removing the log drum, not by adding layers.

Audit of the fork at this commit:

- Scale and chord machinery already exists: eight scales including Dorian and minor pentatonic, a
  degree-based chord table whose 9TH row already produces m9 or maj9 from the scale.
- Swing exists globally and per track. Micro timing, parameter locks, fills and four arranger scenes
  exist.
- A kick duck exists (`G_DUCK`), default 0.
- A scene is a full project slot, so it already carries per-track mute and level.
- The drum step is a 10-byte packed struct with lane, level and ratchet only, statically asserted to
  the same size as a synth step, and project slots sit under a hard 3840-byte cap (3816 used).

So the gap is not harmony, groove plumbing or arrangement machinery. It is that the log drum cannot
be played as a pitched part, and that a fresh unit is at 90 BPM, swing 0, without the genre's kit.

## Decision

1. The log drum is a **synth-track part**, played by the analog engine's sine with pitch envelope,
   glide and filter, as two appended presets (deep and dust). The drum kit's log lane stays as the
   unpitched sub layer. The 10-byte drum step is not touched.
2. Defaults become 112 BPM and swing 30 code units (MPC 57.5%), with the amapiano kit, and a new
   project seeds every synth part with the same key.
3. The chord table gains 11TH, SUS2 and ADD9. No explicit m9 or maj9 row: 9TH already adapts to the
   degree.
4. The drop ships as **content**: four scenes (intro, build, drop, break) in a starter project. No new
   automation layer and no change to `arr_entry_t`.
5. No new DSP engine, no engine-file edits, no format change in v1.

## Rejected alternatives

- **Per-step pitch on the drum step.** Correct musically, but the step is a union with the synth step
  under a static assert, and every saved project stores 64 of them per track. A migration on a single
  flash bank for a feature the synth track already delivers is the wrong first risk.
- **Pitch and glide on the log lane driven by parameter locks.** Attractive: no step growth, only
  twelve bytes of project growth. Rejected for v1 because it lands at 3828 of a 3840-byte cap, needs
  a lock-id remap migration, and spends 12-16 of the drum track's 24 lock slots on one riff. Kept as
  a v2 candidate.
- **A global key parameter with per-track override.** The key gesture already writes the root to
  every synth part. A second source of truth adds override semantics and format bytes for no new note
  capability. The gap is only that a new project must seed all parts alike.
- **Per-key log presets (A, C, D, E, G).** Lane tune and transpose already reach every key; ten
  variants multiply preset slots, trims and golden hashes for character, not pitch.
- **Vocal chop mode.** The slice engine is deliberately not built and there is no audio input.

## Consequences

- One of three synth tracks is spent on the log. Correct for a genre instrument: the log is the bass.
- The pitched log is layered with the kit's log lane; keeping them out of each other's way is content
  and tuning, not code.
- v1 is verifiable entirely in the sandbox: presets and defaults move only intended golden hashes, no
  project migration, no new state in the audio path.
- The deferred lock-driven design stays on file as issue #5 so the analysis is not lost.

## Gates

Host suite and golden diff reviewed hash by hash; the four pre-existing `target_budget.py` overruns
stay red and advisory, never silenced with `BUDGET_UPDATE=1`; full link before any hardware run.
Flashing and listening remain human gates.
