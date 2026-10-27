# libSceIcu technical debt

### Functional

- [Invariant character and version helpers](../../../core/libs/prx/libSceIcu/Version.cpp) are unverified on a console. `u_charsToUChars`, `u_UCharsToChars`, `u_versionFromString`, `u_versionFromUString` and `u_versionToString` follow ICU 78.1 [uinvchar.cpp](https://github.com/unicode-org/icu/blob/release-78.1/icu4c/source/common/uinvchar.cpp) and [putil.cpp](https://github.com/unicode-org/icu/blob/release-78.1/icu4c/source/common/putil.cpp), unchanged since 65.1. Firmware 13.42 reports ICU 78.1 and exports none of them ([#11013](https://github.com/boykopovar/AnyPS5/pull/11013#issuecomment-6400470698)); the [3.20 table](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceIcu.c) has all five. Version parts use the guest `strtoul`, truncated to eight bits; `u_versionToString` keeps ICU's dropped zero tens digit (105 becomes `15`). A null buffer with a positive conversion length throws; ICU dereferences it. Closes when: compared with the console.

### Known export names; ABI prototypes unverified

- The 148 `sceIcuUnknown_*_nid_no_patch` exports in [Export.cpp](../../../core/libs/prx/libSceIcu/Export.cpp) are present in the PS5 3.20 export inventory from [PS5-3.20_Libs](https://github.com/DNNDHH/PS5-3.20_Libs). Their prototypes and behavior are unverified. Each uses the no-parameter import-resolution convention and throws through `NotImplemented_nid_no_patch`; these declarations make no ABI or behavior claim.
