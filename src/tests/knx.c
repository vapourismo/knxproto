#include "testfw.h"
#include "../knx/knx.h"
#include "../msgbuilder.h"

#include <stdbool.h>

inline static bool host_info_equal(const knxnetip_host_info* a,
                                   const knxnetip_host_info* b) {
	return
		a->address == b->address &&
		a->port == b->port &&
		a->protocol == b->protocol;
}

deftest(knxnetip_connection_request, {
	knxnetip_connection_request packet_in = {
		KNXNETIP_CONNECTION_REQUEST_TUNNEL,
		KNXNETIP_LAYER_TUNNEL,
		{KNXNETIP_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345},
		{KNXNETIP_PROTO_UDP, htonl(INADDR_LOOPBACK), 54321}
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
	assert(host_info_equal(&packet_out.payload.conn_req.control_host,
	                       &packet_in.control_host));
	assert(host_info_equal(&packet_out.payload.conn_req.tunnel_host,
	                       &packet_in.tunnel_host));
})

deftest(knxnetip_connection_response, {
	knxnetip_connection_response packet_in = {
		100,
		0,
		{KNXNETIP_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345},
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
	assert(packet_out.payload.conn_res.channel == packet_in.channel);
	assert(packet_out.payload.conn_res.status == packet_in.status);
	assert(host_info_equal(&packet_out.payload.conn_res.host,
	                       &packet_in.host));
})

deftest(knxnetip_disconnect_request, {
	knxnetip_disconnect_request packet_in = {
		100,
		0,
		{KNXNETIP_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345}
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knxnetip_append_disconnect_request(&mb, &packet_in));

	// Parse
	knxnetip_packet packet_out;
	assert(knxnetip_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNXNETIP_DISCONNECT_REQUEST);
	assert(packet_out.payload.dc_req.channel == packet_in.channel);
	assert(packet_out.payload.dc_req.status == packet_in.status);
	assert(host_info_equal(&packet_out.payload.dc_req.host,
	                       &packet_in.host));
})

deftest(knxnetip_disconnect_response, {
	knxnetip_disconnect_response packet_in = {
		100,
		0,
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knxnetip_append_disconnect_response(&mb, &packet_in));

	// Parse
	knxnetip_packet packet_out;
	assert(knxnetip_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNXNETIP_DISCONNECT_RESPONSE);
	assert(packet_out.payload.dc_res.channel == packet_in.channel);
	assert(packet_out.payload.dc_res.status == packet_in.status);
})

deftest(knxnetip_connection_state_request, {
	knxnetip_connection_state_request packet_in = {
		100,
		0,
		{KNXNETIP_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345}
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knxnetip_append_connection_state_request(&mb, &packet_in));

	// Parse
	knxnetip_packet packet_out;
	assert(knxnetip_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNXNETIP_CONNECTIONSTATE_REQUEST);
	assert(packet_out.payload.conn_state_req.channel == packet_in.channel);
	assert(packet_out.payload.conn_state_req.status == packet_in.status);
	assert(host_info_equal(&packet_out.payload.conn_state_req.host,
	                       &packet_in.host));
})

deftest(knxnetip_connection_state_response, {
	knxnetip_connection_state_response packet_in = {
		100,
		0
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knxnetip_append_connection_state_response(&mb, &packet_in));

	// Parse
	knxnetip_packet packet_out;
	assert(knxnetip_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNXNETIP_CONNECTIONSTATE_RESPONSE);
	assert(packet_out.payload.conn_state_res.channel == packet_in.channel);
	assert(packet_out.payload.conn_state_res.status == packet_in.status);
})

deftest(knxnetip, {
	runsubtest(knxnetip_connection_request);
	runsubtest(knxnetip_connection_response);
	runsubtest(knxnetip_disconnect_request);
	runsubtest(knxnetip_disconnect_response);
	runsubtest(knxnetip_connection_state_request);
	runsubtest(knxnetip_connection_state_response);
})
