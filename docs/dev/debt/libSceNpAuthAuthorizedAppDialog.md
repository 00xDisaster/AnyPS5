# libSceNpAuthAuthorizedAppDialog technical debt

### Silent stubs

- [libSceNpAuthAuthorizedAppDialog](../../../core/libs/prx/libSceNpAuthAuthorizedAppDialog/Export.cpp) - the authorized-app dialog runs the common-dialog state machine but shows nothing and reports the user as having cancelled, as there is no PSN sign-in to authorize an app against. The signatures follow the other common dialogs (Open takes a parameter block, GetResult writes a 32-bit result); Minecraft (PPSA17221) imports the seven functions and fails to load without them.
- [sceNpAuthAuthorizedAppDialogClose](../../../core/libs/prx/libSceNpAuthAuthorizedAppDialog/Export.cpp) - sceNpAuthAuthorizedAppDialogClose transitions an initialized dialog directly to COMMON_DIALOG_STATUS_FINISHED even when called before Open, following the Signin / NetCtlAp / PlayerSelection convention; this behavior is unverified against real hardware.
