#include "prx/libSceNpCppWebApi/include/Api.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

using SelfCall = int (APS5_VABI*)(void*);
using Size = std::size_t (APS5_VABI*)(const void*);
using Element = void* (APS5_VABI*)(void*, std::uint64_t);

static void Require(bool value, const char* what) {
    if (value) return;
    std::fprintf(stderr, "%s\n", what);
    std::abort();
}

static void TestIteratorDestructors() {
    unsigned char iterator[0x20];
    std::memset(iterator, 0x5a, sizeof(iterator));
    const SelfCall destructors[] = {
        _ZN3sce2Np9CppWebApi6Common13ConstIteratorINS2_12IntrusivePtrINS1_13InGameCatalog2V57ProductEEEED2Ev,
        _ZN3sce2Np9CppWebApi6Common13ConstIteratorINS2_12IntrusivePtrINS1_17TitleCloudStorage2V110DataStatusEEEED2Ev,
    };
    for (const auto call : destructors) Require(call(iterator) == 0, "an iterator destructor failed");
    for (const auto byte : iterator) Require(byte == 0x5a, "an iterator destructor wrote to the iterator");
}

static const Size sizes[] = {
    _ZNK3sce2Np9CppWebApi6Common6VectorINS2_12IntrusivePtrINS1_13InGameCatalog2V53SkuEEEE4sizeEv,
    _ZNK3sce2Np9CppWebApi6Common6VectorINS2_12IntrusivePtrINS1_13InGameCatalog2V59ContainerEEEE4sizeEv,
};

static const Element elements[] = {
    _ZN3sce2Np9CppWebApi6Common6VectorINS2_12IntrusivePtrINS1_13InGameCatalog2V59ContainerEEEEixEm,
    _ZN3sce2Np9CppWebApi6Common6VectorINS2_12IntrusivePtrINS1_14SessionManager2V113PlayerSessionEEEEixEm,
};

template <typename TCall>
static bool Throws(TCall call) {
    try {
        call();
    } catch (const std::runtime_error&) {
        return true;
    }
    return false;
}

static void TestVectorSizeAndElements() {
    std::uint64_t context = 1;
    IntrusivePtr vector{nullptr, nullptr, &context};
    for (const auto size : sizes) Require(size(&vector) == 0, "a vector without a body has elements");
    for (const auto element : elements) Require(Throws([&] { element(&vector, 0); }), "indexing a vector without a body did not throw");

    alignas(8) std::int32_t first[4] = {1};
    alignas(8) std::int32_t second[4] = {1};
    const IntrusivePtr pushed[] = {{first, nullptr, &context}, {second, nullptr, &context}};
    _ZN3sce2Np9CppWebApi6Common6VectorINS2_12IntrusivePtrINS1_14SessionManager2V112JoinableUserEEEEC1EPNS2_10LibContextE(&vector, &context);
    for (const auto& element : pushed)
        Require(_ZN3sce2Np9CppWebApi6Common6VectorINS2_12IntrusivePtrINS1_14SessionManager2V112JoinableUserEEEE8pushBackERKS8_(&vector, &element) == 0, "pushBack failed");
    for (const auto size : sizes) Require(size(&vector) == 2, "a vector reported the wrong size");
    for (const auto element : elements) {
        Require(static_cast<IntrusivePtr*>(element(&vector, 0))->object == first, "operator[] returned the wrong first element");
        Require(static_cast<IntrusivePtr*>(element(&vector, 1))->object == second, "operator[] returned the wrong second element");
        Require(static_cast<IntrusivePtr*>(element(&vector, 1)) == static_cast<IntrusivePtr*>(element(&vector, 0)) + 1, "operator[] elements are not contiguous");
    }
    _ZN3sce2Np9CppWebApi6Common6VectorINS2_12IntrusivePtrINS1_14SessionManager2V112JoinableUserEEEED1Ev(&vector);
    Require(first[0] == 1 && second[0] == 1, "destroying the vector did not release its references");

    alignas(8) unsigned char foreignBody[0x50] = {};
    IntrusivePtr foreign{foreignBody, nullptr, &context};
    for (const auto size : sizes) Require(Throws([&] { size(&foreign); }), "sizing a foreign body did not throw");
    for (const auto element : elements) Require(Throws([&] { element(&foreign, 0); }), "indexing a foreign body did not throw");
}

int main() {
    TestIteratorDestructors();
    TestVectorSizeAndElements();
    std::puts("NpCppWebApi offline tests passed");
    return 0;
}
