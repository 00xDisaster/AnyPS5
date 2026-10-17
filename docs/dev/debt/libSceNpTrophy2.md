# libSceNpTrophy2 technical debt

### Silent stubs

- [libSceNpTrophy2](../../../core/libs/prx/libSceNpTrophy2/src/GameInfo.cpp): `sceNpTrophy2RegisterUnlockCallback` accepts a callback that is never called, as no trophy is ever unlocked, `sceNpTrophy2UnregisterUnlockCallback` only returns success, and `sceNpTrophy2ShowTrophyList` returns success without showing a list (there is no system trophy UI; seen on PPSA15552).

### Unknown function info

- [Query structures](../../../core/libs/prx/libSceNpTrophy2/include/NpTrophy2Types.hpp): a progress value is a 16-byte `{int32 type, 4 reserved bytes, uint64 value}` and the game, group and trophy structures are 152, 24, 152, 32, 1312 and 32 bytes, as [KytyPS5](https://github.com/KytyPS5/KytyPS5/blob/9e41d8320a2774b303fbad344c2347e8d0292e83/src/libs/libTrophy.cpp) asserts; no other public source confirms the layout. Progress type 1 marks a progressive trophy's target and progress, as in KytyPS5.

- [sceNpTrophy2GetRewardIcon](../../../core/libs/prx/libSceNpTrophy2/src/Context.cpp) (libSceNpTrophy2) takes `(int context, int handle, int reward_id, void* buffer, size_t* size)`, from the call sites in the bundled `Media/Plugins/PSNCore.prx` of PPSA06888, the same shape as `sceNpTrophy2GetTrophyIcon`. The title calls it twice: first with a null buffer to read the size, then with a buffer of that size allocated with `malloc`. The meaning of the third argument is unverified, and the reward icon data is unknown, so every call throws
