# libSceAcm technical debt

### Silent stubs

- [libSceAcm](../../../core/libs/prx/libSceAcm/Export.cpp) batches execute as silent no-ops, and do not execute encoded panner and FFT commands. `sceAcm_ConvReverb_SharedInput` returns success without encoding audio commands.

### Unknown function info

- [Additional ACM exports](../../../core/libs/prx/libSceAcm/Export.cpp) - the eleven name/NID pairs below are defined exports in the [firmware 13.60 inventory](https://github.com/xdCloudy/PS5-13.60-Function-Stub-Library/blob/bfec40deb4c47a17d7603ee2bc79aa6862c0c56b/data/firmware-13.60/libsceacm-a3566cc281.jsonl). Their ABIs and behaviour are unknown. Parameterless `int` declarations are import-resolution placeholders, not established signatures; every call throws `NotImplemented_nid_no_patch`. No audio-chain, reverb or telemetry behaviour is implemented.

| Name | NID |
| --- | --- |
| `sceAcmChain_Audio3dPO1_Clear` | `+o6KUwnCB+0` |
| `sceAcmChain_Audio3dPO1_RenderObject` | `0P2PH7lou4g` |
| `sceAcmChain_Audio3dPO1_Activate` | `5pi7bGbV7JM` |
| `sceAcmChain_Audio3dPO1_Output` | `7EGMt3d6RAk` |
| `sceAcm_ConvReverb_SharedIr_2` | `PVx9oQUaDdk` |
| `sceAcmChain_Audio3dPO1_RenderAmbisonicsChannel` | `ZViifnXeAUc` |
| `sceAcmChain_Audio3dPO2_Activate` | `aUfAPcEySHU` |
| `sceAcmChain_Audio3dPO1_RenderPassthroughObject` | `eU9FRHNi+xU` |
| `sceAcmChain_Audio3dPO2_RenderDelay` | `p23f04CznHs` |
| `sceAcmGetTelemetry` | `vQb-lO9SzEI` |
| `sceAcm_ConvReverb_SharedInput_2` | `wM41Cc+ieog` |

- [libSceAcm](../../../core/libs/prx/libSceAcm/Export.cpp) context creation, batch submission and waiting use host identifiers instead of the native device and ioctl requests. Batch identifiers, timeout handling, DSP scheduling and service-side errors remain unimplemented. Panner source parameter and matrix payloads are forwarded by the native library; their DSP interpretation remains unverified.
- [sceAcm_ConvReverb_SharedIr, sceAcm_IFFT](../../../core/libs/prx/libSceAcm/Export.cpp) (libSceAcm) - their names and NIDs are in the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceAcm.c), but no credible prototypes were found. They are declared without parameters solely to resolve known imports; this is not an ABI claim

- [`sceAcmBatchJobNotification`](../../../core/libs/prx/libSceAcm/Export.cpp) is a parameterless pending declaration for import resolution. Its signature and behavior are unverified; it throws through `NotImplemented_nid_no_patch`.

- The shared-input reverb signature and descriptor fields are recovered from native accesses. Input history indices are mutable; block metadata representation, reverb command encoding and DSP interpretation remain unimplemented in [libSceAcm](../../../core/libs/prx/libSceAcm/Export.cpp).
