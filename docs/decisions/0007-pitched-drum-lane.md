# 0007: pitched drum lane via parameter locks

Date: 2026-11-14. Status: accepted, deferred for v2.

## Decision

Do not build issue #5 in v2. Parameter locks are the right playback seam, but 24 locks per track do
not make a general 64-step pitched lane when a note needs both pitch and glide
(`firmware/src/core.h:19`, `firmware/src/core.h:228-233`). The synth-track log remains the shipping
design from 0001 (`docs/decisions/0001-amapiano-v1.md:27-34`).

Reopen the design on either of these events:

- A lock-storage redesign provides at least 48 locks on the drum track while keeping a saved project
  within 3840 bytes. Forty-eight allow 16 steps with pitch and glide locks and leave 16 locks for the
  rest of the kit. The number and the project-size check are both required.
- The starter project's listening pass produces a written counterexample: a named riff, its note
  sequence and its glide time that LOG DEEP and LOG DUST cannot play adequately on a synth track.
  A general preference for another workflow is not that counterexample.

An implementation after either trigger still ships pitch and glide together. It must also pass the
format, old-render and target-cost gates below. This record does not schedule implementation slices
while the decision is deferred.

## The request in firmware terms

The target is track 4's KICK 2 lane, GM note 35. The lane table puts that note at lane 1, and the
AMAPIANO kit defines KICK2 as the log drum (`firmware/src/drums.c:84-95`,
`tools/gen_drumkits.py:302-305`). A drum step already turns a lane bit into `trk_note_on` for that
lane's GM note (`firmware/src/seq.c:2748-2757`).

The requested design is two lockable track parameters, here called `P_LOG_PIT` and `P_LOG_GLIDE`.
Their proposed ranges are -24..24 semitones and 0..120 ms, both defaulting to zero. On a KICK 2 hit,
pitch is added to the existing lane tune. Glide controls how the sounding voice approaches the next
locked pitch. Other lanes ignore both parameters. This is a new use of `track_t.p[]` and
`track_t.lock[]`, not a new engine and not a new kind of step.

Staying in the project key is additional work, not a property the drum path already has. Drum keys
return a lane note before the synth scale mapping, and `kb_scaled` explicitly excludes the drum track
(`firmware/src/seq.c:182-186`, `firmware/src/seq.c:237-249`). A future implementation must quantise
the log target against the drum track's stored ROOT and SCALE and test that behaviour when the key
changes. Merely adding a semitone offset does not meet the issue's key requirement.

The paths named `eng_drum.c` and `drum.c` in the task brief do not exist at 31e0b36. The actual onset
and synthesis code is `drums.c` and its included `drum_synth.c` (`firmware/src/drums.c:19`). The track
parameter table is `TP[]`, not `GP[]`: `TP[]` starts at `firmware/src/params.c:78`, while `GP[]` is
the global table at `firmware/src/params.c:171`.

## What already carries per-step values

The value array is per track, but the lock table already applies values per step. The definitions
are (`firmware/src/core.h:192-234`):

```c
#define SF_ACCENT 1u
#define SF_SLIDE 2u
#define SF_CONT0 8u
#define SF_CONT(k) (SF_CONT0 << (k))
#define SF_CONT_ALL (15u * SF_CONT0)
#define SF_LEG 0x80u

typedef struct {
    uint8_t note[4];
    uint8_t n;
    uint8_t time;
    uint8_t flags;
    uint8_t vel;
    uint8_t lvl;
    uint8_t rat;
} step_t;

typedef struct {
    uint8_t on[2];
    uint8_t lvl[4];
    uint8_t rat[4];
} dstep_t;

#define NLOCK 24
typedef struct {
    uint8_t step;
    uint8_t param;
    int16_t val;
} plock_t;
#define LOCK_FREE 0xFFu
```

