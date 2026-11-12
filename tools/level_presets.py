#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Measures every factory preset and writes firmware/src/preset_trim.h: a level trim per preset
(1/2 dB, the steps of LEVEL), so every sound comes out as loud as the others at the same LEVEL.
Loudness: ITU-R BS.1770 integrated (gated) over a phrase that fits the sound (tests/preset_level.c:
a bass line, held chords, comping or a melody).

  cc -O2 -Ibuild/gen -Ifirmware/src -Ifirmware/hal tests/preset_level.c -lm -o build/host/preset_level
  build/host/preset_level build/host/presets.wav build/host/presets.txt build/host/presets.raw
  tools/level_presets.py build/host/presets.raw build/host/presets.txt [ENGINE]
  (rebuild and run it again until nothing moves: the trims add to the ones in preset_trim.h)
  ENGINE: only that engine's presets are measured again (a new engine: the others keep their trims, so
  their golden renders do not move; SLOOP 2.4: FM6 = 9).

The report's trim column is the trim the measuring binary was built with, so the tool adds its
correction to the trim that was actually applied rather than to whatever the header says today. A
report measured by a binary built from a different header is refused instead of applied twice; that
double application is what used to walk every preset's trim on each run. Engines the report does not
mention (SLICE, when the host build has no FELUCCA_SLICE) keep their row from the header, and the
table's height and PT_ENGINES come from the header, so a per-engine run leaves them alone.

The report's peak column is the loudest sample of the phrase. A preset whose peak sits at the master
limiter's ceiling (LIM_T) is held there by the limiter: raising its trim raises the input, not what
comes out, so the loudness barely moves and the correction never converges. Those presets are left at
their trim and named in the output instead of being walked one half-step per run.

