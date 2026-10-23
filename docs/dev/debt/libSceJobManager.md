# libSceJobManager technical debt

### Functional

- [sce::Job::JobManager](../../../core/libs/prx/libSceJobManager/Export.cpp) submits commands as work becomes runnable rather than encoding ahead with the rewind protocol recovered from PPSA06888's module (offsets 0x2210 and 0x2650). Commands are recycled after a completed wait; exhausting the bounded command area throws. Native recovery from command exhaustion and console timing equivalence remain unverified.
- The [manager](../../../core/libs/prx/libSceJobManager/Export.cpp) keeps scheduling state and nodes on the host heap; only the header, compute commands and completion labels occupy the caller's block. Blocks smaller than 0x2d8 bytes throw. The worker spin duration and persistent thread count are validated but do not govern scheduling.
- [Sequence and manager operations](../../../core/libs/prx/libSceJobManager/Export.cpp) throw on a compute dispatch on a CPU sequence, a failing `sceAgcDriverAddEqEvent`, a factory priority above 1, and use of a destroyed sequence. `shutdown` throws while compute work is in flight or a caller is blocked in a sequence wait. Worker creation and attribute failures throw where the original does not check.
- [`initialize`](../../../core/libs/prx/libSceJobManager/Export.cpp) returns 0 instead of the result of the original's final import (`BG26hBGiNlw`, unresolved). The `sceRazorCpuJobManager*` profiler calls are not made. Sequence and factory virtual-function order is recovered from module vtables; no calling title has been run.
