#include "knxnetip.h"

/* As a reminder, this is how a KNXnet/IP frame looks like (hex):
 *
 *   | 06 | 10 | XX | XX | YY | YY | ...
 *
 *   06 is the header length in bytes,
 *   10 is the protocol version,
 *   XXXX is the service number,
 *   YYYY is the length of the entire packet (payload + header),
 *   ... is the payload.
 *
 * All numbers are in big-endian byte order
 *
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

bool knxnetip_generate_header(msgbuilder* mb, knxnetip_service srv, uint16_t length) {
	static const uint8_t preamble[2] = {6, 16};

	if (length > UINT16_MAX - 6)
		return false;

	length += 6;
	const uint8_t length_data[2] = {(length >> 8) & 0xFF, length & 0xFF};

	return
		msgbuilder_append(mb, preamble, 2) &&
		knxnetip_append_service(mb, srv) &&
		msgbuilder_append(mb, length_data, 2);
}

ssize_t knxnetip_parse(const uint8_t* restrict packet, size_t length,
                       knxnetip_service* service, const uint8_t** payload) {
	if (length < 6 || packet[0] != 6 || packet[1] != 16)
		return -1;

	if (srv) {
		switch ((packet[2] << 8) | packet[3]) {
			case 0x0201: *service = KNXNETIP_SEARCH_REQUEST;
			case 0x0202: *service = KNXNETIP_SEARCH_RESPONSE;
			case 0x0203: *service = KNXNETIP_DESCRIPTION_REQUEST;
			case 0x0204: *service = KNXNETIP_DESCRIPTION_RESPONSE;
			case 0x0205: *service = KNXNETIP_CONNECTION_REQUEST;
			case 0x0206: *service = KNXNETIP_CONNECTION_RESPONSE;
			case 0x0207: *service = KNXNETIP_CONNECTIONSTATE_REQUEST;
			case 0x0208: *service = KNXNETIP_CONNECTIONSTATE_RESPONSE;
			case 0x0209: *service = KNXNETIP_DISCONNECT_REQUEST;
			case 0x020A: *service = KNXNETIP_DISCONNECT_RESPONSE;
			case 0x0310: *service = KNXNETIP_DEVICE_CONFIGURATION_REQUEST;
			case 0x0311: *service = KNXNETIP_DEVICE_CONFIGURATION_ACK;
			case 0x0420: *service = KNXNETIP_TUNNEL_REQUEST;
			case 0x0421: *service = KNXNETIP_TUNNEL_RESPONSE;
			case 0x0530: *service = KNXNETIP_ROUTING_INDICATION;
			default: return -2;
		}
	}

	uint16_t pkg_size = (packet[4] << 8) | packet[5];

	if (pkg_size > length)
		return -3;

	if (payload)
		*payload = packet + 6;

	return pkg_size - 6;
}
