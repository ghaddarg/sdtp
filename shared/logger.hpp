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

#include <iostream>
#include <string>
#include <sstream>

namespace mg_ftp
{
enum level {
		NONE,
		INFO,
		WARN,
		ERROR
	};
/* ******************************************************************** */
/*                               CLASS DECLARATION                      */
/* ******************************************************************** */

class Logger
{

public:

	/*!
	 * \brief Constructor.
	 */
	Logger(level l) : msg_level_(l) { }
	~Logger() { }

	void debug(std::string func, std::string s)
	{
		if (msg_level_ >= INFO) 
			debug_log(s, "INFO: " + func + "(): "); 
	}
	void warn(std::string func, std::string s)
	{
		if (msg_level_ >= WARN) 
			debug_log(s, "WARNING: " + func + "(): ");
	}
	void error(std::string func, std::string s)
	{
		if (msg_level_ >= ERROR)
			debug_log(s, "ERROR: " + func + "(): ");
	}

private:
	void debug_log(std::string& s, std::string l)
	{
		std::stringstream ss;
		ss << l << s;
		std::cout << ss.str() << std::endl;
	}

	unsigned int msg_level_;	
};

} //namespace mg