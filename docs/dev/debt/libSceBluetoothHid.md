# libSceBluetoothHid technical debt

### Silent stubs

- [libSceBluetoothHid](../../../core/libs/prx/libSceBluetoothHid/Export.cpp): the host still has no device backend or event worker. Registration retains only lifecycle state and never delivers the recovered 104-byte callback events. Custom worker scheduling throws; device ioctl failures and resource exhaustion are not modeled.

### Unknown function info

- [sceBluetoothHidGetDeviceName and sceBluetoothHidGetDeviceInfo](../../../core/libs/prx/libSceBluetoothHid/Export.cpp): output pointers are forwarded to device ioctls without accessing the buffers. Their output sizes and layouts remain unresolved. Report APIs use 64-bit device handles; device-info and disconnect APIs use 32-bit device IDs.
- [sceBluetoothHidParamInitialize](../../../core/libs/prx/libSceBluetoothHid/Export.cpp) follows [SharpProspero](https://github.com/SvenGDK/SharpProspero/blob/5dfdb4b0b166e83dc9ae275001afb695f12c7174/src/SharpProspero/Interop/Bluetooth/SceBluetoothHid.cs#L21-L49), whose signatures were recovered from the PS5 module: it writes the thread-parameter pointer at offset 0 of the 16-byte block, clears the 32-bit word at offset 8 and returns 0. The source names no other byte, so bytes 12 to 15 keep what the caller left there. A null block throws, as no source gives its answer. Not measured on a PS5
- [sceBluetoothHidDebugGetVersion](../../../core/libs/prx/libSceBluetoothHid/Export.cpp): returns a constant without reading arguments, so the supplied implementation does not establish whether the public prototype has unused parameters.
