#ifndef HPKNXD_KNX_DCREQ_H
#define HPKNXD_KNX_DCREQ_H

#include "hostinfo.h"

#include "../msgbuilder.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Disconnect Request
 */
typedef struct {
	uint8_t channel;
	uint8_t status;
	knxnetip_host_info host;
} knxnetip_disconnect_request;

/**
 * Generate the message for a disconnect request.
 */
bool knxnetip_append_disconnect_request(msgbuilder* mb,
                                        const knxnetip_disconnect_request* req);

/**
 * Parse a message (excluding header) which contains a disconnect request.
 */
bool knxnetip_parse_disconnect_request(const uint8_t* message, size_t length,
                                       knxnetip_disconnect_request* req);

#endif
