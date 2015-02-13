#include "tests/testfw.h"

externtest(msgbuilder)
externtest(knxnetip)

int main(void) {
	runtest(msgbuilder);
	runtest(knxnetip);

	return 0;
}
