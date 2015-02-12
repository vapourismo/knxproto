#include "dcres.h"
#include "header.h"

// Disconnect Response:
//   ???

bool knxnetip_append_disconnect_response(msgbuilder* mb,
                                         const knxnetip_disconnect_response* res) {
	return false;
}

bool knxnetip_parse_disconnect_response(const uint8_t* message, size_t length,
                                        knxnetip_disconnect_response* res) {
	return false;
}
