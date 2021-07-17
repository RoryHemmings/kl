#pragma once

#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <windows.h>

namespace IO
{
	bool MkOneDir(const std:string& path);
	bool MkDir(std::string path);
	template <class T> std::string WriteLog(const T& t);

	std::string GetOutputPath(const bool append_seperator=false);
	std::vector<std::string> GetAttatchments(const std::string& path);
}

#endif
