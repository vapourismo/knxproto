#ifndef HPKNXD_KNX_CONNREQ_H
#define HPKNXD_KNX_CONNREQ_H

#include "hostinfo.h"
#include "../msgbuilder.h"

#include <sys/types.h>

/**
 * KNX Connection Type
 */
typedef enum {
	KNXNETIP_CONNECTION_REQUEST_TUNNEL = 4
} knxnetip_conn_type;

/**
 * KNX Layer
 */
typedef enum {
	KNXNETIP_LAYER_TUNNEL = 2
} knxnetip_layer;

/**
 * Connection Request
 */
typedef struct {
	knxnetip_conn_type type;
	knxnetip_layer layer;
	knxnetip_host_info control_host;
	knxnetip_host_info tunnel_host;
} knxnetip_connection_request;

/**
 * Generate the message for a connection request.
 */
bool knxnetip_append_connection_request(msgbuilder* mb,
                                        const knxnetip_connection_request* conn_req);

/**
 * Parse a message (excluding header) which contains a connection request.
 */
bool knxnetip_parse_connection_request(const uint8_t* message, size_t length,
                                       knxnetip_connection_request* req);

#endif
