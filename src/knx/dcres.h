#ifndef HPKNXD_KNX_DCRES_H
#define HPKNXD_KNX_DCRES_H

#include "../msgbuilder.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Disconnect Response
 */
typedef struct {
	uint8_t channel;
	uint8_t status;
} knx_disconnect_response;

/**
 * Generate the message for a disconnect response.
 */
bool knx_append_disconnect_response(msgbuilder* mb,
                                         const knx_disconnect_response* res);

/**
 * Parse a message (excluding header) which contains a disconnect response.
 */
bool knx_parse_disconnect_response(const uint8_t* message, size_t length,
                                        knx_disconnect_response* res);

#endif
