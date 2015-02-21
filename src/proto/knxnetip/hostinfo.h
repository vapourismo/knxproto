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

#ifndef KNXCLIENT_PROTO_KNXNETIP_HOSTINFO_H
#define KNXCLIENT_PROTO_KNXNETIP_HOSTINFO_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * KNXnet/IP Protocol
 */
typedef enum {
	KNX_PROTO_UDP = 1,
	KNX_PROTO_TCP = 2
} knx_proto;

/**
 * Host Information
 */
typedef struct {
	knx_proto protocol;
	in_addr_t address;
	in_port_t port;
} knx_host_info;

/**
 *
 */
#define KNX_HOST_INFO_NAT(prot) {prot, 0, 0}

/**
 * Append host information.
 */
void knx_host_info_generate(uint8_t* buffer, const knx_host_info* host);

/**
 * Retrieve host information.
 */
bool knx_parse_host_info(const uint8_t* message, knx_host_info* host);

/**
 * Size of a host info segment
 */
#define KNX_HOST_INFO_SIZE 8

#endif
