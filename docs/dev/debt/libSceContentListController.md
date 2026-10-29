# libSceContentListController technical debt

### Additional callable exports

- [Additional callable exports](../../../core/libs/prx/libSceContentListController/Export.cpp) - 19 entries observed in the 3.20 export inventory and present in the later 13.60-labelled inventory are import-resolution declarations only. The later inventory label does not authenticate a retail firmware version. Parameter lists, return types and behavior are unverified; parameterless `int` declarations are placeholders, not verified ABI contracts. Every entry throws through `NotImplemented_nid_no_patch`. Unidentified names use literal NID aliases. No runtime functionality is implemented. Identified candidate names match their NIDs in the [public symbol database](https://github.com/zecoxao/sce_symbols/blob/2883963a0a514ba6e77407a08ef4730a65e88254/aerolib.csv) or [supplemental catalog](https://github.com/claimore22/ps5rs/blob/2304be5131221bae0dac24b9767cabffda33f747/data/nids.csv).

| Name | NID |
| --- | --- |
| `sceCLCClearFileList` | `nAF1xF5QRHk` |
| `sceCLCClearTrackList` | `yx2QM7H-nww` |
| `sceCLCGetDeviceInfo` | `dYe+SpaG3vI` |
| `sceCLCGetDeviceList` | `Aqs-JQ9uJUc` |
| `sceCLCGetFileInfo` | `cq8s1ijWULA` |
| `sceCLCGetFileList` | `GvFZOeEJ9aw` |
| `sceCLCGetFileListInfo` | `tulcX6FKfk0` |
| `sceCLCGetFolderInfo` | `lm4JK2NTzoA` |
| `sceCLCGetShuffle` | `2Vf60vmDN3A` |
| `sceCLCGetTrackInfo` | `ZidmCssGGUs` |
| `sceCLCGetTrackList` | `HECulNjRDBA` |
| `sceCLCGetTrackListInfo` | `H1lNYyrbrlQ` |
| `sceCLCGetUsbStorageCallback` | `DGHwTmra1QE` |
| `sceCLCRegisterEventCallback` | `ujyPLnX7SlE` |
| `sceCLCSetPlaybackResult` | `GC51I89ii44` |
| `sceCLCSetShuffle` | `EPOYwPzb14s` |
| `sceCLCSetTrackList` | `5nwdtz34v7I` |
| `sceCLCSetUsbStorageCallback` | `037hAMY7tHY` |
| `Unknown` | `SoBk-kuNLsE` |
