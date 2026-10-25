#ifdef __APPLE__


#include "prx/libc/include/general/AsmMacros.hpp"
#include <CoreFoundation/CoreFoundation.h>
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <dispatch/dispatch.h>
#include <pthread.h>

extern "C" [[noreturn]] void Aps5EnterGuest_nid_no_patch(void* entry, void* block);

asm(".text\n"
    ".p2align 4\n" APS5_ASM_FUNCTION("Aps5EnterGuest_nid_no_patch") "    movq %rdi, %rax\n"
                                                                    "    movq %rsi, %rdi\n"
                                                                    "    andq $-16, %rsp\n"
                                                                    "    pushq $0\n"
                                                                    "    pushq $0\n"
                                                                    "    movq %rsp, %rbp\n"
                                                                    "    xorl %esi, %esi\n"
                                                                    "    call *%rax\n"
                                                                    "    movl %eax, %edi\n"
                                                                    "    call " APS5_ASM_SYMBOL("exit") "\n"
                                                                                                   "    ud2\n"
    APS5_ASM_FUNCTION_END("Aps5EnterGuest_nid_no_patch"));

namespace {

constexpr std::size_t GuestMainStackSize = std::size_t{64} << 20;

std::atomic<bool> mainQueueServed{false};

struct GuestStart {
    void* Entry;
    void* Block;
};

void* RunGuestMain(void* argument) {
    const auto start = *static_cast<GuestStart*>(argument);
    pthread_setname_np("main");
    Aps5EnterGuest_nid_no_patch(start.Entry, start.Block);
}

[[noreturn]] void Fatal(const char* message, int error) {
    std::fprintf(stderr, "guest start: %s (%d)\n", message, error);
    std::abort();
}

}

extern "C" bool LibcMainQueueServed_nid_postfix() {
    return mainQueueServed.load(std::memory_order_acquire);
}

extern "C" [[noreturn]] void Aps5StartGuest_nid_no_patch(void* entry, void* block) {
    static GuestStart start;
    start = {entry, block};
    pthread_attr_t attributes;
    int result = pthread_attr_init(&attributes);
    if (result != 0) Fatal("pthread_attr_init failed", result);
    result = pthread_attr_setstacksize(&attributes, GuestMainStackSize);
    if (result != 0) Fatal("pthread_attr_setstacksize failed", result);
    result = pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    if (result != 0) Fatal("pthread_attr_setdetachstate failed", result);
    mainQueueServed.store(true, std::memory_order_release);
    pthread_t thread;
    result = pthread_create(&thread, &attributes, RunGuestMain, &start);
    if (result != 0) Fatal("pthread_create failed", result);
    pthread_attr_destroy(&attributes);
    for (;;) CFRunLoopRun();
}

#endif
