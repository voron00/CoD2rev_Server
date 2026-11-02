# GSC VM tests

These tests build a small standalone GSC compiler/VM harness from the real
`src/script` sources and run scripted scenarios against it. They exist mainly to
prevent regressions in the `setjmp`/`longjmp` error-recovery path of
`VM_Execute`.

Run them with:

```sh
tests/run_vm_tests.sh
```

By default the harness is built with `-O1` (the optimization level used by the
main Makefile). You can override it with `OPT=-O2` or `OPT=-O3`.

## Why the tests are shaped this way

`VM_Execute` uses `setjmp`/`longjmp` for recoverable GSC errors. State that is
modified between `setjmp` and `longjmp` must be `volatile`; otherwise an
optimized build restores stale register values and the VM resumes at the wrong
bytecode position with the wrong operand stack. The classic trigger is a
non-fatal error such as calling a function with too many parameters, which the
VM is expected to recover from and continue running.

Every recovery script intentionally triggers that path: `toomany_*.gsc`
(too many call parameters), `switch_err.gsc` (non-integer `switch`), and
`inc_err.gsc` (`++` on a non-int). The test runner:

1. builds the harness from the current sources and checks that each script
   produces the expected output;
2. builds a second "mutant" harness from the same sources with the `volatile`
   qualifiers stripped from `VM_Execute`, the `Scr_Read*` helpers, and
   `VM_ArchiveStack`;
3. checks that the mutant actually fails on every recovery script
   (`toomany_*` abort with `top->type != VAR_CODEPOS`, `switch_err`
   segfaults, `inc_err` aborts);
4. checks the static contract: no `optimize(` attribute in `scr_vm.cpp`,
   `volatile` intact in `script_public.h` with cast-free call sites, fixed
   code clean under `-Wclobbered -Werror` while the mutant warns.

Step 3 is the important one: if the regression tests pass but the mutant also
passes, the tests do not actually cover the bug and are treated as failures.
Single-variable reverts usually still pass at `-O1` (register allocation only
goes wrong once the whole set is gone), which is why the mutant strips the
whole set and `-Wclobbered` acts as the per-variable backstop.
