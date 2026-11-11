#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Generate the CSS token layer from DESIGN.md, and check the contrast it claims.

DESIGN.md carries the design system's normative values in YAML frontmatter. Two
readers have to agree with it: the stylesheets, and whoever reviews a UI change.
So the values are compiled rather than retyped:

  tools/gen_tokens.py            writes web/tokens.css
  tools/gen_tokens.py --check    verifies DESIGN.md's contrast pairs, writes nothing
  tools/gen_tokens.py --verify   fails if web/tokens.css is not what DESIGN.md says
  tools/gen_tokens.py --stdout   prints the CSS

The frontmatter is a fixed, small YAML subset (nesting one and two levels deep,
scalars only, `{section.key}` references), so this parses it without a YAML
dependency: the firmware tree is built with Python and Pillow and nothing else.

Contrast is computed, not asserted: DESIGN.md says the values are targets and
that every text/background pairing must be checked against WCAG 2.2 AA. The
component table is the list of pairings, so it is the list checked here.
"""
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
DESIGN = ROOT / "DESIGN.md"
OUT = ROOT / "web" / "tokens.css"

# Family names in DESIGN.md are implementation choices; each carries a stack so a
# machine without Exo 2, Inter or JetBrains Mono still renders a sensible hierarchy.
STACKS = {
    "Exo 2": '"Exo 2", ui-sans-serif, system-ui, -apple-system, "Segoe UI", sans-serif',
    "Inter": '"Inter", ui-sans-serif, system-ui, -apple-system, "Segoe UI", sans-serif',
    "JetBrains Mono": '"JetBrains Mono", ui-monospace, SFMono-Regular, Menlo, monospace',
}

TOP = re.compile(r"^([a-z][\w-]*):\s*(.*)$")
SUB = re.compile(r"^  ([\w-]+):\s*(.*)$")
DEEP = re.compile(r"^    ([\w-]+):\s*(.*)$")


def unquote(v):
    v = v.strip()
    if len(v) >= 2 and v[0] == v[-1] and v[0] in "\"'":
        return v[1:-1]
    return v


def frontmatter(path):
    text = path.read_text(encoding="utf-8")
    if not text.startswith("---\n"):
        raise SystemExit(f"{path}: no YAML frontmatter")
    body, _, rest = text[4:].partition("\n---\n")
    if not rest:
        raise SystemExit(f"{path}: frontmatter is not closed")
    doc, section, sub = {}, None, None
    for line in body.splitlines():
        if not line.strip() or line.lstrip().startswith("#"):
            continue
        m = TOP.match(line)
        if m:
            key, val = m.group(1), unquote(m.group(2))
            section = key
            doc[key] = val if val else {}
            sub = None
            continue
        m = SUB.match(line)
        if m and isinstance(doc.get(section), dict):
            key, val = m.group(1), unquote(m.group(2))
            sub = key
            doc[section][key] = val if val else {}
            continue
        m = DEEP.match(line)
        if m and isinstance(doc[section].get(sub), dict):
            doc[section][sub][m.group(1)] = unquote(m.group(2))
            continue
        raise SystemExit(f"{path}: cannot parse {line!r}")
    return doc


def resolve(value, doc):
    """`{colors.ink}` -> `#0B0316`; anything else passes through."""
    m = re.fullmatch(r"\{(.+)\}", value.strip())
    if not m:
        return value
    node = doc
    for part in m.group(1).split("."):
        if not isinstance(node, dict) or part not in node:
            raise SystemExit(f"{DESIGN}: reference {value!r} does not resolve")
        node = node[part]
    if isinstance(node, dict):
        raise SystemExit(f"{DESIGN}: reference {value!r} is not a scalar")
    return node


def css(doc):
    lines = [
        "/* Generated from DESIGN.md by tools/gen_tokens.py. Do not edit: edit DESIGN.md. */",
        "",
        ":root {",
    ]
    for name, val in doc.get("colors", {}).items():
        lines.append(f"  --color-{name}: {val};")
    for name, typ in doc.get("typography", {}).items():
        for prop, suffix in (("fontFamily", "family"), ("fontSize", "size"), ("fontWeight", "weight"),
                             ("lineHeight", "leading"), ("letterSpacing", "tracking")):
            if prop in typ:
                v = typ[prop]
                if prop == "fontFamily":
                    v = STACKS.get(v, v)
                lines.append(f"  --type-{name}-{suffix}: {v};")
    for name, val in doc.get("rounded", {}).items():
        lines.append(f"  --rounded-{name}: {val};")
    for name, val in doc.get("spacing", {}).items():
        lines.append(f"  --space-{name}: {val};")
    lines += ["}", ""]

    # One utility class per type scale step, so a page sets no font values itself.
    for name in doc.get("typography", {}):
        typ = doc["typography"][name]
        lines += [
            f".t-{name} {{",
            f"  font-family: var(--type-{name}-family);",
            f"  font-size: var(--type-{name}-size);",
            f"  font-weight: var(--type-{name}-weight);",
            f"  line-height: var(--type-{name}-leading);",
        ]
        if "letterSpacing" in typ:
            lines.append(f"  letter-spacing: var(--type-{name}-tracking);")
        lines += ["}", ""]

    # Components: the contract's own list, resolved to concrete declarations.
    for name, comp in doc.get("components", {}).items():
        lines.append(f".g-{name} {{")
        for prop, out in (("backgroundColor", "background-color"), ("textColor", "color"),
                          ("borderColor", "border-color"), ("rounded", "border-radius")):
            if prop in comp:
                lines.append(f"  {out}: {resolve(comp[prop], doc)};")
        lines.append("}")
        lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def luminance(hex_colour):
    h = hex_colour.lstrip("#")
    if len(h) == 3:
        h = "".join(c * 2 for c in h)
    r, g, b = (int(h[i:i + 2], 16) / 255 for i in (0, 2, 4))
    def lin(c):
        return c / 12.92 if c <= 0.04045 else ((c + 0.055) / 1.055) ** 2.4
    return 0.2126 * lin(r) + 0.7152 * lin(g) + 0.0722 * lin(b)


def ratio(a, b):
    la, lb = luminance(a), luminance(b)
    hi, lo = max(la, lb), min(la, lb)
    return (hi + 0.05) / (lo + 0.05)


def check(doc):
    colors = doc["colors"]
    pairs = []
    for name, comp in doc.get("components", {}).items():
        if "textColor" in comp and "backgroundColor" in comp:
            pairs.append((f"component {name}", resolve(comp["textColor"], doc),
                          resolve(comp["backgroundColor"], doc), 4.5))
    # Supporting text and focus, which the components do not spell out.
    for fg, bg in (("muted", "ink"), ("muted", "surface"), ("focus", "ink"),
                   ("on-surface", "surface-raised"), ("light-muted", "light-surface"),
                   ("light-on-surface", "light-surface")):
        pairs.append((f"{fg} on {bg}", colors[fg], colors[bg], 4.5))
    bad = 0
    for label, fg, bg, need in pairs:
        r = ratio(fg, bg)
        verdict = "ok" if r >= need else "FAIL"
        if r < need:
            bad += 1
        print(f"  {verdict:4} {r:5.2f}:1  (needs {need}:1)  {label}: {fg} on {bg}")
    print(f"\n{len(pairs) - bad}/{len(pairs)} pairings pass WCAG 2.2 AA")
    return bad


def main(argv):
    doc = frontmatter(DESIGN)
    if "--check" in argv:
        return 1 if check(doc) else 0
    if "--verify" in argv:
        want = css(doc)
        have = OUT.read_text(encoding="utf-8") if OUT.exists() else ""
        if want != have:
            print(f"{OUT.relative_to(ROOT)} is stale: run tools/gen_tokens.py")
            return 1
        print(f"{OUT.relative_to(ROOT)} matches {DESIGN.name}")
        return 0
    out = css(doc)
    if "--stdout" in argv:
        sys.stdout.write(out)
        return 0
    OUT.write_text(out, encoding="utf-8")
    n = len(doc.get("colors", {})) + len(doc.get("typography", {})) + len(doc.get("components", {}))
    print(f"{OUT.relative_to(ROOT)}: {len(out)} B, {n} tokens and components from {DESIGN.name}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
