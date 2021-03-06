#include <algorithm>

#include "Utils.h"

void Utils::ClearBuffer(size_t len, char* buffer)
{
	for (size_t i = 0; i < len; ++i)
	{
		buffer[i] = '\0';
	}
}

std::string Utils::GetFilenameFromPath(const std::string& path)
{
	size_t sep = path.find_last_of("\\");

	if (sep != std::string::npos)
		return path.substr(sep+1, path.size());
	else
		return path;
}

Utils::DateTime::DateTime()
{
	time_t ms;
	time(&ms);

	struct tm *info;
	info = localtime(&ms);

	D = info->tm_mday;
	m = info->tm_mon + 1;
	y = 1900 + info->tm_year;
	M = info->tm_min;
	H = info->tm_hour;
	S = info->tm_sec;
}

Utils::DateTime::DateTime(int D, int m, int y)
		: D(D), m(m), y(y), H(0), M(0), S(0) {}

Utils::DateTime::DateTime(int D, int m, int y, int H, int M, int S)
		: D(D), m(m), y(y), H(H), M(M), S(S) {}

Utils::DateTime Utils::DateTime::Now()
{
	return Utils::DateTime();
}

std::string Utils::DateTime::GetDateString() const
{
//	return std::string(D < 10 ? "0" : "") + ToString(D) +
//				 std::string(m < 10 ? ".0" : ".") + ToString(m) + "." +
//				 ToString(y);
	return std::string(m < 10 ? "0" : "") + ToString(m) +
		   std::string(D < 10 ? "-0" : "-") + ToString(D) + "-" +
		   ToString(y);


}

std::string Utils::DateTime::GetTimeString(const std::string& sep) const
{
	return std::string(H < 10 ? "0" : "") + ToString(H) + sep +
				 std::string(M < 10 ? "0" : "") + ToString(M) + sep +
				 std::string(S < 10 ? "0" : "") + ToString(S);
}

std::string Utils::DateTime::GetDateTimeString(const std::string& sep) const
{
	return GetDateString() + " " + GetTimeString(sep);
}

std::string Utils::DateTime::GetTimestamp() const
{
	//return std::string(D < 10 ? "0" : "") + ToString(D) +
	//			 std::string(m < 10 ? ".0" : ".") + ToString(m) + "." +
	//			 ToString(y) + std::string(H < 10 ? ".0" : ".") + ToString(H) +
	//			 std::string(M < 10 ? ".0" : ".") + ToString(M) +
	//			 std::string(S < 10 ? ".0" : ".") + ToString(S);
	return std::string(m < 10 ? "0" : "") + ToString(m) +
		   std::string(D < 10 ? "-0" : "-") + ToString(D) + "-" +	 
		   ToString(y) + 
		   std::string(H < 10 ? "_0" : "_") + ToString(H) +
		   std::string(M < 10 ? ".0" : ".") + ToString(M) +
		   std::string(S < 10 ? ".0" : ".") + ToString(S);

}
