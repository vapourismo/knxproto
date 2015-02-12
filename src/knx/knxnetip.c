#include "knxnetip.h"
#include <string.h>

bool knxnetip_generate_conn_req(msgbuilder* mb, const knxnetip_conn_req* conn_req) {
	const uint8_t contents[4] = {4, conn_req->type, conn_req->layer, 0};

	return
		knxnetip_append_header(mb, KNXNETIP_CONNECTION_REQUEST, 20) &&
		knxnetip_append_host_info(mb, &conn_req->control_host) &&
		knxnetip_append_host_info(mb, &conn_req->control_host) &&
		msgbuilder_append(mb, contents, 4);
}

bool knxnetip_generate(msgbuilder* mb, const knxnetip_packet* packet) {
	switch (packet->service) {
		case KNXNETIP_SEARCH_REQUEST:
			return false;

		case KNXNETIP_SEARCH_RESPONSE:
			return false;

		case KNXNETIP_DESCRIPTION_REQUEST:
			return false;

		case KNXNETIP_DESCRIPTION_RESPONSE:
			return false;

		case KNXNETIP_CONNECTION_REQUEST:
			return knxnetip_generate_conn_req(mb, &packet->connection_request);

		case KNXNETIP_CONNECTION_RESPONSE:
			return false;

		case KNXNETIP_CONNECTIONSTATE_REQUEST:
			return false;

		case KNXNETIP_CONNECTIONSTATE_RESPONSE:
			return false;

		case KNXNETIP_DISCONNECT_REQUEST:
			return false;

		case KNXNETIP_DISCONNECT_RESPONSE:
			return false;

		case KNXNETIP_DEVICE_CONFIGURATION_REQUEST:
			return false;

		case KNXNETIP_DEVICE_CONFIGURATION_ACK:
			return false;

		case KNXNETIP_TUNNEL_REQUEST:
			return false;

		case KNXNETIP_TUNNEL_RESPONSE:
			return false;

		case KNXNETIP_ROUTING_INDICATION:
			return false;
	}
}

bool knxnetip_parse_host_info(const uint8_t* message, knxnetip_host_info* host) {
	if (message[0] != 8)
		return false;

	switch (message[1]) {
		case KNXNETIP_PROTO_UDP:
			host->protocol = KNXNETIP_PROTO_UDP;
			break;

		case KNXNETIP_PROTO_TCP:
			host->protocol = KNXNETIP_PROTO_TCP;
			break;

		default:
			return false;
	}

	memcpy(&host->address, message + 2, 4);
	memcpy(&host->port, message + 6, 2);

	return true;
}

bool knxnetip_parse_conn_req(const uint8_t* message, size_t length,
                            knxnetip_conn_req* req) {
	if (length < 20 || message[16] != 4)
		return false;

	switch (message[17]) {
		case KNXNETIP_conn_req_TUNNEL:
			req->type = KNXNETIP_conn_req_TUNNEL;
			break;

		default:
			return false;
	}

	switch (message[18]) {
		case KNXNETIP_LAYER_TUNNEL:
			req->layer = KNXNETIP_LAYER_TUNNEL;
			break;

		default:
			return false;
	}

	return
		knxnetip_parse_host_info(message, &req->control_host) &&
		knxnetip_parse_host_info(message + 8, &req->tunnel_host);
}

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
			return knxnetip_parse_conn_req(message + 6, claimed_len - 6, &packet->connection_request);

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
