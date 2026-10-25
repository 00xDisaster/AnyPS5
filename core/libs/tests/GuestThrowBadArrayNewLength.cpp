#include "prx/libc/include/Api.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>

struct TypeRecord { const void* const* vtable; const char* name; const TypeRecord* base; };
struct Vtable { std::ptrdiff_t offset; const TypeRecord* type; const void* destroy; const void* deleteObject; const char* (APS5_VABI *what)(const LibcException::ExceptionObject*); };

extern "C" {
extern const TypeRecord _ZTISt9bad_alloc_nid_postfix;
extern const TypeRecord _ZTISt20bad_array_new_length_nid_postfix;
extern const Vtable _ZTVSt20bad_array_new_length_nid_postfix;
}

static void Require(bool value) { if (!value) std::abort(); }

int main() {
    const auto& type = _ZTISt20bad_array_new_length_nid_postfix;
    Require(std::strcmp(type.name, "St20bad_array_new_length") == 0);
    Require(type.base == &_ZTISt9bad_alloc_nid_postfix);
    const auto& table = _ZTVSt20bad_array_new_length_nid_postfix;
    Require(table.offset == 0 && table.type == &type && table.what != nullptr);

    bool thrown = false;
    try {
        _ZSt6_ThrowRKSt20bad_array_new_length_nid_postfix(nullptr);
    } catch (...) {
        thrown = true;
    }
    Require(thrown);
    return 0;
}
