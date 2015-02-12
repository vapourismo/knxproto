#include "dcreq.h"

bool knxnetip_append_disconnect_request(msgbuilder* mb,
                                        const knxnetip_disconnect_request* conn_req) {
	return false;
}

bool knxnetip_parse_disconnect_request(const uint8_t* message, size_t length,
                                       knxnetip_disconnect_request* req) {
	return false;
}
