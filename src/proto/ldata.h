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

#ifndef KNXPROTO_PROTO_LDATA_H_
#define KNXPROTO_PROTO_LDATA_H_

#include "tpdu.h"

#include "../util/address.h"

#include <stdbool.h>

/**
 * L_Data Priority
 */
typedef enum {
	KNX_LDATA_PRIO_SYSTEM = 0,
	KNX_LDATA_PRIO_NORMAL = 1,
	KNX_LDATA_PRIO_URGENT = 2,
	KNX_LDATA_PRIO_LOW    = 3
} knx_ldata_prio;

/**
 * L_Data Destination Address Type
 */
typedef enum {
	KNX_LDATA_ADDR_INDIVIDUAL = 0,
	KNX_LDATA_ADDR_GROUP      = 1,
} knx_ldata_addr_type;

/**
 * L_Data Frame
 */
typedef struct {
	/**
	 * Represents Control Field 1
	 */
	struct {
		/**
		 * Frame priority
		 */
		knx_ldata_prio priority;

		/**
		 * Repeat in case of an error
		 */
		bool repeat;

		/**
		 * Perform system broadcast
		 */
		bool system_broadcast;

		/**
		 * Request acknowledgement (only L_Data.req)
		 */
		bool request_ack;

		/**
		 * Indicate an error (only L_Data.con)
		 */
		bool error;
	} control1;

	/**
	 * Represents Control Field 1
	 */
	struct {
		/**
		 * Type of `desination` address
		 */
		knx_ldata_addr_type address_type;

		/**
		 * Routing hop count in range from 0 to 7 (7 means infinite hops)
		 */
		unsigned hops:4;
	} control2;

	/**
	 * Originating device (set to 0 if you want it to be filled in automatically)
	 */
	knx_addr source;

	/**
	 * Destination device or group
	 */
	knx_addr destination;

	/**
	 * Transport data unit
	 */
	knx_tpdu tpdu;
} knx_ldata;

/**
 * Generate a raw L_Data frame.
 *
 * \see knx_ldata_size
 * \param buffer Output buffer, you have to make sure there is enough space
 * \param ldata  Input L_Data frame
 */
bool knx_ldata_generate(uint8_t* buffer, const knx_ldata* ldata);

/**
 * Parse a raw L_Data frame.
 *
 * \param buffer        Raw frame
 * \param buffer_length Number of bytes in `buffer`
 * \param output        Ouput frame
 * \returns `true` if parsing was successful, otherwise `false`
 */
bool knx_ldata_parse(const uint8_t* buffer, size_t buffer_length, knx_ldata* output);

/**
 * Calculate the required space for the given L_Data frame.
 */
size_t knx_ldata_size(const knx_ldata* req);

/**
 * Duplicate the L_Data structure including the data it might refer to.
 */
knx_ldata* knx_ldata_duplicate(const knx_ldata* data);

#endif
