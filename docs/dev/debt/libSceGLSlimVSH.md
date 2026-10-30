# libSceGLSlimVSH technical debt

### Additional callable exports

- [Additional callable exports](../../../core/libs/prx/libSceGLSlimVSH/Export.cpp) - 7 callable NIDs match the [public 3.20 catalogue](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceGLSlimVSH.c) and the [selected export metadata](https://gist.github.com/kyon-eth/e1b845e2be0047e065a7dbb2c2e92322/7a180a59bf97bf6ea42020d4eedf9a8d631b0fd3) from two version-labelled images (SHA-256 `5d1eb4913d2914be044c05a48c4287a3a73e526605f21ec37ce9acfb5b2170ad`). The labels are not authenticated. Prototypes and behavior are unverified; these declarations throw through `NotImplemented_nid_no_patch` and implement no runtime functionality.

| Name | NID |
| --- | --- |
| `sceSlimglCompositorSetInvisibleCanvasCommand` | `DN8v2xNOyjg` |
| `sceSlimglCompositorSetMemoryCommand` | `aswH9c+RN0E` |
| `sceSlimglCompositorSetPostEventCommand` | `WvuRaahcTfA` |
| `sceSlimglCompositorWaitEndOfRendering` | `lcaik1AxwEY` |
| `sceSlimglCompositorWaitPostEvent` | `HwBlC8TLKHQ` |
| `sceSlimglIPCEnd` | `Fa0atOcWPaI` |
| `sceSlimglIPCSetup` | `F4pQ5tsDo74` |
