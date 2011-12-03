/*
 * Since some objects need Singleton, which needs AtExitManager, we create this class for convenient unitttest.
 * You can use macro TEST_WITH_EM like TEST. 
 *
 * For example,
 * TEST_WITH_EM(base::Thread, Basic) {
 *     Thread thread;
 *     ....
 * }
 */
#ifndef BASE_TEST_TSET_WITH_EXIT_MANAGER_H__
#define BASE_TEST_TSET_WITH_EXIT_MANAGER_H__

#include "base/at_exit_manager.h"
#include "gtest/gtest.h"

namespace base {
	class TestWithExitManager : public ::testing::Test {
	protected:
		AtExitManager at_manager;
	};
}

#define TEST_WITH_EM(test_case_name, test_name)\
	GTEST_TEST_(test_case_name, test_name, base::TestWithExitManager, \
	::testing::internal::GetTypeId<base::TestWithExitManager>())

#endif // #ifndef BASE_TEST_TSET_WITH_EXIT_MANAGER_H__

