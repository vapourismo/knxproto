#include "hostinfo.h"

bool knxnetip_append_host_info(msgbuilder* mb, const knxnetip_host_info* host) {
	const uint8_t preamble[2] = {8, host->protocol};

	return
		msgbuilder_append(mb, preamble, 2) &&
		msgbuilder_append(mb, (const uint8_t*) &host->address, 4) &&
		msgbuilder_append(mb, (const uint8_t*) &host->port, 2);
}

bool knxnetip_parse_host_info(const uint8_t* message, knxnetip_host_info* host) {
	if (message[0] != 8)
		return false;

	switch (message[1]) {
		case KNXNETIP_PROTO_UDP:
			host->protocol = KNXNETIP_PROTO_UDP;
			break;

		case KNXNETIP_PROTO_TCP:
			host->protocol = KNXNETIP_PROTO_TCP;
			break;

		default:
			return false;
	}

	memcpy(&host->address, message + 2, 4);
	memcpy(&host->port, message + 6, 2);

	return true;
}
