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

#include "../src/proto/cemi/cemi.h"

#include <stdbool.h>
#include <string.h>

deftest(cemi_ldata, {
	knx_ldata req;
	req.control1.priority = KNX_LDATA_PRIO_LOW;
	req.control1.repeat = true;
	req.control1.system_broadcast = true;
	req.control1.request_ack = true;
	req.control1.error = false;
	req.control2.address_type = KNX_LDATA_ADDR_GROUP;
	req.control2.hops = 7;
	req.source = 0xFFCA;
	req.destination = 0x7FFF;

	uint8_t tpdu[] = {0x0, 0x80, 0xBA, 0xDC, 0x0D, 0xE};
	req.tpdu = tpdu;
	req.length = sizeof(tpdu);

	// Generate
	uint8_t buffer[KNX_CEMI_HEADER_SIZE + knx_ldata_size(&req)];
	assert(knx_cemi_generate_(buffer, KNX_CEMI_LDATA_REQ, &req));

	// Parse
	knx_cemi_frame frame;
	assert(knx_cemi_parse(buffer, sizeof(buffer), &frame));

	// Check
	assert(frame.service == KNX_CEMI_LDATA_REQ);
	assert(frame.add_info_length == 0);

	assert(frame.payload.ldata.control1.priority == req.control1.priority);
	assert(frame.payload.ldata.control1.repeat == req.control1.repeat);
	assert(frame.payload.ldata.control1.system_broadcast == req.control1.system_broadcast);
	assert(frame.payload.ldata.control1.request_ack == req.control1.request_ack);
	assert(frame.payload.ldata.control1.error == req.control1.error);
	assert(frame.payload.ldata.control2.address_type == req.control2.address_type);
	assert(frame.payload.ldata.control2.hops == req.control2.hops);
	assert(frame.payload.ldata.source == req.source);
	assert(frame.payload.ldata.destination == req.destination);
	assert(frame.payload.ldata.length == req.length);
	assert(memcmp(frame.payload.ldata.tpdu, req.tpdu, req.length) == 0);
})

deftest(cemi, {
	runsubtest(cemi_ldata);
})
