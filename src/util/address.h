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

#ifndef KNXPROTO_UTIL_ADDRESS_H_
#define KNXPROTO_UTIL_ADDRESS_H_

#include <stdint.h>

/**
 * Indivdual Address (16-bit unsigned integer)
 *
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *   | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *   | Area          | Line          | Device                        |
 *   +---------------+---------------+-------------------------------+
 *
 * Group Address (15-bit unsigned integer)
 *
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *   | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *   |   | Main group                | Group                         |
 *   +-------------------+-----------+-------------------------------+
 *   |   | Main group    | Sub group | Group                         |
 *   +-------------------+-----------+-------------------------------+
 *
 * Both address types are assumed to be in host byte order.
 * They are converted to network byte order during frame assembly.
 */
typedef uint16_t knx_addr;

/**
 * Create an Individual Address.
 */
#define knx_individual_addr(area, line, device) \
 	((((area) & 15) << 12) | (((line) & 15) << 8) | ((device) & 255))

/**
 * Create a Group Address.
 */
#define knx_group_addr(main, sub, group) \
	((((main) & 15) << 11) | (((sub) & 7) << 8) | ((group) & 255))

#endif
