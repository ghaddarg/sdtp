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

namespace mg_ftp
{

/* ******************************************************************** */
/*                               CLASS DECLARATION                      */
/* ******************************************************************** */

class Parser
{

enum state_ {
	STATE_IP,
	STATE_PORT,
	STATE_CMD,
	STATE_KEY,
	STATE_SIZE,
	STATE_DATA,
	STATE_END
};

public:

	/*!
	 * \brief Constructor.
	 */
	Parser(int& arg_num, char * argv[], Logger& mg_logger);
	Parser(std::string& args, Logger& mg_logger);
	Parser(std::vector<std::string>& args, Logger& mg_logger);
	~Parser();

	std::string get_ip(void);
	const unsigned int& get_port(void);
	std::string get_cmd(void);
	std::string get_key(void);
	const unsigned int& get_size(void);
	std::vector<char>& get_data(void);
	void print_data(void);
	std::string construct_msg(void);

private:

	bool is_valid_ip(const std::string& ip);
	bool is_cmd_rd(const std::string& cmd);
	bool is_cmd_cu(const std::string& cmd);
	bool is_valid_cmd(const std::string& cmd);
	void print_usage(void);

	int parse_args(char * argv[]);

	const unsigned int arg_num_;
	unsigned int port_;
	std::string ip_;
	std::string cmd_;
	std::string key_;
	unsigned int size_;
	// XXX: TODO: @MG Make this vector of strings
	std::vector<char> data_;
	Logger mg_logger_;
};

} //namespace mg