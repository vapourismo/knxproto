#ifndef KNXCLIENT_KNX_CONNREQ_H
#define KNXCLIENT_KNX_CONNREQ_H

#include "hostinfo.h"

#include "../msgbuilder.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * KNX Connection Type
 */
typedef enum {
	KNX_CONNECTION_REQUEST_TUNNEL = 4
} knx_conn_type;

/**
 * KNX Layer
 */
typedef enum {
	KNX_LAYER_TUNNEL = 2
} knx_layer;

/**
 * Connection Request
 */
typedef struct {
	knx_conn_type type;
	knx_layer layer;
	knx_host_info control_host;
	knx_host_info tunnel_host;
} knx_connection_request;

/**
 * Generate the message for a connection request.
 */
bool knx_append_connection_request(msgbuilder* mb,
                                   const knx_connection_request* conn_req);

/**
 * Parse a message (excluding header) which contains a connection request.
 */
bool knx_parse_connection_request(const uint8_t* message, size_t length,
                                  knx_connection_request* req);

#endif
