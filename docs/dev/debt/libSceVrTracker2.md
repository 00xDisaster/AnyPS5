# libSceVrTracker2 technical debt

### Unknown function info

- [sceVrTracker2GetControllerImage, sceVrTracker2IrGetDebugRawImage](../../../core/libs/prx/libSceVrTracker2/Export.cpp) (libSceVrTracker2) - names and NIDs are present in the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceVrTracker2.c), but no credible prototypes were found. They are declared without parameters solely to resolve known imports; this is not an ABI claim.
