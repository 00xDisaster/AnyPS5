# libScePlayGoDialog technical debt

### Unverified behaviors

- [scePlayGoDialogGetResult](../../../core/libs/prx/libScePlayGoDialog/Export.cpp) (libScePlayGoDialog) - checks that the dialog has reached STATUS_FINISHED and returns 0x80ED0005 (ERROR_INVALID_STATE) when called before the dialog completes. The error code 0x80ED0005 falls within the ORBIS_ERROR_DIALOG_ERROR_* range in shadPS4 (ime/ime_error.h), whereas other common dialog modules in the tree return 0x80B80005 (COMMON_DIALOG_ERROR_NOT_FINISHED). The exact return code and intermediate state check have not been verified against a real hardware trace or title measurement.
