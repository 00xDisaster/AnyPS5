#pragma once

#include "SceTypes.hpp"

struct NpMatching2InitializeParameter {
    std::uint64_t poolSize;
    std::uint64_t cpuAffinityMask;
    std::int32_t threadPriority;
    std::uint8_t padding[4];
    std::uint64_t threadStackSize;
    std::uint64_t size;
    std::uint64_t sslPoolSize;
};

extern "C" {

int APS5_VABI sceNpMatching2ContextStart();
int APS5_VABI sceNpMatching2ContextStop();
int APS5_VABI sceNpMatching2CreateContext();
int APS5_VABI sceNpMatching2CreateJoinRoom();
int APS5_VABI sceNpMatching2DestroyContext();
int APS5_VABI sceNpMatching2GetRoomDataExternalList();
int APS5_VABI sceNpMatching2GetRoomDataInternal();
int APS5_VABI sceNpMatching2GetRoomMemberDataInternal();
int APS5_VABI sceNpMatching2GetServerId();
int APS5_VABI sceNpMatching2GetUserInfoList();
int APS5_VABI sceNpMatching2GetWorldInfoList();
int APS5_VABI sceNpMatching2Initialize(const NpMatching2InitializeParameter* param);
int APS5_VABI sceNpMatching2JoinRoom();
int APS5_VABI sceNpMatching2KickoutRoomMember();
int APS5_VABI sceNpMatching2LeaveRoom();
int APS5_VABI sceNpMatching2RegisterContextCallback();
int APS5_VABI sceNpMatching2RegisterRoomEventCallback();
int APS5_VABI sceNpMatching2RegisterRoomMessageCallback();
int APS5_VABI sceNpMatching2RegisterSignalingCallback();
int APS5_VABI sceNpMatching2SearchRoom();
int APS5_VABI sceNpMatching2SendRoomChatMessage();
int APS5_VABI sceNpMatching2SendRoomMessage();
int APS5_VABI sceNpMatching2SetDefaultRequestOptParam();
int APS5_VABI sceNpMatching2SetRoomDataExternal();
int APS5_VABI sceNpMatching2SetRoomDataInternal();
int APS5_VABI sceNpMatching2SetRoomMemberDataInternal();
int APS5_VABI sceNpMatching2SetUserInfo();
int APS5_VABI sceNpMatching2SignalingGetConnectionInfo();
int APS5_VABI sceNpMatching2Terminate(void);

}
