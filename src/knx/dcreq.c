#include "dcreq.h"
#include "header.h"
#include "../alloc.h"

// Disconnect Request:
//   Octet 0:   Channel
//   Octet 1:   Status
//   Octet 2-9: Host info

bool knx_append_disconnect_request(msgbuilder* mb,
                                   const knx_disconnect_request* req) {
	return
		knx_append_header(mb, KNX_DISCONNECT_REQUEST, 10) &&
		msgbuilder_append(mb, anona(const uint8_t, req->channel, req->status), 2) &&
		knx_append_host_info(mb, &req->host);
}

bool knx_parse_disconnect_request(const uint8_t* message, size_t length,
                                  knx_disconnect_request* req) {
	if (length < 10)
		return false;

	req->channel = message[0];
	req->status = message[1];

	return knx_parse_host_info(message + 2, &req->host);
}
