# libSceMbus technical debt

### Silent stubs

- [libSceMbus](../../../core/libs/prx/libSceMbus/Export.cpp) implements only the no-device path: initialization and termination succeed, no device events are reported, device queries return no entries and `sceDeviceServiceGetGeneration` always returns 0, as the device set never changes. The generation a console starts at is unmeasured; [SharpProspero](https://github.com/SvenGDK/SharpProspero/blob/5dfdb4b0b166e83dc9ae275001afb695f12c7174/src/SharpProspero/Interop/Device/DeviceService.cs#L63-L65) documents `int sceDeviceServiceGetGeneration()` as a counter that advances when the device set changes

### Unknown function info

- [sceDeviceServiceQueryDeviceInfo_](../../../core/libs/prx/libSceMbus/Export.cpp): the 112-byte device-info layout and the third argument's meaning remain unresolved. The second argument is a six-byte HID usage filter pointer, not an integer. Initialization takes an optional 16-byte parameter structure with an optional thread configuration; custom scheduling throws because the host has no event worker. `sceDeviceServiceGetEventState` takes a clear flag, not a device type.

- [libSceMbus firmware 3.20 pending exports](../../../core/libs/prx/libSceMbus/PendingExports.cpp) - 99 remaining symbol names are present in [the PS5 3.20 libSceMbus export list](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceMbus.c). Their signatures and console behaviour are unverified. The declarations use the existing no-parameter unknown-signature convention solely to resolve imports, and every call throws through `NotImplemented_nid_no_patch`.

- [sceMbusAddHandleByDeviceId](../../../core/libs/prx/libSceMbus/PendingExports.cpp): the six-argument ABI is confirmed, including the 64-bit device ID. The meanings of the last three 32-bit fields require the device-service backend.

- [sceMbus control and handle registration](../../../core/libs/prx/libSceMbus/PendingExports.cpp): argument positions and transmitted widths are recovered for control acquisition, handle registration, process attachment, user binding and device-ID conversion. Several scalar field meanings still depend on the service protocol; the 64-bit control value and unnamed registration fields must not be interpreted as pointers. AcquireControlList returns a constant error without reading its incoming arguments, so its source signature remains unresolved.
