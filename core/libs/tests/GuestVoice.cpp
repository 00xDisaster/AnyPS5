#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <stdexcept>

extern "C" {
int APS5_VABI sceVoiceInit(VoiceInitParam*, std::int32_t);
int APS5_VABI sceVoiceStart(const VoiceStartParam*);
int APS5_VABI sceVoiceEnd_nid_postfix();
int APS5_VABI sceVoiceCreatePort(std::uint32_t*, const VoicePortParam*);
int APS5_VABI sceVoiceDeletePort(std::uint32_t);
int APS5_VABI sceVoiceConnectIPortToOPort(std::uint32_t, std::uint32_t);
int APS5_VABI sceVoiceDisconnectIPortFromOPort(std::uint32_t, std::uint32_t);
int APS5_VABI sceVoiceGetVolume(std::uint32_t, float*);
int APS5_VABI sceVoiceSetMuteFlag(std::uint32_t, bool);
int APS5_VABI sceVoiceGetMuteFlag(std::uint32_t, bool*);
int APS5_VABI sceVoiceGetPortInfo(std::uint32_t, VoicePortInfo*);
int APS5_VABI sceVoiceReadFromOPort(std::uint32_t, void*, std::uint32_t*);
int APS5_VABI sceVoiceResetPort(std::uint32_t);
int APS5_VABI sceVoicePausePort(std::uint32_t);
int APS5_VABI sceVoiceResumePort(std::uint32_t);
int APS5_VABI sceVoicePausePortAll(void);
int APS5_VABI sceVoiceResumePortAll(void);
int APS5_VABI sceVoiceGetResourceInfo(VoiceResourceInfo*);
int APS5_VABI sceVoiceEnableChat(void);
int APS5_VABI sceVoiceDisableChat(void);
}

namespace {

constexpr int NotInitialized = static_cast<int>(0x804E0801u);
constexpr int AlreadyInitialized = static_cast<int>(0x804E0802u);

void Require(bool value) { if (!value) std::abort(); }

template <typename TFunction>
void RequireThrows(TFunction function) {
    try { function(); } catch (const std::logic_error&) { return; }
    std::abort();
}

void TestNotInitialized() {
    Require(sceVoiceStart(nullptr) == NotInitialized);
    VoiceStartParam start{};
    Require(sceVoiceStart(&start) == NotInitialized);
    Require(sceVoiceCreatePort(nullptr, nullptr) == NotInitialized);
    VoicePortParam badType{};
    badType.port_type = 99;
    std::uint32_t id = 0xA5A5A5A5u;
    Require(sceVoiceCreatePort(&id, &badType) == NotInitialized);
    VoicePortParam param{};
    param.port_type = 3;
    Require(sceVoiceCreatePort(&id, &param) == NotInitialized);
    Require(id == 0xA5A5A5A5u);
    Require(sceVoiceGetPortInfo(0, nullptr) == NotInitialized);
    VoicePortInfo info{};
    std::memset(&info, 0xA5, sizeof(info));
    VoicePortInfo sentinel = info;
    Require(sceVoiceGetPortInfo(0, &info) == NotInitialized);
    Require(std::memcmp(&info, &sentinel, sizeof(info)) == 0);
    Require(sceVoicePausePortAll() == NotInitialized);
    Require(sceVoiceResumePortAll() == NotInitialized);
}

std::uint32_t CreatePort(std::int32_t type, float volume) {
    VoicePortParam param{};
    param.port_type = type;
    param.volume = volume;
    if (type == 2 || type == 4) param.voice.bitrate = 16000;
    std::uint32_t id = 0;
    Require(sceVoiceCreatePort(&id, &param) == 0);
    return id;
}

} // namespace

