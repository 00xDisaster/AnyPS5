#include "prx/libc/include/general/ExportMacros.hpp"
#include "prx/libSceIpmi/include/InventoryExports.hpp"
#include "prx/libSceIpmi/include/Api.hpp"
#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

APS5_DUMMY_FUN
int APS5_VABI sceIpmiMgrGetIpcBlockerPid(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceIpmiMgrGetIpcBlockerPidByAppId(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceIpmiMgrPauseDump(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceIpmiMgrRaiseExceptionToPid(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceIpmiMgrStartDump(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}

extern "C" {

int APS5_VABI libSceIpmiUnknown_fjPNqzuUop8_nid_no_patch() {
    NotImplemented_nid_no_patch(__func__);
}
APS5_EXPORT("fjPNqzuUop8", libSceIpmiUnknown_fjPNqzuUop8_nid_no_patch);

int APS5_VABI libSceIpmiUnknown_gL4bo5h2nwU_nid_no_patch() {
    NotImplemented_nid_no_patch(__func__);
}
APS5_EXPORT("gL4bo5h2nwU", libSceIpmiUnknown_gL4bo5h2nwU_nid_no_patch);

int APS5_VABI libSceIpmiUnknown_rq3L3xKnt3Y_nid_no_patch() {
    NotImplemented_nid_no_patch(__func__);
}
APS5_EXPORT("rq3L3xKnt3Y", libSceIpmiUnknown_rq3L3xKnt3Y_nid_no_patch);

}
