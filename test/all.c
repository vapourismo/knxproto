#include "testfw.h"

externtest(knxnetip)
externtest(cemi)

deftest(all, {
	runsubtest(knxnetip);
	runsubtest(cemi);
})

int main(void) {
	runtest(all);
	return 0;
}
