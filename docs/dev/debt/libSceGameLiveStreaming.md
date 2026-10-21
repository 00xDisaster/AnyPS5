# libSceGameLiveStreaming technical debt

### Unknown function info

- [sceGameLiveStreamingGetCurrentStatus2](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - the status layout (user id, on-air flag, spectator, text and command message counts, broadcast resolution, 48 reserved bytes) is taken from shadPS4's `OrbisGameLiveStreamingStatus2`; the call reports that no stream is on air, with an invalid user id
- [sceGameLiveStreamingScreenInitialize](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingScreenInitializeSeparateModeParameter](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingScreenTerminate](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingScreenCloseSeparateMode](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingScreenConfigureSeparateMode](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingScreenOpenSeparateMode](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingScreenSetMode](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingPermitLiveStreaming](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetMaxBitrate](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingGetCurrentStatus](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingEnableSocialFeedback](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingGetSocialFeedbackMessagesCount](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingGetSocialFeedbackMessages](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingPermitServerSideRecording](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetMetadata](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetSpoilerTag](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingClearSpoilerTag](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetPresetSocialFeedbackCommands](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingClearPresetSocialFeedbackCommands](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetPresetSocialFeedbackCommandsDescription](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingClearSocialFeedbackMessages](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingPostSocialMessage](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetCameraFrameSetting](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetServiceProviderPermission](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetDefaultServiceProviderPermission](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetInvitationSessionId](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingStartGenerateStandbyScreenResource](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingStopGenerateStandbyScreenResource](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingRegisterCallback](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingUnregisterCallback](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingCheckCallback](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetStandbyScreenResource](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingGetCurrentBroadcastScreenLayout](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingLaunchLiveViewer](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingLaunchLiveViewerA](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetGuardAreas](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingStartSocialFeedbackMessageFiltering](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingStopSocialFeedbackMessageFiltering](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingApplySocialFeedbackMessageFilter](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingSetLinkCommentPreset](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingStartDebugBroadcast](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature
- [sceGameLiveStreamingStopDebugBroadcast](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - unknown signature

### Functional

- [sceGameLiveStreamingEnableLiveStreaming](../../../core/libs/prx/libSceGameLiveStreaming/Export.cpp) (libSceGameLiveStreaming) - a call outside `sceGameLiveStreamingInitialize` and `sceGameLiveStreamingTerminate` throws, as the console's error for it is unknown ([fpPS4](https://github.com/red-prig/fpPS4/blob/04cefd43e6fddd1ab033e7980cd356d14c964905/src/ps4_libscegamelivestreaming.pas#L18) defines `SCE_GAME_LIVE_STREAMING_ERROR_NOT_INITIALIZED` but does not return it from this call). Otherwise the flag is accepted for both values and changes nothing, as no broadcast is ever on air to allow or block. The signature, `int sceGameLiveStreamingEnableLiveStreaming(const bool isEnableLiveStreaming)`, follows [Orbital](https://github.com/AlexAltea/orbital/blob/cc408367da6a6ebbf5ac1ac559fefeadc73b2831/tools/ida/db_types.json#L810) and [fpPS4](https://github.com/red-prig/fpPS4/blob/04cefd43e6fddd1ab033e7980cd356d14c964905/src/ps4_libscegamelivestreaming.pas#L111)
