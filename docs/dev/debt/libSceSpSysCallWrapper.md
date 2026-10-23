# libSceSpSysCallWrapper technical debt

### Unknown function info

- [sceSpGetPid, sceSpSelect and sceSpPthreadMutexattrSetprioceiling](../../../core/libs/prx/libSceSpSysCallWrapper/Export.cpp) (libSceSpSysCallWrapper) - still throw. `sceSpGetPid` and `sceSpSelect` are in the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceSpSysCallWrapper.c) but not exported by firmware 13.60's `libSceSpSysCallWrapper.sprx`, so no prototype could be read. There, `sceSpPthreadMutexattrSetprioceiling` only jumps to libkernel's `scePthreadMutexattrSetprioceiling`, which AnyPS5 does not implement yet (read from disassembly, not measured on a console)
