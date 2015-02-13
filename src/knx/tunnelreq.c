#include "tunnelreq.h"
#include "header.h"

// Tunnel Request:
//   Octet 0:   Structure length
//   Octet 1:   Channel
//   Octet 2:   Sequence number
//   Octet 3:   Reserved
//   Octet 4-n: Payload

bool knxnetip_append_tunnel_request(msgbuilder* mb,
                                    const knxnetip_tunnel_request* req) {
	const uint8_t info[4] = {4, req->channel, req->seq_number, 0};

	// Prevent integer overflow
	// Why 10? To prevent cases where `knxnetip_append_header`
	// would double-check (req->size + 4 > UINT16_MAX - 6)
	if (req->size > UINT16_MAX - 10)
		return false;

	return
		knxnetip_append_header(mb, KNXNETIP_TUNNEL_REQUEST, 4 + req->size) &&
		msgbuilder_append(mb, info, 4) &&
		msgbuilder_append(mb, req->data, req->size);
}

bool knxnetip_parse_tunnel_request(const uint8_t* message, size_t length,
                                   knxnetip_tunnel_request* req) {
	if (length < 4 || message[0] != 4)
		return false;

	req->channel = message[1];
	req->seq_number = message[2];
	req->size = length - 4;
	req->data = message + 4;

	return true;
}
