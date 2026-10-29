# libSceAvcap2mvr technical debt

### Additional callable exports

- [Additional callable exports](../../../core/libs/prx/libSceAvcap2mvr/Export.cpp) - 13 entries observed in the 3.20 export inventory and present in the later 13.60-labelled inventory are import-resolution declarations only. The later inventory label does not authenticate a retail firmware version. Parameter lists, return types and behavior are unverified; parameterless `int` declarations are placeholders, not verified ABI contracts. Every entry throws through `NotImplemented_nid_no_patch`. Unidentified names use literal NID aliases. No runtime functionality is implemented. Identified candidate names match their NIDs in the [public symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) or [supplemental catalog](https://github.com/claimore22/ps5rs/blob/2304be5131221bae0dac24b9767cabffda33f747/data/nids.csv).

| Name | NID |
| --- | --- |
| `Unknown` | `AVuH0FujF30` |
| `Unknown` | `JgAC1LkbWns` |
| `Unknown` | `Kt2IOGEfaFc` |
| `Unknown` | `Mq1MuGlr48g` |
| `Unknown` | `QSLdUawhEhg` |
| `Unknown` | `R5tFB0tMbgY` |
| `Unknown` | `Tjmx+DOF65c` |
| `Unknown` | `ZMR+Q0NYcmU` |
| `Unknown` | `nPlusHmt9NY` |
| `Unknown` | `s7aQGkLitBU` |
| `Unknown` | `sM27f1nWpCc` |
| `Unknown` | `tq-PG8QaNDM` |
| `Unknown` | `u4wtg4AegL8` |
