#!/usr/bin/env bash
#
# Build a small standalone GSC compiler/VM harness and run the VM recovery
# regression tests against it.
#
# The harness is deliberately built with assertions enabled so that a broken
# setjmp/longjmp recovery path aborts instead of silently corrupting the VM.
#
# Usage:
#   tests/run_vm_tests.sh
#   OPT=-O2 tests/run_vm_tests.sh
#   BUILD_DIR=/tmp/my-vm-build tests/run_vm_tests.sh
#
# Exit status is 0 only when:
#   1. the fixed harness produces the expected output for every test script;
#   2. a "mutant" harness with the volatile qualifiers stripped fails every
#      recovery script (i.e. the tests actually exercise the bug);
#   3. the fixed VM source builds warning-free under -Wclobbered -Werror while
#      the mutant triggers -Wclobbered warnings (i.e. the compiler itself
#      confirms the locals are live across longjmp);
#   4. the old optimize("O0") workaround is gone and the public header still
#      carries the volatile contract.
#
set -u

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
ROOT=$(cd -- "$SCRIPT_DIR/.." && pwd)

CC=${CC:-gcc}
OPT=${OPT:--O1}
BUILD_DIR=${BUILD_DIR:-"$ROOT/obj_test/vm"}

# Keep the project's code-generation flags, but intentionally omit -DNDEBUG:
# the VAR_CODEPOS assertion is part of what makes this test useful.
CFLAGS=${CFLAGS:-"-fno-pie $OPT -fno-omit-frame-pointer -msse2 -mfpmath=sse -DUSE_SSE2"}
LDFLAGS=${LDFLAGS:-"-no-pie"}
LIBS=${LIBS:-"-lm -lpthread -lstdc++ -ldl"}

COMMON_SOURCES=(
	src/script/scr_animtree.cpp
	src/script/scr_compiler.cpp
	src/script/scr_evaluate.cpp
	src/script/scr_main.cpp
	src/script/scr_memorytree.cpp
	src/script/scr_parser.cpp
	src/script/scr_stringlist.cpp
	src/script/scr_tempmemory.cpp
	src/script/scr_variable.cpp
	src/script/scr_yacc.cpp
	src/universal/com_memory.cpp
	src/universal/q_parse.cpp
	tests/gsc_standalone.cpp
)

pass=0
fail=0

die() {
	echo "error: $*" >&2
	exit 2
}

pass_msg() {
	echo "PASS: $*"
	pass=$((pass + 1))
}

fail_msg() {
	echo "FAIL: $*"
	fail=$((fail + 1))
}

build_common_objects() {
	local src name out

	mkdir -p "$BUILD_DIR/common" || die "cannot create $BUILD_DIR/common"

	for src in "${COMMON_SOURCES[@]}"; do
		name=$(basename "$src" .cpp)
		out="$BUILD_DIR/common/$name.o"

		if ! $CC -c $CFLAGS -o "$out" "$ROOT/$src"; then
			die "failed to compile $src"
		fi
	done
}

