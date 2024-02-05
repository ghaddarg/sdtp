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
#include <iostream>

namespace mg_ftp
{
/* ******************************************************************** */
/*                               CLASS DECLARATION                      */
/* ******************************************************************** */

class ServerException : public std::exception
{
protected:
	std::string msg_;
public:
	ServerException(const std::string &f_name, const char * msg) : msg_(f_name + "(): " + msg) {}
	const char * what() const noexcept { return this->msg_.c_str(); }// "Server Exception. Something went wrong"; }
};

class ClientException : public ServerException
{
public:
	ClientException(const std::string &f_name, const char * msg) : ServerException(f_name, msg) {}
};

class FileException : public ServerException
{
public:
	FileException(const std::string &f_name, const char * msg) : ServerException(f_name, msg) {}
};

/*
class ServerListenFailureException : public ServerException
{
public:
	ServerListenFailureException(const std::string &f_name, const std::string &errno_) :
		ServerException(f_name + " Error binding socket to local address: " + errno_) {}
};
*/
} //namespace mg