`step_t` and `dstep_t` are both exactly 10 bytes by static assertion. There is no pitch field and no
lock bit in `flags`. Bit 2 is free, `SF_ACCENT` and `SF_SLIDE` use bits 0 and 1, `SF_CONT0` starts the
continuation bits at bit 3, and `SF_LEG` uses bit 7 (`firmware/src/core.h:193-224`). A drum step uses
all 10 bytes for 16 lane bits, 16 two-bit levels and 16 two-bit ratchets
(`firmware/src/core.h:218-224`, `firmware/src/drums.c:142-159`). A free synth flag bit therefore is
not spare drum-step storage.

Each track has `int16_t p[P_COUNT]`, 64 steps and 24 four-byte locks. Locks are separate records keyed
by step and parameter, not fields in the step (`firmware/src/core.h:19`, `firmware/src/core.h:228-239`,
`firmware/src/core.h:264-276`). On entry to a step, `lock_step` restores parameters absent from that
step and applies the step's locks. `lock_set` rejects a non-lockable parameter or an exhausted table
(`firmware/src/seq.c:1154-1179`, `firmware/src/seq.c:1199-1262`).

For the two-parameter design, the required addition is therefore two parameter ids and descriptors,
their inclusion in `p_lockable`, and a drum-visible editing page. It adds **zero bytes per step** and
uses the existing lock records. The current drum page filter hides the ordinary SEL 2 and VOICE
pages, so adding descriptors alone would not expose them on the drum track
(`firmware/src/params.c:425-446`, `firmware/src/params.c:473-510`).

## Project size and RAM cost

The current writer is FUN6, not FUN3 or FUN4. Its project is 3816 bytes, checked by the host test, and
the payload limit is 3840 bytes: a 4096-byte flash sector minus a 256-byte header
(`firmware/src/project.c:9-11`, `firmware/src/project.c:27-47`, `firmware/src/storage.c:17-20`,
`tests/project_test.c:130-139`). The current headroom is **24 bytes**.

There are 64 allocated steps per track even though P_SLEN defaults to 16. Four tracks therefore own
256 steps, not 64 (`firmware/src/core.h:12-19`, `firmware/src/params.c:108`). The lock design costs
no additional step storage. A separate one-byte pitch lane would cost:

| Shape | Project growth | Project size | Result against 3840 |
| --- | ---: | ---: | --- |
| Pitch and glide as existing lock records | 0 B in steps | 3816 B before new parameter bases | 24 B headroom |
| One pitch byte on each of 64 drum steps | 64 B | 3880 B | 40 B over |
| One pitch byte on all 256 steps | 256 B | 4072 B | 232 B over |
| One pitch byte on only 16 drum steps | 16 B | 3832 B | 8 B headroom, but incompatible with NSTEP 64 |

The step sizes follow from `NSTEP 64`, `NTRK 4` and the 10-byte step assertion
(`firmware/src/core.h:12-19`, `firmware/src/core.h:209-224`). The existing lock arrays total
24 x 4 x 4 = **384 bytes** across four tracks, already included in both the working tracks and FUN6;
the proposed feature does not increase that allocation (`firmware/src/core.h:228-233`,
`firmware/src/project.c:48-65`).

For a concrete two-parameter format, append both ids immediately before P_E0 and serialise each in
one byte. The proposed pitch byte uses a signed decode, since the present byte path clamps to 0..255
and cannot directly preserve a negative offset (`firmware/src/project.c:45-78`). This is a proposed
v7 layout, not current code. Its cost is:

| Item | Calculation | Cost |
| --- | --- | ---: |
| Saved project | 3816 + 2 x 4 | **3824 B, 8 B growth, 16 B headroom** |
| Working `p` and `p_old` arrays | 2 params x 2 arrays x 2 B x 4 tracks | **32 B static RAM** |
| Two-core preset-fade scratch `pnew` | 2 params x 2 buffers x 2 B | **8 B static RAM** |
| Four project slots in `.noinit` | 8 B x 4 | **32 B RAM** |
| Autosave buffer | one project | **8 B RAM** |
| Total identified static RAM | 32 + 8 + 32 + 8 | **80 B** |
| Flash staging union | FUN5 remains the largest member at 3840 B | **0 B growth** |

