# libSceSaveDataDialog.native technical debt

### Silent stubs

- [libSceSaveDataDialog.native](../../../core/libs/prx/libSceSaveDataDialog.native/Export.cpp) shows no UI and ignores progress values. Mode 5 (progress) runs until closed (Skyrim PPSA03747); other modes finish immediately returning button 1 and first nonempty directory. Console timing and selection are unverified. Lifecycle errors follow shadPS4: `sceSaveDataDialogClose` and progress updates return `SAVE_DATA_DIALOG_ERROR_NOT_RUNNING` when not running, `sceSaveDataDialogGetResult` returns `SAVE_DATA_DIALOG_ERROR_NOT_FINISHED` before null checks, and `sceSaveDataDialogTerminate` closes running dialogs, returning `SAVE_DATA_DIALOG_ERROR_NOT_INITIALIZED` when uninitialized. Parameter size in `sceSaveDataDialogOpen` is unvalidated as console behavior is unmeasured.
- [sceSaveDataDialogIsReadyToDisplay](../../../core/libs/prx/libSceSaveDataDialog.native/Export.cpp) returns 1 without checking initialization or readiness.
