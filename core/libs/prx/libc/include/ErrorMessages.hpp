#ifndef CORE_LIBS_PRX_LIBC_INCLUDE_ERRORMESSAGES_HPP
#define CORE_LIBS_PRX_LIBC_INCLUDE_ERRORMESSAGES_HPP

#include <cstddef>

extern "C" void FreeBsdErrorMessage_nid_no_patch(int error, char* buffer, std::size_t length);

#endif
