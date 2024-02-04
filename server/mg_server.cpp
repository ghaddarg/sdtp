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
extern "C" {
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
}

#include <thread>
#include <logger.hpp>
#include <ftp_def.hpp>
#include <ftp_server.hpp>

using namespace mg_ftp;
/* -------------------------------------------------------------------- */
/*                                MAIN FUNCTION                         */
/* -------------------------------------------------------------------- */
int main(void)
{
	Logger mg_log(INFO);
	Server mg_ftp_server(SERVER_PORT_NUM, MAX_CLIENTS_NUM, mg_log);
	return mg_ftp_server.main_loop();
}
