# libSceNpPartner001 technical debt

### Unknown function info

- [sceNpEAAccessInitialize, sceNpEAAccessTerminate](../../../core/libs/prx/libSceNpPartner001/Export.cpp) (libSceNpPartner001) - names and the no-argument signatures follow [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/main/src/core/libraries/np/np_partner.cpp); imported by PPSA23566, not confirmed on a PS5 title

### Functional

- [sceNpEAAccessInitialize, sceNpEAAccessTerminate](../../../core/libs/prx/libSceNpPartner001/Export.cpp) (libSceNpPartner001) - initialization marks the library initialized without contacting the server and termination clears it, as in [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/main/src/core/libraries/np/np_partner.cpp); whether the console fails initialization while signed out is unknown. Termination without initialization throws where shadPS4 returns the PS4 error `0x819D0001`, whose PS5 value is unverified, and a second initialization throws where shadPS4 returns 0 without checking. PPSA23566 imports both, not confirmed on its start-up path
