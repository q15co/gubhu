# 0002: what comes after the v1 plumbing

Date: 2026-11-12. Status: accepted.

## Context

The v1 plumbing is done: the amapiano defaults, the log drum as a pitched part, the levelling tool,
the lint gate and the chord rows all landed today (PRs #8, #11, #13, #15, #16, #17). Four candidates
compete for the next block of work:

- **A / #4** starter project, four scenes at 112 BPM. Content, not machinery: the arranger already
  carries per-track mute and level per scene. Its dependencies (defaults, log presets, chord rows)
  are now all in place, which was the thing blocking it.
- **B / #5** pitched drum lane through parameter locks. Deferred by design: ~12 bytes against a hard
  3840 cap, a lock-id remap on a single-flash-bank device, and 24 lock slots shared with everything
  else the drum track wants.
- **C / #6** the firmware screens adapted to the design system. ~7 work items, ~10 acceptance
  criteria, five screens plus splash and dialogs, and a token set that cannot be shipped unchanged
  (declared primary purple is 2.9:1 on ink).
- **D** the CPU target-budget gate, which overruns 4 entries on a clean checkout and is currently
  advisory.

Three models (Kimi K3.1 Pro, GLM-5.3-Flash, Mistral Large 4) were given the same brief and asked to
rank the four and rule on D. All three independently ranked A, D, C, B and ruled that D must be
resolved or deleted rather than left advisory.

## Decision

1. **A is next.** It is now dependency-complete, it is the first thing a new owner experiences, and
   it needs no new machinery. The first step is the drop (scene C), not scene A: the drop is the
   aesthetic risk, and everything else is built around an accepted one.
2. **A's completion criterion is a listening pass, not a green test run.** The host tests can prove
   the project loads, the scenes switch and the golden is stable. They cannot prove the groove is
   right. A is therefore "drafted, then blocked on ears", and is not scheduled as a session of
   finished work.
3. **D is second, in one bounded block, and ends resolved.** Either the gate becomes trustworthy
   (budgets rebased with per-entry provenance, or the regression found and fixed) or it is deleted
   with its budget file in the same commit. Advisory mode is not an outcome.
4. **C is deferred** until A is merged and D has a ruling. Its first slice is the token compiler and
   the append-only GUBHU palette, not a restyle of every surface.
5. **B stays deferred** on its existing triggers: the synth-track log proving musically inadequate in
   A's listening pass, or the lock table growing.

## Why D is second and not first

The gate is stale by construction rather than by suspicion. `tests/target_budget.txt` was last
touched at SLOOP 2.4 (8d3823f, 2026-10-07); the engines it budgets have changed since, and
`drums.c` changed today. A 10 % tolerance against 36-day-old numbers cannot pass, so the overruns
are explained as drift, not as a demonstrated regression.

That is a diagnosis, not a proof. The block that closes D should build at 8d3823f and check that the
budgets were right when they were written, because `sample_render` at 4.7x its budget is large enough
to hide a real regression inside a genuine drift.

## What would reverse this

Two things, both named by all three councils:

- D's triage showing `sample_render`'s growth is a live regression rather than drift. Then D is first
  and A waits.
- A's draft overwriting a saved project, or moving any golden other than its own. Then A stops and
  format safety comes first.
