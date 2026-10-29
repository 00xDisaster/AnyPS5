#include "prx/libSceNpCppWebApi/include/Api.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <source_location>
#include <stdexcept>

namespace {

void Require(bool value, std::source_location at = std::source_location::current()) {
    if (!value) {
        std::fprintf(stderr, "requirement failed at %s:%u\n", at.file_name(), at.line());
        std::abort();
    }
}

constexpr int kNotSignedIn = static_cast<int>(0x80552907);
constexpr int kAlreadyInitialized = static_cast<int>(0x8055350f);
constexpr int kNotInitialized = static_cast<int>(0x80553510);
constexpr std::size_t kParameterSize = 0x10;
constexpr std::size_t kGuardSize = 0x10;
constexpr unsigned char kFill = 0xA5;

struct Storage {
    alignas(8) unsigned char bytes[kParameterSize + kGuardSize];
};

bool GuardIntact(const Storage& storage) {
    for (std::size_t index = kParameterSize; index < sizeof(storage.bytes); ++index) {
        if (storage.bytes[index] != kFill) return false;
    }
    return true;
}

using SimpleFn = int (APS5_VABI*)(void*);
using InitFn = int (APS5_VABI*)(void*, const std::uint64_t*);

const SimpleFn kSignedOut[] = {
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V129SetDataInfoRequestBodyFactory6createEPNS1_6Common10LibContextEPvmPNS5_12IntrusivePtrINS3_22SetDataInfoRequestBodyEEE,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi10uploadDataEiRKNS4_21ParameterToUploadDataERNS1_6Common19UpStreamTransactionINS8_12IntrusivePtrINS3_22UploadDataResponseBodyEEENSA_INS4_25UploadDataResponseHeadersEEEEE,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi10uploadDataEiRKNS4_42ParameterWithBinaryRequestBodyToUploadDataERNS1_6Common11TransactionINS8_12IntrusivePtrINS3_22UploadDataResponseBodyEEENSA_INS4_25UploadDataResponseHeadersEEEEE,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi11setDataInfoEiRKNS4_22ParameterToSetDataInfoERNS1_6Common11TransactionINS8_15DefaultResponseENS8_12IntrusivePtrINS4_26SetDataInfoResponseHeadersEEEEE,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi12downloadDataEiRKNS4_23ParameterToDownloadDataERNS1_6Common11TransactionINS8_12IntrusivePtrINS8_6BinaryEEENSA_INS4_27DownloadDataResponseHeadersEEEEE,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi21deleteMultiDataByUserEiRKNS4_32ParameterToDeleteMultiDataByUserERNS1_6Common11TransactionINS8_15DefaultResponseENS8_12IntrusivePtrINS8_18ResponseHeaderBaseEEEEE,
};
const SimpleFn kAccessors[] = {
    _ZNK3sce2Np9CppWebApi17TitleCloudStorage2V110DataStatus22getLastUpdatedDateTimeEv,
    _ZNK3sce2Np9CppWebApi17TitleCloudStorage2V110DataStatus7getInfoEv,
    _ZNK3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi25UploadDataResponseHeaders24getXPsnAtomicOperationIdEv,
    _ZNK3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi25UploadDataResponseHeaders24hasXPsnAtomicOperationIdEv,
};
const SimpleFn kSetters[] = {};
const SimpleFn kConstructors[] = {
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi21ParameterToUploadDataC1Ev,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi22ParameterToSetDataInfoC1Ev,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi32ParameterToDeleteMultiDataByUserC1Ev,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi42ParameterWithBinaryRequestBodyToUploadDataC1Ev,
};
const SimpleFn kDestructors[] = {
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi21ParameterToUploadDataD1Ev,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi22ParameterToSetDataInfoD1Ev,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi32ParameterToDeleteMultiDataByUserD1Ev,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi42ParameterWithBinaryRequestBodyToUploadDataD1Ev,
};
const SimpleFn kTerminators[] = {
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi21ParameterToUploadData9terminateEv,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi22ParameterToSetDataInfo9terminateEv,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi32ParameterToDeleteMultiDataByUser9terminateEv,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi42ParameterWithBinaryRequestBodyToUploadData9terminateEv,
};
const InitFn kInitializers[] = {
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi21ParameterToUploadData10initializeEPNS1_6Common10LibContextEPKci,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi22ParameterToSetDataInfo10initializeEPNS1_6Common10LibContextEPKciNS6_12IntrusivePtrINS3_22SetDataInfoRequestBodyEEE,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi32ParameterToDeleteMultiDataByUser10initializeEPNS1_6Common10LibContextEPKcSA_,
    _ZN3sce2Np9CppWebApi17TitleCloudStorage2V17DataApi42ParameterWithBinaryRequestBodyToUploadData10initializeEPNS1_6Common10LibContextEPKci,
};

template <typename TCall>
void RequireThrows(TCall call) {
    bool threw = false;
    try { call(); } catch (const std::runtime_error&) { threw = true; }
    Require(threw);
}

}

int main() {
    for (const auto fn : kSignedOut) Require(fn(nullptr) == kNotSignedIn);
    for (const auto fn : kAccessors) RequireThrows([&] { (void)fn(nullptr); });
    for (const auto fn : kSetters) Require(fn(nullptr) == 0);

    for (const auto fn : kConstructors) {
        Storage storage;
        std::memset(storage.bytes, kFill, sizeof(storage.bytes));
        Require(fn(storage.bytes) == 0);
        Require(GuardIntact(storage));
    }
    for (const auto fn : kDestructors) {
        Storage storage;
        std::memset(storage.bytes, kFill, sizeof(storage.bytes));
        Require(fn(storage.bytes) == 0);
        Require(GuardIntact(storage));
    }
    for (const auto fn : kTerminators) {
        Storage storage;
        std::memset(storage.bytes, 0, sizeof(storage.bytes));
        Require(fn(storage.bytes) == kNotInitialized);
    }
    for (const auto fn : kInitializers) {
        Storage storage;
        std::memset(storage.bytes, kFill, sizeof(storage.bytes));
        std::memset(storage.bytes, 0, kParameterSize);
        std::uint64_t context = 1;
        Require(fn(storage.bytes, &context) == 0);
        Require(fn(storage.bytes, &context) == kAlreadyInitialized);
        Require(kTerminators[0](storage.bytes) == 0);
        Require(kTerminators[0](storage.bytes) == kNotInitialized);
        Require(GuardIntact(storage));
    }
    return 0;
}
