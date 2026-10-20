#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
int APS5_VABI sceNpSessionSignalingInitialize(void* param);
int APS5_VABI sceNpSessionSignalingCreateContext2(const void* param, std::uint32_t* contextId);
std::int32_t APS5_VABI sceNpSessionSignalingGetLocalNetInfo(std::int32_t contextId, void* info);
int APS5_VABI sceNpSessionSignalingRequestPrepare(std::uint32_t contextId, std::uint32_t* requestId);
std::int32_t APS5_VABI sceNpSessionSignalingGetConnectionStatus(std::int32_t contextId, std::int32_t connectionId, std::int32_t* status, void* peerAddress, std::uint16_t* peerPort);
std::int32_t APS5_VABI sceNpSessionSignalingGetGroupInfo(std::int32_t contextId, std::int32_t groupId, void* info);
std::int32_t APS5_VABI sceNpSessionSignalingGetGroupFromSessionId(std::int32_t contextId, const void* sessionId, std::int32_t* groupId);
std::int32_t APS5_VABI sceNpSessionSignalingGetConnectionFromNetAddress(std::int32_t contextId, std::uint32_t address, std::uint16_t port, std::int32_t* connectionId);
std::int32_t APS5_VABI sceNpSessionSignalingGetConnectionFromNetAddress2(std::int32_t contextId, std::int32_t groupId, std::uint32_t address, std::uint16_t port, std::int32_t* connectionId);
std::int32_t APS5_VABI sceNpSessionSignalingGetConnectionFromPeerAddress(std::int32_t contextId, const void* peerAddress, std::int32_t* connectionId);
std::int32_t APS5_VABI sceNpSessionSignalingGetConnectionFromPeerAddress2(std::int32_t contextId, std::int32_t groupId, const void* peerAddress, std::int32_t* connectionId);
int APS5_VABI sceNpSessionSignalingTerminate(void);
}

namespace {

constexpr int InvalidArgument = static_cast<int>(0x80553303u);
constexpr int Unavailable = static_cast<int>(0x80552D06u);
constexpr int GroupNotFound = static_cast<int>(0x8055330Au);
constexpr int ConnectionNotFound = static_cast<int>(0x8055330Cu);

void Require(bool condition, const char* message) {
    if (!condition) {
        std::fprintf(stderr, "NpSessionSignaling: %s\n", message);
        std::abort();
    }
}

}

int main() {
    unsigned char initializeParam[32] = {};
    Require(sceNpSessionSignalingInitialize(initializeParam) == 0, "initialize failed");
    unsigned char contextParam[64] = {};
    std::uint32_t contextId = 0;
    Require(sceNpSessionSignalingCreateContext2(contextParam, &contextId) == 0, "context creation failed");

    std::uint32_t requestId = 0xa5a5a5a5u;
    Require(sceNpSessionSignalingRequestPrepare(contextId, nullptr) == InvalidArgument, "null request id");
    Require(sceNpSessionSignalingRequestPrepare(contextId, &requestId) == Unavailable, "prepare without network");
    Require(requestId == 0xa5a5a5a5u, "failed prepare wrote a request id");

    const auto netContextId = static_cast<std::int32_t>(contextId);
    unsigned char info[16];
    std::memset(info, 0xa5, sizeof(info));
    unsigned char untouched[sizeof(info)];
    std::memcpy(untouched, info, sizeof(info));
    Require(sceNpSessionSignalingGetLocalNetInfo(netContextId, nullptr) == InvalidArgument, "null info");
    Require(sceNpSessionSignalingGetLocalNetInfo(netContextId, info) == Unavailable, "local net info without network");
    Require(std::memcmp(info, untouched, sizeof(info)) == 0, "failed query modified the info");

    std::int32_t status = 0x5a5a5a5a;
    std::uint32_t peerAddress = 0xa5a5a5a5u;
    std::uint16_t peerPort = 0xa5a5;
    Require(sceNpSessionSignalingGetConnectionStatus(netContextId, 1, nullptr, &peerAddress, &peerPort) == InvalidArgument, "null connection status");
    Require(sceNpSessionSignalingGetConnectionStatus(netContextId, 1, &status, &peerAddress, &peerPort) == ConnectionNotFound, "connection without network");
    Require(sceNpSessionSignalingGetConnectionStatus(netContextId, 1, &status, nullptr, nullptr) == ConnectionNotFound, "connection without peer outputs");
    Require(status == 0x5a5a5a5a && peerAddress == 0xa5a5a5a5u && peerPort == 0xa5a5, "failed status query wrote an output");

    Require(sceNpSessionSignalingGetGroupInfo(netContextId, 1, nullptr) == InvalidArgument, "null group info");
    Require(sceNpSessionSignalingGetGroupInfo(netContextId, 1, info) == GroupNotFound, "group info without a session");
    Require(std::memcmp(info, untouched, sizeof(info)) == 0, "failed group query modified the info");

    const unsigned char sessionId[48] = {};
    const unsigned char peer[40] = {};
    std::int32_t found = 0x5a5a5a5a;
    Require(sceNpSessionSignalingGetGroupFromSessionId(netContextId, nullptr, &found) == InvalidArgument, "null session id");
    Require(sceNpSessionSignalingGetGroupFromSessionId(netContextId, sessionId, nullptr) == InvalidArgument, "null group id");
    Require(sceNpSessionSignalingGetGroupFromSessionId(netContextId, sessionId, &found) == GroupNotFound, "group from session id without a session");
    Require(sceNpSessionSignalingGetConnectionFromNetAddress(netContextId, 0x0100007fu, 3658, nullptr) == InvalidArgument, "null connection id from net address");
    Require(sceNpSessionSignalingGetConnectionFromNetAddress(netContextId, 0x0100007fu, 3658, &found) == ConnectionNotFound, "connection from net address without a session");
    Require(sceNpSessionSignalingGetConnectionFromNetAddress2(netContextId, 1, 0x0100007fu, 3658, nullptr) == InvalidArgument, "null connection id from group net address");
    Require(sceNpSessionSignalingGetConnectionFromNetAddress2(netContextId, 1, 0x0100007fu, 3658, &found) == GroupNotFound, "connection from group net address without a session");
    Require(sceNpSessionSignalingGetConnectionFromPeerAddress(netContextId, nullptr, &found) == InvalidArgument, "null peer address");
    Require(sceNpSessionSignalingGetConnectionFromPeerAddress(netContextId, peer, nullptr) == InvalidArgument, "null connection id from peer address");
    Require(sceNpSessionSignalingGetConnectionFromPeerAddress(netContextId, peer, &found) == ConnectionNotFound, "connection from peer address without a session");
    Require(sceNpSessionSignalingGetConnectionFromPeerAddress2(netContextId, 1, nullptr, &found) == InvalidArgument, "null group peer address");
    Require(sceNpSessionSignalingGetConnectionFromPeerAddress2(netContextId, 1, peer, nullptr) == InvalidArgument, "null connection id from group peer address");
    Require(sceNpSessionSignalingGetConnectionFromPeerAddress2(netContextId, 1, peer, &found) == GroupNotFound, "connection from group peer address without a session");
    Require(found == 0x5a5a5a5a, "a failed lookup wrote an id");

    Require(sceNpSessionSignalingTerminate() == 0, "terminate failed");
}
