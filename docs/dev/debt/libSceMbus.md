# libSceMbus technical debt

### Silent stubs

- [libSceMbus](../../../core/libs/prx/libSceMbus/Export.cpp) implements only the no-device path: initialization and termination succeed, no device events are reported, device queries return no entries and `sceDeviceServiceGetGeneration` always returns 0, as the device set never changes. The generation a console starts at is unmeasured; [SharpProspero](https://github.com/SvenGDK/SharpProspero/blob/5dfdb4b0b166e83dc9ae275001afb695f12c7174/src/SharpProspero/Interop/Device/DeviceService.cs#L63-L65) documents `int sceDeviceServiceGetGeneration()` as a counter that advances when the device set changes

### Unknown function info

- [sceDeviceServiceQueryDeviceInfo_](../../../core/libs/prx/libSceMbus/Export.cpp): the 112-byte device-info layout and the third argument's meaning remain unresolved. The second argument is a six-byte HID usage filter pointer, not an integer. Initialization takes an optional 16-byte parameter structure with an optional thread configuration; custom scheduling throws because the host has no event worker. `sceDeviceServiceGetEventState` takes a clear flag, not a device type.

- [libSceMbus firmware 3.20 pending exports](../../../core/libs/prx/libSceMbus/PendingExports.cpp) - remaining symbol names are present in [the PS5 3.20 libSceMbus export list](https://github.com/DNNDHH/PS5-3.20_Libs/blob/b2a77c09cd71c01f34c790cfbe98c751a13262ad/libSceMbus.c). Their signatures and console behaviour are unverified. The declarations use the existing no-parameter unknown-signature convention solely to resolve imports, and every call throws through `NotImplemented_nid_no_patch`.

- [sceMbusAddHandleByDeviceId](../../../core/libs/prx/libSceMbus/PendingExports.cpp): the six-argument ABI is confirmed, including the 64-bit device ID. The meanings of the last three 32-bit fields require the device-service backend.

- [sceMbus control and handle registration](../../../core/libs/prx/libSceMbus/PendingExports.cpp): argument positions and transmitted widths are recovered for control acquisition, handle registration, process attachment, user binding and device-ID conversion. Several scalar field meanings still depend on the service protocol; the 64-bit control value and unnamed registration fields must not be interpreted as pointers. AcquireControlList returns a constant error without reading its incoming arguments, so its source signature remains unresolved.

- [sceMbus debug process and device queries](../../../core/libs/prx/libSceMbus/PendingExports.cpp): process information is transmitted as 16 bytes, startup input/output as eight bytes, control status as 24 bytes, internal information as 2832 bytes and priority information as 3088 bytes. Device enumeration writes a 32-bit count followed by 396-byte entries. These pointee fields and the semantics of unnamed process flags remain unresolved; declarations use distinct incomplete types instead of guessing their fields.

- [sceMbus event subscriptions](../../../core/libs/prx/libSceMbus/PendingExports.cpp): the subscription transports two 32-bit and two 64-bit fields whose service-side meanings remain unresolved. EventCreate uses 80-byte queue records and copies a 16-byte callback table; callback signatures and event fields still require the dispatch path. EventCallbackFuncsInit only clears memory and has no defined return value; its protected stub retains the existing integer return. Eight dump/BGM wrappers have confirmed zero-argument ABIs. EventBusStatusChangeUnsubscribe ignores its first incoming argument and consumes a 32-bit second argument; its first parameter type remains unresolved.

- [sceMbus device and driver payloads](../../../core/libs/prx/libSceMbus/PendingExports.cpp): device-info records occupy 256 bytes, descriptions 264 bytes, simulated-handle status and Spark state 16 bytes each. Their service-side fields remain unresolved. Driver registration copies four callback pointers; driver-event notification transports two 64-bit values without dereferencing them. Init, IsBgmProhibited, ReenableBgmForShellUi and RemoveCameraAppModuleFocus consume no incoming arguments.
