#include "tunnelreq.h"

bool knxnetip_append_tunnel_request(msgbuilder* mb,
                                    const knxnetip_tunnel_request* req) {
	return false;
}

bool knxnetip_parse_tunnel_request(const uint8_t* message, size_t length,
                                   knxnetip_tunnel_request* req) {
	return false;
}
