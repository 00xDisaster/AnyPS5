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
    _ZN3sce2Np9CppWebApi11Matchmaking2V115LocationFactory6createEPNS1_6Common10LibContextEPNS5_12IntrusivePtrINS3_8LocationEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V116AttributeFactory6createEPNS1_6Common10LibContextEPKcNS3_13AttributeTypeES9_PNS5_12IntrusivePtrINS3_9AttributeEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V128PlayerForTicketCreateFactory6createEPNS1_6Common10LibContextERKmNS3_8PlatformEPNS5_12IntrusivePtrINS3_21PlayerForTicketCreateEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V130SubmitTicketRequestBodyFactory6createEPNS1_6Common10LibContextEPKcRKNS5_6VectorINS5_12IntrusivePtrINS3_21PlayerForTicketCreateEEEEEPNSB_INS3_23SubmitTicketRequestBodyEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api12cancelTicketEiRKNS4_23ParameterToCancelTicketERNS1_6Common11TransactionINS8_15DefaultResponseENS8_12IntrusivePtrINS8_18ResponseHeaderBaseEEEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api12submitTicketEiRKNS4_23ParameterToSubmitTicketERNS1_6Common11TransactionINS8_12IntrusivePtrINS3_24SubmitTicketResponseBodyEEENSA_INS8_18ResponseHeaderBaseEEEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api8getOfferEiRKNS4_19ParameterToGetOfferERNS1_6Common11TransactionINS8_12IntrusivePtrINS3_20GetOfferResponseBodyEEENSA_INS8_18ResponseHeaderBaseEEEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api9getTicketEiRKNS4_20ParameterToGetTicketERNS1_6Common11TransactionINS8_12IntrusivePtrINS3_21GetTicketResponseBodyEEENSA_INS8_18ResponseHeaderBaseEEEEE,
};
const SimpleFn kAccessors[] = {
    _ZN3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead19getPlayerAttributesEv,
    _ZN3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody10getPlayersEv,
    _ZN3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody10getPlayersEv,
    _ZN3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody19getTicketAttributesEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead11getOnlineIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead11getPlatformEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead11getTeamNameEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead12getAccountIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead13onlineIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead13platformIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead13teamNameIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead14accountIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V113PlayerForRead21playerAttributesIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead11getOnlineIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead11getPlatformEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead11getTeamNameEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead11getTicketIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead12getAccountIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead13onlineIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead13platformIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead13teamNameIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead13ticketIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V118PlayerForOfferRead14accountIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody10getOfferIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody11getLocationEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody11statusIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody12offerIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody12playersIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody13locationIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody14getRulesetNameEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody16rulesetNameIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody18getCreatedDateTimeEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody18getUpdatedDateTimeEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody20createdDateTimeIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody20updatedDateTimeIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V120GetOfferResponseBody9getStatusEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody11getTicketIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody11statusIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody12getSubmitterEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody12playersIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody13ticketIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody14getRulesetNameEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody14submitterIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody16rulesetNameIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody18getCreatedDateTimeEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody18getUpdatedDateTimeEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody20createdDateTimeIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody20updatedDateTimeIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody21ticketAttributesIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V121GetTicketResponseBody9getStatusEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V124SubmitTicketResponseBody11getTicketIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V124SubmitTicketResponseBody13ticketIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V18Location16getGameSessionIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V18Location18gameSessionIdIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V19Attribute7getNameEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V19Attribute7getTypeEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V19Attribute8getValueEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V19Submitter11getPlatformEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V19Submitter12getAccountIdEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V19Submitter13platformIsSetEv,
    _ZNK3sce2Np9CppWebApi11Matchmaking2V19Submitter14accountIdIsSetEv,
};
const SimpleFn kSetters[] = {
    _ZN3sce2Np9CppWebApi11Matchmaking2V121PlayerForTicketCreate10setNatTypeERKi,
    _ZN3sce2Np9CppWebApi11Matchmaking2V121PlayerForTicketCreate11setTeamNameEPKc,
    _ZN3sce2Np9CppWebApi11Matchmaking2V121PlayerForTicketCreate19setPlayerAttributesERKNS1_6Common6VectorINS5_12IntrusivePtrINS3_9AttributeEEEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V123SubmitTicketRequestBody11setLocationERKNS1_6Common12IntrusivePtrINS3_8LocationEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V123SubmitTicketRequestBody19setTicketAttributesERKNS1_6Common6VectorINS5_12IntrusivePtrINS3_9AttributeEEEEE,
    _ZN3sce2Np9CppWebApi11Matchmaking2V18Location16setGameSessionIdEPKc,
};
const SimpleFn kConstructors[] = {
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api19ParameterToGetOfferC1Ev,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api20ParameterToGetTicketC1Ev,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api23ParameterToCancelTicketC1Ev,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api23ParameterToSubmitTicketC1Ev,
};
const SimpleFn kDestructors[] = {
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api19ParameterToGetOfferD1Ev,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api20ParameterToGetTicketD1Ev,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api23ParameterToCancelTicketD1Ev,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api23ParameterToSubmitTicketD1Ev,
};
const SimpleFn kTerminators[] = {
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api19ParameterToGetOffer9terminateEv,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api20ParameterToGetTicket9terminateEv,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api23ParameterToCancelTicket9terminateEv,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api23ParameterToSubmitTicket9terminateEv,
};
const InitFn kInitializers[] = {
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api19ParameterToGetOffer10initializeEPNS1_6Common10LibContextEPKc,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api20ParameterToGetTicket10initializeEPNS1_6Common10LibContextEPKc,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api23ParameterToCancelTicket10initializeEPNS1_6Common10LibContextEPKc,
    _ZN3sce2Np9CppWebApi11Matchmaking2V13Api23ParameterToSubmitTicket10initializeEPNS1_6Common10LibContextENS6_12IntrusivePtrINS3_23SubmitTicketRequestBodyEEE,
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
