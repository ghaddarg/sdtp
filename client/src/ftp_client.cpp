/*****************************************************/
/*
 * This is source file
 *
 * Created by: @JbrilCC++
 *
 * The Content of this belongs to DLG Inc.
 * For any questions email: ghaddama@dlgnetworks.com
 * 
 */
/*****************************************************/
#include <iostream>
#include <string>

extern "C" {
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <errno.h>
	#include <netdb.h>
	#include <unistd.h>
}

#include <logger.hpp>
#include <ftp_def.hpp>
#include <ftp_exceptions.hpp>
#include <ftp_client.hpp>

//using namespace std;

namespace mg_ftp 
{

/* -------------------------------------------------------------------- */
/*                             CON/DE-STRUCTOR                          */
/* -------------------------------------------------------------------- */
Client::Client(const unsigned int& port_num, const std::string& ip_address, Logger& mg_logger)
	: port_num_(port_num), ip_address_(ip_address), client_fd_(0), mg_logger_(mg_logger)
{
	this->mg_logger_.debug("Client Object Created");

	if (this->port_num_ < 0)
		throw ServerInvalidPortException(__FUNCTION__);

	//server_addr_ = { 0 };
	memset(&this->server_addr_, 0, sizeof(sockaddr_in));
}
Client::~Client()
{
	std::cout << "Client Object Destroyed" << std::endl;
}
/* -------------------------------------------------------------------- */
/*                            PRIVATE FUNCTIONS                         */
/* -------------------------------------------------------------------- */
int Client::setup(void)
{
	int ret = EXIT_FAILURE;

	/* Set up and create socket */
	std::cout << "setup a socket and connection tools on port: " << port_num_ << " and IP: " << this->ip_address_ << std::endl;

	//std::unique_ptr<struct hostent> host = gethostbyname(this->ip_address_.c_str());
	struct hostent * host = gethostbyname(this->ip_address_.c_str());
	
	this->server_addr_.sin_family = AF_INET;

	//XXX: @MG TODO: fix to 
	this->server_addr_.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	//this->server_addr_.sin_addr.s_addr = inet_addr(inet_ntoa(*(reinterpret_cast<struct in_addr *>)(*host->h_addr_list)));

	this->server_addr_.sin_port = htons(port_num_);

	std::cout << "open stream oriented socket with internet address with socket descriptor " << std::endl;

	this->client_fd_ = socket(this->server_addr_.sin_family, SOCK_STREAM, 0);
	if (this->client_fd_ < 0) {
		throw ServerInvalidSocketDescriptorException(__FUNCTION__, strerror(errno));
		return ret;
	}

	ret = EXIT_SUCCESS;
	return ret;
}

int Client::connect(void)
{
	int ret = EXIT_FAILURE;

	/* connect socket */
	std::cout << "Try to conenct to server address" << std::endl;
	if (::connect(this->client_fd_, reinterpret_cast<struct sockaddr *>(&server_addr_), sizeof(this->server_addr_)) < 0) {
		throw ServerBindFailException(__FUNCTION__, strerror(errno));
		return ret;
	}

	std::cout << "Connected to server" << std::endl;

	ret = EXIT_SUCCESS;
	return ret;
}

int Client::close_connection(void)
{
	int ret = EXIT_FAILURE;

	/* Close socket connection */
	std::cout << "Try to close connection to server address" << std::endl;
	close(this->client_fd_);

	std::cout << "******** Session ENDED ********" << std::endl;

	ret = EXIT_SUCCESS;
	return ret;
}
int Client::send_cmd(std::string& msg_)
{
	int ret = 0;
	std::string request(msg_);

	mg_logger_.debug("Send command to server");

	while ((ret = send(this->client_fd_, (void *)request.c_str(), request.size(), 0))) {

		mg_logger_.debug("Sent " + std::to_string(ret) + " byte to server");

		std::string response;
		char msg[MAX_MSG_LEN] = { 0 };
		size_t max_len = MAX_MSG_LEN; //100 bytes

		mg_logger_.debug("Recieve response from server");
		ret = recv(this->client_fd_, msg, max_len, 0);
		response = std::string(msg);
		mg_logger_.debug("Recieved: " + std::to_string(ret) + " byte from server: " + response);
	}

	if (!ret) {
		mg_logger_.error("Failed to Send command to server");
		ret = EXIT_FAILURE;
		return ret;
	}

	ret = EXIT_SUCCESS;
	return ret;
}
/* -------------------------------------------------------------------- */
/*                             PUBLIC FUNCTIONS                         */
/* -------------------------------------------------------------------- */

int Client::main_loop(std::string& msg)
{
	int ret;
	std::cout << "HELLO WORLD" << std::endl;

	ret = this->setup();
	if (ret)
		return ret;
	ret = this->connect();
	if (ret)
		return ret;

	while (true) {

		this->send_cmd(msg);
		//sleep(100);
		break;
	}

	ret = this->close_connection();
	if (ret)
		return ret;

	return ret;
}

}