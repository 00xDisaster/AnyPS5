#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI tls_client_nid_postfix();
int APS5_VABI tls_close_nid_postfix();
int APS5_VABI tls_config_free_nid_postfix();
int APS5_VABI tls_config_new_nid_postfix();
int APS5_VABI tls_configure_nid_postfix();
int APS5_VABI tls_error_nid_postfix();
int APS5_VABI tls_free_nid_postfix();
int APS5_VABI tls_read_nid_postfix();
int APS5_VABI tls_server_nid_postfix();
int APS5_VABI tls_write_nid_postfix();

}
