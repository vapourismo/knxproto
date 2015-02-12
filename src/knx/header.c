#include "header.h"

/* A KNXnet/IP frame is a header followed by a payload.
 * The header follows this structure:
 *
 *   Octet 0:   Header length
 *   Octet 1:   Protocol version
 *   Octet 2-3: Service number
 *   Octet 4-5: Packet length including header size
 *
 * All integers are in network byte order (big-endian).
 */

static const uint8_t knxnetip_search_request_data[2]               = {0x02, 0x01};
static const uint8_t knxnetip_search_response_data[2]              = {0x02, 0x02};
static const uint8_t knxnetip_description_request_data[2]          = {0x02, 0x03};
static const uint8_t knxnetip_description_response_data[2]         = {0x02, 0x04};
static const uint8_t knxnetip_connection_request_data[2]           = {0x02, 0x05};
static const uint8_t knxnetip_connection_response_data[2]          = {0x02, 0x06};
static const uint8_t knxnetip_connectionstate_request_data[2]      = {0x02, 0x07};
static const uint8_t knxnetip_connectionstate_response_data[2]     = {0x02, 0x08};
static const uint8_t knxnetip_disconnect_request_data[2]           = {0x02, 0x09};
static const uint8_t knxnetip_disconnect_response_data[2]          = {0x02, 0x0A};
static const uint8_t knxnetip_device_configuration_request_data[2] = {0x03, 0x10};
static const uint8_t knxnetip_device_configuration_ack_data[2]     = {0x03, 0x11};
static const uint8_t knxnetip_tunnel_request_data[2]               = {0x04, 0x20};
static const uint8_t knxnetip_tunnel_response_data[2]              = {0x04, 0x21};
static const uint8_t knxnetip_routing_indication_data[2]           = {0x05, 0x30};

static bool knxnetip_append_service(msgbuilder* mb, knxnetip_service srv) {
	switch (srv) {
		case KNXNETIP_SEARCH_REQUEST:
			return msgbuilder_append(mb, knxnetip_search_request_data, 2);

		case KNXNETIP_SEARCH_RESPONSE:
			return msgbuilder_append(mb, knxnetip_search_response_data, 2);

		case KNXNETIP_DESCRIPTION_REQUEST:
			return msgbuilder_append(mb, knxnetip_description_request_data, 2);

		case KNXNETIP_DESCRIPTION_RESPONSE:
			return msgbuilder_append(mb, knxnetip_description_response_data, 2);

		case KNXNETIP_CONNECTION_REQUEST:
			return msgbuilder_append(mb, knxnetip_connection_request_data, 2);

		case KNXNETIP_CONNECTION_RESPONSE:
			return msgbuilder_append(mb, knxnetip_connection_response_data, 2);

		case KNXNETIP_CONNECTIONSTATE_REQUEST:
			return msgbuilder_append(mb, knxnetip_connectionstate_request_data, 2);

		case KNXNETIP_CONNECTIONSTATE_RESPONSE:
			return msgbuilder_append(mb, knxnetip_connectionstate_response_data, 2);

		case KNXNETIP_DISCONNECT_REQUEST:
			return msgbuilder_append(mb, knxnetip_disconnect_request_data, 2);

		case KNXNETIP_DISCONNECT_RESPONSE:
			return msgbuilder_append(mb, knxnetip_disconnect_response_data, 2);

		case KNXNETIP_DEVICE_CONFIGURATION_REQUEST:
			return msgbuilder_append(mb, knxnetip_device_configuration_request_data, 2);

		case KNXNETIP_DEVICE_CONFIGURATION_ACK:
			return msgbuilder_append(mb, knxnetip_device_configuration_ack_data, 2);

		case KNXNETIP_TUNNEL_REQUEST:
			return msgbuilder_append(mb, knxnetip_tunnel_request_data, 2);

		case KNXNETIP_TUNNEL_RESPONSE:
			return msgbuilder_append(mb, knxnetip_tunnel_response_data, 2);

		case KNXNETIP_ROUTING_INDICATION:
			return msgbuilder_append(mb, knxnetip_routing_indication_data, 2);
	}
}

bool knxnetip_append_header(msgbuilder* mb, knxnetip_service srv, uint16_t length) {
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

	return
		msgbuilder_append(mb, preamble, 2) &&
		knxnetip_append_service(mb, srv) &&
		msgbuilder_append(mb, length_data, 2);
}
