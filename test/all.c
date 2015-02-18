#include "testfw.h"

externtest(knxnetip)

deftest(all, {
	runsubtest(knxnetip);
})

int main(void) {
	runtest(all);
	return 0;
}
