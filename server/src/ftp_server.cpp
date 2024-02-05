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
#include <thread>
#include <fstream>
#include <cstdio>
#include <cassert>

extern "C" {
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <errno.h>
}

#include <logger.hpp>
#include <ftp_def.hpp>
#include <ftp_exceptions.hpp>
#include <ftp_server.hpp>
#include <cmd_parser.hpp>

using namespace std;

namespace mg_ftp 
{

/* -------------------------------------------------------------------- */
/*                             CON/DE-STRUCTOR                          */
/* -------------------------------------------------------------------- */
Server::Server(const unsigned int& port_num, const unsigned int& max_client, Logger& mg_logger) 
	: mg_logger_(mg_logger), port_num_(port_num), max_client_(max_client), server_fd_(0), client_sd_(0)
{
	mg_logger_.debug(__FUNCTION__, "Server Object Created");

	try {
		if (this->port_num_ < 0 || this->max_client_ <= 0)
			throw ServerException(__FUNCTION__, "Error eInvalid port of max clients allowed");
	} catch (ServerException e) {
		mg_logger_.error(__FUNCTION__, e.what());
	}
	

	memset(&this->server_addr_, 0, sizeof(sockaddr_in));

	cf_.emplace("create", std::bind(&Server::enqueue, this, std::placeholders::_1, std::placeholders::_2));
	cf_.emplace("update", std::bind(&Server::update, this, std::placeholders::_1, std::placeholders::_2));
	cf_.emplace("read", std::bind(&Server::get_payload, this, std::placeholders::_1, std::placeholders::_2));
	cf_.emplace("delete", std::bind(&Server::dequeue, this, std::placeholders::_1, std::placeholders::_2));
}
Server::~Server()
{
	mg_logger_.debug(__FUNCTION__, "Server Object Destroyed");
}
/* -------------------------------------------------------------------- */
/*                            PRIVATE FUNCTIONS                         */
/* -------------------------------------------------------------------- */
int Server::create_socket(void)
{
	int ret = EXIT_FAILURE;

	mg_logger_.debug(__FUNCTION__, "setup a socket and connection tools on port: " + std::to_string(port_num_));
	
	this->server_addr_.sin_family = AF_INET;
	this->server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	this->server_addr_.sin_port = htons(port_num_);

	mg_logger_.debug(__FUNCTION__, "open stream oriented socket with internet address with socket descriptor ");

	try {
		this->server_fd_ = socket(this->server_addr_.sin_family, SOCK_STREAM, 0);
		if (this->server_fd_ < 0) {
			throw ServerException(__FUNCTION__, "Error creating the server socket");
		}
	} catch (ServerException e) {
		this->mg_logger_.error(__FUNCTION__, strerror(errno));
		this->mg_logger_.error(__FUNCTION__, e.what());
		return ret;
	}
	

	ret = EXIT_SUCCESS;
	return ret;
}

int Server::bind_socket(void)
{
	int ret = EXIT_FAILURE, bind_ret;

	mg_logger_.debug(__FUNCTION__, "bind the socket to its local address");

	try {
		bind_ret = ::bind(this->server_fd_, reinterpret_cast<struct sockaddr *>(&server_addr_), sizeof(this->server_addr_));
		if (bind_ret < 0) {
			throw ServerException(__FUNCTION__, "Error binding socket to local address");
			
		}
	} catch (ServerException e) {
		mg_logger_.error(__FUNCTION__, strerror(errno));
		mg_logger_.error(__FUNCTION__, e.what());
		return ret;	
	}
	

	ret = EXIT_SUCCESS;
	return ret;
}

int Server::listen_socket(void)
{
	int ret = EXIT_FAILURE, listen_ret;

	mg_logger_.debug(__FUNCTION__, "Listen for a client...");

	try {
		listen_ret = listen(server_fd_, this->max_client_);
		if (listen_ret < 0)
			throw ServerException(__FUNCTION__, "Server Listen Failure");
	} catch (ServerException e) {
		mg_logger_.warn(__FUNCTION__, e.what());
		mg_logger_.error(__FUNCTION__, strerror(errno));
		return ret;
	}
	
	mg_logger_.debug(__FUNCTION__, "Waiting for a client to connect");

	ret = EXIT_SUCCESS;
	return ret;
}

int Server::accept_connection(void)
{
	/* Receive a request from client using accept we need a new address to connect with the client */
    sockaddr_in client_sock_addr;
    socklen_t client_sock_addr_size = sizeof(client_sock_addr);

	/* Accept, create a new socket descriptor to handle the new connection with client */
    try {
    	this->client_sd_ = accept(this->server_fd_, (sockaddr *)&client_sock_addr, &client_sock_addr_size);
		if(this->client_sd_ < 0)
			throw ServerException(__FUNCTION__, "Error accepting request from client!");
		else
			this->mg_logger_.debug(__FUNCTION__, "Connected with client:" + std::to_string(this->client_sd_));
	} catch (ServerException& e) {
		mg_logger_.warn(__FUNCTION__, e.what());
		mg_logger_.error(__FUNCTION__, strerror(errno));
	}

	return this->client_sd_;
}

std::string Server::get_response(std::string request)
{
	m.lock();

	CmdMap::iterator i;
	int ret = EXIT_FAILURE;

	assert(request.size());

	Parser mg_cmd_parser_(request, this->mg_logger_);

	std::cout << request.size() << std::endl;
	if (request.size() < 2) {

		mg_logger_.warn(__FUNCTION__, "request size too small");
		return std::string("Failure");	
	}

	if ((i = this->cf_.find(mg_cmd_parser_.get_cmd())) == this->cf_.end()) {

		mg_logger_.error(__FUNCTION__, "Command not in map. Something went wrong");
		return std::string("Failure");;
	}

	if (this->cf_.end() != i)
		ret = i->second(mg_cmd_parser_.get_key(), mg_cmd_parser_.get_data());

	std::string response;

	if (EXIT_FAILURE == ret) {
		response = std::string("Failure: " + mg_cmd_parser_.get_key());

		if (mg_cmd_parser_.get_cmd() == "create")
			response += " already exists";
		else
			response += " not found";
	} else {
		response = std::string("Success: " + mg_cmd_parser_.get_key() + " " + mg_cmd_parser_.get_cmd());
		if (mg_cmd_parser_.get_cmd() != "read")
			response +=  "d";
		else
			response += get_payload_(mg_cmd_parser_.get_key());
	}

	m.unlock();

	return response;
}

int Server::parse_commands(std::thread::id thread_id, int client_num_)
{
	int ret = EXIT_FAILURE;
	char msg[MAX_MSG_LEN] = { 0 };
	size_t max_len = MAX_MSG_LEN; //100 bytes
	
	std::stringstream ss;
	ss << thread_id;

	mg_logger_.debug(__FUNCTION__, "**************************************************************");
	mg_logger_.debug(__FUNCTION__, "Thread " + ss.str() + " Created to Recieve request from Client: " +
		std::to_string(client_num_));
	mg_logger_.debug(__FUNCTION__, "**************************************************************");

	while ((ret = recv(this->client_sd_, reinterpret_cast<void *>(msg), max_len, 0))) {

		std::string request(msg);
		mg_logger_.debug(__FUNCTION__, "Recieved: " + std::to_string(ret) + " byte from client: " + request);
		if (!ret) {
			mg_logger_.error(__FUNCTION__, "Failed to recieve request from client");
			ret = EXIT_FAILURE;
			return ret;
		}

		std::string response = get_response(request);
		mg_logger_.debug(__FUNCTION__, "Send response to client");

		try {
			//XXX: TODO: @MG Smart pointers
			ret = send(this->client_sd_, (void *)response.c_str(), strlen(response.c_str()), 0);
			if (!ret)
				throw ServerException(__FUNCTION__, "Failed to Send response to client");
		} catch(ServerException& e) {
			this->mg_logger_.error(__FUNCTION__, e.what());
			this->mg_logger_.warn(__FUNCTION__, strerror(errno));
			ret = EXIT_FAILURE;
			return ret;
		}

		this->mg_logger_.debug(__FUNCTION__, "Sent "+ std::to_string(ret) + " byte to client: " + response);
	}

	ret = EXIT_SUCCESS;
	return ret;
}
/* -------------------------------------------------------------------- */
/*                                 IO FUNCTIONS                         */
/* -------------------------------------------------------------------- */
std::string Server::get_payload_(const Key& k)
{
	std::string read_payload("\n"), payload;
	std::ifstream input_file;
	input_file.open(k);

	while (std::getline(input_file, payload))
		read_payload += payload;

	return read_payload;
}
int Server::get_payload(const Key& k, const Payload& d)
{
	ServerMap::iterator i;
	int ret = EXIT_FAILURE;
	Payload read_payload;
	(void)d;

	if ((i = this->sm_.find(k)) == this->sm_.end()) {

		mg_logger_.error(__FUNCTION__, "Something went wrong");
		return ret;
	}

	ret = EXIT_SUCCESS;

	return ret;
}
int Server::dequeue(const Key& k, const Payload& d)
{
	ServerMap::iterator i;
	int ret = EXIT_FAILURE;

	if ((i = this->sm_.find(k)) == this->sm_.end()) {

		mg_logger_.error(__FUNCTION__, "Something went wrong");
		return ret;
	}

	if (i != sm_.end()) {
		ret = file_io(k , d, ENUM_D);
		sm_.erase(k);
		ret = EXIT_SUCCESS;
	}

	return ret;
}
int Server::update(const Key& k, const Payload& d)
{
	ServerMap::iterator i;
	int ret = EXIT_SUCCESS;

	if ((i = this->sm_.find(k)) == this->sm_.end())
		ret = file_io(k , d, ENUM_W);

	if (i != sm_.end()) {

		mg_logger_.error(__FUNCTION__, "Something went wrong");
		return ret;
	} 
		

	return ret;
}
int Server::enqueue(const Key& k, const Payload& d)
{
	ServerMap::iterator i;
	int ret = EXIT_SUCCESS;

	if ((i = sm_.find(k)) == sm_.end()) {

		// Add file to system
		ret = file_io(k , d, ENUM_W);

		File f_name(k);
		sm_.emplace(k, f_name);
	}
	if (sm_.end() != i) {

		mg_logger_.error(__FUNCTION__, "Something went wrong");
		ret = EXIT_FAILURE;
	}

	return ret;
}
/* -------------------------------------------------------------------- */
/*                             FILE IO FUNCTION                         */
/* -------------------------------------------------------------------- */
int Server::file_io(const std::string& f_name, const Payload& d, int operation)
{
	std::ofstream f_file;
	int ret = EXIT_FAILURE;
	std::string payload;

	try {

		f_file.open(f_name);

		if (ENUM_R == operation) {

			std::ifstream input_file;

			while (std::getline(input_file, payload)) {

				Payload temp(payload.begin(), payload.end());
				
				//XXX: TODO: @MG: FIX ME!!!
			}
		} else if (ENUM_W == operation) {
			for (const auto &c : d)
				f_file << c;
		} else if (ENUM_D == operation) {
			f_file.close();
			if (std::remove(f_name.c_str()))
				throw FileException(__FUNCTION__, "Disk IO Error: Failed to delete file");
		}
	} catch (FileException& e) {

		mg_logger_.warn(__FUNCTION__, e.what());
		mg_logger_.error(__FUNCTION__, strerror(errno));
	} 
	
	f_file.close();
	
	ret = EXIT_SUCCESS;
	return ret;
}
/* -------------------------------------------------------------------- */
/*                             PUBLIC FUNCTIONS                         */
/* -------------------------------------------------------------------- */
int Server::main_loop(void)
{
	int ret;
	std::vector<std::thread> client_threads;

	try {

		ret = this->create_socket();
		if (ret)
			return ret;
		ret = this->bind_socket();
		if (ret)
			return ret;
		ret = this->listen_socket();
		if (ret)
			return ret;

		while ((ret = accept_connection()) > 0)
			client_threads.emplace_back([&]() {Server::parse_commands(std::this_thread::get_id(), ret);});
	} catch (const exception& e) {

		mg_logger_.warn(__FUNCTION__, e.what());
	}

	for (auto& th : client_threads)
			th.join();

	ret = EXIT_SUCCESS;
	return ret;
}

}