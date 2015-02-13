#include "tunnelreq.h"
#include "header.h"
#include "../alloc.h"

// Tunnel Request:
//   Octet 0:   Structure length
//   Octet 1:   Channel
//   Octet 2:   Sequence number
//   Octet 3:   Reserved
//   Octet 4-n: Payload

bool knx_append_tunnel_request(msgbuilder* mb,
                                    const knx_tunnel_request* req) {
	// Prevent integer overflow
	// Why 10? To prevent cases where `knx_append_header`
	// would double-check (req->size + 4 > UINT16_MAX - 6)
	if (req->size > UINT16_MAX - 10)
		return false;

	return
		knx_append_header(mb, KNX_TUNNEL_REQUEST, 4 + req->size) &&
		msgbuilder_append(mb, anona(const uint8_t, 4, req->channel, req->seq_number, 0), 4) &&
		msgbuilder_append(mb, req->data, req->size);
}

bool knx_parse_tunnel_request(const uint8_t* message, size_t length,
                                   knx_tunnel_request* req) {
	if (length < 4 || message[0] != 4)
		return false;

	req->channel = message[1];
	req->seq_number = message[2];
	req->size = length - 4;
	req->data = message + 4;

	return true;
}
