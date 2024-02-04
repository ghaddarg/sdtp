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

#include <exception>
#include <stdexcept>
#include <string>

namespace mg_ftp
{
/* ******************************************************************** */
/*                               CLASS DECLARATION                      */
/* ******************************************************************** */

class ServerException : public std::exception
{
public:
	ServerException(const std::string& msg) : std::runtime_error(msg) {}
	char * what() { return  "Server Exception. Something went wrong"; }
};

class ServerInvalidPortException : public ServerException
{
public:
	ServerInvalidPortException(const std::string &f_name) :
		ServerException(f_name + " Function: port num < 0 error") {}
};

class ServerInvalidSocketDescriptorException : public ServerException
{
public:
	ServerInvalidSocketDescriptorException(const std::string &f_name, const std::string &errno_) :
		ServerException(f_name + " Error establishing the server socket: " + errno_) {}
};

class ServerBindFailException : public ServerException
{
public:
	ServerBindFailException(const std::string &f_name, const std::string &errno_) :
		ServerException(f_name + " Error binding socket to local address: " + errno_) {}
};

class ServerListenFailureException : public ServerException
{
public:
	ServerListenFailureException(const std::string &f_name, const std::string &errno_) :
		ServerException(f_name + " Error binding socket to local address: " + errno_) {}
};

} //namespace mg