#include "prx/libSceMsgDialog/include/Api.hpp"
#include "prx/libSceMsgDialog/include/MsgDialogParam.hpp"
#include "prx/libSceIme/include/ModalDialog.hpp"
#include "SceTypes.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

constexpr int StatusRunning = 2;
constexpr int StatusFinished = 3;
constexpr int ErrBusy = static_cast<int>(0x80B80007u);
constexpr int ErrNotRunning = static_cast<int>(0x80B8000Bu);

void Require(bool value, const char* what) {
    if (value) return;
    std::fprintf(stderr, "msg dialog: %s\n", what);
    std::abort();
}

struct UserMessage {
    std::int32_t buttonType;
    std::int32_t pad;
    const char* message;
};

struct Param {
    std::uint8_t bytes[MsgDialog::ParamBytes] = {};

    Param(std::int32_t mode, const UserMessage* user) {
        std::memcpy(bytes + 0x38, &mode, 4);
        std::memcpy(bytes + 0x40, &user, 8);
    }
};

ModalDialogState Session(std::size_t* chosen = nullptr) {
    std::size_t value = 99;
    const auto state = ModalDialogPoll_nid_postfix(value);
    if (chosen != nullptr) *chosen = value;
    return state;
}

void Headless() {
    UserMessage user{MsgDialog::ButtonTypeYesNo, 0, "Continue?"};
    Param param(MsgDialog::ModeUserMsg, &user);
    Require(!ModalDialogHasPresenter_nid_postfix(), "a presenter is registered");
    Require(sceMsgDialogInitialize() == 0, "initialize");
    Require(sceMsgDialogOpen(&param) == 0, "open without a presenter");
    Require(Session() == ModalDialogState::Idle, "a dialog was shown without a presenter");
    Require(sceMsgDialogGetStatus() == StatusRunning, "first poll without a presenter");
    Require(sceMsgDialogGetStatus() == StatusFinished, "second poll without a presenter");
    Require(sceMsgDialogTerminate() == 0, "terminate");
}

void Shown() {
    ModalDialogSetPresenter_nid_postfix(true);
    UserMessage user{MsgDialog::ButtonTypeYesNo, 0, "Continue?"};
    Param param(MsgDialog::ModeUserMsg, &user);
    Require(sceMsgDialogOpen(&param) == 0, "open with a presenter");
    Require(Session() == ModalDialogState::Showing, "the dialog was not shown");
    ModalDialogView view;
    Require(ModalDialogReadView_nid_postfix(view) && view.message == u"Continue?" && view.buttons.size() == 2, "the shown dialog is not the message");
    for (int poll = 0; poll < 5; ++poll) Require(sceMsgDialogGetStatus() == StatusRunning && sceMsgDialogUpdateStatus() == StatusRunning, "a shown dialog finished before it was answered");
    Require(sceMsgDialogOpen(&param) == ErrBusy, "a second open was accepted while the dialog ran");
    ModalDialogPublishInput_nid_postfix(ModalDialogInputKind::Next);
    ModalDialogPublishInput_nid_postfix(ModalDialogInputKind::Confirm);
    Require(sceMsgDialogGetStatus() == StatusFinished, "an answered dialog did not finish");
    Require(sceMsgDialogClose() == ErrNotRunning, "a finished dialog could be closed");
    Require(sceMsgDialogOpen(&param) == 0 && Session() == ModalDialogState::Showing, "a new dialog was not shown after the answer");
    Require(sceMsgDialogClose() == 0, "close");
    Require(Session() == ModalDialogState::Closed, "close did not close the session");
    Require(sceMsgDialogGetStatus() == StatusFinished, "a closed dialog is still running");
    Require(sceMsgDialogOpen(&param) == 0 && Session() == ModalDialogState::Showing, "a new dialog was not shown after a close");
    Require(sceMsgDialogTerminate() == 0, "terminate");
    Require(Session() == ModalDialogState::Idle, "terminate left the session");
    Require(sceMsgDialogGetStatus() == 0, "status after terminate");
}

void NotPresented() {
    UserMessage none{MsgDialog::ButtonTypeNone, 0, "Wait"};
    Param withoutButtons(MsgDialog::ModeUserMsg, &none);
    Require(sceMsgDialogOpen(&withoutButtons) == 0 && Session() == ModalDialogState::Idle, "a dialog without buttons was shown");
    Require(sceMsgDialogGetStatus() == StatusRunning && sceMsgDialogGetStatus() == StatusFinished, "a dialog without buttons did not finish on the second poll");
    UserMessage user{MsgDialog::ButtonTypeOk, 0, "Progress"};
    Param progress(MsgDialog::ModeProgressBar, &user);
    Require(sceMsgDialogOpen(&progress) == 0 && Session() == ModalDialogState::Idle, "a progress bar was shown");
    Require(sceMsgDialogClose() == 0, "close of the progress bar");
    Param system(MsgDialog::ModeSystemMsg, &user);
    Require(sceMsgDialogOpen(&system) == 0 && Session() == ModalDialogState::Idle, "a system message was shown");
    Require(sceMsgDialogGetStatus() == StatusRunning && sceMsgDialogGetStatus() == StatusFinished, "a system message did not finish on the second poll");
    UserMessage busy{MsgDialog::ButtonTypeOk, 0, "Other"};
    ModalDialogRequest other;
    other.message = u"Other";
    other.buttons = {u"OK"};
    Require(ModalDialogOpen_nid_postfix(other), "the session did not open");
    Param param(MsgDialog::ModeUserMsg, &busy);
    Require(sceMsgDialogOpen(&param) == 0, "open while another dialog is showing");
    Require(sceMsgDialogGetStatus() == StatusRunning && sceMsgDialogGetStatus() == StatusFinished, "a dialog refused by the session did not finish on the second poll");
    ModalDialogReset_nid_postfix();
}

}

int main() {
    Headless();
    Shown();
    NotPresented();
    ModalDialogSetPresenter_nid_postfix(false);
}
