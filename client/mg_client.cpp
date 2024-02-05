/*****************************************************/
/*
 * This is main file
 *
 * Created by: @JbrilCC++
 *
 * The Content of this belongs to DLG Inc.
 * For any questions email: ghaddama@dlgnetworks.com
 * 
 */
/*****************************************************/
#include <string>
#include <iostream>

extern "C" {
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
}

#include <logger.hpp>
#include <ftp_client.hpp>
#include <cmd_parser.hpp>

using namespace mg_ftp;
/* -------------------------------------------------------------------- */
/*                                MAIN FUNCTION                         */
/* -------------------------------------------------------------------- */
int main(int argc, char * argv[])
{
	int ret = EXIT_FAILURE;
	std::string str;
	Logger mg_log(INFO);
	std::vector<std::string> args;

	if (argc > 1)
		args.assign(argv + 1, argv + argc);
	else
		return ret;

	Parser mg_cmd_parser(args, mg_log);
	str = mg_cmd_parser.construct_msg();
	Client mg_ftp_client(mg_cmd_parser.get_port(), mg_cmd_parser.get_ip(), mg_log);
	ret = mg_ftp_client.main_loop(str);

	return ret;
}
