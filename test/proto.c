/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014, Ole Krüger <ole@vprsm.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "testfw.h"

#include "../src/proto/knxnetip.h"
#include "../src/util/msgbuilder.h"

#include <stdbool.h>
#include <string.h>


inline static bool host_info_equal(const knx_host_info* a,
                                   const knx_host_info* b) {
	return
		a->address == b->address &&
		a->port == b->port &&
		a->protocol == b->protocol;
}

deftest(knx_connection_request, {
	knx_connection_request packet_in = {
		KNX_CONNECTION_REQUEST_TUNNEL,
		KNX_LAYER_TUNNEL,
		{KNX_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345},
		{KNX_PROTO_UDP, htonl(INADDR_LOOPBACK), 54321}
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knx_append_connection_request(&mb, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNX_CONNECTION_REQUEST);
	assert(packet_out.payload.conn_req.type == packet_in.type);
	assert(packet_out.payload.conn_req.layer == packet_in.layer);
	assert(host_info_equal(&packet_out.payload.conn_req.control_host,
	                       &packet_in.control_host));
	assert(host_info_equal(&packet_out.payload.conn_req.tunnel_host,
	                       &packet_in.tunnel_host));
})

deftest(knx_connection_response, {
	knx_connection_response packet_in = {
		100,
		0,
		{KNX_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345},
		{1, 2, 3}
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knx_append_connection_response(&mb, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNX_CONNECTION_RESPONSE);
	assert(packet_out.payload.conn_res.channel == packet_in.channel);
	assert(packet_out.payload.conn_res.status == packet_in.status);
	assert(host_info_equal(&packet_out.payload.conn_res.host,
	                       &packet_in.host));
})

deftest(knx_disconnect_request, {
	knx_disconnect_request packet_in = {
		100,
		0,
		{KNX_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345}
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knx_append_disconnect_request(&mb, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNX_DISCONNECT_REQUEST);
	assert(packet_out.payload.dc_req.channel == packet_in.channel);
	assert(packet_out.payload.dc_req.status == packet_in.status);
	assert(host_info_equal(&packet_out.payload.dc_req.host,
	                       &packet_in.host));
})

deftest(knx_disconnect_response, {
	knx_disconnect_response packet_in = {
		100,
		0,
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knx_append_disconnect_response(&mb, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNX_DISCONNECT_RESPONSE);
	assert(packet_out.payload.dc_res.channel == packet_in.channel);
	assert(packet_out.payload.dc_res.status == packet_in.status);
})

deftest(knx_connection_state_request, {
	knx_connection_state_request packet_in = {
		100,
		0,
		{KNX_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345}
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knx_append_connection_state_request(&mb, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNX_CONNECTIONSTATE_REQUEST);
	assert(packet_out.payload.conn_state_req.channel == packet_in.channel);
	assert(packet_out.payload.conn_state_req.status == packet_in.status);
	assert(host_info_equal(&packet_out.payload.conn_state_req.host,
	                       &packet_in.host));
})

deftest(knx_connection_state_response, {
	knx_connection_state_response packet_in = {
		100,
		0
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knx_append_connection_state_response(&mb, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNX_CONNECTIONSTATE_RESPONSE);
	assert(packet_out.payload.conn_state_res.channel == packet_in.channel);
	assert(packet_out.payload.conn_state_res.status == packet_in.status);
})

deftest(knx_tunnel_request, {
	const uint8_t example_data[4] = {11, 22, 33, 44};

	knx_tunnel_request packet_in = {
		100,
		0,
		4,
		example_data
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knx_append_tunnel_request(&mb, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNX_TUNNEL_REQUEST);
	assert(packet_out.payload.tunnel_req.channel == packet_in.channel);
	assert(packet_out.payload.tunnel_req.seq_number == packet_in.seq_number);
	assert(packet_out.payload.tunnel_req.size == packet_in.size);
	assert(memcmp(packet_out.payload.tunnel_req.data, packet_in.data, packet_in.size) == 0);
})

deftest(knx_tunnel_response, {
	knx_tunnel_response packet_in = {
		100,
		50,
		0,
	};

	// Generate
	msgbuilder mb;
	msgbuilder_init(&mb, 0);
	assert(knx_append_tunnel_response(&mb, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(mb.buffer, mb.used, &packet_out));

	// Check
	assert(packet_out.service == KNX_TUNNEL_RESPONSE);
	assert(packet_out.payload.tunnel_res.channel == packet_in.channel);
	assert(packet_out.payload.tunnel_res.seq_number == packet_in.seq_number);
	assert(packet_out.payload.tunnel_res.status == packet_in.status);
})

deftest(knxnetip, {
	runsubtest(knx_connection_request);
	runsubtest(knx_connection_response);
	runsubtest(knx_disconnect_request);
	runsubtest(knx_disconnect_response);
	runsubtest(knx_connection_state_request);
	runsubtest(knx_connection_state_response);
	runsubtest(knx_tunnel_request);
	runsubtest(knx_tunnel_response);
})
