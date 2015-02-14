#ifndef HPKNXD_KNX_NET_TUNNELCLIENT_H
#define HPKNXD_KNX_NET_TUNNELCLIENT_H

#include "../knx.h"
#include "../data/pkgqueue.h"
#include "../../net/dgramsock.h"

#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

/**
 * Tunnel Connection
 */
typedef struct {
	dgramsock sock;
	ip4addr gateway;

	pthread_t worker_thread;
	volatile bool do_work;
	knx_pkgqueue incoming, outgoing;

	bool established;
	uint8_t channel;
	knx_host_info host_info;
	time_t last_heartbeat;
} knx_tunnel_connection;

/**
 * Connect to a gateway.
 */
bool knx_tunnel_connect(knx_tunnel_connection* conn, const ip4addr* gateway);

/**
 * Disconnect from a gateway. If `wait_for_worker` is true, this function will
 * block until a disconnect response has been given by the gateway.
 */
void knx_tunnel_disconnect(knx_tunnel_connection* conn, bool wait_for_worker);

#endif
