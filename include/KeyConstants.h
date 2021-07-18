#pragma once

#ifndef KEY_CONSTANTS_H
#define KEY_CONSTANTS_H

#include <map>
#include <string>
#include <stdexcept>
#include <windows.h>

struct KeyPair
{
	// TODO try compiling without this line
	KeyPair(const std::string& vk="", const std::string& name="") : VKName(vk), Name(name) { }

	std::string VKName;
	std::string Name;
};

class Keys
{
public:
	static const std::map<int, KeyPair> KEYS;
	static inline std::string GetKey(DWORD code)
	{
		try {
			return Keys::KEYS.at(code).Name;
		} catch (const std::out_of_range& e) {
			return "[UNKNOWN]";
		}
	}
};

#endif
