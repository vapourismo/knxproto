#include "connstatereq.h"
#include "header.h"

// Connection State Request
//   Octet 0:   Channel
//   Octet 1:   Status
//   Octet 2-9: Host info

bool knxnetip_append_connection_state_request(msgbuilder* mb,
                                              const knxnetip_connection_state_request* req) {
	const uint8_t info[2] = {req->channel, req->status};

	return
		knxnetip_append_header(mb, KNXNETIP_CONNECTIONSTATE_REQUEST, 10) &&
		msgbuilder_append(mb, info, 2) &&
		knxnetip_append_host_info(mb, &req->host);
}

bool knxnetip_parse_connection_state_request(const uint8_t* message, size_t length,
                                             knxnetip_connection_state_request* req) {
	if (length < 10)
		return false;

	req->channel = message[0];
	req->status = message[1];

	return knxnetip_parse_host_info(message + 2, &req->host);
}
