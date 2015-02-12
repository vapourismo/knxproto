#include "testfw.h"
#include "../knx/connreq.h"
#include "../knx/connres.h"
#include "../knx/knx.h"
#include "../msgbuilder.h"

deftest(knxnetip_connection_request, {
	knxnetip_connection_request packet_in = {
		KNXNETIP_CONNECTION_REQUEST_TUNNEL,
		KNXNETIP_LAYER_TUNNEL,
		{KNXNETIP_PROTO_UDP, 0, 0},
		{KNXNETIP_PROTO_UDP, 0, 0}
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knxnetip_append_connection_request(&mb, &packet_in));

	// Parse
	knxnetip_packet packet_out;
	assert(knxnetip_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNXNETIP_CONNECTION_REQUEST);
	assert(packet_out.payload.conn_req.type == packet_in.type);
	assert(packet_out.payload.conn_req.layer == packet_in.layer);
	assert(packet_out.payload.conn_req.control_host.protocol == packet_in.control_host.protocol);
	assert(packet_out.payload.conn_req.control_host.address == packet_in.control_host.address);
	assert(packet_out.payload.conn_req.control_host.port == packet_in.control_host.port);
	assert(packet_out.payload.conn_req.tunnel_host.protocol == packet_in.tunnel_host.protocol);
	assert(packet_out.payload.conn_req.tunnel_host.address == packet_in.tunnel_host.address);
	assert(packet_out.payload.conn_req.tunnel_host.port == packet_in.tunnel_host.port);
})

deftest(knxnetip_connection_response, {
	knxnetip_connection_response packet_in = {
		100,
		0,
		{KNXNETIP_PROTO_UDP, 0, 0},
		{1, 2, 3}
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knxnetip_append_connection_response(&mb, &packet_in));

	// Parse
	knxnetip_packet packet_out;
	assert(knxnetip_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNXNETIP_CONNECTION_RESPONSE);
})

deftest(knxnetip, {
	runsubtest(knxnetip_connection_request);
	runsubtest(knxnetip_connection_response);
})
