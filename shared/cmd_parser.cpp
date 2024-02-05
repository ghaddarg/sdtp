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
#include <fstream>

extern "C" {
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
}

#include <ftp_exceptions.hpp>
#include <logger.hpp>
#include <ftp_def.hpp>
#include <cmd_parser.hpp>

namespace mg_ftp 
{

/* -------------------------------------------------------------------- */
/*                             CON/DE-STRUCTOR                          */
/* -------------------------------------------------------------------- */
Parser::Parser(int& arg_num, char * argv[], Logger& mg_logger)
	: arg_num_(arg_num), port_(SERVER_PORT_NUM), ip_(LOCAL_HOST_IP), size_(0), mg_logger_(mg_logger)
{
	mg_logger_.debug(__FUNCTION__, "Parser Object Created");

	if (EXIT_SUCCESS != this->parse_args(argv)) {
		mg_logger_.error(__FUNCTION__, "Error parsing arguments");
		this->print_usage();
		return;
	}
}
Parser::~Parser()
{
	mg_logger_.debug(__FUNCTION__, "Parser Object Destroyed");
}

Parser::Parser(std::string& args, Logger& mg_logger)
	: arg_num_(args.size()), port_(SERVER_PORT_NUM), ip_(LOCAL_HOST_IP), size_(0), mg_logger_(mg_logger)
{
	mg_logger_.debug(__FUNCTION__, "Parser Object Created");

	// XXX: TODO: @MG: FIXME: args is string not vector
	try {
		if (this->arg_num_ < 2)
			throw ParserException(__FUNCTION__, "Num of Arguments is too small");
	} catch (ParserException& e) {

		mg_logger_.warn(__FUNCTION__, e.what());
		mg_logger_.error(__FUNCTION__, std::to_string(args.size()));
		this->print_usage();
		return;
	}

	this->cmd_ = args.substr(0, args.find(" "));
	args = args.substr(args.find_first_of(" ") + 1);
	this->key_ = args.substr(0, args.find(" "));

	mg_logger_.debug(__FUNCTION__, " CMD: " + this->cmd_);
	mg_logger_.debug(__FUNCTION__, " Key: " + this->key_);

	if (!is_valid_cmd(this->cmd_)) {

		mg_logger_.error(__FUNCTION__, this->cmd_ + "not a valid command");
		this->print_usage();
		return;
	}

	if (is_cmd_cu(this->cmd_)) {

		args = args.substr(args.find_first_of(" ") + 1);
		this->size_ = atoi(args.substr(0, args.find(" ")).c_str());

		args = args.substr(args.find_first_of(" ") + 1);
		std::vector<char> v(args.begin(), args.end());
		this->data_.insert(this->data_.end(), v.begin(), v.end());
		mg_logger_.debug(__FUNCTION__, std::to_string(this->size_));
	}

}

Parser::Parser(std::vector<std::string>& args, Logger& mg_logger)
	: arg_num_(args.size()), port_(SERVER_PORT_NUM), ip_(LOCAL_HOST_IP), size_(0), mg_logger_(mg_logger)
{
	int state = STATE_IP;
	std::string str;
	mg_logger_.debug(__FUNCTION__, "Parser Object Created");

	mg_logger_.debug(__FUNCTION__, std::to_string(this->arg_num_));

	try {
		if (this->arg_num_ < 2)
			throw ParserException(__FUNCTION__, "Num of Arguments is too small");
	} catch (ParserException& e) {

		mg_logger_.warn(__FUNCTION__, e.what());
		mg_logger_.error(__FUNCTION__, std::to_string(args.size()));
		this->print_usage();
		return;
	}

	for (std::vector<std::string>::iterator it = args.begin(); ; ) {

		switch (state)
		{
			case STATE_IP:

				if (is_valid_ip(*it)) {
					this->ip_ = *it;
					state = STATE_PORT;
					str = " IP: " + this->ip_;
				} else {
					this->cmd_ = *it;
					state = STATE_KEY;
					str = " CMD: " + this->cmd_;
				}
				mg_logger_.debug(__FUNCTION__, str);
				++it;

				break;

			case STATE_PORT:

				this->port_ = atoi((*it).c_str());
				mg_logger_.debug(__FUNCTION__, " Port: " + *it);
				state = STATE_CMD;
				it++;

				break;

			case STATE_CMD:

				this->cmd_ = *it;
				state = STATE_KEY;
				mg_logger_.debug(__FUNCTION__, " CMD: " + this->cmd_);

				if (!is_valid_cmd(this->cmd_)) {

					mg_logger_.error(__FUNCTION__, this->cmd_ + "not a valid command");
					this->print_usage();
					return;
				}

				++it;
				break;

			case STATE_KEY:

				this->key_ = *it;
				state = STATE_SIZE;
				mg_logger_.debug(__FUNCTION__, " Key: " + this->key_);
				++it;
				break;

			case STATE_SIZE:
			case STATE_DATA:

				if (is_cmd_cu(this->cmd_)) {
					std::string line("test");

					while(std::cin) {

						if (std::getline(std::cin, line)) {
							std::vector<char> v(line.begin(), line.end());
							this->data_.insert(this->data_.end(), v.begin(), v.end());
						}
					}

					this->size_ = data_.size();
					state = STATE_END;
					mg_logger_.debug(__FUNCTION__, " Size: " + std::to_string(this->size_));
					mg_logger_.debug(__FUNCTION__, " Data: ");
					print_data();					
				} else {
					return;
				}

				break;

			case STATE_END:

				return;
				break;
		}
	}

}
/* -------------------------------------------------------------------- */
/*                            PRIVATE FUNCTIONS                         */
/* -------------------------------------------------------------------- */
void Parser::print_usage(void)
{
	// Usage: 
	// mg_client.exe <ip-address> <port-number> <cmd> "key" SIZE DATA

	mg_logger_.debug(__FUNCTION__, "*** Error ***");
	mg_logger_.debug(__FUNCTION__,"Usage: ");
	mg_logger_.debug(__FUNCTION__,"1. mg_client.exe <ip-address> <port-number> <cmd> \"key\" SIZE DATA");
	mg_logger_.debug(__FUNCTION__,"2. mg_client.exe <cmd> \"key\" SIZE DATA");
}
bool Parser::is_valid_ip(const std::string& ip)
{
	struct sockaddr_in ip_address;
	return inet_pton(AF_INET, ip.c_str(), &ip_address.sin_addr) != 0;
}
bool Parser::is_cmd_rd(const std::string& cmd)
{
	return cmd == "read" || cmd == "delete";
}
bool Parser::is_cmd_cu(const std::string& cmd)
{
	return cmd == "create" || cmd == "update";
}
bool Parser::is_valid_cmd(const std::string& cmd)
{
	return cmd == "create" || cmd == "read" || cmd == "update" || cmd == "delete";
}
int Parser::parse_args(char * argv[])
{
	mg_logger_.debug(__FUNCTION__,"Parsing num args " + std::to_string(this->arg_num_));

	if (this->arg_num_ <= 2) {
		this->print_usage();
		return EXIT_FAILURE;
	} else if (this->arg_num_ == 3) {

		if (is_valid_ip(std::string(argv[1]))) {
			this->ip_.assign(argv[1], strlen(argv[1]));
			this->port_ = atoi(argv[2]);
		} else if (is_cmd_rd(std::string(argv[1]))) {
			this->cmd_ = std::string(argv[1]);
			this->key_ = std::string(argv[2]);
		} else {
			this->print_usage();
			return EXIT_FAILURE;
		}
	} else {
		int i = 1;

		if (is_valid_ip(std::string(argv[i]))) {
			this->ip_ = std::string(argv[i++]);
			this->port_ = atoi(argv[i++]);
		}

		if (is_valid_cmd(argv[i])) {

			this->cmd_ = std::string(argv[i++]);
			this->key_ = std::string(argv[i]);

			if (is_cmd_cu(this->cmd_)) {

				std::string line("test");

				while(std::cin) {

					if (std::getline(std::cin, line)) {
						std::vector<char> v(line.begin(), line.end());
						this->data_.insert(this->data_.end(), v.begin(), v.end());
					}
				}

				this->size_ = data_.size();
			}
		}	
	}

	 return EXIT_SUCCESS;
}
std::string Parser::construct_msg(void)
{
	std::string msg = this->get_cmd() + " " + this->get_key();
	if (this->get_size()) {

		msg.append(" ");
		msg.append(std::to_string(this->get_size()));
		msg.append(" ");

		for (std::vector<char>::iterator it = this->get_data().begin(); it != this->get_data().end(); ++it)
			msg += *it;
	}

	return msg;
}
/* -------------------------------------------------------------------- */
/*                             PUBLIC FUNCTIONS                         */
/* -------------------------------------------------------------------- */
std::string Parser::get_ip(void)
{
	return this->ip_;
}
const unsigned int& Parser::get_port(void)
{
	return this->port_;
}
std::string Parser::get_cmd(void)
{
	return this->cmd_;
}
std::string Parser::get_key(void)
{
	return this->key_;
}
const unsigned int& Parser::get_size(void)
{
	return this->size_;
}
std::vector<char>& Parser::get_data(void)
{
	return this->data_;
}
void Parser::print_data(void)
{
	for (std::vector<char>::iterator it = this->data_.begin(); it != this->data_.end(); ++it)
		std::cout << *it;

	std::cout << std::endl;
}

}