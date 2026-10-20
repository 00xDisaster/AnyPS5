# libSceVrTracker2 technical debt

### Silent stubs

- [libSceVrTracker2](../../../core/libs/prx/libSceVrTracker2/Export.cpp) never initializes; initialized tracking behavior is absent. Its uninitialized responses were measured on firmware 13.60 without a headset: most exports return 0x8A740001 without writes, while specific output/reset helpers clear buffers or write defaults. These helpers still lack complete signatures. Named export signatures and play-area output layouts are recovered; initialization option semantics and result fields need reconstruction. Anonymous exports remain grouped by their observed response rather than their individual contracts.

### Unknown function info

- [sceVrTracker2Initialize](../../../core/libs/prx/libSceVrTracker2/Export.cpp) - the 72-byte parameter layout is recovered, including thread priorities, CPU affinity masks, work memory, size/alignment and coordinate system. Meanings of the 32-bit option at 0x30 and byte options at 0x38/0x39 remain unresolved; initialization remains unimplemented.
- [libSceVrTracker2](../../../core/libs/prx/libSceVrTracker2/Export.cpp) - the seven anonymous throwing exports now have their consumed argument slots restored. Calibration output (`jRmuXjsknsI`) uses a 32-bit capacity with 608/1216-byte variants; its fields remain unresolved. Mesh requirements (`NBhWy+Gl0tY`) contain four 32-bit counts/sizes. The mode/buffer/capacity/size export (`9R6W33Au4B4`) uses 64-bit sizes. The 32/48-byte result parameters are recovered; result output and pose parameters still need full layouts; the telemetry input (`oZDm1bynCaA`) spans 7024 bytes. Their bodies remain unimplemented.
