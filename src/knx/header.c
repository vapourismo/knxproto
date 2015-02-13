#include "header.h"
#include <arpa/inet.h>

// Header:
//   Octet 0:   Header length
//   Octet 1:   Protocol version
//   Octet 2-3: Service number
//   Octet 4-5: Packet length including header size

bool knx_append_header(msgbuilder* mb, knx_service srv, uint16_t length) {
	// This preamble will always be there,
	// unless the underlying KNXnet/IP version changes.
	static const uint8_t preamble[2] = {6, 16};

	// Since the protocol specifies the payload length
	// to be a 16-bit unsigned integer, we have to make
	// sure the given length + header size do not exceed
	// the uint16_t bounds.
	if (length > UINT16_MAX - 6)
		return false;

	// Generate a 16-bit unsigned integer (big-endian).
	length += 6;
	const uint8_t length_data[2] = {(length >> 8) & 0xFF, length & 0xFF};

	// Reserve space for future calls to `msgbuilder_append`.
	msgbuilder_reserve(mb, length);

	// Generate service descriptor
	uint16_t u16srv = htons(srv);

	return
		msgbuilder_append(mb, preamble, 2) &&
		msgbuilder_append(mb, (const uint8_t *) &u16srv, 2) &&
		msgbuilder_append(mb, length_data, 2);
}
