#ifndef HPKNXD_KNX_CONNSTATERES_H
#define HPKNXD_KNX_CONNSTATERES_H

#include "../msgbuilder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Connection State Response
 */
typedef struct {
	uint8_t channel;
	uint8_t status;
} knxnetip_connection_state_response;

/**
 * Generate the message for a connection response.
 */
bool knxnetip_append_connection_state_response(msgbuilder* mb,
                                               const knxnetip_connection_state_response* res);

/**
 * Parse a message (excluding header) which contains a connection response.
 */
bool knxnetip_parse_connection_state_response(const uint8_t* message, size_t length,
                                              knxnetip_connection_state_response* res);

#endif
