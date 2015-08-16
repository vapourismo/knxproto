#include "testfw.h"
#include "../src/util/log.h"

externtest(knxnetip)
externtest(cemi)
externtest(tunnelclient)

deftest(all, {
	runsubtest(knxnetip);
	runsubtest(cemi);
	runsubtest(tunnelclient);
})

int main(void) {
	knx_log_setup(NULL, LOG_DEBUG);
	runtest(all);
	return 0;
}
