#ifndef HPKNXD_KNX_KNXNETIP_H
#define HPKNXD_KNX_KNXNETIP_H

#include "../message-builder.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>

/**
 * KNXnet/IP Service Type
 */
typedef enum {
	KNXNETIP_SEARCH_REQUEST,
	KNXNETIP_SEARCH_RESPONSE,
	KNXNETIP_DESCRIPTION_REQUEST,
	KNXNETIP_DESCRIPTION_RESPONSE,
	KNXNETIP_CONNECTION_REQUEST,
	KNXNETIP_CONNECTION_RESPONSE,
	KNXNETIP_CONNECTIONSTATE_REQUEST,
	KNXNETIP_CONNECTIONSTATE_RESPONSE,
	KNXNETIP_DISCONNECT_REQUEST,
	KNXNETIP_DISCONNECT_RESPONSE,
	KNXNETIP_DEVICE_CONFIGURATION_REQUEST,
	KNXNETIP_DEVICE_CONFIGURATION_ACK,
	KNXNETIP_TUNNEL_REQUEST,
	KNXNETIP_TUNNEL_RESPONSE,
	KNXNETIP_ROUTING_INDICATION
} knxnetip_service;

/**
 * KNX Connection Type
 */
typedef enum {
	KNXNETIP_CONNREQ_TUNNEL = 4
} knxnetip_conn_type;

/**
 * KNX Layer
 */
typedef enum {
	KNXNETIP_LAYER_TUNNEL = 2
} knxnetip_layer;

/**
 * KNXnet/IP Protocol
 */
typedef enum {
	KNXNETIP_PROTO_UDP = 1,
	KNXNETIP_PROTO_TCP = 2
} knxnetip_proto;

/**
 * Host Information
 */
typedef struct {
	knxnetip_proto protocol;
	struct sockaddr_in address;
} knxnetip_host_info;

/**
 * Connection Request
 */
typedef struct {
	knxnetip_conn_type type;
	knxnetip_layer layer;
	knxnetip_host_info control_host;
	knxnetip_host_info tunnel_host;
} knxnetip_connreq;

/**
 * KNXnet/IP Packet
 */
typedef struct {
	knxnetip_service service;
	union {
		knxnetip_connreq connection_request;
	};
} knxnetip_packet;

/**
 * Generate the message for a connection request.
 */
bool knxnetip_generate_connreq(msgbuilder* mb, const knxnetip_connreq* connreq);

/**
 * Generate the message from the given packet information.
 */
bool knxnetip_generate(msgbuilder* mb, const knxnetip_packet* packet);

/**
 * Parse a given message into a packet.
 */
bool knxnetip_parse(const uint8_t* msg, size_t length,
                    knxnetip_packet* packet);

#endif
