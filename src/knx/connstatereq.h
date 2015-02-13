#ifndef HPKNXD_KNX_CONNSTATEREQ_H
#define HPKNXD_KNX_CONNSTATEREQ_H

#include "../msgbuilder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Connection Request
 */
typedef struct {
	uint8_t channel;
	uint8_t state;
} knxnetip_connection_state_request;

/**
 * Generate the message for a connection request.
 */
bool knxnetip_append_connection_state_request(msgbuilder* mb,
                                              const knxnetip_connection_state_request* conn_req);

/**
 * Parse a message (excluding header) which contains a connection request.
 */
bool knxnetip_parse_connection_state_request(const uint8_t* message, size_t length,
                                             knxnetip_connection_state_request* req);

#endif
