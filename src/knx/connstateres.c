#include "connstateres.h"
#include "header.h"

// Connection State Response:
//   Octet 0: Channel
//   Octet 1: Status

bool knxnetip_append_connection_state_response(msgbuilder* mb,
                                               const knxnetip_connection_state_response* res) {
	return
		knxnetip_append_header(mb, KNXNETIP_CONNECTIONSTATE_RESPONSE, 2) &&
		msgbuilder_append(mb, anona(const uint8_t, res->channel, res->status), 2);
}

bool knxnetip_parse_connection_state_response(const uint8_t* message, size_t length,
                                              knxnetip_connection_state_response* res) {
	if (length < 2)
		return false;

	res->channel = message[0];
	res->status = message[1];

	return true;
}
