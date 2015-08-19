#include "testfw.h"
#include "../src/util/log.h"

externtest(knxnetip)
externtest(cemi)

deftest(all, {
	runsubtest(knxnetip);
	runsubtest(cemi);
})

int main(void) {
	knx_log_setup(NULL, LOG_DEBUG);
	runtest(all);
	return 0;
}