Needs numpy and scipy."""
import re
import sys
from pathlib import Path

import numpy as np
from scipy.signal import lfilter

FS, CTL = 44100, 32
SEG = (FS * 9 // 2) // CTL * CTL + (FS // 4) // CTL * CTL
OUT = Path(__file__).resolve().parent.parent / "firmware/src/preset_trim.h"
TARGET = -15.0                                 # LUFS (integrated), at the default LEVEL
LIM_T = 18000                                  # (fx.c: the master limiter's ceiling, of the DAC's 32768)
LIM_MARGIN = 32                                # (the limiter fits the peak just under LIM_T: 0.3 dB below it
                                               #  means it is holding this preset, not that it nearly did)
ROLE = {0: 0.0, 1: -1.0, 2: 0.0, 3: 0.0, 4: 0.0}   # bass, chords held (pads, organs), comping, melody, one-key chords
SKIP = {"GM KIT"}                              # (the drum map on a synth track: as it is)
PT_MAX_DEFAULT = 32


def lufs(x):
    b1 = [1.53512485958697, -2.69169618940638, 1.19839281085285]
    a1 = [1, -1.69065929318241, 0.73248077421585]
    b2, a2 = [1, -2, 1], [1, -1.99004745483398, 0.99007225036621]
    k = lfilter(b2, a2, lfilter(b1, a1, x)) ** 2
    w, h = int(0.4 * FS), int(0.1 * FS)
    bl = np.array([k[i:i + w].mean() for i in range(0, len(k) - w, h)])
    bl = bl[bl > 1e-7]
    if not len(bl):
        return None
    g = bl[10 * np.log10(bl) - 0.691 > -70]
    rel = 10 * np.log10(g.mean()) - 0.691 - 10
    g = g[10 * np.log10(g) - 0.691 > rel]
    return 10 * np.log10(g.mean()) - 0.691


def parse_header(path):
    """The table as it stands on disk: its height, PT_MAX, and (engine, preset) -> (name, trim)."""
    text = Path(path).read_text() if Path(path).exists() else ""
    m = re.search(r"#define PT_ENGINES (\d+)u", text)
    neng = int(m.group(1)) if m else 0
    m = re.search(r"#define PT_MAX (\d+)u", text)
    pmax = int(m.group(1)) if m else PT_MAX_DEFAULT
    kept = {}
    for e, p, name, v in re.findall(r"/\* (\d+)\.(\d+) (.*?) \*/ (-?\d+)", text):
        kept[(int(e), int(p))] = (name, int(v))
    return neng, pmax, kept


def parse_report(path):
    """(engine, preset, role, trim the binary applied or None, peak or None, name). Names hold spaces:
    the numbers come before the name, so a name is never split."""
    rows = []
    for line in Path(path).read_text().splitlines():
        if not line.strip():
            continue
        f = line.split(None, 5)
        if len(f) == 6 and re.fullmatch(r"-?\d+", f[3]) and re.fullmatch(r"-?\d+", f[4]):
            rows.append((int(f[0]), int(f[1]), int(f[2]), int(f[3]), int(f[4]), f[5]))
        else:
            f = line.split(None, 4)
            if len(f) == 5 and re.fullmatch(r"-?\d+", f[3]):
                rows.append((int(f[0]), int(f[1]), int(f[2]), int(f[3]), None, f[4]))
            else:
                f = line.split(None, 3)
                rows.append((int(f[0]), int(f[1]), int(f[2]), None, None, f[3]))
    return rows


def main(raw, rep, only=None):
    rows = parse_report(rep)
    d = np.fromfile(raw, np.int32).astype(float) / 32768.0
    neng_h, pmax_h, kept = parse_header(OUT)
    measured = {e for e, _, _, _, _, _ in rows}
    neng = max([neng_h] + [e + 1 for e in measured])
    pmax = max(pmax_h, PT_MAX_DEFAULT)
    for e, p, _, _, _, name in rows:
        assert p < pmax, name
    # the report was made by a binary built with some header: if that is not the one on disk, adding a
    # correction to the disk's trims applies it twice (2.4: every run walked SUPERSAW, WOBBLE, REESE)
    stale = [(e, p, name, rt, kept[(e, p)]) for e, p, _, rt, _, name in rows
             if rt is not None and (e, p) in kept and kept[(e, p)] != (name, rt)]
    if stale:
        for e, p, name, rt, (hn, hv) in stale[:6]:
            print(f"level_presets: {e}.{p} {name}: the report's trim is {rt}, the header has {hv} ({hn})")
        print("level_presets: the report was measured by a binary built from another header.\n"
              "  Rebuild build/host/preset_level and measure again; nothing was written.")
        return 1
    # only the engines the report covers are rebuilt; every other engine's row is kept as it is, so a
    # per-engine run leaves SLICE (which the host build does not enumerate) and the others alone
    rebuilt = {only} if only is not None else measured
    tab = {k: v for k, v in kept.items() if k[0] not in rebuilt}
    held = []
    for i, (e, p, role, rt, pk, name) in enumerate(rows):
        L = lufs(d[i * SEG:(i + 1) * SEG])
        t = rt if rt is not None else (kept[(e, p)][1] if (e, p) in kept else 0)
        if L is not None and name not in SKIP and (only is None or e == only):
            if pk is not None and pk >= LIM_T - LIM_MARGIN:   # (the limiter holds this preset's output: trim
                held.append((e, p, name, L))            #  raises the input, not what comes out)
            else:
                delta = (TARGET + ROLE[role] - L) * 2   # in half-dB steps, the trim's own resolution
                if abs(delta) >= 1.0:                   # (a move smaller than one step is not a move: it
                    t += int(round(delta))              #  only re-adds rounding noise on every run)
        tab[(e, p)] = (name, max(-40, min(23, t)))   # (LEVEL 104 + 23 = 127: the top of the table)
    lines = ["/* generated by tools/level_presets.py: the level trim of each factory preset, 1/2 dB */",
             "#pragma once", f"#define PT_ENGINES {neng}u", f"#define PT_MAX {pmax}u",
             "static const int8_t PRESET_TRIM[PT_ENGINES][PT_MAX] = {"]
    for e in range(neng):
        cells = [f"/* {e}.{p} {tab[(e, p)][0]} */ {tab[(e, p)][1]}" for p in range(pmax) if (e, p) in tab]
        lines.append("    {" + ", ".join(cells) + "},")
    lines.append("};")
    OUT.write_text("\n".join(lines) + "\n")
    print(f"{len(rows)} presets levelled -> {OUT}")
    if held:
        print(f"level_presets: {len(held)} preset(s) held at the master limiter (peak {LIM_T - LIM_MARGIN}..{LIM_T}):")
        print("  trim cannot move them; they are left as they are")
        for e, p, name, L in held[:8]:
            print(f"  {e}.{p} {name}: {L:.2f} LUFS, left at {tab[(e, p)][1]}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1], sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else None))
