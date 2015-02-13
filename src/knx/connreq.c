#include "connreq.h"
#include "header.h"
#include "../alloc.h"

// Connection Request:
//   Octet 0-5:   Header
//   Octet 6-13:  Control host information
//   Octet 14-21: Tunnelling host information
//   Octet 22-25: Connection request information

// Connection Request Information:
//   Octet 0: Structure length
//   Octet 1: Connection type
//   Octet 2: KNX Layer
//   Octet 3: Reserved (should be 0)

bool knxnetip_append_connection_request(msgbuilder* mb,
                                        const knxnetip_connection_request* conn_req) {
	return
		knxnetip_append_header(mb, KNXNETIP_CONNECTION_REQUEST, 20) &&
		knxnetip_append_host_info(mb, &conn_req->control_host) &&
		knxnetip_append_host_info(mb, &conn_req->tunnel_host) &&
		msgbuilder_append(mb, anona(const uint8_t, 4, conn_req->type, conn_req->layer, 0), 4);
}

bool knxnetip_parse_connection_request(const uint8_t* message, size_t length,
                                       knxnetip_connection_request* req) {
	if (length < 20 || message[16] != 4)
		return false;

	// This seems redundant, but is required for the
	// purposes of extensibility.
	switch (message[17]) {
		case KNXNETIP_CONNECTION_REQUEST_TUNNEL:
			req->type = KNXNETIP_CONNECTION_REQUEST_TUNNEL;
			break;

		default:
			return false;
	}

	// This looks redundant aswell. Same purpose here.
	switch (message[18]) {
		case KNXNETIP_LAYER_TUNNEL:
			req->layer = KNXNETIP_LAYER_TUNNEL;
			break;

		default:
			return false;
	}

	return
		knxnetip_parse_host_info(message, &req->control_host) &&
		knxnetip_parse_host_info(message + 8, &req->tunnel_host);
}
