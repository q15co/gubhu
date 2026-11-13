#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Compile DESIGN.md's firmware roles to RGB565 and check their real contrast.

  tools/gen_theme.py            write firmware/src/felucca_theme.h
  tools/gen_theme.py --check    fail when that generated header is stale
  tools/gen_theme.py --verify   check firmware pairings after RGB565 quantisation
  tools/gen_theme.py --stdout   print the header

The frontmatter parser is gen_tokens.py's: the site and firmware therefore read
exactly the same small YAML subset, with no YAML package. RGB565 expansion uses
the editor's THEME.from565 rule (nearest full-scale 8-bit channel), so --verify
checks the codes sent to the panel rather than the unquantised hex targets.
"""
import re
import sys
from pathlib import Path

from gen_tokens import frontmatter, ratio, resolve

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
DESIGN = ROOT / "DESIGN.md"
OUT = ROOT / "firmware" / "src" / "felucca_theme.h"

# C identifier, firmware role. Every value is emitted as a literal RGB565 code:
# consumers (including the web agreement test) never reimplement quantisation.
ROLES = (
    ("GUBHU_BG", "background"),
    ("GUBHU_ACCENT", "accent"),
    ("GUBHU_PANEL", "panel"),
    ("GUBHU_EMPTY", "empty"),
    ("GUBHU_STEP_OUTLINE", "empty-outline"),
    ("GUBHU_C_LINE", "line"),
    ("GUBHU_C_DIM", "dim"),
    ("GUBHU_C_GRAY", "labels"),
    ("GUBHU_C_AMB", "secondary"),
    ("GUBHU_C_HI", "values"),
    ("GUBHU_TE_G1", "empty"),
    ("GUBHU_TE_G2", "panel"),
    ("GUBHU_TE_G3", "labels"),
    ("GUBHU_TE_G4", "values"),
    ("GUBHU_TRACK_1", "track-1"),
    ("GUBHU_TRACK_2", "track-2"),
    ("GUBHU_TRACK_3", "track-3"),
    ("GUBHU_TRACK_4", "track-4"),
)

# Actual usages, not a Cartesian product of unrelated colours. Terminus 8x16 is
# normal text throughout, so every text pair uses the 4.5:1 gate. Track colours
# are included because the UI draws track labels and the K1..K4 labels with them.
TEXT_PAIRS = tuple(
    [(role, "background") for role in
     ("values", "labels", "secondary", "dim", "accent", "track-1", "track-2", "track-3", "track-4")]
    + [(role, bg) for bg in ("panel", "empty") for role in ("values", "labels")]
    + [("background", bg) for bg in ("values", "accent", "track-1", "track-2", "track-3", "track-4")]
)
GRAPHIC_PAIRS = (("line", "background"), ("line", "panel"),
                 ("empty-outline", "background"), ("empty-outline", "empty"))
HEX = re.compile(r"^#[0-9A-Fa-f]{6}$")


def rgb565(value):
    if not HEX.fullmatch(value):
        raise SystemExit(f"{DESIGN}: firmware colour {value!r} is not #RRGGBB")
    r, g, b = (int(value[i:i + 2], 16) for i in (1, 3, 5))
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def shown(code):
    """RGB565 code -> the editor's nearest full-scale 8-bit representation."""
    r = (((code >> 11) & 31) * 255 + 15) // 31
    g = (((code >> 5) & 63) * 255 + 31) // 63
    b = ((code & 31) * 255 + 15) // 31
    return f"#{r:02X}{g:02X}{b:02X}"


def colours(doc):
    fw = doc.get("firmware")
    if not isinstance(fw, dict):
        raise SystemExit(f"{DESIGN}: missing firmware role table")
    out = {}
    for _, role in ROLES:
        if role not in fw:
            raise SystemExit(f"{DESIGN}: missing firmware.{role}")
        value = resolve(fw[role], doc)
        out[role] = (value, rgb565(value))
    return out


def header(doc):
    lines = [
        "/* Generated from DESIGN.md by tools/gen_theme.py. Do not edit: edit DESIGN.md. */",
        "/* RGB565 codes; --verify checks contrast after quantisation, not just the source hex. */",
        "#ifndef FELUCCA_THEME_H",
        "#define FELUCCA_THEME_H",
        "",
        "/* Normative colours, including the screen-safe variants. */",
    ]
    for name, value in doc["colors"].items():
        if HEX.fullmatch(value):
            macro = "GUBHU_COLOR_" + re.sub(r"[^A-Za-z0-9]", "_", name).upper()
            lines.append(f"#define {macro} 0x{rgb565(value):04X}u /* {value} */")
    lines += ["", "/* FM1 semantic roles (gfx.c's five steps, surfaces, accent and tracks). */"]
    fw = colours(doc)
    for macro, role in ROLES:
        value, code = fw[role]
        lines.append(f"#define {macro} 0x{code:04X}u /* firmware.{role}: {value} */")
    lines += ["", "#endif", ""]
    return "\n".join(lines)


def verify(doc):
    fw = colours(doc)
    bad = 0
    pairs = [(fg, bg, 4.5, "text") for fg, bg in TEXT_PAIRS]
    pairs += [(fg, bg, 3.0, "graphic") for fg, bg in GRAPHIC_PAIRS]
    for fg, bg, gate, kind in pairs:
        fhex, fcode = fw[fg]
        bhex, bcode = fw[bg]
        actual = ratio(shown(fcode), shown(bcode))
        ok = actual >= gate
        bad += not ok
        print(f"  {'ok' if ok else 'FAIL':4} {actual:5.2f}:1 (needs {gate}:1) {kind:7} "
              f"{fg} on {bg}: {fhex} -> {shown(fcode)} on {bhex} -> {shown(bcode)}")

    # A low-contrast empty fill is intentional only because the shape cue is a
    # separate role and is itself gated above. It is not treated as a text pair.
    empty = ratio(shown(fw["empty"][1]), shown(fw["background"][1]))
    print(f"  cue  {empty:5.2f}:1           graphic empty on background: "
          "the rectangular empty-outline, not this fill alone, carries the state")
    print(f"\n{len(pairs) - bad}/{len(pairs)} RGB565 firmware pairings pass; "
          "empty state also has a non-colour outline")
    return bad


def main(argv):
    if len(argv) > 1 or (argv and argv[0] not in ("--check", "--verify", "--stdout")):
        print(f"usage: {Path(sys.argv[0]).name} [--check|--verify|--stdout]", file=sys.stderr)
        return 2
    doc = frontmatter(DESIGN)
    if argv == ["--verify"]:
        return 1 if verify(doc) else 0
    out = header(doc)
    if argv == ["--stdout"]:
        sys.stdout.write(out)
        return 0
    if argv == ["--check"]:
        have = OUT.read_text(encoding="utf-8") if OUT.exists() else ""
        if have != out:
            print(f"{OUT.relative_to(ROOT)} is stale: run tools/gen_theme.py")
            return 1
        print(f"{OUT.relative_to(ROOT)} matches {DESIGN.name}")
        return 0
    OUT.write_text(out, encoding="utf-8")
    print(f"{OUT.relative_to(ROOT)}: {len(out)} B from {DESIGN.name}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))