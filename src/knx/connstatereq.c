#include "connstatereq.h"

bool knxnetip_append_connection_state_request(msgbuilder* mb,
                                              const knxnetip_connection_state_request* conn_req) {
	return false;
}

bool knxnetip_parse_connection_state_request(const uint8_t* message, size_t length,
                                             knxnetip_connection_state_request* req) {
	return false;
}
