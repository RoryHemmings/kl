#pragma once

#ifndef SEND_MAIL_H
#define SEND_MAIL_H

#include <string>
#include <vector>

namespace MailUtils
{
	// powershell script that sends the mail
	const std::string PowerShellScript;

	/**
	 * Sends email to and from account 
	 * defined in Settings.h
	 */
	int SendMail(const std::string& subject, const std::string& body, const std::string& attatchment);
	int SendMail(const std::string& subject, const std::string& body, const std::vector<std::string>& attatchments);
}

#endif
