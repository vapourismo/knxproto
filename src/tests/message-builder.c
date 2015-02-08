#include "testfw.h"
#include "../message-builder.h"

#include <stdint.h>
#include <string.h>

deftest(msgbuilder_init_reset, {
	msgbuilder target;

	// Initialize structure
	assert(msgbuilder_init(&target, 0));
	assert(target.buffer == NULL);
	assert(target.used == 0);
	assert(target.max == 0);

	// Reset structure
	msgbuilder_reset(&target);
	assert(target.buffer == NULL);
	assert(target.used == 0);
	assert(target.max == 0);

	// Initialize structure
	assert(msgbuilder_init(&target, 100));
	assert(target.buffer != NULL);
	assert(target.used == 0);
	assert(target.max == 100);

	// Reset structure
	msgbuilder_reset(&target);
	assert(target.buffer == NULL);
	assert(target.used == 0);
	assert(target.max == 0);
})

deftest(msgbuilder_reserve, {
	msgbuilder target;
	msgbuilder_init(&target, 0);

	// Reserve space/reinitialize
	assert(msgbuilder_reserve(&target, 100));
	assert(target.buffer != NULL);
	assert(target.used == 0);
	assert(target.max == 100);

	// Reserve non-additional space
	assert(msgbuilder_reserve(&target, 50));
	assert(target.buffer != NULL);
	assert(target.used == 0);
	assert(target.max == 100);

	// Reserve additional space
	assert(msgbuilder_reserve(&target, 150));
	assert(target.buffer != NULL);
	assert(target.used == 0);
	assert(target.max == 150);

	msgbuilder_reset(&target);
})

const uint8_t example_data1[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
const uint8_t example_data2[9] = {11, 22, 33, 44, 55, 66, 77, 88, 99};
const uint8_t example_data_sum[18] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9,
	11, 22, 33, 44, 55, 66, 77, 88, 99
};

deftest(msgbuilder_append, {
	msgbuilder target;
	msgbuilder_init(&target, 20);

	// Append example data
	assert(msgbuilder_append(&target, example_data1, 9));
	assert(target.used == 9);
	assert(memcmp(target.buffer, example_data1, 9) == 0);

	// Append example data
	assert(msgbuilder_append(&target, example_data2, 9));
	assert(target.used == 18);
	assert(memcmp(target.buffer + 9, example_data2, 9) == 0);
	assert(memcmp(target.buffer, example_data_sum, 18) == 0);

	// Overflow
	assert(!msgbuilder_append(&target, example_data1, 9));

	// Reset
	msgbuilder_reset(&target);

	// Overflow
	assert(!msgbuilder_append(&target, example_data1, 9));
})

deftest(msgbuilder_append_mb, {
	msgbuilder target;
	msgbuilder_init(&target, 20);

	msgbuilder sample1;
	msgbuilder_init(&sample1, 10);
	msgbuilder_append(&sample1, example_data1, 9);

	msgbuilder sample2;
	msgbuilder_init(&sample2, 10);
	msgbuilder_append(&sample2, example_data2, 9);

	// Append example data
	assert(msgbuilder_append_mb(&target, &sample1));
	assert(target.used == 9);
	assert(memcmp(target.buffer, example_data1, 9) == 0);

	// Append example data
	assert(msgbuilder_append_mb(&target, &sample2));
	assert(target.used == 18);
	assert(memcmp(target.buffer + 9, example_data2, 9) == 0);
	assert(memcmp(target.buffer, example_data_sum, 18) == 0);

	// Overflow
	assert(!msgbuilder_append_mb(&target, &sample1));

	// Reset
	msgbuilder_reset(&target);

	// Overflow
	assert(!msgbuilder_append_mb(&target, &sample1));
})

deftest(msgbuilder, {
	runsubtest(msgbuilder_init_reset);
	runsubtest(msgbuilder_reserve);
	runsubtest(msgbuilder_append);
	runsubtest(msgbuilder_append_mb);
})
