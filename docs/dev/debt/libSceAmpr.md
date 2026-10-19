# libSceAmpr technical debt

### Unknown function info

- [sceAmprCommandBufferConstructor](../../../core/libs/prx/libSceAmpr/Export.cpp) (libSceAmpr) - the result for a null command buffer is unknown for `sceAmprCommandBufferConstructor`, `sceAmprAprCommandBufferConstructor`, `sceAmprCommandBufferReset` and `sceAmprCommandBufferSetBuffer`; they return `SCE_KERNEL_ERROR_EINVAL` like the other command buffer exports, while KytyPS5 returns OK for the constructors and EPERM for Reset