The arrays behind these figures are at `firmware/src/core.h:238-239`,
`firmware/src/core.h:324-325`, `firmware/src/fx.c:655-668`, `firmware/src/project.c:150-153` and
`firmware/src/project.c:690-696`. Four slots grow from 15264 to 15296 bytes. The existing assertion
allows 15440 bytes for them, leaving 144 rather than 176 (`tests/project_test.c:134-135`). The staging
union contains the 3840-byte FUN5 structure as well as the current project
(`firmware/src/project.c:562-570`). Stack arrays sized by P_COUNT grow by four bytes per live instance
and are not in the 80-byte static total. No extra per-step or lock RAM is included.

Using two 16-bit stored values instead gives 3816 + 4 x 4 = **3832 bytes**, **16 bytes growth** and
**8 bytes headroom**. It raises the four slots and autosave by 80 bytes rather than 40, so identified
static RAM becomes **120 bytes**. The issue's estimate of about 12 bytes and a 3828-byte slot is not
what the current two-byte-aligned `proj_trk_t` layout produces by adding three bytes per track:
three data bytes round to four per track. The byte-serialised 3824-byte layout is the one costed for a
future design.

These are source-layout calculations for a proposed format, not a target link measurement. Firmware
instructions, the two descriptors, a new page and any extra voice-glide state are not included. A
future implementation should first try the existing per-voice `pitch16`, `pitch_cur` and `gstep`
fields rather than adding state blindly (`firmware/src/core.h:126-130`). Their suitability for drum
glide is unverified.

## Format version and migration

Two dedicated ids move P_E0 from 63 to 65 and P_COUNT from 71 to 73. The current values are pinned by
`tests/project_test.c:130-133`, and the append-before-P_E0 rule is at `firmware/src/core.h:66-78`.
The present FUN6 loader accepts an exact current magic, size and checksum, then has converters for
FUN1 through FUN5 (`firmware/src/project.c:155-164`, `firmware/src/project.c:380-388`). It cannot
interpret two inserted ids, a changed P_E0 or the new size as an unchanged FUN6.

The dedicated-parameter design therefore needs **FUN7**, not a v4 bump and not a silent FUN6 change.
Its required migration is:

1. Keep the exact 3816-byte FUN6 structure as a read-only type and recognise its magic, size and sum.
2. Copy all old common values and engine values by id, initialise both new parameters to zero, and
   preserve steps, nudge/fill bytes, selection, globals and checksums.
3. Add two to every occupied lock id that was 63 or greater, moving old E0..E7 locks to new E0..E7.
   Leave locks below 63 unchanged and preserve free slots as LOCK_FREE. This is the same class of
   remap that FUN5 to FUN6 already performs at `firmware/src/project.c:273-307`.
4. Route FUN1 through FUN5 through the new layout with both additions at their defaults. Add a
   checked-in FUN6 fixture with locks below and inside E0..E7, plus a FUN7 round trip. Assert 3824 of
   3840 and the four-slot `.noinit` bound. The existing migration and layout tests are the direct
   model (`tests/project_test.c:249-315`).
5. Apply the same id audit to user presets. They already store P_COUNT, map their last eight values
   back to E0..E7 and allow P_COUNT growth only just before P_E0. UP_PMAX is 72, so P_COUNT 73 does
   not fit. Raising UP_PMAX to 74 grows the inline record from 192 to 196 bytes and each 16-record
   bank from 3080 to 3144 bytes. That changes the bank layout, so it needs an UP_VER migration or a
   different preset encoding, not just a larger array. The two banks have no RAM mirror, so this adds
   128 bytes across their stored payloads but no static RAM mirror (`firmware/src/upreset.c:3-16`,
   `firmware/src/upreset.c:23-40`, `firmware/src/upreset.c:87-95`).

