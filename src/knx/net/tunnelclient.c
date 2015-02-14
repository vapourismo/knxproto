#include "tunnelclient.h"

void knx_tunnel_worker(knx_tunnel_connection* conn) {
	msgbuilder mb;
	if (!msgbuilder_init(&mb, 0))
		return;

	while (conn->do_work) {
		// Check if we need to send a heartbeat
		if (conn->established && difftime(time(NULL), conn->last_heartbeat) >= 30) {
			puts("worker: Heartbeat required");

			knx_packet packet = {
				KNX_CONNECTIONSTATE_REQUEST,
				{
					.conn_state_req = {
						conn->channel,
						0,
						conn->host_info
					}
				}
			};

			// Send and mark time of departure
			if (knx_generate(&mb, &packet)) {
				dgramsock_send(conn->sock, mb.buffer, mb.used, &conn->gateway);
				conn->last_heartbeat = time(NULL);
				puts("worker: Heartbeat sent");
			}
		}

		// Sender loop
		while (conn->do_work) {
			knx_packet to_send;

			// Dequeue packet or exit sender loop
			if (!knx_pkgqueue_dequeue(&conn->outgoing, &to_send))
				break;


			// Generate message and send
			if (knx_generate(&mb, &to_send))
				dgramsock_send(conn->sock, mb.buffer, mb.used, &conn->gateway);

			printf("worker: Sent (service = 0x%04X)\n", to_send.service);
		}

		// Clear buffer and ensure enough space
		mb.used = 0;
		msgbuilder_reserve(&mb, 100);

		size_t receive_iter = 0;

		// Receiver loop
		while (conn->do_work && receive_iter++ < 100 &&
		       dgramsock_ready(conn->sock, 0, 100000)) {

			knx_packet pkg_in;

			// FIXME: Do not allow every endpoint
			ssize_t r = dgramsock_recv(conn->sock, mb.buffer, mb.max, NULL, 0);

			if (r > 0 && knx_parse(mb.buffer, r, &pkg_in)) {
				printf("worker: Received (service = 0x%04X)\n", pkg_in.service);

				switch (pkg_in.service) {
					// Connection successfully establish
					case KNX_CONNECTION_RESPONSE:
						conn->established = (pkg_in.payload.conn_res.status == 0);


						if (conn->established) {
							conn->channel = pkg_in.payload.conn_res.channel;
							conn->host_info = pkg_in.payload.conn_res.host;

							printf("worker: Connected (channel = %i)\n", conn->channel);
						} else {
							printf("worker: Connection failed (code = %i)\n",
							       pkg_in.payload.conn_res.status);
						}

						break;

					// Heartbeat
					case KNX_CONNECTIONSTATE_RESPONSE:
						if (pkg_in.payload.conn_state_res.channel == conn->channel)
							conn->established &= (pkg_in.payload.conn_state_res.status == 0);

						printf("worker: Heartbeat response (channel = %i, status = %i)\n",
						       pkg_in.payload.conn_state_res.channel,
						       pkg_in.payload.conn_state_res.status);
						break;

					// Result of a disconnect request (duh)
					case KNX_DISCONNECT_RESPONSE:
						conn->established &= !(pkg_in.payload.dc_res.channel == conn->channel &&
						                       pkg_in.payload.dc_req.status == 0);

						printf("worker: Disconnect (channel = %i, status = %i)\n",
						       pkg_in.payload.dc_req.channel,
						       pkg_in.payload.dc_req.status);

						// Gently shut down this worker thread
						if (!conn->established)
							conn->do_work = false;

						break;

					// Tunnel Request
					case KNX_TUNNEL_REQUEST:
						// Send tunnel response if a connection is established
						if (conn->established) {
							knx_packet tunnel_res = {
								KNX_TUNNEL_RESPONSE,
								{
									.tunnel_res = {
										conn->channel,
										pkg_in.payload.tunnel_req.seq_number,
										0
									}
								}
							};
							knx_pkgqueue_enqueue(&conn->outgoing, &tunnel_res);
						}

						knx_pkgqueue_enqueue(&conn->incoming, &pkg_in);

						break;

					// Everything else should be ignored
					default:
						break;
				}
			} else if (r < 0)
				break;
		}
	}

	// Clear the outgoing queue one last time
	while (true) {
		knx_packet to_send;

		// Dequeue packet or exit sender loop
		if (!knx_pkgqueue_dequeue(&conn->outgoing, &to_send))
			break;

		// Generate message and send
		if (knx_generate(&mb, &to_send))
			dgramsock_send(conn->sock, mb.buffer, mb.used, &conn->gateway);
	}

	// Free buffers and queues
	msgbuilder_destroy(&mb);
	knx_pkgqueue_destroy(&conn->incoming);
	knx_pkgqueue_destroy(&conn->outgoing);

	// The worker has to terminate the connection
	dgramsock_close(conn->sock);
}

extern void* knx_tunnel_worker_thread(void* conn) {
	knx_tunnel_worker(conn);
	pthread_exit(NULL);
}

bool knx_tunnel_connect(knx_tunnel_connection* conn, const ip4addr* gateway) {
	conn->gateway = *gateway;
	conn->established = false;
	conn->channel = 0;
	conn->last_heartbeat = 0;
	conn->do_work = true;

	// Setup UDP socket
	if ((conn->sock = dgramsock_create(NULL, false)) < 0)
		return false;

	// Start the worker thread
	if (pthread_create(&conn->worker_thread, NULL, &knx_tunnel_worker_thread, conn) != 0) {
		dgramsock_close(conn->sock);
		return false;
	}

	knx_pkgqueue_init(&conn->incoming);
	knx_pkgqueue_init(&conn->outgoing);

	// Queue a connection request
	knx_packet req = {
		KNX_CONNECTION_REQUEST,
		{
			.conn_req = {
				KNX_CONNECTION_REQUEST_TUNNEL,
				KNX_LAYER_TUNNEL,
				KNX_HOST_INFO_NAT(KNX_PROTO_UDP),
				KNX_HOST_INFO_NAT(KNX_PROTO_UDP)
			}
		}
	};
	knx_pkgqueue_enqueue(&conn->outgoing, &req);

	return true;
}

void knx_tunnel_disconnect(knx_tunnel_connection* conn, bool wait_for_worker) {
	// Queue a disconnect request
	knx_packet req = {
		KNX_DISCONNECT_REQUEST,
		{
			.dc_req = {
				conn->channel,
				0,
				conn->host_info
			}
		}
	};
	knx_pkgqueue_enqueue(&conn->outgoing, &req);

	// Make the worker stop.
	if (!conn->do_work)
		return;

	if (wait_for_worker) {
		pthread_join(conn->worker_thread, NULL);
	} else {
		conn->do_work = false;
		pthread_detach(conn->worker_thread);
	}
}
