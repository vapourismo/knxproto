#ifndef HPKNXD_KNX_CONNSTATE_H
#define HPKNXD_KNX_CONNSTATE_H

#include "hostinfo.h"
#include "../msgbuilder.h"

#include <stdint.h>
#include <stdbool.h>

/**
 * Connection Response
 */
typedef struct {
	uint8_t channel;
	uint8_t status;
	knxnetip_host_info host;
	uint8_t extended[3];
} knxnetip_connection_response;

/**
 * Generate the message for a connection response.
 */
bool knxnetip_append_connection_response(msgbuilder* mb,
                                         const knxnetip_connection_response* res);

/**
 * Parse a message (excluding header) which contains a connection response.
 */
bool knxnetip_parse_connection_response(const uint8_t* message, size_t length,
                                        knxnetip_connection_response* res);

#endif
