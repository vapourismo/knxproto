#include "connstateres.h"
#include "header.h"

// Connection State Response:
//   Octet 0: Channel
//   Octet 1: Status

bool knx_append_connection_state_response(msgbuilder* mb,
                                          const knx_connection_state_response* res) {
	return
		knx_append_header(mb, KNX_CONNECTIONSTATE_RESPONSE, 2) &&
		msgbuilder_append(mb, anona(const uint8_t, res->channel, res->status), 2);
}

bool knx_parse_connection_state_response(const uint8_t* message, size_t length,
                                         knx_connection_state_response* res) {
	if (length < 2)
		return false;

	res->channel = message[0];
	res->status = message[1];

	return true;
}
