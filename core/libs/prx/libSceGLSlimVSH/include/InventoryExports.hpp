#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceSlimglCompositorSetInvisibleCanvasCommand();
int APS5_VABI sceSlimglCompositorSetMemoryCommand();
int APS5_VABI sceSlimglCompositorSetPostEventCommand();
int APS5_VABI sceSlimglCompositorWaitEndOfRendering();
int APS5_VABI sceSlimglCompositorWaitPostEvent();
int APS5_VABI sceSlimglIPCEnd();
int APS5_VABI sceSlimglIPCSetup();

}
