# libSceAcm technical debt

### Silent stubs

- [libSceAcm](../../../core/libs/prx/libSceAcm/Export.cpp) batches execute as silent no-ops, and do not execute encoded panner, FFT and reverb commands.

### Unknown function info

- [libSceAcm](../../../core/libs/prx/libSceAcm/Export.cpp) context creation, batch submission and waiting use host identifiers instead of the native device and ioctl requests. Batch identifiers, timeout handling, DSP scheduling and service-side errors remain unimplemented. Panner source parameter and matrix payloads are forwarded by the native library; their DSP interpretation remains unverified.
- [sceAcm_ConvReverb_SharedIr, sceAcm_IFFT](../../../core/libs/prx/libSceAcm/Export.cpp) (libSceAcm) - their names and NIDs are in the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceAcm.c), but no credible prototypes were found. They are declared without parameters solely to resolve known imports; this is not an ABI claim

- [`sceAcmBatchJobNotification`](../../../core/libs/prx/libSceAcm/Export.cpp) is a parameterless pending declaration for import resolution. Its signature and behavior are unverified; it throws through `NotImplemented_nid_no_patch`.

- [`sceAcm_ConvReverb_SharedInput`](../../../core/libs/prx/libSceAcm/Export.cpp) encodes the PS5 13.40 reverb jobs, except with 128-sample blocks, where it throws: its input stage needs a 256-point `sceAcm_FFT` job, which `sceAcm_FFT` does not encode. The meaning of the reverb descriptor's block metadata for the DSP is unverified.
