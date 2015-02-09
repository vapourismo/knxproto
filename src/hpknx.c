#include "tests/testfw.h"
#include "knx/knxnetip.h"

#include <stdio.h>
#include <string.h>

externtest(msgbuilder)

static void msgbuilder_debug(const msgbuilder* mb) {
	for (size_t i = 0; i < mb->used; i++) {
		printf("%02X ", mb->buffer[i]);
	}

	putchar('\n');
}

int main(void) {
	runtest(msgbuilder);

	knxnetip_packet packet;

	packet.service = KNXNETIP_CONNECTION_REQUEST;
	packet.payload.connection_request.type = KNXNETIP_CONNREQ_TUNNEL;
	packet.payload.connection_request.layer = KNXNETIP_LAYER_TUNNEL;

	memset(&packet.payload.connection_request.control_host, 0,
	       sizeof(packet.payload.connection_request.control_host));
	memset(&packet.payload.connection_request.tunnel_host, 0,
	       sizeof(packet.payload.connection_request.tunnel_host));

	packet.payload.connection_request.control_host.protocol = KNXNETIP_PROTO_UDP;
	packet.payload.connection_request.tunnel_host.protocol = KNXNETIP_PROTO_UDP;

	msgbuilder mb;
	knxnetip_generate(&mb, &packet);
	msgbuilder_debug(&mb);

	return 0;
}
