#include <iostream>
#include <string>

#include "Settings.h"
#include "Utils.h"
#include "Socket.h"

#define PORT

int receiveFile(const Socket& target)
{
	const size_t buflen = 1024;
	char buffer[buflen];
	
	size_t len = target.Recv(buflen, buffer);
	if (len <= 0)
		return len;

	std::string filename(buffer);
	std::cout << filename << std::endl;

	return 1;
}

int main(int argc, char** argv)
{
	int res;

	ServerSocket server(Utils::ToString(CC_PORT));
	if (!server.IsActive())
	{
		std::cerr << "Server failed to open on port " << CC_PORT << std::endl;
		return 1;
	}
	
	std::cout << "Server opened on port " << CC_PORT << std::endl;

	while (true)
	{
		Socket target = server.Accept();
		if (!target.IsActive())
		{
			std::cout << "Target failed to connect" << std::endl;
			target.Close();
			continue;
		}

		do
		{
			res = receiveFile(target);
		} while (res > 0);
	}

	return 0;
}

