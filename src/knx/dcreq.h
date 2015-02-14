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
	knx_host_info host;
} knx_disconnect_request;

/**
 * Generate the message for a disconnect request.
 */
bool knx_append_disconnect_request(msgbuilder* mb,
                                   const knx_disconnect_request* req);

/**
 * Parse a message (excluding header) which contains a disconnect request.
 */
bool knx_parse_disconnect_request(const uint8_t* message, size_t length,
                                  knx_disconnect_request* req);

#endif
