#include "routingind.h"
#include "header.h"

#include <string.h>

// Routing Indication:
//   Octet 0-n: Payload

void knx_generate_routing_indication(uint8_t* buffer, const knx_routing_indication* ind) {
	memcpy(buffer, ind->data, ind->size);
}

bool knx_parse_routing_indication(const uint8_t* message, size_t length, knx_routing_indication* ind) {
	ind->size = length;
	ind->data = message;

	return true;
}
