#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# C static checks: what a Go tree gets from go vet and staticcheck, where C has an equivalent.
#
#   tests/lint.sh                 the C this branch changes (the lint-changed model)
#   tests/lint.sh --all           every firmware source, as a periodic sweep
#   tests/lint.sh FILE...         the given sources
#
# Run from the repo root after a host build (build/gen holds the generated headers).
#
# Two gates, both scoped to the sources selected above:
#   1. the compiler's warnings, through the host's translation unit (tests/boot_sound_test.c pulls in
#      editor_host.c, which pulls in every source the host build compiles). The warning has to name one
#      of the selected files to count: the parent tree has warnings of its own, and a fork that fails on
#      them teaches people to ignore the gate.
#   2. cppcheck (warning, portability) over the selected sources. The findings that exist in the parent
#      tree today are in tests/lint-baseline.txt, one file:id per line: triaged, not forgotten. Anything
#      not in that file fails. Delete a line when the finding is fixed; add one only with its reason, in
#      the commit that adds it.
#
# Left out on purpose: a formatter. This tree is hand-formatted (dense tables, aligned trailing
# comments, statements packed where they fit) and clang-format would rewrite every line of it. The style
# belongs to the parent; these checks are about defects. -Wshadow is out for the same reason: the tree
# uses `fx` as a parameter name throughout punch.c and seq.c while fx.c holds a global of that name.
set -e
cd "$(dirname "$0")/.."
CC="${CC:-cc}"
BASE=tests/lint-baseline.txt
INC="-Ibuild/gen -Ifirmware/src -Ifirmware/hal"
W="-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-int-to-pointer-cast -Wno-unused-function"
fails=0

[ -d build/gen ] || { echo "lint: no build/gen (build the firmware once: ./build.sh)"; exit 1; }
command -v cppcheck >/dev/null || { echo "lint: no cppcheck (nix shell nixpkgs#cppcheck)"; exit 1; }

case "${1:-}" in
    "")   srcs=$(git diff --name-only --diff-filter=d main...HEAD -- 'firmware/src/*.c' 'tests/*.c' 2>/dev/null || true) ;;
    --all) srcs=$(ls firmware/src/*.c) ;;
    *)    srcs="$*" ;;
esac

# 1. the compiler's warnings. Carets and notes off, one line per warning, so a warning can be attributed
#    to its file. --all sweeps the parent tree's warnings but does not fail on them: it is here for
#    cppcheck, and the tree is not warning-free today.
warn=$($CC -O2 -fsyntax-only -fno-diagnostics-show-caret $W $INC tests/boot_sound_test.c 2>&1 || true)
mine=$(printf '%s\n' "$warn" | awk -v list="$srcs" '
    BEGIN { n = split(list, a, " "); for (i = 1; i <= n; i++) want[a[i]] = 1 }
    {
        line = $0
        sub(/^\.\//, "", line)
        sub(/^tests\/\.\.\//, "", line)
        if (line !~ /: warning:/) next
        split(line, f, ":")
        if (f[1] in want) print line
    }')
if [ "${1:-}" = "--all" ]; then
    echo "lint: cc: $(printf '%s\n' "$warn" | grep -c ': warning:') warning(s) in the tree, none of them a gate"
elif [ -n "$mine" ]; then
    printf '%s\n' "$mine" | sed 's/^/lint: cc: /'
    echo "lint: cc: the warnings above are in the files checked"
    fails=$((fails + 1))
else
    echo "lint: cc: clean ($(printf '%s\n' $srcs | grep -c .) file(s))"
fi

# 2. cppcheck. One file at a time, with a time limit: a deep include chain can otherwise hold the sweep
#    up, and a file that times out is reported rather than silently skipped.
tmp=$(mktemp /tmp/lint.XXXXXX)
keys=$(mktemp /tmp/lint.XXXXXX)
base=$(mktemp /tmp/lint.XXXXXX)
trap 'rm -f "$tmp" "$keys" "$base"' EXIT
for f in $srcs; do
    out=$(timeout 25 cppcheck --enable=warning,portability --inline-suppr --quiet \
              --template='{file}:{id}:{line}: {message}' -I build/gen -I firmware/src "$f" 2>&1) || true
    [ -n "$out" ] && printf '%s\n' "$out" >> "$tmp"
done
[ -f "$tmp" ] || : > "$tmp"
cut -d: -f1,2 "$tmp" | grep . | sort -u > "$keys" || : > "$keys"
# the baseline's trailing comments are for people; the comparison is the file:id alone
if [ -f "$BASE" ]; then
    sed 's/[[:space:]]*#.*$//; s/[[:space:]]*$//' "$BASE" | grep . | sort -u > "$base" || true
fi
new=$(comm -23 "$keys" "$base")
if [ -n "$new" ]; then
    for k in $new; do grep -F "$k:" "$tmp" | sed 's/^/lint: cppcheck: /'; done
    echo "lint: cppcheck: the findings above are new (a fixed one can go from $BASE)"
    fails=$((fails + 1))
else
    echo "lint: cppcheck: clean ($(printf '%s\n' $srcs | grep -c .) file(s))"
fi

exit $((fails != 0))
