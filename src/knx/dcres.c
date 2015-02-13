#include "dcres.h"
#include "header.h"
#include "../alloc.h"

// Disconnect Response:
//   Octet 0: Channel
//   Octet 1: Status

bool knx_append_disconnect_response(msgbuilder* mb,
                                         const knx_disconnect_response* res) {
	return
		knx_append_header(mb, KNX_DISCONNECT_RESPONSE, 2) &&
		msgbuilder_append(mb, anona(const uint8_t, res->channel, res->status), 2);
}

bool knx_parse_disconnect_response(const uint8_t* message, size_t length,
                                        knx_disconnect_response* res) {
	if (length < 2)
		return false;

	res->channel = message[0];
	res->status = message[1];

	return true;
}
