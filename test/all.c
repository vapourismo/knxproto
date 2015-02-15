#include "testfw.h"

externtest(knxnetip)
externtest(msgbuilder)

deftest(all, {
	runsubtest(knxnetip);
	runsubtest(msgbuilder);
})

int main(void) {
	runtest(all);
	return 0;
}
