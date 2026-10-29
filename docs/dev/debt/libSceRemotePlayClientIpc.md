# libSceRemotePlayClientIpc technical debt

### Additional callable exports

- [Additional callable exports](../../../core/libs/prx/libSceRemotePlayClientIpc/Export.cpp) - 18 entries observed in the 3.20 export inventory and present in the later 13.60-labelled inventory are import-resolution declarations only. The later inventory label does not authenticate a retail firmware version. Parameter lists, return types and behavior are unverified; parameterless `int` declarations are placeholders, not verified ABI contracts. Every entry throws through `NotImplemented_nid_no_patch`. Unidentified names use literal NID aliases. No runtime functionality is implemented. Identified candidate names match their NIDs in the [public symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) or [supplemental catalog](https://github.com/claimore22/ps5rs/blob/2304be5131221bae0dac24b9767cabffda33f747/data/nids.csv).

[Version-pinned selected ELF export records](https://gist.github.com/kyon-eth/752e1b1d4a6ac8a26c97e693527fefbd/6a60d79146e8960fe6da14716d2b38055db04228) list every NID below with its module, raw qualified symbol, ELF function type, value and size for each inspected version-labelled module. The extract SHA-256 is `52357fe6274f0b5057b20f12d4e3ba5446161db8d9aac0c926beed1dd22b0bbd`; its full-module sorted-function-NID digests and raw module-parameter words permit independent comparison. The extract is an analyst-produced metadata record, not independent authentication of the original images or proof of prototypes or behavior. The separately published 3.20 source catalog has no module file for this library.

| Name | NID |
| --- | --- |
| `sceRemotePlayClientIpcGetClientStatus` | `irNARl+D5xU` |
| `sceRemotePlayClientIpcInitialize` | `jJ38aat7wbo` |
| `sceRemotePlayClientIpcSetClientStatus` | `GJM1+qhe8hU` |
| `sceRemotePlayClientIpcTerminateStreaming` | `9CcjWFByTOA` |
| `sceRemotePlayClientIpcTerminate` | `iFHBKxCdlwI` |
| `sceRemotePlayClientIpcWaitConnectionAsync` | `70mIea0Xnxk` |
| `Unknown` | `0Ezd4jv7F-4` |
| `Unknown` | `2dzIQ1US7Ag` |
| `Unknown` | `4+sKR9yWkWY` |
| `Unknown` | `EVbYn8UOvMs` |
| `Unknown` | `JGKtr0+B87A` |
| `Unknown` | `JVKO2gPlWPw` |
| `Unknown` | `M16-T2ERA8c` |
| `Unknown` | `PnLBwmAGb-g` |
| `Unknown` | `RiyUwZBmS-k` |
| `Unknown` | `a3LqZtr60o4` |
| `Unknown` | `gO3jwkOQ834` |
| `Unknown` | `gqmq4cTnR0M` |
