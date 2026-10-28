# libSceResourceArbitrator technical debt

### Additional callable export inventory

- [Additional callable exports](../../../core/libs/prx/libSceResourceArbitrator/Export.cpp) - 14 entries have import-resolution declarations only. Parameter lists, return types and behavior are unverified; parameterless `int` declarations are placeholders, not verified ABI contracts. Every entry throws through `NotImplemented_nid_no_patch`. Unidentified names use literal NID aliases. No runtime support, implemented-function increase or title unblock is claimed.

| Name | NID |
| --- | --- |
| `sceResourceArbitratorChangeState` | `CYjfezkWcaE` |
| `sceResourceArbitratorClearCallback` | `LqmYZGB-UEQ` |
| `sceResourceArbitratorGetFeatureIdList` | `fBX+DBOwNis` |
| `sceResourceArbitratorIsExecutable` | `3iZXcGeRgJk` |
| `sceResourceArbitratorSetCallback` | `kJvjeet+f9I` |
| `Unknown` | `3xRoVp7fPPY` |
| `Unknown` | `GmaE0Ew4h4s` |
| `Unknown` | `OM6SXhDZdiU` |
| `Unknown` | `TYiIrKTzQ8s` |
| `Unknown` | `cT1vA0DTMcs` |
| `Unknown` | `jzKwHzhEBIQ` |
| `Unknown` | `mtLIcm9hwFM` |
| `Unknown` | `rMJnXXj6GIo` |
| `Unknown` | `ru9qkt9UCLQ` |
