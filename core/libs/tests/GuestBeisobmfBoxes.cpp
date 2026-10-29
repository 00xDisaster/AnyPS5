#include "prx/libSceBeisobmf/include/Api.hpp"
#include "prx/libc/include/GuestArena.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

void Require(bool condition, const char *message) {
  if (!condition) {
    std::fprintf(stderr, "BEISOBMF BOXES: %s\n", message);
    std::abort();
  }
}

void Write(void *destination, const void *source, std::size_t bytes) {
  const GuestArena::HostWrite access(destination, bytes);
  Require(access.Open(), "guest fixture memory was not writable");
  std::memcpy(destination, source, bytes);
}

template <typename TValue>
void WriteField(void *object, std::size_t offset, TValue value) {
  Write(static_cast<std::byte *>(object) + offset, &value, sizeof(value));
}

void *Allocate(std::size_t bytes) {
  void *memory = GuestArena::GuestArenaAllocate_nid_postfix(bytes, 16);
  Require(memory != nullptr, "guest fixture allocation failed");
  const std::array<std::byte, 0x100> zeros{};
  auto *destination = static_cast<std::byte *>(memory);
  for (std::size_t offset = 0; offset < bytes; offset += zeros.size())
    Write(destination + offset, zeros.data(),
          (bytes - offset < zeros.size()) ? bytes - offset : zeros.size());
  return memory;
}

} // namespace

int main() {
  auto *box = static_cast<std::byte *>(Allocate(0xa0));
  auto *inlineMember = static_cast<std::byte *>(Allocate(0x70));
  auto *pointerMember = static_cast<std::byte *>(Allocate(0x70));
  auto *payload = static_cast<std::byte *>(Allocate(8));
  auto *child = Allocate(0x10);
  auto *next = Allocate(0x10);
  auto *decoder = static_cast<std::byte *>(Allocate(0xd0));

  WriteField(box, 0x10, child);
  WriteField(box, 0x20, next);
  WriteField(box, 0x40, inlineMember);
  WriteField(box, 0x80, std::uint64_t{0x1122334455667788});
  WriteField(box, 0x8c, std::uint32_t{7});
  WriteField(box, 0x90, std::uint32_t{0x1234});
  WriteField(inlineMember, 0x10, pointerMember);
  constexpr char flagsName[] = "flags";
  constexpr char inlineName[] = "inline";
  constexpr char pointerName[] = "external";
  Write(inlineMember + 0x18, flagsName, sizeof(flagsName));
  Write(inlineMember + 0x58, "\x34\x12\x01", 3);
  WriteField(inlineMember, 0x68, std::uint32_t{3});
  WriteField(inlineMember, 0x6c, std::uint32_t{0});
  Write(pointerMember + 0x18, pointerName, sizeof(pointerName));
  WriteField(pointerMember, 0x68, std::uint32_t{8});
  WriteField(pointerMember, 0x6c, std::uint32_t{11});
  WriteField(pointerMember, 0x60, payload);
  WriteField(decoder, 0xc0, std::uint64_t{0xfedcba9876543210});

  Require(isobmf_box_getchildbox_nid_postfix(box) == child,
          "child box offset is wrong");
  Require(isobmf_box_getnextbox_nid_postfix(box) == next,
          "next box offset is wrong");
  Require(isobmf_box_getdiskoffset_nid_postfix(box) == 0x1122334455667788,
          "box disk offset is wrong");
  Require(isobmf_box_getstate_nid_postfix(box) == 7,
          "box state offset is wrong");
  Require(isobmf_box_gettype_nid_postfix(box) == 0x1234,
          "box type offset is wrong");
  Require(isobmf_box_getflags32b_nid_postfix(box) == 0x011234,
          "flags value is wrong");
  Require(isobmf_box_getmember_length_nid_postfix(box, flagsName) == 3,
          "inline member length is wrong");
  Require(isobmf_box_getmember_type_nid_postfix(box, flagsName) == 0,
          "inline member type is wrong");
  Require(isobmf_box_getmember_ptr_nid_postfix(box, flagsName) ==
              inlineMember + 0x58,
          "inline member pointer is wrong");
  Require(isobmf_box_getmember_length_nid_postfix(box, pointerName) == 8,
          "external member length is wrong");
  Require(isobmf_box_getmember_type_nid_postfix(box, pointerName) == 11,
          "external member type is wrong");
  Require(isobmf_box_getmember_ptr_nid_postfix(box, pointerName) == payload,
          "external member pointer is wrong");
  Require(isobmf_box_getmember_ptr_nid_postfix(box, "missing") == nullptr,
          "missing member pointer is not null");
  Require(isobmf_box_getmember_length_nid_postfix(box, "missing") == 0,
          "missing member length is not zero");
  Require(isobmf_box_getmember_type_nid_postfix(box, "missing") == 0,
          "missing member type is not zero");
  Require(isobmf_decoder_next_disk_offset_nid_postfix(decoder) ==
              0xfedcba9876543210,
          "decoder disk offset is wrong");
  Require(isobmf_decoder_next_disk_offset_nid_postfix(nullptr) == 0xffffffff,
          "null decoder sentinel is wrong");

  std::puts("BEISOBMF BOXES tests passed");
  return 0;
}