The issue's single-flash-bank description is not the current storage design. Each object has A/B
sectors, a save erases and writes the other copy, and the header is committed last so a torn write
leaves the previous copy valid (`firmware/src/storage.c:3-9`, `firmware/src/storage.c:175-215`).
That does not remove the need for the lock-id tests, but migration is not an in-place single-bank
rewrite.

There is a storage-free variant: reuse P_TRANS and P_GLIDE, which already have semitone and time
descriptors (`firmware/src/params.c:107`, `firmware/src/params.c:117`). This adds no project or P_COUNT
storage, but it is not a compatibility shortcut. P_GLIDE is already lockable, and old drum tracks can
already contain values for both parameters even though their ordinary pages are hidden
(`firmware/src/seq.c:1160-1164`, `firmware/src/params.c:473-510`). Reusing them would require a
semantic FUN7 migration that clears or deliberately interprets those dormant values and locks. It
does not avoid a compatibility proof, and it does nothing for the 24-lock limit.

## The playback seam and cost gate

No function reads a drum step's pitch today: `dstep_t` has only lane, level and ratchet data
(`firmware/src/core.h:218-224`). The sequencer already has the correct ordering for adding it. On a
drum step it calls `lock_step(t, idx)` and then `drum_step` (`firmware/src/seq.c:2856-2882`).
`drum_step` calls `trk_note_on`, whose drum branch calls `drum_on` (`firmware/src/seq.c:2748-2757`,
`firmware/src/voice.c:468-479`). No lock scan or parameter lookup needs to move into a per-sample
loop.

Pitch must be applied after the note has selected its lane and sound, not by adding semitones to the
GM note before selection. `drum_on` uses that note to select user, synthesised and sampled kits
(`firmware/src/drums.c:293-335`), and `ds_lane` also derives a sound-specific pitch offset from it
(`firmware/src/drum_synth.c:43-60`). The two existing tuning seams are:

- Synthesised kits: `drum_synth_on` passes `drum_tune[lane] * 16` to `ds_on`. `ds_on` puts that value
  in `base16` and derives the initial phase increment (`firmware/src/drums.c:250-277`,
  `firmware/src/drum_synth.c:127-148`). The locked log offset belongs in this `tune16` sum.
- Sampled kits: `drum_on` adds lane tune to `shift`, then computes the Q16 playback rate with
  `pow2_q16` (`firmware/src/drums.c:354-374`). The locked offset belongs in `shift`. The sample loop
  reads that rate once before its per-sample loop (`firmware/src/drums.c:429-443`).

For glide, synthesised voices already recompute `inc_to` once per control block in `ds_control`, and
`ds_render` ramps the increment between blocks (`firmware/src/drum_synth.c:188-215`,
`firmware/src/drum_synth.c:225-235`, `firmware/src/drum_synth.c:319-326`). A log target can join that
control-rate calculation. A sampled log target can update its rate before the per-sample loop at
`firmware/src/drums.c:429-443`. Zero pitch and zero glide must retain the current arithmetic and
initialisation so old projects render bit for bit.

The target gate counts instructions in loops, weights nested loops, and fails above budget +10 %; it
is a static target-code measure, not a cycle measurement (`tests/target_budget.py:4-12`,
`tests/target_budget.py:17-24`, `tests/target_budget.py:52-75`, `tests/target_budget.py:103-114`).
The relevant recorded budgets are `drums_mix` 6153 and `fm1_alnk0_irq` 174
(`tests/target_budget.txt:23-24`). Their numerical limits are 6768.3 and 191.4, so integer costs of
6768 and 191 are the largest passing values. Keeping new work at note-on or control-block rate avoids
the highest-weight sample loops, but the actual delta is unverified until a target disassembly
exists. `tests/run_tests.sh:185-193` also runs the golden/audio/CPU regression before this target
check.

## Alternatives

### A per-track pitch offset

