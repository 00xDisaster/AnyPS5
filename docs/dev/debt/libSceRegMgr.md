# libSceRegMgr technical debt

### Unknown function info

- [sceRegMgrNonSysGetInt](../../../core/libs/prx/libSceRegMgr/Export.cpp) - signature (`int64_t key, int32_t unk, int32_t* data`) and NID come from a community dump of PS5 3.20 firmware, cross-checked against the open-source ChonkyStation4 PS4 emulator, not from Sony documentation; behaviour is unverified on hardware and the export always throws
