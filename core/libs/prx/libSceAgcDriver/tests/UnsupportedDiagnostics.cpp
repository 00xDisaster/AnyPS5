#include "prx/libSceAgcDriver/include/AgcDriver.hpp"
#include "SceTypes.hpp"

#include <stdexcept>
#include <string>

int main() {
    if (sceAgcDriverGetCoredumpAddress() != nullptr) return 1;
    if (sceAgcDriverSetTargetRingForDiag() != static_cast<int>(0x8A6D0001)) return 2;
    try {
        sceAgcDriverSetFlip();
        return 3;
    } catch (const std::runtime_error& error) {
        if (std::string(error.what()) != "sceAgcDriverSetFlip not implemented") return 4;
    }
    return 0;
}
