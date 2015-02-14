#ifndef KNXCLIENT_KNX_CONNSTATEREQ_H
#define KNXCLIENT_KNX_CONNSTATEREQ_H

#include "hostinfo.h"

#include "../msgbuilder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Connection State Request
 */
typedef struct {
	uint8_t channel;
	uint8_t status;
	knx_host_info host;
} knx_connection_state_request;

/**
 * Generate the message for a connection state request.
 */
bool knx_append_connection_state_request(msgbuilder* mb,
                                         const knx_connection_state_request* req);

/**
 * Parse a message (excluding header) which contains a connection state request.
 */
bool knx_parse_connection_state_request(const uint8_t* message, size_t length,
                                        knx_connection_state_request* req);

#endif
