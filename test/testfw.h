/* KNX Client Library
 * A library which provides the means to communicate with several
 * KNX-related devices or services.
 *
 * Copyright (C) 2014-2015, Ole Krüger <ole@vprsm.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KNXPROTO_TEST_TESTFW_H_
#define KNXPROTO_TEST_TESTFW_H_

#include "../src/util/alloc.h"

#include <stdio.h>
#include <stdbool.h>

typedef enum {
	TR_ASSERT,
	TR_FAIL,
	TR_SUBTEST
} __testfailreason;

typedef struct {
	__testfailreason reason;
	const char* filename;
	int fileline;
	const char* info;
} __testinfo;

#define __testcase_name(name) __testcase_##name

#define __testcase_failed(r) { __vtestinfo->reason = (r); __vtestinfo->filename = __FILE__; __vtestinfo->fileline = __LINE__; }

inline static void __testcase_pass(const char* name) {
	printf("\033[0;32mPASS\033[0m %s\n", name);
}

inline static void __testcase_fail(const char* name, const __testinfo* info) {
	printf("\033[0;31mFAIL\033[0m %s [%s:%i]\n", name, info->filename, info->fileline);

	switch (info->reason) {
		case TR_ASSERT:
			printf("     + Assertion failed: %s\n", info->info);
			break;

		case TR_FAIL:
			printf("     + User message: %s\n", info->info);
			break;

		case TR_SUBTEST:
			printf("     + Sub test failed: %s\n", info->info);
			break;
	}
}

/**
 * Define a test case.
 * Example:
 * 	deftest(my_test, {
 * 		assert(2 + 2 == 1);
 * 	})
 */
#define deftest(name, ...) \
	bool __testcase_name(name)(__testinfo* __vtestinfo) { \
		{ __VA_ARGS__ }; \
		return true; \
	}

/**
 * Simply generate the signature of this test case.
 * Use this within header files.
 */
#define externtest(name) \
	bool __testcase_name(name)(__testinfo* __vtestinfo);

/**
 * Perform a test.
 */
#define runtest(name) { \
	__testinfo __vtestinfo; \
	if (__testcase_name(name)(&__vtestinfo)) { \
		__testcase_pass(__STRING(name)); \
	} else { \
		__testcase_fail(__STRING(name), &__vtestinfo); \
	} \
}

/**
 * Perform a test within a test.
 */
#define runsubtest(name) { \
	__testinfo __vsubtestinfo; \
	if (__testcase_name(name)(&__vsubtestinfo)) { \
		__testcase_pass(__STRING(name)); \
	} else { \
		__testcase_fail(__STRING(name), &__vsubtestinfo); \
		__testcase_failed(TR_SUBTEST); \
		__vtestinfo->info = __STRING(name); \
		return false; \
	} \
}

/**
 * Assert that the given expression `expr` is true.
 */
#define assert(expr) { \
	if (!(expr)) { \
		__testcase_failed(TR_ASSERT); \
		__vtestinfo->info = __STRING(expr); \
		return false; \
	} \
}

/**
 * Fail with a custom error message.
 */
#define fail(msg) { \
	__testcase_failed(TR_FAIL); \
	__vtestinfo->info = (msg); \
	return false; \
}

#endif
