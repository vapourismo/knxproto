#ifndef HPKNXD_KNX_TUNNELRES_H
#define HPKNXD_KNX_TUNNELRES_H

#include "../msgbuilder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Tunnel Response
 */
typedef struct {
	uint8_t channel;
	uint8_t seq_number;
	uint8_t status;
} knxnetip_tunnel_response;

/**
 * Generate the message for a tunnel response.
 */
bool knxnetip_append_tunnel_response(msgbuilder* mb,
                                     const knxnetip_tunnel_response* res);

/**
 * Parse a message (excluding header) which contains a tunnel response.
 */
bool knxnetip_parse_tunnel_response(const uint8_t* message, size_t length,
                                    knxnetip_tunnel_response* res);

#endif
