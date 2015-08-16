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

#ifdef KNXCLIENT_TEST_TUNNEL

#include "../src/tunnelclient.h"

static
const uint8_t example_ldata_payload[4] = {0, 11, 22, 33, 44};

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

deftest(tunnelclient, {
	knx_tunnel_client client;
	assert(knx_tunnel_connect(&client, "127.0.0.1", 3671));

	assert(knx_tunnel_send(&client, &example_ldata));

	knx_tunnel_disconnect(&client);
})

#else

deftest(tunnelclient, {
	assert(true);
})

#endif
