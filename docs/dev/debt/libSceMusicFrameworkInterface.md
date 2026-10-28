# libSceMusicFrameworkInterface technical debt

### Unknown function info

- [sceMusicFwGetCurrentTime, sceMusicFwGetLoop, sceMusicFwGetMuted, sceMusicFwGetNetworkState, sceMusicFwGetVolume, sceMusicFwIsEnded, sceMusicFwIsPlaying, sceMusicFwIsStopped, sceMusicFwRemoveSrc, sceMusicFwSendMessageAsynchronously, sceMusicFwSendMessageSynchronously, sceMusicFwSetCurrentTime, sceMusicFwSetLoop, sceMusicFwSetShuffle, sceMusicFwSetVolume, sceMusicFwSkipNext and sceMusicFwSkipPrevious](../../../core/libs/prx/libSceMusicFrameworkInterface/Export.cpp) (libSceMusicFrameworkInterface) - names and NIDs are present in the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceMusicFrameworkInterface.c), but no credible prototypes were found. The declarations have no parameters solely to resolve known imports; this is not an ABI claim and callers must not call them until signatures are verified

### Additional callable export inventory

- [Additional callable exports](../../../core/libs/prx/libSceMusicFrameworkInterface/Export.cpp) - 20 entries have import-resolution declarations only. Parameter lists, return types and behavior are unverified; parameterless `int` declarations are placeholders, not verified ABI contracts. Every entry throws through `NotImplemented_nid_no_patch`. Unidentified names use literal NID aliases. No runtime support, implemented-function increase or title unblock is claimed.

| Name | NID |
| --- | --- |
| `sceMusicFwAddSrc` | `fr04nQ3Dkpc` |
| `sceMusicFwFinalize` | `3Y8a-RRYbhk` |
| `sceMusicFwGetCapability` | `scDaf7LO-PE` |
| `sceMusicFwGetDuration` | `ccOo+E83pGY` |
| `sceMusicFwGetShuffle` | `FiQU5-kdM94` |
| `sceMusicFwGetVolume2` | `sj8BkAXqNb0` |
| `sceMusicFwInitialize` | `11wRJfoIJxE` |
| `sceMusicFwInitialize2` | `Yn7uLcMKP1g` |
| `sceMusicFwIsPaused` | `t5LNf+DD9OY` |
| `sceMusicFwIsSeeking` | `Sim10+qqYUw` |
| `sceMusicFwLoad` | `cNLzn-IWmrw` |
| `sceMusicFwPause` | `HknA0+ZloHk` |
| `sceMusicFwPlay` | `4J-YAdBNHCo` |
| `sceMusicFwSetMuted` | `-lMRBsJ20fM` |
| `sceMusicFwSetVolume2` | `8ixuO68kLfM` |
| `sceMusicFwStop` | `5YQ-72Mdsuo` |
| `Unknown` | `X9GN6gWabWM` |
| `Unknown` | `tTQ+AFZiGS0` |
| `Unknown` | `uAMoi7RinCQ` |
| `Unknown` | `vmCGqN-OFaY` |
