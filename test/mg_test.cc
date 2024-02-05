// This sample shows how to write a simple unit test for a function,
// using Google C++ testing framework.
//
// Writing a unit test using Google C++ testing framework is easy as 1-2-3:


// Step 1. Include necessary header files such that the stuff your
// test logic needs is declared.
//
// Don't forget gtest.h, which declares the testing framework.

// Step 2. Use the TEST macro to define your tests.
//
// TEST has two parameters: the test case name and the test name.
// After using the macro, you should define your test logic between a
// pair of braces.  You can use a bunch of macros to indicate the
// success or failure of a test.  EXPECT_TRUE and EXPECT_EQ are
// examples of such macros.  For a complete list, see gtest.h

#include "gtest/gtest.h"

#include <string>

#include <logger.hpp>
#include <ftp_def.hpp>
#include <cmd_parser.hpp>
#include <ftp_exceptions.hpp>
/****************************************************/
/*                    DUMMY TEST                    */
/****************************************************/
TEST(SdtpDummy, TestDummy)
{
	int expected = 1;
	int actual = 1;

	EXPECT_EQ(expected, actual);

	GTEST_SUCCEED();
}
 /****************************************************/
/*                 GROUP TEST ID                     */
/*****************************************************/
TEST(ParserGroupID, TestDelete)
{
	std::string args = "delete \"MG\"";

	mg_ftp::Logger mg_log(mg_ftp::NONE);
	mg_ftp::Parser mg_cmd_parser(args, mg_log);

	std::string expected("127.0.0.1"), actual = mg_cmd_parser.get_ip();
	EXPECT_EQ(expected, actual);

	unsigned int expected_port = 2020, actual_port = mg_cmd_parser.get_port();
	EXPECT_EQ(expected_port, actual_port);

	expected = "delete";
	actual = mg_cmd_parser.get_cmd();
	EXPECT_EQ(expected, actual);

	expected = "\"MG\"";
	actual = mg_cmd_parser.get_key();
	EXPECT_EQ(expected, actual);
	
	expected = "delete \"MG\"";
	actual = mg_cmd_parser.construct_msg();
	EXPECT_EQ(expected, actual);

	GTEST_SUCCEED();
}

TEST(BGgroupConfiguration, TestInValid)
{
	std::string arg = "create";
	std::vector<std::string> args;
	args.push_back(arg);

	mg_ftp::Logger mg_log(mg_ftp::NONE);
	EXPECT_NO_THROW(mg_ftp::Parser mg_cmd_parser(args, mg_log));

	GTEST_SUCCEED();
}
