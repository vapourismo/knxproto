#ifndef HPKNXD_KNX_KNXNETIP_H
#define HPKNXD_KNX_KNXNETIP_H

#include "../message-builder.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * KNXnet/IP Service Type
 */
typedef enum {
	KNXNETIP_SEARCH_REQUEST,
	KNXNETIP_SEARCH_RESPONSE,
	KNXNETIP_DESCRIPTION_REQUEST,
	KNXNETIP_DESCRIPTION_RESPONSE,
	KNXNETIP_CONNECTION_REQUEST,
	KNXNETIP_CONNECTION_RESPONSE,
	KNXNETIP_CONNECTIONSTATE_REQUEST,
	KNXNETIP_CONNECTIONSTATE_RESPONSE,
	KNXNETIP_DISCONNECT_REQUEST,
	KNXNETIP_DISCONNECT_RESPONSE,
	KNXNETIP_DEVICE_CONFIGURATION_REQUEST,
	KNXNETIP_DEVICE_CONFIGURATION_ACK,
	KNXNETIP_TUNNEL_REQUEST,
	KNXNETIP_TUNNEL_RESPONSE,
	KNXNETIP_ROUTING_INDICATION
} knxnetip_service;

/**
 * Generate the KNXnet/IP header and append it to the message.
 */
bool knxnetip_header(msgbuilder* mb, knxnetip_service service, uint16_t length);

/**
 * Parse a KNXnet/IP packet.
 */
ssize_t knxnetip_parse(const uint8_t* restrict packet, size_t length,
                       knxnetip_service* service, const uint8_t** payload);

#endif
