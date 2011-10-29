#include <iostream>
#include "gtest/gtest.h"
int main(int argc, char *argv[]) {
	std::cout << "Running all unit test case" << std::endl;
	testing::InitGoogleTest(&argc,argv);
	RUN_ALL_TESTS();
	std::cout << "All unit test case end" << std::endl;
	return 0;
}