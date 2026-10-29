#include "SceTypes.hpp"
#include "prx/libSceBeisobmf/include/Api.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libc/include/GuestArena.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace {

template <typename TValue>
TValue ReadField(const void *object, std::size_t offset) {
  if (!object)
    throw std::invalid_argument("isobmf box pointer is null");
  TValue value{};
  std::memcpy(&value, static_cast<const std::byte *>(object) + offset,
              sizeof(value));
  return value;
}

std::uintptr_t ReadPointer(const void *object, std::size_t offset) {
  return ReadField<std::uintptr_t>(object, offset);
}

void WritePointer(void *object, std::size_t offset, std::uintptr_t value) {
  std::memcpy(static_cast<std::byte *>(object) + offset, &value, sizeof(value));
}

std::uintptr_t FindMember(void *box, const char *name) {
  if (!box || !name)
    throw std::invalid_argument("isobmf member lookup requires a box and name");
  auto *cursor = static_cast<std::byte *>(box) + 0x50;
  const GuestArena::HostWrite write(cursor, sizeof(std::uintptr_t));
  if (!write.Open())
    throw std::runtime_error(
        "isobmf member cursor is not writable guest memory");
  WritePointer(box, 0x50, ReadPointer(box, 0x40));
  for (auto member = ReadPointer(box, 0x50); member != 0;
       member = ReadPointer(box, 0x50)) {
    const auto *memberName = reinterpret_cast<const char *>(member + 0x18);
    if (std::strncmp(memberName, name, 0x40) == 0)
      return member;
    WritePointer(box, 0x50,
                 ReadPointer(reinterpret_cast<const void *>(member), 0x10));
  }
  return 0;
}

void *MemberPointer(std::uintptr_t member) {
  const auto type =
      ReadField<std::uint32_t>(reinterpret_cast<const void *>(member), 0x6c);
  if (type < 20 && ((0x97800u >> (type & 0x1f)) & 1u) != 0)
    return reinterpret_cast<void *>(
        ReadPointer(reinterpret_cast<const void *>(member), 0x60));
  return reinterpret_cast<void *>(member + 0x58);
}

} // namespace

extern "C" {

void *APS5_VABI isobmf_box_getchildbox_nid_postfix(void *box) {
  return reinterpret_cast<void *>(ReadPointer(box, 0x10));
}

std::uint64_t APS5_VABI isobmf_box_getdiskoffset_nid_postfix(void *box) {
  return ReadField<std::uint64_t>(box, 0x80);
}

std::uint32_t APS5_VABI isobmf_box_getflags32b_nid_postfix(void *box) {
  if (!box)
    return 0;
  const auto member = FindMember(box, "flags");
  if (!member)
    return 0;
  const auto *value = static_cast<const std::uint8_t *>(MemberPointer(member));
  return static_cast<std::uint32_t>(value[0]) |
         (static_cast<std::uint32_t>(value[1]) << 8) |
         (static_cast<std::uint32_t>(value[2]) << 16);
}

std::uint32_t APS5_VABI
isobmf_box_getmember_length_nid_postfix(void *box, const char *name) {
  const auto member = FindMember(box, name);
  return member ? ReadField<std::uint32_t>(
                      reinterpret_cast<const void *>(member), 0x68)
                : 0;
}

void *APS5_VABI isobmf_box_getmember_ptr_nid_postfix(void *box,
                                                     const char *name) {
  const auto member = FindMember(box, name);
  return member ? MemberPointer(member) : nullptr;
}

std::uint32_t APS5_VABI
isobmf_box_getmember_type_nid_postfix(void *box, const char *name) {
  const auto member = FindMember(box, name);
  return member ? ReadField<std::uint32_t>(
                      reinterpret_cast<const void *>(member), 0x6c)
                : 0;
}

void *APS5_VABI isobmf_box_getnextbox_nid_postfix(void *box) {
  return reinterpret_cast<void *>(ReadPointer(box, 0x20));
}

std::uint32_t APS5_VABI isobmf_box_getstate_nid_postfix(void *box) {
  return ReadField<std::uint32_t>(box, 0x8c);
}

std::uint32_t APS5_VABI isobmf_box_gettype_nid_postfix(void *box) {
  return ReadField<std::uint32_t>(box, 0x90);
}

int APS5_VABI isobmf_decoder_clear_nid_postfix(void) {
  NotImplemented_nid_no_patch(__func__);
  return 0;
}

int APS5_VABI isobmf_decoder_create_nid_postfix(void) {
  NotImplemented_nid_no_patch(__func__);
  return 0;
}

int APS5_VABI isobmf_decoder_decode_nid_postfix(void) {
  NotImplemented_nid_no_patch(__func__);
  return 0;
}

int APS5_VABI isobmf_decoder_destroy_nid_postfix(void) {
  NotImplemented_nid_no_patch(__func__);
  return 0;
}

int APS5_VABI isobmf_decoder_get_decoder_tree_nid_postfix(void) {
  NotImplemented_nid_no_patch(__func__);
  return 0;
}

int APS5_VABI isobmf_decoder_getquerydata_nid_postfix(void) {
  NotImplemented_nid_no_patch(__func__);
  return 0;
}

std::uint64_t APS5_VABI
isobmf_decoder_next_disk_offset_nid_postfix(void *decoder) {
  if (!decoder)
    return 0xffffffff;
  return ReadField<std::uint64_t>(decoder, 0xc0);
}

int APS5_VABI isobmf_decoder_query_nid_postfix(void) {
  NotImplemented_nid_no_patch(__func__);
  return 0;
}

int APS5_VABI isobmf_memmanager_set_nid_postfix(void) {
  NotImplemented_nid_no_patch(__func__);
  return 0;
}
}
