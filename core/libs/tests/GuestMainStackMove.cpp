#include "prx/libc/include/GuestArena.hpp"
#include <crt_externs.h>
#include <pthread.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

extern "C" [[noreturn]] void Aps5StartGuest_nid_no_patch(void (*entry)(void*, void*), void* block);
extern "C" void Aps5RunOnMainThread_nid_no_patch(void (*function)(void*), void* context);

namespace {

constexpr std::uintptr_t MappingAlignment = 0x10000;
constexpr std::size_t MappingBytes = 0x10000;

std::uintptr_t originalStack = 0;
std::string originalArgument;
bool originalInArena = false;

void Require(bool value, const char* what) {
    if (value) return;
    std::fprintf(stderr, "check failed: %s\n", what);
    std::exit(1);
}

bool InArena(std::uintptr_t address) {
    std::uintptr_t base = 0;
    std::size_t bytes = 0;
    GuestArena::GuestArenaRange_nid_postfix(&base, &bytes);
    return address >= base && address < base + bytes;
}

void CheckMainThread(void*) {
    int local = 0;
    Require(pthread_main_np() != 0, "the check runs on the main thread");
    const auto address = reinterpret_cast<std::uintptr_t>(&local);
    if (originalInArena) {
        Require(!InArena(address), "the main thread stack is outside the guest arena");
        Require(address != originalStack, "the main thread stack moved");
        const auto old = reinterpret_cast<const void*>(originalStack & ~(MappingAlignment - 1));
        try {
            GuestArena::GuestArenaMarkUsed_nid_postfix(old, MappingBytes);
        } catch (const std::runtime_error&) {
            Require(false, "a fixed guest mapping fits where the main thread stack was");
        }
        GuestArena::GuestArenaRelease_nid_postfix(old, MappingBytes);
    }
    Require(originalArgument == (*_NSGetArgv())[0], "argv survived the move");
    Require(std::getenv("PATH") != nullptr, "the environment survived the move");
}

void Guest(void*, void*) {
    Aps5RunOnMainThread_nid_no_patch(CheckMainThread, nullptr);
    std::exit(0);
}

}

int main(int, char** argv) {
    int local = 0;
    originalStack = reinterpret_cast<std::uintptr_t>(&local);
    originalInArena = InArena(originalStack);
    originalArgument = argv[0];
    Aps5StartGuest_nid_no_patch(Guest, nullptr);
}