This gives every hit the same offset. The firmware already has a stronger per-lane version:
`drum_tune[16]` covers -8..+7 semitones and is packed with DECAY into the drum track's E1..E7 project
fields (`firmware/src/drums.c:103-110`, `firmware/src/project.c:420-457`). P_TRANS also already
provides -24..+24 semitones, but drum keys bypass its scale path (`firmware/src/params.c:107`,
`firmware/src/seq.c:237-249`).

A fixed offset costs no per-step storage and is the correct base tune. It cannot encode a riff, so it
loses as an answer to issue #5.

### A drum-specific pitch sequence lane

One signed pitch byte for each of the drum track's 64 steps costs 64 project bytes and raises FUN6
to 3880, 40 over the payload cap. Extending that field to all four tracks costs 256 bytes and raises
it to 4072, 232 over. Glide storage and an editor lane are still extra. This wins on lock capacity
but loses on the hard format bound (`firmware/src/core.h:19`, `firmware/src/project.c:48-65`,
`firmware/src/storage.c:17-20`).

### A second drum track with its own tuning

A second track could hold an independent pattern and locks, but it is not a small allocation. One
current `proj_trk_t` is 934 bytes by the checked project layout: adding one to the 3816-byte project
would produce 4750 bytes, 910 over the cap. It would also add a full `track_t`, whose current size
and final target RAM cost were not measured here. Reusing one of the three synth tracks as a drum
track would instead retain the track cost that this issue is meant to remove. The fixed four-track
layout and single drum index are at `firmware/src/core.h:11-19`; the stored track fields are at
`firmware/src/project.c:48-65`.

### The full parameter-lock design

This is the only alternative that provides independent pitch and glide per existing step without a
new audio engine or a bigger step. Its cost is two parameter bases, a FUN7 migration and the shared
lock table. It wins over a second sequence lane and a second track on storage.

It loses on capacity today. With both values locked on every hit, a six-hit riff on distinct steps
needs 12 locks and leaves 12. An eight-hit riff needs 16 and leaves 8. Twelve such steps consume all
24, so a 64-step pattern can have at most 12 log steps with both values locked before any filter,
insert, pan, gate or other drum lock. Additional hits without locks reuse the base pitch and glide.
Ratchets on one locked step share its locks, but they do not increase the number of independent
pitches. The 24-entry table and the other lockable drum parameters are at
`firmware/src/core.h:228-233` and `firmware/src/seq.c:1160-1164`.

That capacity limit, not a missing lock mechanism, is the reason to defer. The synth-track log
already carries full note sequences without spending drum locks (`docs/decisions/0001-amapiano-v1.md:27-34`).

## Evidence and unverified items

Verified in this worktree at 31e0b36:

- Issue #5 and decisions 0001 and 0002 were read before this record.
- The actual drum, sequencer, parameter, project, preset and storage paths above were read.
- Generated host inputs were built and `build/host/project_test` passed. Its output confirms FUN6 is
  3816 of 3840 bytes, the old-format conversions pass, locks survive capture/apply and damaged locks
  are handled.
- No firmware source was changed.

Unverified:

- No target `build/felucca.dis` was available. Current target costs, the proposed cost delta and
  passage of the +10 % gate are unmeasured. The 6768 and 191 figures are arithmetic limits from the
  checked-in budgets, not measurements.
- The proposed FUN7 layouts and RAM totals were not compiled or linked for AC792N. Firmware ROM
  growth, final linker RAM placement, stack high-water use and any additional drum-glide voice state
  remain unmeasured.
- No FUN6 to FUN7 fixture, migration or old-project render exists yet. Compatibility of the proposed
  migration is a required implementation test, not a result claimed here. The user-preset bank
  migration and its 196-byte record are likewise proposed arithmetic, not a tested format.
- No hardware flash, playback or listening test was run. In particular, there is no verified
  listening counterexample showing that the synth-track log is musically inadequate.
- The exact editor gestures for entering a scale-quantised log lock and the sound of glide on each
  sampled or synthesised kit remain design work. This record verifies the seams and their costs, not
  an implementation that does not exist.
