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

#ifndef KNXCLIENT_PROTO_CEMI_TPDU_H
#define KNXCLIENT_PROTO_CEMI_TPDU_H

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

#endif
