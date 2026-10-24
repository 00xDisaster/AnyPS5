#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceInvitationDialogGetResult();
int APS5_VABI sceInvitationDialogInitialize();
int APS5_VABI sceInvitationDialogOpen();
int APS5_VABI sceInvitationDialogTerminate();
int APS5_VABI sceInvitationDialogUpdateStatus();
int APS5_VABI sceInvitationDialogClose(void);

}
