/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014-2015, Ole Krüger <ole@vprsm.de>
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

#include "../src/proto/proto.h"

#include <stdbool.h>
#include <string.h>

static
const uint8_t example_ldata_payload[5] = {0, 11, 22, 33, 44};

static
const knx_ldata example_ldata = {
	.control1 = {KNX_LDATA_PRIO_LOW, true, true, true, false},
	.control2 = {KNX_LDATA_ADDR_GROUP, 7},
	.source = 123,
	.destination = 456,
	.tpdu = {
		.tpci = KNX_TPCI_UNNUMBERED_DATA,
		.info = {
			.data = {
				.apci = KNX_APCI_GROUPVALUEWRITE,
				.payload = example_ldata_payload,
				.length = sizeof(example_ldata_payload)
			}
		}
	}
};

inline static bool host_info_equal(const knx_host_info* a,
                                   const knx_host_info* b) {
	return
		a->address == b->address &&
		a->port == b->port &&
		a->protocol == b->protocol;
}

deftest(knx_connection_request, {
	const knx_connection_request packet_in = {
		KNX_CONNECTION_REQUEST_TUNNEL,
		KNX_CONNECTION_LAYER_TUNNEL,
		{KNX_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345},
		{KNX_PROTO_UDP, htonl(INADDR_LOOPBACK), 54321}
	};

	// Generate
	uint8_t buffer[KNX_HEADER_SIZE + KNX_CONNECTION_REQUEST_SIZE];
	assert(knx_generate(buffer, KNX_CONNECTION_REQUEST, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

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
	uint8_t buffer[KNX_HEADER_SIZE + knx_connection_response_size(&packet_in)];
	assert(knx_generate(buffer, KNX_CONNECTION_RESPONSE, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

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
	uint8_t buffer[KNX_HEADER_SIZE + KNX_DISCONNECT_REQUEST_SIZE];
	assert(knx_generate(buffer, KNX_DISCONNECT_REQUEST, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

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
	uint8_t buffer[KNX_HEADER_SIZE + KNX_DISCONNECT_RESPONSE_SIZE];
	assert(knx_generate(buffer, KNX_DISCONNECT_RESPONSE, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

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
	uint8_t buffer[KNX_HEADER_SIZE + KNX_CONNECTION_STATE_REQUEST_SIZE];
	assert(knx_generate(buffer, KNX_CONNECTION_STATE_REQUEST, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

	// Check
	assert(packet_out.service == KNX_CONNECTION_STATE_REQUEST);
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
	uint8_t buffer[KNX_HEADER_SIZE + KNX_CONNECTION_STATE_RESPONSE_SIZE];
	assert(knx_generate(buffer, KNX_CONNECTION_STATE_RESPONSE, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

	// Check
	assert(packet_out.service == KNX_CONNECTION_STATE_RESPONSE);
	assert(packet_out.payload.conn_state_res.channel == packet_in.channel);
	assert(packet_out.payload.conn_state_res.status == packet_in.status);
})

deftest(knx_tunnel_request, {
	knx_tunnel_request packet_in = {
		100,
		0,
		{
			KNX_CEMI_LDATA_REQ,
			0,
			NULL,
			{
				.ldata = example_ldata
			}
		}
	};

	// Generate
	uint8_t buffer[KNX_HEADER_SIZE + knx_tunnel_request_size(&packet_in)];
	assert(knx_generate(buffer, KNX_TUNNEL_REQUEST, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

	// Check
	assert(packet_out.service == KNX_TUNNEL_REQUEST);
	assert(packet_out.payload.tunnel_req.channel == packet_in.channel);
	assert(packet_out.payload.tunnel_req.seq_number == packet_in.seq_number);

	// TODO: Verify data (might not be necessary)
})

deftest(knx_tunnel_response, {
	knx_tunnel_response packet_in = {
		100,
		50,
		0,
	};

	// Generate
	uint8_t buffer[KNX_HEADER_SIZE + KNX_TUNNEL_RESPONSE_SIZE];
	assert(knx_generate(buffer, KNX_TUNNEL_RESPONSE, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

	// Check
	assert(packet_out.service == KNX_TUNNEL_RESPONSE);
	assert(packet_out.payload.tunnel_res.channel == packet_in.channel);
	assert(packet_out.payload.tunnel_res.seq_number == packet_in.seq_number);
	assert(packet_out.payload.tunnel_res.status == packet_in.status);
})

// deftest(knx_routing_indication, {
// 	assert(true);
// })

deftest(knx_description_request, {
	knx_description_request packet_in = {
		{KNX_PROTO_UDP, htonl(INADDR_LOOPBACK), 12345}
	};

	// Generate
	uint8_t buffer[KNX_HEADER_SIZE + KNX_DESCRIPTION_REQUEST_SIZE];
	assert(knx_generate(buffer, KNX_DESCRIPTION_REQUEST, &packet_in));

	// Parse
	knx_packet packet_out;
	assert(knx_parse(buffer, sizeof(buffer), &packet_out) > KNX_HEADER_SIZE);

	// Check
	assert(packet_out.service == KNX_DESCRIPTION_REQUEST);
	assert(host_info_equal(&packet_out.payload.description_req.control_host, &packet_in.control_host));
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
	// runsubtest(knx_routing_indication);
	runsubtest(knx_description_request);
})
