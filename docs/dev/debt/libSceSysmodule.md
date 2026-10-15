# libSceSysmodule technical debt

### Silent stubs

- [sceSysmoduleIsLoaded, sceSysmoduleLoadModule, sceSysmoduleUnloadModule](../../../core/libs/prx/libSceSysmodule/Export.cpp) (libSceSysmodule) accept a module id missing from the module table: they log it and return 0 without loading anything, so `sceSysmoduleIsLoaded` reports an unknown module as loaded and unloading it succeeds, because titles that load or unload such an id aborted otherwise. The internal variants (`sceSysmoduleLoadModuleInternal`, `sceSysmoduleUnloadModuleInternal`, `sceSysmoduleLoadModuleInternalWithArg`, `sceSysmoduleGetModuleHandleInternal`) throw for an unknown id. What the console returns for an id it does not know is not verified.
