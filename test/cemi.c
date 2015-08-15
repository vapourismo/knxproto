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

#include "../src/proto/cemi.h"

#include <stdbool.h>
#include <string.h>

deftest(cemi, {
	uint8_t example_Data[4] = {11, 22, 33, 44};

	knx_cemi req = {
		KNX_CEMI_LDATA_REQ,
		0,
		NULL,
		{
			.ldata = {
				.control1 = {KNX_LDATA_PRIO_LOW, true, true, true, false},
				.control2 = {KNX_LDATA_ADDR_GROUP, 7},
				.source = 123,
				.destination = 456,
				.tpdu = {
					.tpci = KNX_TPCI_UNNUMBERED_DATA,
					.info = {
						.data = {
							.apci = KNX_APCI_GROUPVALUEWRITE,
							.payload = example_Data,
							.length = sizeof(example_Data)
						}
					}
				}
			}
		}
	};

	// Generate
	uint8_t buffer[knx_cemi_size(&req)];
	assert(knx_cemi_generate(buffer, &req));

	// Parse
	knx_cemi frame;
	assert(knx_cemi_parse(buffer, sizeof(buffer), &frame));

	// Check
	assert(frame.service == req.service);
	assert(frame.add_info_length == req.add_info_length);

	assert(frame.payload.ldata.control1.priority == req.payload.ldata.control1.priority);
	assert(frame.payload.ldata.control1.repeat == req.payload.ldata.control1.repeat);
	assert(frame.payload.ldata.control1.system_broadcast == req.payload.ldata.control1.system_broadcast);
	assert(frame.payload.ldata.control1.request_ack == req.payload.ldata.control1.request_ack);
	assert(frame.payload.ldata.control1.error == req.payload.ldata.control1.error);
	assert(frame.payload.ldata.control2.address_type == req.payload.ldata.control2.address_type);
	assert(frame.payload.ldata.control2.hops == req.payload.ldata.control2.hops);
	assert(frame.payload.ldata.source == req.payload.ldata.source);
	assert(frame.payload.ldata.destination == req.payload.ldata.destination);

	assert(frame.payload.ldata.tpdu.tpci == req.payload.ldata.tpdu.tpci);
	assert(frame.payload.ldata.tpdu.seq_number == req.payload.ldata.tpdu.seq_number);
	assert(frame.payload.ldata.tpdu.info.data.apci == req.payload.ldata.tpdu.info.data.apci);
	assert(frame.payload.ldata.tpdu.info.data.length == req.payload.ldata.tpdu.info.data.length);

	// We ignore the first byte, because it contains part of the APCI
	assert(memcmp(frame.payload.ldata.tpdu.info.data.payload + 1,
	              req.payload.ldata.tpdu.info.data.payload + 1, req.payload.ldata.tpdu.info.data.length - 1) == 0);
})
