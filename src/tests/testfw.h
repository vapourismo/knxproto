#ifndef HPKNX_TESTS_TESTFW_H
#define HPKNX_TESTS_TESTFW_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {
	TR_ASSERT,
	TR_FAIL
} __testfailreason;

typedef struct {
	__testfailreason reason;
	const char* filename;
	int fileline;
	union {
		struct {
			const char* expression;
		} assertion;
		struct {
			const char* message;
		} user;
	} info;
} __testinfo;

#define __testcase_name(name) __testcase_##name

#define __testcase_failed(r) { __testinfo->reason = (r); __testinfo->filename = __FILE__; __testinfo->fileline = __LINE__; }

inline void __testcase_pass(const char* name) {
	printf("\033[0;32mPASS\033[0m %s\n", name);
}

inline void __testcase_fail(const char* name, const __testinfo* info) {
	printf("\033[0;31mFAIL\033[0m %s [%s:%i]\n", name, info->filename, info->fileline);

	switch (info->reason) {
		case TR_ASSERT:
			printf("     + Assertion failed: %s\n", info->info.assertion.expression);
			break;

		case TR_FAIL:
			printf("     + User message: %s\n", info->info.user.message);
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
#define deftest(name, code) \
	bool __testcase_name(name)(__testinfo* __testinfo) { \
		{ code; } \
		return true; \
	}

/**
 * Simply generate the signature of this test case.
 * Use this within header files.
 */
#define exporttest(name) \
	bool __testcase_name(name)(__testinfo* __testinfo);

/**
 * Perform a test.
 */
#define runtest(name) { \
	__testinfo info; \
	if (__testcase_name(name)(&info)) { \
		__testcase_pass(__STRING(name)); \
	} else { \
		__testcase_fail(__STRING(name), &info); \
	} \
}

/**
 * Assert that the given expression `expr` is true.
 */
#define assert(expr) { \
	if (!(expr)) { \
		__testcase_failed(TR_ASSERT); \
		__testinfo->info.assertion.expression = __STRING(expr); \
		return false; \
	} \
}

/**
 * Fail with a custom error message.
 */
#define fail(msg) { \
	__testcase_failed(TR_FAIL); \
	__testinfo->info.user.message = (msg); \
	return false; \
}

#endif
