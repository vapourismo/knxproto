#ifndef KNXCLIENT_KNX_HEADER_H
#define KNXCLIENT_KNX_HEADER_H

#include "../msgbuilder.h"

#include <stdint.h>
#include <stdbool.h>

/**
 * KNXnet/IP Service Type
 */
typedef enum {
	KNX_SEARCH_REQUEST               = 0x0201,
	KNX_SEARCH_RESPONSE              = 0x0202,
	KNX_DESCRIPTION_REQUEST          = 0x0203,
	KNX_DESCRIPTION_RESPONSE         = 0x0204,
	KNX_CONNECTION_REQUEST           = 0x0205,
	KNX_CONNECTION_RESPONSE          = 0x0206,
	KNX_CONNECTIONSTATE_REQUEST      = 0x0207,
	KNX_CONNECTIONSTATE_RESPONSE     = 0x0208,
	KNX_DISCONNECT_REQUEST           = 0x0209,
	KNX_DISCONNECT_RESPONSE          = 0x020A,
	KNX_DEVICE_CONFIGURATION_REQUEST = 0x0310,
	KNX_DEVICE_CONFIGURATION_ACK     = 0x0311,
	KNX_TUNNEL_REQUEST               = 0x0420,
	KNX_TUNNEL_RESPONSE              = 0x0421,
	KNX_ROUTING_INDICATION           = 0x0530
} knx_service;

/**
 * Append a KNXnet/IP header which advocates the given service and payload length.
 */
bool knx_append_header(msgbuilder* mb, knx_service srv, uint16_t length);

#endif
