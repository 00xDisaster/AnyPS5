# libSceNpCommerce technical debt

### Silent stubs

- [libSceNpCommerce](../../../core/libs/prx/libSceNpCommerce/Export.cpp) - the PS Store icon show/hide calls and `sceNpCommerceSetPsStoreIconLayout` do nothing

### Unknown function info

- [sceNpCommerceDialogInitializeInternal](../../../core/libs/prx/libSceNpCommerce/Export.cpp) (libSceNpCommerce) - its name and NID are in the [PS5 3.20 export inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceNpCommerce.c); the no-parameter prototype follows [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/f68923fe807f657348e6719ce658bcbd0a2b1c72/src/core/libraries/np/np_commerce/np_commerce.cpp#L61) and is not measured on a console. It is declared without parameters solely to resolve known imports; this is not an ABI claim, and every call throws.
- [sceNpCommerceDialogOpen2](../../../core/libs/prx/libSceNpCommerce/Export.cpp): the single pointer argument and 136-byte layout are recovered, including the product pointer array, count and result user-data value. User checks, mode validation, process-version-dependent restrictions and result-buffer semantics remain unimplemented; the host dialog still finishes immediately as cancelled. Mode-specific meanings of the option and 64-bit argument remain unresolved.

- [sceNpCommerceDialogOpen](../../../core/libs/prx/libSceNpCommerce/Export.cpp): Product copying and service submission remain part of the cancelled UI stub.
