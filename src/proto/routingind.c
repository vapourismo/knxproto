#include "routingind.h"
#include "header.h"

bool knx_append_routing_indication(msgbuilder* mb,
                                   const knx_routing_indication* ind) {
	return
		knx_append_header(mb, KNX_ROUTING_INDICATION, ind->size) &&
		msgbuilder_append(mb, ind->data, ind->size);
}

bool knx_parse_routing_indication(const uint8_t* message, size_t length,
                                  knx_routing_indication* ind) {
	ind->size = length;
	ind->data = message;

	return true;
}
