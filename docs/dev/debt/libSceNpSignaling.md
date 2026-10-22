# libSceNpSignaling technical debt

### Unknown function info

- [sceNpSignalingDeactivateConnection](../../../core/libs/prx/libSceNpSignaling/Export.cpp) (libSceNpSignaling) - imported by PS4 titles, which do not load without it, and added without an implementation: declared without parameters, every call throws
