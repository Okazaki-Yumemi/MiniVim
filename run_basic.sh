#!/usr/bin/env bash

set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

TEST_DIR="$ROOT/test/basic"
OUT_DIR="$ROOT/.test-output"
BIN="$ROOT/code"

# 优先使用环境变量 VTEMU，其次 PATH，最后尝试兄弟目录 ../vtemu
if [[ -n "${VTEMU:-}" ]]; then
    VTEMU_BIN="$VTEMU"
elif command -v vtemu >/dev/null 2>&1; then
    VTEMU_BIN="$(command -v vtemu)"
elif [[ -x "$ROOT/../vtemu/bin/vtemu" ]]; then
    VTEMU_BIN="$ROOT/../vtemu/bin/vtemu"
else
    echo "[ERROR] Cannot find vtemu."
    echo "Set it manually, for example:"
    echo "  VTEMU=../vtemu/bin/vtemu ./run_basic.sh"
    exit 1
fi

echo "== MiniVim Basic Regression =="
echo "vtemu: $VTEMU_BIN"
echo

echo "[BUILD] make clean && make"
if ! make clean || ! make; then
    echo "[FAIL] build failed"
    exit 1
fi

if [[ ! -x "$BIN" ]]; then
    echo "[FAIL] $BIN does not exist or is not executable"
    exit 1
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

# Basic 保存测试会在仓库根目录产生这些文件。
rm -f save save1 save2

pass=0
fail=0

check_screen() {
    local name="$1"
    local input="$TEST_DIR/$name.in"
    local answer="$TEST_DIR/$name.ans"
    local output="$OUT_DIR/$name.out"

    printf "%-8s " "[$name]"

    "$VTEMU_BIN" -l 24 -c 80 -x 20 "$BIN" \
        < "$input" \
        > "$output"

    if diff -q "$output" "$answer" >/dev/null; then
        echo "PASS"
        ((pass += 1))
        return 0
    else
        echo "FAIL"
        echo "  screen diff:"
        diff -u "$answer" "$output" | head -n 60
        ((fail += 1))
        return 1
    fi
}

check_saved_file() {
    local actual="$1"
    local expected="$2"
    local label="$3"

    if [[ ! -e "$actual" ]]; then
        echo "  [FAIL] $label: '$actual' was not created"
        ((fail += 1))
        return 1
    fi

    if cmp -s "$actual" "$expected"; then
        echo "  [SAVE PASS] $label"
        return 0
    else
        echo "  [SAVE FAIL] $label"
        echo "  expected: $expected"
        echo "  actual:   $actual"
        ((fail += 1))
        return 1
    fi
}

# 1 ~ 8
for i in $(seq 1 8); do
    check_screen "$i"
done

# 9 分成两次
check_screen "9-1"
check_screen "9-2"

# 10 ~ 17
for i in $(seq 10 17); do
    check_screen "$i"
done

# 18 必须按顺序执行，并保留 save1 / save2
check_screen "18-1"
check_saved_file \
    "$ROOT/save1" \
    "$TEST_DIR/18-1.save.ans" \
    "18-1 save1"

check_screen "18-2"
check_saved_file \
    "$ROOT/save1" \
    "$TEST_DIR/18-2.save.ans" \
    "18-2 save1"

check_screen "18-3"
check_saved_file \
    "$ROOT/save1" \
    "$TEST_DIR/18-3.save.ans" \
    "18-3 save1"

check_screen "18-4"
check_saved_file \
    "$ROOT/save2" \
    "$TEST_DIR/18-4.save.ans" \
    "18-4 save2"

check_screen "19"

# 20 还需要检查 save
rm -f "$ROOT/save"
check_screen "20"
check_saved_file \
    "$ROOT/save" \
    "$TEST_DIR/20.save.ans" \
    "20 save"

echo
echo "=============================="
echo "Screen tests passed: $pass"
echo "Failures:            $fail"
echo "Outputs:             $OUT_DIR"
echo "=============================="

if (( fail == 0 )); then
    echo "ALL BASIC TESTS PASSED"
    exit 0
else
    echo "BASIC REGRESSION FAILED"
    exit 1
fi