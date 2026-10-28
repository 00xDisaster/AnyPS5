#include "prx/libSceMsgDialog.native/libSceMsgDialog.native.hpp"
#include "prx/libSceMsgDialog/include/MsgDialogParam.hpp"
#include "prx/libSceIme/include/ModalDialog.hpp"
#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" {
extern int SceMsgDialogNativeModuleLoaded_nid_no_patch;

}

namespace {

void Require(bool value) { if (!value) std::abort(); }

constexpr int kStatusInitialized = 1;
constexpr int kStatusFinished = 3;
constexpr int kErrAlreadyInitialized = static_cast<int>(0x80B80004u);
constexpr int kErrNotInitialized = static_cast<int>(0x80B80003u);
constexpr int kErrNotFinished = static_cast<int>(0x80B80005u);
constexpr int kErrNotRunning = static_cast<int>(0x80B8000Bu);
constexpr int kErrArgNull = static_cast<int>(0x80B8000Du);
constexpr int kButtonIdOk = 1;
constexpr int kButtonIdNo = 2;
constexpr int kStatusRunning = 2;

struct UserMessage {
    std::int32_t buttonType;
    std::int32_t pad;
    const char* message;
};

struct UserParam {
    std::uint8_t bytes[MsgDialog::ParamBytes] = {};

    UserParam(std::int32_t mode, const UserMessage* user) {
        std::memcpy(bytes + 0x38, &mode, 4);
        std::memcpy(bytes + 0x40, &user, 8);
    }
};

ModalDialogState Session() {
    std::size_t chosen = 0;
    return ModalDialogPoll_nid_postfix(chosen);
}

void ShownDialog() {
    UserMessage yesNo{MsgDialog::ButtonTypeYesNo, 0, "Continue?"};
    UserParam param(MsgDialog::ModeUserMsg, &yesNo);
    MsgDialogResult result{};
    ModalDialogSetPresenter_nid_postfix(true);
    Require(sceMsgDialogInitialize() == 0);
    Require(sceMsgDialogOpen(&param) == 0);
    Require(Session() == ModalDialogState::Showing);
    for (int poll = 0; poll < 3; ++poll) Require(sceMsgDialogUpdateStatus() == kStatusRunning && sceMsgDialogGetStatus() == kStatusRunning);
    Require(sceMsgDialogGetResult(&result) == kErrNotFinished);
    ModalDialogPublishInput_nid_postfix(ModalDialogInputKind::Next);
    ModalDialogPublishInput_nid_postfix(ModalDialogInputKind::Confirm);
    Require(sceMsgDialogGetStatus() == kStatusFinished);
    result = MsgDialogResult{};
    Require(sceMsgDialogGetResult(&result) == 0 && result.result == 0 && result.button_id == kButtonIdNo);

    Require(sceMsgDialogOpen(&param) == 0 && sceMsgDialogGetStatus() == kStatusRunning);
    ModalDialogPublishInput_nid_postfix(ModalDialogInputKind::Confirm);
    Require(sceMsgDialogUpdateStatus() == kStatusFinished);
    Require(sceMsgDialogGetResult(&result) == 0 && result.button_id == kButtonIdOk);

    UserMessage focusNo{MsgDialog::ButtonTypeYesNoFocusNo, 0, "Delete?"};
    UserParam focusParam(MsgDialog::ModeUserMsg, &focusNo);
    Require(sceMsgDialogOpen(&focusParam) == 0);
    ModalDialogPublishInput_nid_postfix(ModalDialogInputKind::Confirm);
    Require(sceMsgDialogGetStatus() == kStatusFinished);
    Require(sceMsgDialogGetResult(&result) == 0 && result.button_id == kButtonIdNo);
    Require(sceMsgDialogTerminate() == 0);
    ModalDialogSetPresenter_nid_postfix(false);
}

}

int main() {
    Require(SceMsgDialogNativeModuleLoaded_nid_no_patch == 1);
    std::uint8_t param[0x60]{};
    MsgDialogResult result{};

    Require(sceMsgDialogTerminate() == kErrNotInitialized);
    Require(sceMsgDialogClose() == kErrNotRunning);
    Require(sceMsgDialogOpen(nullptr) == kErrNotInitialized);
    Require(sceMsgDialogOpen(param) == kErrNotInitialized);
    Require(sceMsgDialogGetResult(nullptr) == kErrNotInitialized);
    Require(sceMsgDialogGetResult(&result) == kErrNotInitialized);
    Require(sceMsgDialogUpdateStatus() == 0);
    Require(sceMsgDialogGetStatus() == 0);

    Require(sceMsgDialogInitialize() == 0);
    Require(sceMsgDialogInitialize() == kErrAlreadyInitialized);
    Require(sceMsgDialogClose() == kErrNotRunning);
    Require(sceMsgDialogGetResult(&result) == kErrNotFinished);
    Require(sceMsgDialogGetResult(nullptr) == kErrArgNull);
    Require(sceMsgDialogOpen(nullptr) == kErrArgNull);
    Require(sceMsgDialogUpdateStatus() == kStatusInitialized);
    Require(sceMsgDialogGetStatus() == kStatusInitialized);

    Require(sceMsgDialogProgressBarSetValue(0, 50) == kErrNotRunning);
    Require(sceMsgDialogOpen(param) == 0);
    Require(sceMsgDialogUpdateStatus() == kStatusFinished);
    Require(sceMsgDialogGetStatus() == kStatusFinished);
    Require(sceMsgDialogOpen(nullptr) == kErrArgNull);
    Require(sceMsgDialogOpen(param) == 0);
    Require(sceMsgDialogProgressBarInc(0, 10) == kErrNotRunning);
    Require(sceMsgDialogProgressBarSetMsg(0, "progress") == kErrNotRunning);
    Require(sceMsgDialogProgressBarSetValue(0, 100) == kErrNotRunning);
    Require(sceMsgDialogClose() == kErrNotRunning);
    Require(sceMsgDialogUpdateStatus() == kStatusFinished);
    Require(sceMsgDialogGetStatus() == kStatusFinished);

    result = MsgDialogResult{};
    result.button_id = -1;
    Require(sceMsgDialogGetResult(&result) == 0);
    Require(result.result == 0);
    Require(result.button_id == kButtonIdOk);

    Require(sceMsgDialogTerminate() == 0);
    Require(sceMsgDialogTerminate() == kErrNotInitialized);
    Require(sceMsgDialogOpen(nullptr) == kErrNotInitialized);
    Require(sceMsgDialogOpen(param) == kErrNotInitialized);
    Require(sceMsgDialogGetResult(nullptr) == kErrNotInitialized);
    Require(sceMsgDialogGetResult(&result) == kErrNotInitialized);
    Require(sceMsgDialogClose() == kErrNotRunning);
    Require(sceMsgDialogGetStatus() == 0);

    ShownDialog();
}
