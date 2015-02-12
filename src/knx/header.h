#ifndef HPKNXD_KNX_HEADER_H
#define HPKNXD_KNX_HEADER_H

#include "../msgbuilder.h"

/**
 * KNXnet/IP Service Type
 */
typedef enum {
	KNXNETIP_SEARCH_REQUEST               = 0x0201,
	KNXNETIP_SEARCH_RESPONSE              = 0x0202,
	KNXNETIP_DESCRIPTION_REQUEST          = 0x0203,
	KNXNETIP_DESCRIPTION_RESPONSE         = 0x0204,
	KNXNETIP_CONNECTION_REQUEST           = 0x0205,
	KNXNETIP_CONNECTION_RESPONSE          = 0x0206,
	KNXNETIP_CONNECTIONSTATE_REQUEST      = 0x0207,
	KNXNETIP_CONNECTIONSTATE_RESPONSE     = 0x0208,
	KNXNETIP_DISCONNECT_REQUEST           = 0x0209,
	KNXNETIP_DISCONNECT_RESPONSE          = 0x020A,
	KNXNETIP_DEVICE_CONFIGURATION_REQUEST = 0x0310,
	KNXNETIP_DEVICE_CONFIGURATION_ACK     = 0x0311,
	KNXNETIP_TUNNEL_REQUEST               = 0x0420,
	KNXNETIP_TUNNEL_RESPONSE              = 0x0421,
	KNXNETIP_ROUTING_INDICATION           = 0x0530
} knxnetip_service;

/**
 * Append a KNXnet/IP header which advocates the given service and payload length.
 */
bool knxnetip_append_header(msgbuilder* mb, knxnetip_service srv, uint16_t length);

#endif
