#include "connres.h"
#include "header.h"
#include "../alloc.h"

#include <string.h>

// Connection Response:
//   Octet 0:     Channel
//   Octet 1:     Status
//   Octet 2-9:   Host info
//   Octet 10-13: Connection response info

// Connection Response Information
//   Octet 0: Structure length
//   Octet 1-3: Unknown

bool knx_append_connection_response(msgbuilder* mb,
                                    const knx_connection_response* res) {
	return
		knx_append_header(mb, KNX_CONNECTION_RESPONSE, 14) &&
		msgbuilder_append(mb, anona(const uint8_t, res->channel, res->status), 2) &&
		knx_append_host_info(mb, &res->host) &&
		msgbuilder_append_single(mb, 4) &&
		msgbuilder_append(mb, res->extended, 3);
}

bool knx_parse_connection_response(const uint8_t* message, size_t length,
                                   knx_connection_response* res) {
	if (length < 2)
		return false;

	res->channel = message[0];
	res->status = message[1];

	if (length >= 10 && !knx_parse_host_info(message + 2, &res->host))
		return false;

	if (length >= 14)
		// TODO: Figure out what the last 4 octets do.
		memcpy(res->extended, message + 11, 3);

	return true;
}