int main() {
    TestNotInitialized();
    constexpr auto notInitialized = static_cast<std::int32_t>(0x804e0801u);
    constexpr auto invalidPort = static_cast<std::int32_t>(0x804e0804u);
    constexpr auto invalidArgument = static_cast<std::int32_t>(0x804e0805u);
    constexpr auto portNotFound = static_cast<std::int32_t>(0x804e0807u);
    Require(sceVoiceResetPort(0) == notInitialized);
    Require(sceVoicePausePort(0) == notInitialized);
    Require(sceVoiceResumePort(0) == notInitialized);
    Require(sceVoiceGetResourceInfo(nullptr) == notInitialized);
    Require(sceVoiceEnableChat() == notInitialized);
    Require(sceVoiceDisableChat() == notInitialized);
    VoiceInitParam init{};
    init.app_type = 0x10000000;
    VoiceStartParam start{};
    Require(sceVoiceInit(&init, 100) == 0);
    Require(sceVoiceInit(&init, 100) == AlreadyInitialized);
    Require(sceVoiceStart(&start) == 0);
    Require(sceVoiceGetResourceInfo(nullptr) == invalidArgument);
    struct ResourceOutput {
        VoiceResourceInfo info;
        std::uint32_t guard;
    } resourceOutput{{}, 0xa5a5a5a5u};
    Require(sceVoiceGetResourceInfo(&resourceOutput.info) == 0);
    Require(resourceOutput.info.maxVoiceInputPorts == 15);
    Require(resourceOutput.info.maxVoiceOutputPorts == 4);
    Require(resourceOutput.info.maxDeviceInputPorts == 4);
    Require(resourceOutput.info.maxDeviceOutputPorts == 4);
    Require(resourceOutput.guard == 0xa5a5a5a5u);
    RequireThrows([] { sceVoiceEnableChat(); });
    RequireThrows([] { sceVoiceDisableChat(); });
    const auto input = CreatePort(2, 0.5f);
    const auto output = CreatePort(3, 0.25f);
    Require(sceVoiceConnectIPortToOPort(input, output) == 0);
    Require(sceVoiceSetMuteFlag(output, true) == 0);
    VoicePortInfo before{};
    Require(sceVoiceGetPortInfo(input, &before) == 0);

    Require(sceVoiceResetPort(input) == 0);
    Require(sceVoiceResetPort(output) == 0);
    Require(sceVoiceResetPort(output + 1) == portNotFound);
    Require(sceVoiceResetPort(128) == invalidPort);

    Require(sceVoicePausePort(input) == 0);
    Require(sceVoicePausePort(input) == 0);
    Require(sceVoicePausePort(output) == 0);
    VoicePortInfo paused{};
    Require(sceVoiceGetPortInfo(input, &paused) == 0);
    Require(paused.port_type == before.port_type && paused.state == before.state && paused.frame_size == before.frame_size);
    Require(sceVoiceResumePort(input) == 0);
    Require(sceVoiceResumePort(input) == 0);
    Require(sceVoiceResumePort(output) == 0);
    Require(sceVoicePausePortAll() == 0);
    Require(sceVoicePausePortAll() == 0);
    Require(sceVoiceGetPortInfo(input, &paused) == 0);
    Require(paused.port_type == before.port_type && paused.state == before.state && paused.frame_size == before.frame_size);
    Require(sceVoiceResumePortAll() == 0);
    Require(sceVoiceResumePortAll() == 0);
    Require(sceVoicePausePort(output + 1) == portNotFound);
    Require(sceVoicePausePort(128) == invalidPort);
    Require(sceVoiceResumePort(output + 1) == portNotFound);
    Require(sceVoiceResumePort(128) == invalidPort);

    VoicePortInfo after{};
    Require(sceVoiceGetPortInfo(input, &after) == 0);
    Require(after.port_type == before.port_type && after.state == before.state && after.frame_size == before.frame_size);
    float volume = 0;
    bool muted = false;
    Require(sceVoiceGetVolume(output, &volume) == 0 && volume == 0.25f);
    Require(sceVoiceGetMuteFlag(output, &muted) == 0 && muted);
    std::uint8_t data[64]{};
    std::uint32_t size = sizeof(data);
    Require(sceVoiceReadFromOPort(output, data, &size) == 0 && size == 0);
    Require(sceVoiceDisconnectIPortFromOPort(input, output) == 0);

    Require(sceVoiceDeletePort(input) == 0);
    Require(sceVoiceResetPort(input) == portNotFound);
    Require(sceVoicePausePort(input) == portNotFound);
    Require(sceVoiceResetPort(output) == 0);
    Require(sceVoiceEnd_nid_postfix() == 0);
    TestNotInitialized();
    Require(sceVoiceResetPort(output) == notInitialized);
    Require(sceVoiceResumePort(output) == notInitialized);
    Require(sceVoiceGetResourceInfo(nullptr) == notInitialized);
    Require(sceVoiceEnableChat() == notInitialized);
    Require(sceVoiceDisableChat() == notInitialized);
    for (const auto mode : {4, 0x08000000, 0x10000000, 0x20000000}) {
        init.app_type = mode;
        Require(sceVoiceInit(&init, 100) == 0);
        Require(sceVoiceGetResourceInfo(&resourceOutput.info) == 0);
        Require(resourceOutput.info.maxVoiceInputPorts == (mode == 4 ? 0 : mode == 0x20000000 ? 7 : 15));
        Require(resourceOutput.info.maxVoiceOutputPorts == (mode == 4 ? 0 : 4));
        Require(resourceOutput.info.maxDeviceInputPorts == 4);
        Require(resourceOutput.info.maxDeviceOutputPorts == (mode == 4 ? 5 : 4));
        Require(resourceOutput.guard == 0xa5a5a5a5u);
        Require(sceVoiceEnd_nid_postfix() == 0);
    }

}
