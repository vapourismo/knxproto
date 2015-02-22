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

#include "../src/proto/cemi/cemi.h"

#include <stdbool.h>
#include <string.h>

deftest(cemi_ldata_apdu, {
	knx_ldata req;
	req.control1 = 0x8C;
	req.control2 = 0xF0;
	req.source = 0xFFCA;
	req.destination = 0x7FFF;
	req.tpci = KNX_LDATA_TPCI_UNNUMBERED_DATA;

	uint32_t val = 0xBADC0DE;
	req.payload.apdu.apci = KNX_LDATA_APCI_GROUPVALUEWRITE;
	req.payload.apdu.data = (const uint8_t*) &val;
	req.payload.apdu.length_over_6bit = sizeof(val);

	// Generate
	uint8_t buffer[KNX_CEMI_HEADER_SIZE + knx_ldata_size(&req)];
	assert(knx_cemi_generate_(buffer, KNX_CEMI_LDATA_REQ, &req));

	// Parse
	knx_cemi_frame frame;
	assert(knx_cemi_parse(buffer, sizeof(buffer), &frame));

	// Check
	assert(frame.service == KNX_CEMI_LDATA_REQ);
	assert(frame.add_info_length == 0);

	assert(frame.payload.ldata.control1 == req.control1);
	assert(frame.payload.ldata.control2 == req.control2);
	assert(frame.payload.ldata.source == req.source);
	assert(frame.payload.ldata.destination == req.destination);
	assert(frame.payload.ldata.tpci == req.tpci);
	assert(frame.payload.ldata.payload.apdu.apci == req.payload.apdu.apci);
	assert(frame.payload.ldata.payload.apdu.length_over_6bit == req.payload.apdu.length_over_6bit);
	assert(memcmp(frame.payload.ldata.payload.apdu.data,
	              req.payload.apdu.data, sizeof(val)) == 0);
})

deftest(cemi_ldata_control, {
	knx_ldata req;
	req.control1 = 0x8C;
	req.control2 = 0xF0;
	req.source = 0xFFCA;
	req.destination = 0x7FFF;
	req.tpci = KNX_LDATA_TPCI_UNNUMBERED_CONTROL;
	req.payload.control = KNX_LDATA_CONTROL_ERROR;

	// Generate
	uint8_t buffer[KNX_CEMI_HEADER_SIZE + knx_ldata_size(&req)];
	assert(knx_cemi_generate_(buffer, KNX_CEMI_LDATA_IND, &req));

	// Parse
	knx_cemi_frame frame;
	assert(knx_cemi_parse(buffer, sizeof(buffer), &frame));

	// Check
	assert(frame.service == KNX_CEMI_LDATA_IND);
	assert(frame.add_info_length == 0);

	assert(frame.payload.ldata.control1 == req.control1);
	assert(frame.payload.ldata.control2 == req.control2);
	assert(frame.payload.ldata.source == req.source);
	assert(frame.payload.ldata.destination == req.destination);
	assert(frame.payload.ldata.tpci == req.tpci);
	assert(frame.payload.ldata.payload.control == req.payload.control);
})

deftest(cemi, {
	runsubtest(cemi_ldata_apdu);
	runsubtest(cemi_ldata_control);
})
