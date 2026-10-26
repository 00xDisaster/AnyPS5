#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <thread>
#include <vector>

struct GuestTlsDescriptor {
    std::uint64_t Active;
    std::uint64_t InitImage;
    std::uint64_t FileSize;
    std::uint64_t BlockSize;
    std::uint64_t Alignment;
    std::uint64_t Key;
};
static_assert(sizeof(GuestTlsDescriptor) == 0x30);

struct GuestTlsIndex {
    std::uint64_t Descriptor;
    std::uint64_t Offset;
};

extern "C" void* Aps5GuestTlsGetAddr_nid_no_patch(const GuestTlsIndex* index);
extern "C" void* Aps5GuestTlsAllocateBlock_nid_no_patch(GuestTlsDescriptor* descriptor);

namespace {

void Require(bool condition, const char* message, int round) {
    if (condition) return;
    std::fprintf(stderr, "round %d: %s\n", round, message);
    std::exit(1);
}

void RaceFirstAccess(int round, bool throughIndex) {
    constexpr int Threads = 16;
    alignas(64) static GuestTlsDescriptor descriptor{};
    descriptor = GuestTlsDescriptor{1, 0, 0, 64, 16, 0};
    std::atomic<int> ready{0};
    std::atomic<int> recorded{0};
    std::atomic<bool> start{false};
    std::vector<void*> blocks(Threads, nullptr);
    std::vector<void*> again(Threads, nullptr);
    std::vector<std::uint64_t> keys(Threads, 0);
    std::vector<std::thread> pool;
    for (int thread = 0; thread < Threads; ++thread)
        pool.emplace_back([&, thread] {
            ready.fetch_add(1);
            while (!start.load(std::memory_order_acquire)) {}
            GuestTlsIndex index{reinterpret_cast<std::uint64_t>(&descriptor), 8};
            blocks[thread] = throughIndex ? Aps5GuestTlsGetAddr_nid_no_patch(&index)
                                          : Aps5GuestTlsAllocateBlock_nid_no_patch(&descriptor);
            keys[thread] = std::atomic_ref<std::uint64_t>(descriptor.Key).load(std::memory_order_acquire);
            again[thread] = throughIndex ? Aps5GuestTlsGetAddr_nid_no_patch(&index)
                                         : Aps5GuestTlsAllocateBlock_nid_no_patch(&descriptor);
            recorded.fetch_add(1);
            while (recorded.load() != Threads) {}
        });
    while (ready.load() != Threads) {}
    start.store(true, std::memory_order_release);
    for (auto& thread : pool) thread.join();
    const std::set<std::uint64_t> distinctKeys(keys.begin(), keys.end());
    const std::set<void*> distinctBlocks(blocks.begin(), blocks.end());
    Require(distinctKeys.size() == 1 && *distinctKeys.begin() != 0, "racing first accesses registered more than one key", round);
    Require(distinctBlocks.size() == Threads && distinctBlocks.count(nullptr) == 0, "threads did not get distinct blocks", round);
    for (int thread = 0; thread < Threads; ++thread)
        Require(again[thread] == blocks[thread], "a thread's second access returned a different block", round);
}

}

int main() {
    for (int round = 0; round < 100; ++round) RaceFirstAccess(round, true);
    for (int round = 0; round < 100; ++round) RaceFirstAccess(round, false);
    return 0;
}
