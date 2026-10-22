# libSceFreeTypeOptOl technical debt

### Unknown function info

- [FT_Get_PFR_Advance, FT_Get_PFR_Kerning, FT_Get_PFR_Metrics](../../../core/libs/prx/libSceFreeTypeOptOl/Export.cpp) - names and NIDs match the [PS5 3.20 inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceFreeTypeOptOl.c); signatures and behavior are unverified. No-parameter declarations resolve imports and throw through `NotImplemented_nid_no_patch` when called.
