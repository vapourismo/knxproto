#include "knx.h"
#include <string.h>

bool knxnetip_parse(const uint8_t* message, size_t length,
                    knxnetip_packet* packet) {
	if (length < 6 || message[0] != 6 || message[1] != 16)
		return false;

	uint16_t claimed_len = (message[4] << 8) | message[5];
	if (claimed_len > length || claimed_len < 6)
		return false;

	switch ((message[2] << 8) | message[3]) {
		case 0x0201:
			packet->service = KNXNETIP_SEARCH_REQUEST;
			return false;

		case 0x0202:
			packet->service = KNXNETIP_SEARCH_RESPONSE;
			return false;

		case 0x0203:
			packet->service = KNXNETIP_DESCRIPTION_REQUEST;
			return false;

		case 0x0204:
			packet->service = KNXNETIP_DESCRIPTION_RESPONSE;
			return false;

		case 0x0205:
			packet->service = KNXNETIP_CONNECTION_REQUEST;
			return knxnetip_parse_connection_request(message + 6, claimed_len - 6,
			                                         &packet->conn_req);

		case 0x0206:
			packet->service = KNXNETIP_CONNECTION_RESPONSE;
			return false;

		case 0x0207:
			packet->service = KNXNETIP_CONNECTIONSTATE_REQUEST;
			return false;

		case 0x0208:
			packet->service = KNXNETIP_CONNECTIONSTATE_RESPONSE;
			return false;

		case 0x0209:
			packet->service = KNXNETIP_DISCONNECT_REQUEST;
			return false;

		case 0x020A:
			packet->service = KNXNETIP_DISCONNECT_RESPONSE;
			return false;

		case 0x0310:
			packet->service = KNXNETIP_DEVICE_CONFIGURATION_REQUEST;
			return false;

		case 0x0311:
			packet->service = KNXNETIP_DEVICE_CONFIGURATION_ACK;
			return false;

		case 0x0420:
			packet->service = KNXNETIP_TUNNEL_REQUEST;
			return false;

		case 0x0421:
			packet->service = KNXNETIP_TUNNEL_RESPONSE;
			return false;

		case 0x0530:
			packet->service = KNXNETIP_ROUTING_INDICATION;
			return false;

		default:
			return false;
	}
}
