#include "hostinfo.h"

bool knxnetip_append_host_info(msgbuilder* mb, const knxnetip_host_info* host) {
	const uint8_t preamble[2] = {8, host->protocol};

	return
		msgbuilder_append(mb, preamble, 2) &&
		msgbuilder_append(mb, (const uint8_t*) &host->address, 4) &&
		msgbuilder_append(mb, (const uint8_t*) &host->port, 2);
}
