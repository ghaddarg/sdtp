/*****************************************************/
/*
 * This is Client header file
 *
 * Created by: @JbrilCC++
 *
 * The Content of this belongs to DLG Inc.
 * For any questions email: ghaddama@dlgnetworks.com
 * 
 */
/*****************************************************/
#pragma once

namespace mg_ftp
{

/* ******************************************************************** */
/*                               CLASS DECLARATION                      */
/* ******************************************************************** */

class Client
{

public:

	/*!
	 * \brief Constructor.
	 */
	Client(const unsigned int& port_num, const std::string& ip_address, Logger& mg_logger);
	~Client();

	/*!
	 * \brief main loop.
	 * 
	 * \returns EXIT_SUCCESS for SUCCESS else FAIL;
	 */
	int main_loop(std::string& msg);

private:

	int setup(void);
	int connect(void);
	int close_connection(void);
	int send_cmd(std::string& msg);

	sockaddr_in server_addr_;
	const unsigned int port_num_;
	std::string ip_address_;
	
	int client_fd_;
	Logger mg_logger_;
};

} //namespace mg