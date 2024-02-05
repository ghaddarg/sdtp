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
#include <cassert>

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
	this->mg_logger_.debug(__FUNCTION__, "Client Object Created");

	try {
		if (this->port_num_ < 0)
			throw ClientException(__FUNCTION__, "Error Invalid port");
	} catch (ClientException e) {
		this->mg_logger_.warn(__FUNCTION__, e.what());
	}

	//server_addr_ = { 0 };
	memset(&this->server_addr_, 0, sizeof(sockaddr_in));
}
Client::~Client()
{
	mg_logger_.debug(__FUNCTION__, "Client Object Destroyed");
}
/* -------------------------------------------------------------------- */
/*                            PRIVATE FUNCTIONS                         */
/* -------------------------------------------------------------------- */
int Client::setup(void)
{
	int ret = EXIT_FAILURE;

	/* Set up and create socket */
	mg_logger_.debug(__FUNCTION__, "setup a socket and connection tools on port: "
		+ std::to_string(port_num_) + " and IP: " + this->ip_address_);

	//std::unique_ptr<struct hostent> host = gethostbyname(this->ip_address_.c_str());
	struct hostent * host = gethostbyname(this->ip_address_.c_str());
	this->server_addr_.sin_family = AF_INET;

	//XXX: @MG TODO: FIXME 
	this->server_addr_.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
	//this->server_addr_.sin_addr.s_addr = inet_addr(inet_ntoa(*(reinterpret_cast<struct in_addr *>)(*host->h_addr_list)));

	this->server_addr_.sin_port = htons(port_num_);

	mg_logger_.debug(__FUNCTION__, "open stream oriented socket with internet address with socket descriptor");

	try {
		this->client_fd_ = socket(this->server_addr_.sin_family, SOCK_STREAM, 0);
		if (this->client_fd_ < 0) {
			throw ClientException(__FUNCTION__, "Failed to create a socket");
		}
	} catch (ClientException e) {
		mg_logger_.error(__FUNCTION__, strerror(errno));
		mg_logger_.warn(__FUNCTION__, e.what());
		return ret;
	}
	
	ret = EXIT_SUCCESS;
	return ret;
}

int Client::connect(void)
{
	int ret = EXIT_FAILURE;

	/* connect socket */
	this->mg_logger_.debug(__FUNCTION__, "Try to conenct to server address");
	try {
		if (::connect(this->client_fd_, reinterpret_cast<struct sockaddr *>(&server_addr_), sizeof(this->server_addr_)) < 0)	
			throw ClientException(__FUNCTION__, "Failed to conenct to server address");
	} catch (ClientException e) {
		this->mg_logger_.error(__FUNCTION__,  e.what());
		this->mg_logger_.warn(__FUNCTION__,  strerror(errno));
		return ret;
	}

	this->mg_logger_.debug(__FUNCTION__, "Connected to server");

	ret = EXIT_SUCCESS;
	return ret;
}

int Client::close_connection(void)
{
	int ret = EXIT_FAILURE;

	/* Close socket connection */
	mg_logger_.debug(__FUNCTION__, "Try to close connection to server address");
	ret = close(this->client_fd_);
	assert(ret >= 0);
	mg_logger_.debug(__FUNCTION__, "******** Session ENDED ********");

	ret = EXIT_SUCCESS;
	return ret;
}
int Client::send_cmd(std::string& msg_)
{
	int ret = 0;
	std::string request(msg_);

	mg_logger_.debug(__FUNCTION__, "Send command to server");

	while ((ret = send(this->client_fd_, (void *)request.c_str(), request.size(), 0))) {

		mg_logger_.debug(__FUNCTION__, "Sent " + std::to_string(ret) + " byte to server");

		assert(ret > 0);

		std::string response;
		char msg[MAX_MSG_LEN] = { 0 };
		size_t max_len = MAX_MSG_LEN; //100 bytes

		try {
			this->mg_logger_.debug(__FUNCTION__, "Recieve response from server");
			ret = recv(this->client_fd_, msg, max_len, 0);
			if (ret <= 0)
				throw ClientException(__FUNCTION__, "Failed to Recieve data from server");
			else
				response = std::string(msg);
			this->mg_logger_.debug(__FUNCTION__, "Recieved: " + std::to_string(ret) + " byte from server: " + response);
		} catch (ClientException m) {
			mg_logger_.error(__FUNCTION__, strerror(errno));
			mg_logger_.warn(__FUNCTION__, m.what());
		}
	}

	if (!ret) {
		mg_logger_.error(__FUNCTION__, "Failed to Send command to server");
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

	try {

		ret = this->setup();
		if (ret)
			return ret;
		ret = this->connect();
		if (ret)
			return ret;

		ret = this->send_cmd(msg);
		if (ret)
			return ret;

		ret = this->close_connection();
		if (ret)
			return ret;

	} catch (const std::exception& e) {

		mg_logger_.warn(__FUNCTION__, e.what());
	}
	
	return ret;
}

}