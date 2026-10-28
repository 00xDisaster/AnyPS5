#include "prx/libSceKeyboard/include/Api.hpp"
#include "prx/libSceKeyboard/include/keyboard_structs.h"
#include "prx/libSceKeyboard/include/KeyboardState.hpp"
#include "prx/libSceIme/include/ModalDialog.hpp"
#include "prx/libSceVideoOut/include/KeyboardInput.hpp"
#include "SDL_events.h"
#include "SDL_keyboard.h"

#include <cstdio>
#include <cstdlib>

namespace {

constexpr unsigned Window = 7;

void Require(bool value, const char* what) {
    if (value) return;
    std::fprintf(stderr, "keyboard dialog: %s\n", what);
    std::abort();
}

SDL_Event Key(Uint32 type, SDL_Scancode scancode) {
    SDL_Event event{};
    event.type = type;
    event.key.windowID = Window;
    event.key.keysym.scancode = scancode;
    return event;
}

int Read(KeyboardData* data) {
    return sceKeyboardRead(KEYBOARD_HANDLE, data, 16);
}

void OpenDialog() {
    ModalDialogRequest request;
    request.message = u"Choose";
    request.buttons = {u"Yes", u"No"};
    Require(ModalDialogOpen_nid_postfix(request), "the dialog did not open");
}

}

int main() {
    KeyboardData data[16]{};
    Require(sceKeyboardInit() == KEYBOARD_OK, "init failed");
    Require(sceKeyboardOpen(1, 0, 0, nullptr) == KEYBOARD_HANDLE, "open failed");
    Require(Read(data) == 1, "the open state was not queued");
    KeyboardInput input;
    ModalDialogSetPresenter_nid_postfix(true);

    OpenDialog();
    input.HandleEvent(Key(SDL_KEYDOWN, SDL_SCANCODE_A), Window);
    Require(Read(data) == 0, "a key press reached the title while a dialog was shown");
    ModalDialogClose_nid_postfix();
    input.HandleEvent(Key(SDL_KEYDOWN, SDL_SCANCODE_A), Window);
    Require(Read(data) == 1 && data[0].key_code[0] == 0x04, "a key press did not reach the title after the dialog closed");
    ModalDialogReset_nid_postfix();

    OpenDialog();
    input.HandleEvent(Key(SDL_KEYUP, SDL_SCANCODE_A), Window);
    Require(Read(data) == 1 && data[0].length == 1 && data[0].key_code[0] == 0, "a key release did not reach the title while a dialog was shown");
    ModalDialogClose_nid_postfix();
    ModalDialogReset_nid_postfix();

    ModalDialogSetPresenter_nid_postfix(false);
    Require(sceKeyboardClose(KEYBOARD_HANDLE) == KEYBOARD_OK, "close failed");
}
