#include "connreq.h"
#include "connres.h"
#include "knx.h"

#include <string.h>

bool knx_parse(const uint8_t* message, size_t length,
                    knx_packet* packet) {
	if (length < 6 || message[0] != 6 || message[1] != 16)
		return false;

	uint16_t claimed_len = (message[4] << 8) | message[5];
	if (claimed_len > length || claimed_len < 6)
		return false;

	switch ((message[2] << 8) | message[3]) {
		case KNX_SEARCH_REQUEST:
			packet->service = KNX_SEARCH_REQUEST;
			return false;

		case KNX_SEARCH_RESPONSE:
			packet->service = KNX_SEARCH_RESPONSE;
			return false;

		case KNX_DESCRIPTION_REQUEST:
			packet->service = KNX_DESCRIPTION_REQUEST;
			return false;

		case KNX_DESCRIPTION_RESPONSE:
			packet->service = KNX_DESCRIPTION_RESPONSE;
			return false;

		case KNX_CONNECTION_REQUEST:
			packet->service = KNX_CONNECTION_REQUEST;
			return knx_parse_connection_request(message + 6, claimed_len - 6,
			                                         &packet->payload.conn_req);

		case KNX_CONNECTION_RESPONSE:
			packet->service = KNX_CONNECTION_RESPONSE;
			return knx_parse_connection_response(message + 6, claimed_len - 6,
			                                          &packet->payload.conn_res);

		case KNX_CONNECTIONSTATE_REQUEST:
			packet->service = KNX_CONNECTIONSTATE_REQUEST;
			return knx_parse_connection_state_request(message + 6, claimed_len - 6,
			                                               &packet->payload.conn_state_req);

		case KNX_CONNECTIONSTATE_RESPONSE:
			packet->service = KNX_CONNECTIONSTATE_RESPONSE;
			return knx_parse_connection_state_response(message + 6, claimed_len - 6,
			                                                &packet->payload.conn_state_res);

		case KNX_DISCONNECT_REQUEST:
			packet->service = KNX_DISCONNECT_REQUEST;
			return knx_parse_disconnect_request(message + 6, claimed_len - 6,
			                                         &packet->payload.dc_req);

		case KNX_DISCONNECT_RESPONSE:
			packet->service = KNX_DISCONNECT_RESPONSE;
			return knx_parse_disconnect_response(message + 6, claimed_len - 6,
			                                          &packet->payload.dc_res);

		case KNX_DEVICE_CONFIGURATION_REQUEST:
			packet->service = KNX_DEVICE_CONFIGURATION_REQUEST;
			return false;

		case KNX_DEVICE_CONFIGURATION_ACK:
			packet->service = KNX_DEVICE_CONFIGURATION_ACK;
			return false;

		case KNX_TUNNEL_REQUEST:
			packet->service = KNX_TUNNEL_REQUEST;
			return knx_parse_tunnel_request(message + 6, claimed_len - 6,
			                                     &packet->payload.tunnel_req);

		case KNX_TUNNEL_RESPONSE:
			packet->service = KNX_TUNNEL_RESPONSE;
			return knx_parse_tunnel_response(message + 6, claimed_len - 6,
			                                      &packet->payload.tunnel_res);

		case KNX_ROUTING_INDICATION:
			packet->service = KNX_ROUTING_INDICATION;
			return false;

		default:
			return false;
	}
}
