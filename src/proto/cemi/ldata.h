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

#ifndef KNXCLIENT_PROTO_CEMI_LDATA_H
#define KNXCLIENT_PROTO_CEMI_LDATA_H

#include "../../util/address.h"

#include <stdbool.h>

/**
 * Transport Protocol Control Information
 */
typedef enum {
	KNX_LDATA_TPCI_UNNUMBERED_DATA    = 0,
	KNX_LDATA_TPCI_NUMBERED_DATA      = 1,
	KNX_LDATA_TPCI_UNNUMBERED_CONTROL = 2,
	KNX_LDATA_TPCI_NUMBERED_CONTROL   = 3
} knx_ldata_tpci;

/**
 * Control codes used when TPCI set to control mode
 */
typedef enum {
	KNX_LDATA_CONTROL_CONNECTED    = 0,
	KNX_LDATA_CONTROL_DISCONNECTED = 1,
	KNX_LDATA_CONTROL_ACK          = 2,
	KNX_LDATA_CONTROL_ERROR        = 3
} knx_ldata_ctrl_code;

/**
 * Application Protocol Control Information
 */
typedef enum {
	KNX_LDATA_APCI_GROUPVALUEREAD         = 0,
	KNX_LDATA_APCI_GROUPVALUERESPONSE     = 1,
	KNX_LDATA_APCI_GROUPVALUEWRITE        = 2,
	KNX_LDATA_APCI_INDIVIDUALADDRWRITE    = 3,
	KNX_LDATA_APCI_INDIVIDUALADDRREQUEST  = 4,
	KNX_LDATA_APCI_INDIVIDUALADDRRESPONSE = 5,
	KNX_LDATA_APCI_ADCREAD                = 6,
	KNX_LDATA_APCI_ADCRESPONSE            = 7,
	KNX_LDATA_APCI_MEMORYREAD             = 8,
	KNX_LDATA_APCI_MEMORYRESPONSE         = 9,
	KNX_LDATA_APCI_MEMORYWRITE            = 10,
	KNX_LDATA_APCI_USERMESSAGE            = 11,
	KNX_LDATA_APCI_MASKVERSIONREAD        = 12,
	KNX_LDATA_APCI_MASKVERSIONRESPONSE    = 13,
	KNX_LDATA_APCI_RESTART                = 14,
	KNX_LDATA_APCI_ESCAPE                 = 15
} knx_ldata_apci;

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
	 * Transport information
	 */
	knx_ldata_tpci tpci;

	/**
	 * Sequence number (only for numbered communication)
	 */
	unsigned seq_number:4;

	/**
	 * Based on the TPCI this contains either the application data or
	 * control information
	 */
	union {
		/**
		 * Control information
		 */
		knx_ldata_ctrl_code control;

		/**
		 * Application data
		 */
		struct {
			/**
			 * How to interpret the data
			 */
			knx_ldata_apci apci;

			/**
			 * Data length in bytes; if set to 0 it means 6 bits of data exist (unless data is NULL)
			 */
			uint8_t length_over_6bit;

			/**
			 * Actual application data (set to NULL if there is no data)
			 */
			const uint8_t* data;
		} apdu;
	} payload;
} knx_ldata;

/**
 * Generate an L_Data frame.
 */
void knx_ldata_generate(uint8_t* buffer, const knx_ldata* req);

/**
 * Parse a message containing a L_Data frame.
 * Note: If the frame contains less or equal to 6 bits of application data
 * it will modify the buffer in order to avoid allocation of extraneous memory.
 */
bool knx_ldata_parse(uint8_t* buffer, size_t length, knx_ldata* output);

/**
 * Calculate the required space for the given L_Data frame.
 */
size_t knx_ldata_size(const knx_ldata* req);

#endif