build_fixed_vm() {
	mkdir -p "$BUILD_DIR/fixed"

	if ! $CC -c $CFLAGS -o "$BUILD_DIR/fixed/scr_vm.o" \
		"$ROOT/src/script/scr_vm.cpp"; then
		die "failed to compile fixed src/script/scr_vm.cpp"
	fi

	if ! $CC $LDFLAGS -o "$BUILD_DIR/fixed/gsc" \
		"$BUILD_DIR"/common/*.o "$BUILD_DIR/fixed/scr_vm.o" $LIBS; then
		die "failed to link fixed harness"
	fi
}

build_mutant_vm() {
	local mutant_src="$BUILD_DIR/mutant/src"

	mkdir -p "$BUILD_DIR/mutant"

	# Copy the headers and VM source so the mutant's declarations agree with
	# its definition. The rest of the sources are built once and shared.
	rm -rf "$mutant_src"
	cp -a "$ROOT/src" "$mutant_src" || die "failed to copy src for mutant build"

	sed -i 's/volatile //g' \
		"$mutant_src/script/scr_vm.cpp" \
		"$mutant_src/script/script_public.h" \
		|| die "failed to strip volatile for mutant build"

	if cmp -s "$ROOT/src/script/scr_vm.cpp" "$mutant_src/script/scr_vm.cpp"; then
		die "mutant build did not modify scr_vm.cpp"
	fi

	if ! $CC -c $CFLAGS -o "$BUILD_DIR/mutant/scr_vm.o" \
		"$mutant_src/script/scr_vm.cpp"; then
		die "failed to compile mutant src/script/scr_vm.cpp"
	fi

	if ! $CC $LDFLAGS -o "$BUILD_DIR/mutant/gsc" \
		"$BUILD_DIR"/common/*.o "$BUILD_DIR/mutant/scr_vm.o" $LIBS; then
		die "failed to link mutant harness"
	fi
}

run_harness() {
	local binary=$1
	local name=$2
	local output=$3

	(
		cd "$SCRIPT_DIR" || exit 2
		timeout 30 "$binary" "gsc/$name" main
		rc=$?
		exit "$rc"
	) >"$output" 2>&1

	return $?
}

run_case() {
	local name=$1
	local expect_mutant_fail=$2
	local expected="$SCRIPT_DIR/gsc/expected/$name.txt"
	local fixed_out="$BUILD_DIR/$name.fixed.out"
	local mutant_out="$BUILD_DIR/$name.mutant.out"
	local fixed_rc mutant_rc

	[ -f "$expected" ] || die "missing expected output: $expected"

	run_harness "$BUILD_DIR/fixed/gsc" "$name" "$fixed_out"
	fixed_rc=$?

	if [ "$fixed_rc" -ne 0 ]; then
		fail_msg "$name (fixed harness exited $fixed_rc)"
		sed -n '1,20p' "$fixed_out" | sed 's/^/      /'
		return
	fi

	if ! diff -u "$expected" "$fixed_out" >"$BUILD_DIR/$name.fixed.diff"; then
		fail_msg "$name (fixed harness output differs from expected)"
		sed -n '1,40p' "$BUILD_DIR/$name.fixed.diff" | sed 's/^/      /'
		return
	fi

	pass_msg "$name (fixed harness)"

	if run_harness "$BUILD_DIR/mutant/gsc" "$name" "$mutant_out"; then
		mutant_rc=0
	else
		mutant_rc=$?
	fi

	if [ "$expect_mutant_fail" -eq 1 ]; then
		if [ "$mutant_rc" -eq 0 ] && diff -q "$expected" "$mutant_out" >/dev/null; then
			fail_msg "$name (mutant harness unexpectedly passed; test does not bite)"
			return
		fi

		pass_msg "$name (mutant harness failed as expected, rc=$mutant_rc)"
		sed -n '1p' "$mutant_out" | sed 's/^/      /'
	else
		if [ "$mutant_rc" -ne 0 ]; then
			fail_msg "$name (control script failed in mutant harness, rc=$mutant_rc)"
			sed -n '1,20p' "$mutant_out" | sed 's/^/      /'
			return
		fi

		if ! diff -u "$expected" "$mutant_out" >"$BUILD_DIR/$name.mutant.diff"; then
			fail_msg "$name (control script output differs in mutant harness)"
			sed -n '1,40p' "$BUILD_DIR/$name.mutant.diff" | sed 's/^/      /'
			return
		fi

		pass_msg "$name (control script still passes in mutant harness)"
	fi
}

if [ "$OPT" = "-O0" ]; then
	echo "warning: OPT=-O0 generally does not reproduce the setjmp/longjmp bug;" >&2
	echo "warning: use -O1 or higher to validate that the tests bite." >&2
fi

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR" || die "cannot create build directory $BUILD_DIR"

echo "==> building harness ($CC $CFLAGS)"
build_common_objects
build_fixed_vm
build_mutant_vm

echo "==> running VM recovery tests"
shopt -s nullglob
recovery_scripts=("$SCRIPT_DIR"/gsc/toomany_*.gsc "$SCRIPT_DIR"/gsc/switch_err.gsc "$SCRIPT_DIR"/gsc/inc_err.gsc)

[ "${#recovery_scripts[@]}" -gt 0 ] || die "no recovery test scripts found"

for path in "${recovery_scripts[@]}"; do
	name=$(basename "$path" .gsc)
	run_case "$name" 1
done

# A script that does not hit the error-recovery path must work in both builds.
# This catches a mutant that is simply broken for unrelated reasons.
if [ -f "$SCRIPT_DIR/gsc/normal_control.gsc" ]; then
	run_case normal_control 0
fi

echo "==> running static contract checks"

# The old workaround must stay gone: VM_Execute may not pin an optimization
# level via a function attribute.
if grep -q 'optimize(' "$ROOT/src/script/scr_vm.cpp"; then
	fail_msg "src/script/scr_vm.cpp still contains an optimize() attribute"
else
	pass_msg "no optimize() workaround in VM_Execute"
fi

# The public header is part of the fix: VM_ArchiveStack must take a volatile
# localId so callers can pass &localId without casting volatile away.
if grep -q 'VM_ArchiveStack(int size, const char \*codePos, VariableValue \*top, unsigned int localVarCount, volatile unsigned int \*localId)' \
	"$ROOT/src/script/script_public.h"; then
	pass_msg "VM_ArchiveStack keeps volatile contract in header"
else
	fail_msg "VM_ArchiveStack lost volatile contract in script_public.h"
fi

# Call sites must not cast volatile away anymore.
if grep -q '(unsigned int \*)&localId' "$ROOT/src/script/scr_vm.cpp"; then
	fail_msg "scr_vm.cpp still casts volatile localId away at VM_ArchiveStack call sites"
else
	pass_msg "no volatile-discarding casts at VM_ArchiveStack call sites"
fi

# The compiler's own longjmp analysis must agree: fixed code builds clean with
# -Wclobbered -Werror, while the volatile-stripped mutant must warn. This is
# the per-variable backstop for locals whose miscompilation needs specific
# register pressure to turn into a runtime crash.
if $CC -c $CFLAGS -Wclobbered -Werror -o "$BUILD_DIR/fixed/scr_vm_clobbered.o" \
	"$ROOT/src/script/scr_vm.cpp" 2>"$BUILD_DIR/fixed/clobbered.err"; then
	pass_msg "fixed scr_vm.cpp builds clean under -Wclobbered -Werror"
else
	fail_msg "fixed scr_vm.cpp warns under -Wclobbered -Werror"
	sed -n '1,20p' "$BUILD_DIR/fixed/clobbered.err" | sed 's/^/      /'
fi

if $CC -c $CFLAGS -Wclobbered -o "$BUILD_DIR/mutant/scr_vm_clobbered.o" \
	"$BUILD_DIR/mutant/src/script/scr_vm.cpp" 2>"$BUILD_DIR/mutant/clobbered.err"; then
	if grep -q 'Wclobbered' "$BUILD_DIR/mutant/clobbered.err"; then
		pass_msg "mutant scr_vm.cpp triggers -Wclobbered as expected"
		grep -oP '(variable|argument) ‘\K[^’]+' "$BUILD_DIR/mutant/clobbered.err" \
			| sort -u | tr '\n' ' ' | sed 's/^/      clobbered: /'
	else
		fail_msg "mutant scr_vm.cpp built without -Wclobbered warnings; static check does not bite"
	fi
else
	# A hard error under -Wclobbered (without -Werror) also proves the point,
	# but surface it for inspection.
	fail_msg "mutant scr_vm.cpp failed to compile under -Wclobbered"
	sed -n '1,20p' "$BUILD_DIR/mutant/clobbered.err" | sed 's/^/      /'
fi

echo
echo "==> results: $pass passed, $fail failed"

if [ "$fail" -ne 0 ]; then
	exit 1
fi

exit 0
