# libSceJsc technical debt

### Unknown function info

- [JSString ABI and ownership](../../../core/libs/prx/libSceJsc/Export.cpp): the console JavaScriptCore version, allocation policy, opaque object layout and thread initialization remain unverified. These host objects cannot be passed to a title-supplied native JavaScriptCore module. The ten names occur in the [PS5 3.20 inventory](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceJsc.c); ownership and signatures follow pinned WebKit [JSStringRef.cpp](https://github.com/WebKit/WebKit/blob/f274ef4c03ebace644c46f8ec55746b391acdcc8/Source/JavaScriptCore/API/JSStringRef.cpp). A null handle given to retain, release or the maximum-size query, and a UTF-16 length above 2147483635, throw where WebKit faults. Other JavaScriptCore APIs remain unavailable.

### Functional

- [JSString conversion](../../../core/libs/prx/libSceJsc/Export.cpp): console behavior is unverified. UTF-16 storage, null/empty distinction and partial UTF-8 output follow pinned WebKit [OpaqueJSString.cpp](https://github.com/WebKit/WebKit/blob/f274ef4c03ebace644c46f8ec55746b391acdcc8/Source/JavaScriptCore/API/OpaqueJSString.cpp) and [UTF8Conversion.cpp](https://github.com/WebKit/WebKit/blob/f274ef4c03ebace644c46f8ec55746b391acdcc8/Source/WTF/wtf/unicode/UTF8Conversion.cpp). Malformed UTF-8 creates a null string, distinct from an empty string; output stops before an unpaired UTF-16 surrogate. The PS5 11.x [OpaqueJSString.h](https://github.com/neonmodder123/PS5-WebKit-11.x/blob/80f77e94c63ee7202eec3d4b5b1a021dd89011ba/Source/JavaScriptCore/API/OpaqueJSString.h#L49-L52) keeps the low 32 bits of a UTF-16 length of 2^32 or more, which throws here as in pinned WebKit.

### Known export names; ABI prototypes unverified

- The 150 `sceJscUnknown_*_nid_no_patch` exports in [Export.cpp](../../../core/libs/prx/libSceJsc/Export.cpp) are present in the PS5 3.20 `libSceJsc` export inventory from [PS5-3.20_Libs](https://github.com/DNNDHH/PS5-3.20_Libs). Their NIDs are derived from exported mangled symbols. Prototypes and behavior are unverified; each uses the no-parameter import-resolution convention and throws through `NotImplemented_nid_no_patch`. These declarations make no ABI or behavior claim.
