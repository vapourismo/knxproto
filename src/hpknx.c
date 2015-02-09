#include "tests/testfw.h"
#include "knx/knxnetip.h"
#include "net/udp.h"
#include "net/address.h"

#include <stdio.h>
#include <string.h>

externtest(msgbuilder)

deftest(knxnetip_connreq, {
	knxnetip_connreq packet_in;

	// Clear the empty padding
	memset(&packet_in.control_host.address.sin_zero, 0, sizeof(packet_in.control_host.address.sin_zero));
	memset(&packet_in.tunnel_host.address.sin_zero, 0, sizeof(packet_in.tunnel_host.address.sin_zero));

	// Setup structure
	packet_in.type = KNXNETIP_CONNREQ_TUNNEL;
	packet_in.layer = KNXNETIP_LAYER_TUNNEL;
	packet_in.control_host.protocol = KNXNETIP_PROTO_UDP;
	packet_in.tunnel_host.protocol = KNXNETIP_PROTO_UDP;

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knxnetip_generate_connreq(&mb, &packet_in));

	// Parse
	knxnetip_packet packet_out;
	assert(knxnetip_parse(mb.buffer, mb.used, &packet_out));
	assert(packet_out.service == KNXNETIP_CONNECTION_REQUEST);
	assert(memcmp(&packet_out.connection_request,
	              &packet_in,
	              sizeof(packet_in)) == 0);
})

int main(void) {
	runtest(msgbuilder);
	runtest(knxnetip_connreq);

	return 0;
}
