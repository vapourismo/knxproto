#ifndef HPKNXD_KNX_CONNRES_H
#define HPKNXD_KNX_CONNRES_H

#include "hostinfo.h"
#include "../msgbuilder.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Connection Response
 */
typedef struct {
	uint8_t channel;
	uint8_t status;
	knx_host_info host;
	uint8_t extended[3];
} knx_connection_response;

/**
 * Generate the message for a connection response.
 */
bool knx_append_connection_response(msgbuilder* mb,
                                         const knx_connection_response* res);

/**
 * Parse a message (excluding header) which contains a connection response.
 */
bool knx_parse_connection_response(const uint8_t* message, size_t length,
                                        knx_connection_response* res);

#endif
