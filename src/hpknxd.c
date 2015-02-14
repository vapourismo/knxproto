// #include "tests/testfw.h"
#include "knx/net/tunnelclient.h"

#include <signal.h>

// externtest(msgbuilder)
// externtest(knxnetip)

static volatile bool running = true;

static void sighandler(int sig) {
	if (sig == SIGINT) {
		running = false;
	}
}

int main(void) {
	// runtest(msgbuilder);
	// runtest(knxnetip);

	signal(SIGINT, &sighandler);

	ip4addr gateway;
	ip4addr_from_string(&gateway, "192.168.1.5", 3671);

	knx_tunnel_connection conn;

	puts("main: Connecting ...");
	if (!knx_tunnel_connect(&conn, &gateway)) {
		puts("main: Connection failed");
		return false;
	}

	// while (running) sleep(1);
	sleep(30);

	puts("main: Disconnecting...");
	knx_tunnel_disconnect(&conn, true);

	return 0;
}
