/*****************************************************/
/*
 * This is header file
 *
 * Created by: @JbrilCC++
 *
 * The Content of this belongs to DLG Inc.
 * For any questions email: ghaddama@dlgnetworks.com
 * 
 */
/*****************************************************/
#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>

namespace mg_ftp
{

/* ******************************************************************** */
/*                               CLASS DECLARATION                      */
/* ******************************************************************** */

class Server
{
	enum operation {
		ENUM_R,
		ENUM_W,
		ENUM_D
	};

public:
	typedef std::string Key;
	typedef std::string File;
	typedef std::vector<char> Payload;
	typedef std::unordered_map<Key, File> ServerMap;

	typedef std::function<int(const Key&, const Payload&)> CallbackFunction;
	typedef std::unordered_map<Key, CallbackFunction> CmdMap;

public:

	/*!
	 * \brief Constructor.
	 */
	Server(const unsigned int& port_num, const unsigned int& max_client, Logger& mg_logger);
	~Server();

	/*!
	 * \brief main loop.
	 * 
	 * \returns EXIR_SUCCESS for SUCCESS else FAIL;
	 */
	int main_loop(void);

private:
	
	int create_socket(void);
	int accept_connection(void);
	int bind_socket(void);
	int listen_socket(void);

	int parse_commands(std::thread::id thread_id, int client_num_);
	std::string get_response(std::string request);

	int file_io(const std::string& f_name, const Payload& d, int operation);
	int enqueue(const Key& k,const Payload& d);
	int dequeue(const Key& k,const Payload& d);
	int get_payload(const Key& k,const Payload& d);
	std::string get_payload_(const Key& k);
	int update(const Key& k,const Payload& d);

	Logger mg_logger_;
	const unsigned int port_num_;
	const unsigned int max_client_;
	int server_fd_;
	int client_sd_;
	sockaddr_in server_addr_;
	ServerMap sm_;
	CmdMap cf_;
	std::mutex m;
};

} //namespace mg