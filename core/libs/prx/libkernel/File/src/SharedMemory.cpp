#include "prx/libc/include/General.hpp"
#include "prx/libkernel/File/include/FileFlags.hpp"

#include <cerrno>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <sys/stat.h>
#include <atomic>
#include <filesystem>
#include <random>
#else
#include "nid/Sha1.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#endif

extern "C" int* APS5_VABI __error_nid_postfix();

namespace {

constexpr int GuestNoMemory = 12;
constexpr int GuestInvalid = 22;
constexpr int GuestTooManyFilesInSystem = 23;
constexpr int GuestTooManyFiles = 24;
constexpr int GuestCloseOnExec = 0x00100000;
constexpr int AcceptedFlags = SCE_KERNEL_O_ACCMODE | SCE_KERNEL_O_CREAT | SCE_KERNEL_O_EXCL | SCE_KERNEL_O_TRUNC | GuestCloseOnExec;

const char* const anonymousObject = reinterpret_cast<const char*>(1);

int Fail(int error) {
    *__error_nid_postfix() = error;
    return -1;
}

int GuestError(int error) {
    switch (error) {
    case EMFILE:
        return GuestTooManyFiles;
    case ENFILE:
        return GuestTooManyFilesInSystem;
    case ENOMEM:
        return GuestNoMemory;
    default:
        return 0;
    }
}

#ifndef _WIN32
int NamedGuestError(int error) {
    switch (error) {
    case ENOENT:
        return 2;
    case EINTR:
        return 4;
    case EACCES:
        return 13;
    case EFAULT:
        return 14;
    case EEXIST:
        return 17;
    case EINVAL:
        return GuestInvalid;
    case ENOSPC:
        return 28;
    case EROFS:
        return 30;
    case ENAMETOOLONG:
        return 63;
    default:
        return GuestError(error);
    }
}

int OpenNamedFile(const char* path, int flags, int mode) {
    if (path == nullptr) return Fail(14);
    std::size_t length = 0;
    while (length < 1024 && path[length] != '\0') ++length;
    if (length == 1024) return Fail(63);
    if (length == 0 || path[0] != '/') return Fail(GuestInvalid);
    const auto digest = Nid::Sha1(std::vector<std::uint8_t>(path, path + length));
    std::string hostName = "/anyps5-shm-";
    for (const auto byte : digest) {
        hostName += "0123456789abcdef"[byte >> 4];
        hostName += "0123456789abcdef"[byte & 15];
    }
    int hostFlags = (flags & SCE_KERNEL_O_ACCMODE) == SCE_KERNEL_O_RDWR ? O_RDWR : O_RDONLY;
    if (flags & SCE_KERNEL_O_CREAT) hostFlags |= O_CREAT;
    if (flags & SCE_KERNEL_O_EXCL) hostFlags |= O_EXCL;
    if ((flags & SCE_KERNEL_O_TRUNC) && (flags & SCE_KERNEL_O_ACCMODE) == SCE_KERNEL_O_RDWR) hostFlags |= O_TRUNC;
    const int descriptor = ::shm_open(hostName.c_str(), hostFlags | O_CLOEXEC, mode & 0777);
    if (descriptor >= 0) return descriptor;
    const int error = errno;
    if (const int guest = NamedGuestError(error)) return Fail(guest);
    throw std::runtime_error("shm_open: opening the host named object failed, errno=" + std::to_string(error));
}
#endif

int CreateAnonymousFile() {
#ifdef _WIN32
    static std::atomic<unsigned> sequence{0};
    const auto directory = std::filesystem::temp_directory_path();
    const auto seed = std::to_string(std::random_device{}());
    for (int attempt = 0; attempt < 64; ++attempt) {
        const auto path = directory / ("anyps5-shm-" + seed + "-" + std::to_string(sequence++));
        int descriptor = -1;
        const int error = _wsopen_s(&descriptor, path.c_str(),
                                    _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY | _O_TEMPORARY | _O_SHORT_LIVED | _O_NOINHERIT,
                                    _SH_DENYNO, _S_IREAD | _S_IWRITE);
        if (error == 0) return descriptor;
        if (error != EEXIST) {
            errno = error;
            return -1;
        }
    }
    errno = EEXIST;
    return -1;
#else
    return memfd_create("anyps5 shm_open", MFD_CLOEXEC);
#endif
}

}

extern "C" int APS5_VABI shm_open_nid_postfix(const char* path, int flags, int mode) {
    const int accessMode = flags & SCE_KERNEL_O_ACCMODE;
    if (accessMode != SCE_KERNEL_O_RDONLY && accessMode != SCE_KERNEL_O_RDWR) return Fail(GuestInvalid);
    if ((flags & ~AcceptedFlags) != 0) return Fail(GuestInvalid);
    if (path != anonymousObject) {
#ifdef _WIN32
        NotImplemented_nid_no_patch("shm_open of a named object");
#else
        return OpenNamedFile(path, flags, mode);
#endif
    }
    if (accessMode == SCE_KERNEL_O_RDONLY) return Fail(GuestInvalid);
    const int descriptor = CreateAnonymousFile();
    if (descriptor >= 0) return descriptor;
    const int error = errno;
    if (const int guest = GuestError(error)) return Fail(guest);
    throw std::runtime_error("shm_open: creating the host anonymous file failed, errno=" + std::to_string(error));
}
