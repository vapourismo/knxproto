#include "connres.h"
#include "header.h"

#include <string.h>

bool knxnetip_append_connection_response(msgbuilder* mb,
                                         const knxnetip_connection_response* conn_req) {
	const uint8_t data[2] = {conn_req->channel, conn_req->status};

	return
		knxnetip_append_header(mb, KNXNETIP_CONNECTION_RESPONSE, 14) &&
		msgbuilder_append(mb, data, 2) &&
		knxnetip_append_host_info(mb, &conn_req->host) &&
		msgbuilder_append_single(mb, 4) &&
		msgbuilder_append(mb, conn_req->extended, 3);
}

bool knxnetip_parse_connection_response(const uint8_t* message, size_t length,
                                        knxnetip_connection_response* req) {
	if (length < 14 || message[10] != 4)
		return false;

	req->channel = message[0];
	req->status = message[1];

	// TODO: Figure out what the last 4 octets do.
	// So far:
	//   Octet 10:   Structure length
	//   Octet 11-13: Unknown
	memcpy(req->extended, message + 11, 3);

	return knxnetip_parse_host_info(message + 2, &req->host);
}
