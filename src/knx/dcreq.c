#include "dcreq.h"
#include "header.h"

// Disconnect Request:
//   Octet 0:   Channel
//   Octet 1:   Status
//   Octet 2-9: Host info

bool knxnetip_append_disconnect_request(msgbuilder* mb,
                                        const knxnetip_disconnect_request* req) {
	const uint8_t info[2] = {req->channel, req->status};

	return
		knxnetip_append_header(mb, KNXNETIP_DISCONNECT_REQUEST, 10) &&
		msgbuilder_append(mb, info, 2) &&
		knxnetip_append_host_info(mb, &req->host);
}

bool knxnetip_parse_disconnect_request(const uint8_t* message, size_t length,
                                       knxnetip_disconnect_request* req) {
	if (length < 10)
		return false;

	req->channel = message[0];
	req->status = message[1];

	return knxnetip_parse_host_info(message + 2, &req->host);
}
