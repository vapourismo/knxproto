#ifndef HPKNXD_KNX_DCREQ_H
#define HPKNXD_KNX_DCREQ_H

#include "../msgbuilder.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	int a;
} knxnetip_disconnect_request;

/**
 * Generate the message for a disconnect request.
 */
bool knxnetip_append_disconnect_request(msgbuilder* mb,
                                        const knxnetip_disconnect_request* conn_req);

/**
 * Parse a message (excluding header) which contains a disconnect request.
 */
bool knxnetip_parse_disconnect_request(const uint8_t* message, size_t length,
                                       knxnetip_disconnect_request* req);

#endif
