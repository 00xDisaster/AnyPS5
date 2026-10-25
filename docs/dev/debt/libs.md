# libs technical debt

### Unknown function info

- [SceAvPlayerVideoEx](../../../core/libs/SceTypes.hpp) (libSceAvPlayer) - frame rate field and encoding unknown; it is left zero in frame and stream info

### Functional

- [GuestKernelSys.cpp](../../../core/libs/tests/GuestKernelSys.cpp) (libs) - Windows removes the fixture's symbolic links and the fixture without std::filesystem: the MinGW libstdc++ (GCC 15.2.0) reports a dangling symbolic link as missing and clears the error, so `remove` neither deletes it nor fails, and `remove_all` loops forever over a tree that holds one (`recursive_directory_iterator::__erase` retries the entry while the error stays clear). The tests delete reparse points with `DeleteFileW` and `RemoveDirectoryW` and the fixture through the shell

- [nid_patcher](../../../core/libs/nid/src/main.cpp) (libs) - `--exec` supports little-endian ELF64 with section headers, PE32/PE32+ named imports and thin Mach-O64 chained imports. ELF symbol version definitions, Mach-O bind opcodes and compressed chained symbols are rejected. Signed Mach-O inputs must have their signature removed before patching and be signed again afterwards. Signed PE, bound imports and delay imports are rejected; a growing PE import table requires space for another section header. ELF/Mach-O loader execution of this mode has not been validated on their native platforms.
