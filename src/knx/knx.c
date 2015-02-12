#include "connreq.h"
#include "connres.h"
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
		case KNXNETIP_SEARCH_REQUEST:
			packet->service = KNXNETIP_SEARCH_REQUEST;
			return false;

		case KNXNETIP_SEARCH_RESPONSE:
			packet->service = KNXNETIP_SEARCH_RESPONSE;
			return false;

		case KNXNETIP_DESCRIPTION_REQUEST:
			packet->service = KNXNETIP_DESCRIPTION_REQUEST;
			return false;

		case KNXNETIP_DESCRIPTION_RESPONSE:
			packet->service = KNXNETIP_DESCRIPTION_RESPONSE;
			return false;

		case KNXNETIP_CONNECTION_REQUEST:
			packet->service = KNXNETIP_CONNECTION_REQUEST;
			return knxnetip_parse_connection_request(message + 6, claimed_len - 6,
			                                         &packet->payload.conn_req);

		case KNXNETIP_CONNECTION_RESPONSE:
			packet->service = KNXNETIP_CONNECTION_RESPONSE;
			return knxnetip_parse_connection_response(message + 6, claimed_len - 6,
			                                          &packet->payload.conn_res);

		case KNXNETIP_CONNECTIONSTATE_REQUEST:
			packet->service = KNXNETIP_CONNECTIONSTATE_REQUEST;
			return false;

		case KNXNETIP_CONNECTIONSTATE_RESPONSE:
			packet->service = KNXNETIP_CONNECTIONSTATE_RESPONSE;
			return false;

		case KNXNETIP_DISCONNECT_REQUEST:
			packet->service = KNXNETIP_DISCONNECT_REQUEST;
			return knxnetip_parse_disconnect_request(message + 6, claimed_len - 6,
			                                         &packet->payload.dc_req);

		case KNXNETIP_DISCONNECT_RESPONSE:
			packet->service = KNXNETIP_DISCONNECT_RESPONSE;
			return false;

		case KNXNETIP_DEVICE_CONFIGURATION_REQUEST:
			packet->service = KNXNETIP_DEVICE_CONFIGURATION_REQUEST;
			return false;

		case KNXNETIP_DEVICE_CONFIGURATION_ACK:
			packet->service = KNXNETIP_DEVICE_CONFIGURATION_ACK;
			return false;

		case KNXNETIP_TUNNEL_REQUEST:
			packet->service = KNXNETIP_TUNNEL_REQUEST;
			return false;

		case KNXNETIP_TUNNEL_RESPONSE:
			packet->service = KNXNETIP_TUNNEL_RESPONSE;
			return false;

		case KNXNETIP_ROUTING_INDICATION:
			packet->service = KNXNETIP_ROUTING_INDICATION;
			return false;

		default:
			return false;
	}
}
