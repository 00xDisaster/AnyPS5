# libSceCdlgPlayerReview technical debt

### Unknown function info

- [scePlayerReviewDialogOpen](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) uses a recovered 168-byte parameter layout; the 40-byte target record at byte 60 and mode semantics remain unresolved. Active-user verification is not implemented.
- [scePlayerReviewDialogClose](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) (libSceCdlgPlayerReview) - `scePlayerReviewDialogClose` moves an initialized dialog straight to `COMMON_DIALOG_STATUS_FINISHED` even when called before Open, as Signin, NetCtlAp and PlayerSelection do; MsgDialog.native, SaveDataDialog.native and ErrorDialog return an error there instead. Unverified against real hardware

### Functional

- [Player review dialog](../../../core/libs/prx/libSceCdlgPlayerReview/Export.cpp) (libSceCdlgPlayerReview) shows nothing: `scePlayerReviewDialogOpen` finishes at once and the result reports that the user canceled, as `sceWebBrowserDialogOpen` does
