# libSceLibreSsl technical debt

### Functional

- [SSL_select_next_proto](../../../core/libs/prx/libSceLibreSsl/src/ProtocolSelection.cpp) has no console ABI or behaviour verification. Its signature, peer preference, borrowed result pointer and first-supported-protocol fallback follow LibreSSL 3.4.1's [ssl.h](https://github.com/libressl/openbsd/blob/078b3d1df8ca2011c1605cd40418c58fa0f5b143/src/lib/libssl/ssl.h#L619-L626) and [ssl_lib.c](https://github.com/libressl/openbsd/blob/078b3d1df8ca2011c1605cd40418c58fa0f5b143/src/lib/libssl/ssl_lib.c#L1501-L1537). Null output pointers, null or empty lists, zero-length names and truncated encodings throw without writing outputs; the console's handling of these inputs is unknown.

### Unknown function info

- [libSceLibreSsl pending exports](../../../core/libs/prx/libSceLibreSsl/Export.cpp) - 399 named callable exports are listed in the [PS5 3.20 export table](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceLibreSsl.c) and were cross-checked against the same module in PS5 firmware 13.60. Their signatures and behaviour are unverified; the declarations use the no-parameter unknown-signature convention and throw through `NotImplemented_nid_no_patch`.
