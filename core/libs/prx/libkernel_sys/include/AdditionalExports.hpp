#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceKernelTraceAutoCaptureCreate();
int APS5_VABI sceKernelTraceAutoCaptureDelete();
int APS5_VABI sceKernelTraceAutoCaptureMap();
int APS5_VABI sceKernelTraceAutoCaptureStart();
int APS5_VABI sceKernelTraceAutoCaptureStop();
int APS5_VABI sceKernelTraceAutoCaptureStore();
int APS5_VABI sceKernelTraceAutoCaptureUnmap();

}
