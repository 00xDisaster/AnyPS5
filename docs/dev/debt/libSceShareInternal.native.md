# libSceShareInternal.native technical debt

### Additional callable exports

- [Additional callable exports](../../../core/libs/prx/libSceShareInternal.native/Export.cpp) - 20 entries observed in the 3.20 export inventory and present in the later 13.60-labelled inventory are import-resolution declarations only. The later inventory label does not authenticate a retail firmware version. Parameter lists, return types and behavior are unverified; parameterless `int` declarations are placeholders, not verified ABI contracts. Every entry throws through `NotImplemented_nid_no_patch`. Unidentified names use literal NID aliases. No runtime functionality is implemented. Identified candidate names match their NIDs in the [public symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) or [supplemental catalog](https://github.com/claimore22/ps5rs/blob/2304be5131221bae0dac24b9767cabffda33f747/data/nids.csv).

| Name | NID |
| --- | --- |
| `sceShareEventCallbackProcessed` | `1b7lsCtwocs` |
| `sceShareFeaturePermitCompatible` | `KfYRGDAbBp8` |
| `sceShareFeatureProhibitCompatible` | `NiEk-9J9gUA` |
| `sceShareGetContentParam` | `Y7ZE2F525XM` |
| `sceShareGetStoredAppStatus` | `9AMk7pW7i1Q` |
| `sceShareInitializeForShellCore` | `MpK73-5MRgY` |
| `sceShareInitializeInternal` | `AxAOjyc3QMQ` |
| `sceShareRegisterEventCallback` | `t1KtEtRCXYY` |
| `sceShareSetCurrentStatus` | `A4P0NTFvFOU` |
| `sceShareSetUploadEventInfo` | `yOI3xaWu7oY` |
| `sceShareTerminateForShellCore` | `YNfU-whRzF4` |
| `sceShareTerminateInternal` | `W-XUH997Geg` |
| `sceShareUnregisterEventCallback` | `Zypvbu9ShEY` |
| `Unknown` | `+LB32s0kCYg` |
| `Unknown` | `0IbPXX-WvQQ` |
| `Unknown` | `8E7VGc9qRvY` |
| `Unknown` | `9fIEK6i1idg` |
| `Unknown` | `9oywh7VdSjM` |
| `Unknown` | `IvkDssl52mA` |
| `Unknown` | `zyzD8P5ZKPA` |
