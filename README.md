<p align="center"><img src="assets/branding/logo.png" alt="gubhu" width="560"></p>

<p align="center"><b>An amapiano instrument for the M-VAVE FM-1.</b><br>
A fork of <a href="https://github.com/isod89/sloop-fm1">SLOOP</a>, free and open source under GPL-3.0-only.</p>

<p align="center">
<a href="docs/decisions/0001-amapiano-v1.md">Why this fork exists</a> ·
<a href="SLOOP.md">Manual</a> ·
<a href="GUIDE.md">Guide</a> ·
<a href="BUILDING.md">Building</a> ·
<a href="DESIGN.md">Design system</a> ·
<a href="../../issues">Issues</a>
</p>

---

*gubhu* is isiZulu for drum. This is an amapiano instrument, not a preset pack: it boots in the
genre's tempo range, it plays the log drum as a bass line rather than a percussion hit, and its
default project is an arrangement whose drop is the log drum entering.

It is built on SLOOP, which is already a four-track groovebox for any style. gubhu keeps that
instrument and changes what it does on the first bar.

> **Status:** fork of SLOOP 2.8.5, firmware v1 in progress. The v1 scope is recorded in
> [the decision record](docs/decisions/0001-amapiano-v1.md) and tracked as
> [issues #1-#5](../../issues). Still a beta: install at your own risk, and please report what you
> find.

## What gubhu changes

The genre needs four things: 108-115 BPM, swing on the sixteenths, a log drum that carries melodic
movement between the kick hits, and short vamps of extended chords. SLOOP already has the machinery
for all of it, so the work is defaults, reachability and a playable log part rather than new DSP.

| | Change | Issue |
| --- | --- | --- |
| Defaults | Boots at 112 BPM with 30 units of swing (MPC 57.5%), the amapiano kit, and every synth part in the same key | [#1](../../issues/1) |
| The log drum | Two appended analog presets, deep and dust, mono and gliding over 20-80 ms, so the log can play root, third and fifth | [#2](../../issues/2) |
| Chords | 11th, sus2 and add9 join the degree-based chord table. The 9th row already gives m9 and maj9 from the scale | [#3](../../issues/3) |
| The arrangement | A four-bar starter project in four scenes: intro, build, drop, break | [#4](../../issues/4) |

A fifth item, per-step pitch on the drum lane itself, is deferred to
[#5](../../issues/5) with its arithmetic on file.

## What it deliberately does not do

- **No new synthesis engines.** The engines are where a fork breaks a unit; the divergence data across
  every public Felucca and SLOOP fork says the same thing.
- **No audio input.** There is no ADC in the path; the 3.5 mm jack is TRS MIDI.
- **No second automation layer.** A scene is a project slot and already carries per-track mute and
  level, so the drop is content, not code.
- **No format change in v1.** The drum step stays 10 bytes and project slots stay under the
  3840-byte cap.

## The instrument

Four tracks: three synths and a drum machine with sixteen sounds on the white keys. Twelve synthesis
engines, including six-operator FM with DX7 patches, physical models and noise. Sixty-four steps with
parameter locks, micro timing, fills, ratchets and ghost notes. Swing globally and per track, punch-in
effects, a song mode you play by hand, USB audio, MIDI in on the jack and MIDI clock.

The full manual is [SLOOP.md](SLOOP.md), inherited from upstream and still correct for everything
outside the amapiano changes.

## Install

Build the package, then flash it with M-VAVE's own updater:

```sh
./build.sh
# build/felucca.fwsc
```

`build/felucca.fwsc` is the installable package: the app plus the update loader. It identifies as
`FM-1_900`.

- **USB rescue:** hold OCT- at power-on to enter the mask-ROM loader if an install goes wrong.
- **Back to SLOOP:** flash `docs/firmware/sloop-2.8.5.fwsc` the same way.
- **Back to M-VAVE's firmware:** use M-UPGRADE and the stock image from M-VAVE. This repository does
  not carry it.

Your projects, presets, samples and settings survive an update. They do not survive a format
migration done wrong, which is why v1 has none.

## Build from source

Prerequisites are in [BUILDING.md](BUILDING.md): Python 3 with Pillow, the JieLi pi32v2 toolchain, and
three files from the JieLi AC79 SDK (Apache-2.0).

```sh
export JIELI_TOOLCHAIN=$HOME/.jieli/toolchain
export AC79_SDK=$HOME/fw-AC79_AIoT_SDK
ulimit -n 8192
./build.sh
```

On Linux x86-64 the toolchain runs natively; macOS needs Docker because the toolchain is Linux-only.
`./build.sh --release 0.9-beta` produces a release identity and a named package.

Outputs, at the recorded baseline:

| File | Size |
| --- | --- |
| `build/felucca.fwsc` | 610,076 B, identity `FM-1_900` |
| `build/felucca.bin` + `loader/ota.bin` | the app and the loader inside the package |
| RAM `.data` + `.bss` | 59,340 of 98,304 B |

## Tests

```sh
tests/run_tests.sh
```

The suite is the gate, and it is stronger than it looks: 182 golden renders hashing every engine
render, the drum kit, the voice modes and a four-track mix; health and routing; a 20,000-iteration
ASan/UBSan fuzz of the editor; drum levels, FM6, the boot sound, install and site tests.

Two rules:

- **Golden changes are reviewed hash by hash.** `GOLDEN_UPDATE=1` is for intended changes only, never
  a blanket pass.
- **`tests/target_budget.py` is advisory.** It has four pre-existing overruns, proven against an
  untouched checkout. Never silence them with `BUDGET_UPDATE=1`.

Before any hardware run, the full link has to pass: a green host suite does not prove an image
boots.

## Repository map

| Path | What |
| --- | --- |
| `firmware/` | the firmware, C, one engine per file |
| `tests/` | host tests, golden renders, the fuzz harness, budget checks |
| `tools/` | generators for kits, samples, presets and packs |
| `assets/` | kits, samples, fonts, icons, brand |
| `docs/` | decisions, the installer webapp, the firmware image |
| `web/` | the browser editor and installer page |

## Contributing

Small PRs, one slice each, with the gate green and the diff explained. Open an issue before a
feature, not after. Do not touch `keytouch.c`, `timer5.c`, `adpcm_dsp.c` or `eng_adpcm.c`: they are
hardware-tied, they have no divergence history to learn from, and a mistake there costs a unit.

The project is GPL-3.0-only. Any build you distribute needs its source published.

## Credits

gubhu stands on other people's work, and says so:

- **[SLOOP](https://github.com/isod89/sloop-fm1)** by isod89, GPL-3.0. The four-track groovebox, the
  sequencer, the engines, the web installer and the manual are theirs. This fork changes defaults,
  presets and content.
- **[Felucca](https://github.com/hugelton/Felucca)** by Leo Kuroshita / Hügelton Instruments, which
  SLOOP is built on.
- **JieLi AC79 SDK**, Apache-2.0, for the bootloader and configuration files the package needs.
- **CC0 samples** from Versilian Studios, attributed in `assets/samples-cc0/ATTRIBUTION.txt`.
- **Terminus** font, for the screen, under its own licence in `assets/fonts/`.

## Licence

GNU General Public License v3.0, only. See [LICENSE](LICENSE) and [LICENSING.md](LICENSING.md).
