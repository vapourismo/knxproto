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

#ifndef KNXPROTO_PROTO_HOSTINFO_H_
#define KNXPROTO_PROTO_HOSTINFO_H_

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * KNXnet/IP Protocol
 */
typedef enum {
	KNX_PROTO_UDP = 1,

	/**
	 * \note For reference only
	 */
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
 * Initialize for `knx_host_info` in NAT mode
 */
#define KNX_HOST_INFO_NAT(prot) {prot, 0, 0}

/**
 * Generate raw host information.
 *
 * \see KNX_HOST_INFO_SIZE
 * \param buffer Output buffer, you have to make sure there is enough space
 * \param host   Input host information
 */
void knx_host_info_generate(uint8_t* buffer, const knx_host_info* host);

/**
 * Retrieve host information.
 *
 * \param message        Raw host information
 * \param message_length Number of bytes in `message`
 * \param host           Output host information
 * \returns `true` if parsing was successful, otherwise `false`
 */
bool knx_host_info_parse(const uint8_t* message, size_t message_length, knx_host_info* host);

/**
 * Size of a host info segment
 */
#define KNX_HOST_INFO_SIZE 8

#endif
