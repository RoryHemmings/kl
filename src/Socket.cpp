#include <fstream>
#include <iostream>

#include "IO.h"
#include "Utils.h"
#include "Socket.h"

Socket::Socket()
{
	WSADATA wsa_data;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		IO::WriteAppLog("Failed to initialize winsock");
		WSACleanup();
		return;
	}
}

Socket::Socket(SOCKET sock)
	: sock(sock)
{ 
	if (sock == INVALID_SOCKET)
	{
		Close();
		return;
	}

	active = true;
}

int Socket::Send(size_t len, char* data) 
{
	int status = ::send(sock, (const char*)data, len, 0);	
	if (status == SOCKET_ERROR)
	{
		Close();
		WSACleanup();

		return FAILURE;
	}

	return SUCCESS;
}

size_t Socket::Recv(size_t buflen, char* in) const
{
	size_t len = ::recv(sock, in, buflen, 0);
	if (len < 0)
	{
		std::cout << "Recv failed. code: " << WSAGetLastError() << std::endl;
		return -1;
	}
	else if (len == 0)
	{
		std::cout << "Target Closed Connection" << std::endl;
		return 0;
	}

	return len;
}

void Socket::Close()
{
	closesocket(sock);
	WSACleanup();
	active = false;
}

ClientSocket::ClientSocket(const std::string& adress, int port)
{
	// Initialize Socket
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		IO::WriteAppLog("Failed to create socket. code: " + WSAGetLastError());
		WSACleanup();
		return;
	}

	struct sockaddr_in addr_in;
	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_addr.s_addr = inet_addr(adress.c_str());
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);

	if (connect(sock, (SOCKADDR*)&addr_in, sizeof(addr_in)) == SOCKET_ERROR)
	{
		IO::WriteAppLog("Failed to connect to server");
		WSACleanup();
		Close();
		return;
	}

	active = true;
}

/**
 *   Primary Packet Structure
 * 1 byte   uint8_t   status either, 1 or 2 (1 in this case)
 * 4 bytes  uint32_t  total packets
 * x bytes  cstring   filename
 */
size_t createPrimaryFtpPacket(uint32_t totalPackets, const std::string& filename, size_t buflen, char* out)
{
	Utils::ClearBuffer(buflen, out);

	size_t len = 0;
	size_t numReservedBytes = 1 + sizeof(totalPackets);

	// status byte (primary packet)
	out[0] = (uint8_t)1;
	len += 1;
	
	// append totalPackets
	memcpy(out+len, &totalPackets, sizeof(totalPackets));
	len += sizeof(totalPackets);

	// append filename (if filename is too long, it will be truncated
	strncpy(out+len, filename.c_str(), buflen - numReservedBytes);
	len += min(filename.size(), (size_t)buflen - numReservedBytes);

	return len;
}

/**
 * Secondary Packet Structure
 * 1 bytes  uint_8_t status, either 1 or 2 (2 in this case)
 * x bytes  char*    file data
 *
 * Make sure to shift the data over by one byte so that 
 * there is space for the status (much faster than copying 
 * entire buffers worth of data)
 */
size_t createFtpPacket(uint16_t dataLength, char* out)
{
	size_t len = 0;

	// Status byte (secondary packet)
	out[0] = (uint8_t)2;
	len += 1;

	// Apend dataLength
	memcpy(out + len, &dataLength, sizeof(dataLength));
	len += sizeof(dataLength);

	// Skip data that is already in the buffer
	len += dataLength;

	return len;
}

RESPONSE_CODE ClientSocket::SendFile(const std::string& path)
{
	const size_t buflen = 1024;
	char out[buflen];

	// Open the file
	std::ifstream infile(path, std::ios::binary);
	if (!infile.is_open())
	{
		IO::WriteAppLog("Failed to open file " + path + " locally");
		return FAILURE;
	}

	// Get the file size
	infile.seekg(0, infile.end);
	int fileSize = infile.tellg();
	infile.seekg(0, infile.beg);

	// Last packet = floor(filesize/amount per file) + 1
	// Last packet = number of passes required in decimal without decimal + an extra
	uint32_t totalPackets = floor(fileSize / (buflen - 3)) + 1;
	Send(createPrimaryFtpPacket(totalPackets, Utils::GetFilenameFromPath(path), buflen, out), out);

	while (!infile.eof())
	{
		Utils::ClearBuffer(buflen, out);

		RESPONSE_CODE response = recv();
		if (response != SUCCESS)
			return response;

		// Start at 3 to leave space for status byte and length
		size_t i = 3;
		while (i < buflen)
		{
			char c = (char)infile.get();
			if (infile.eof())
				break;

			out[i] = c;
			++i;
		}

		uint16_t length = i - 3;
		Send(createFtpPacket(length, out), out);
	}
	
	return SUCCESS;
}

/**
 * Since the only thing the client should ever receive is 
 * one of three numbers, we can store its response in a single 
 * uint8_t
 */
RESPONSE_CODE ClientSocket::recv()
{
	uint8_t* res_code = NULL;

	int status = ::recv(sock, (char*)res_code, 1, 0);
	if (status == 0)
	{
		IO::WriteAppLog("Attempted to read from closed socket");
		return FAILURE;
	}
	else if (status < 0)
	{
		IO::WriteAppLog("Unable to read from socket. code: " + WSAGetLastError());
		return FAILURE;
	}

	return (RESPONSE_CODE)(*res_code);
}

ServerSocket::ServerSocket(const std::string& port)
	: port(port)
{
	int res;

	struct addrinfo* result = NULL;
	struct addrinfo hints;
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if ((res = getaddrinfo(NULL, port.c_str(), &hints, &result)) != 0)
	{
		std::cout << "getaddrinfo failed. code: " << res << std::endl;
		Close();
		return;
	}

	if ((sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET)
	{
		std::cout << "socket failed. code: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		Close();
		return;
	}

	if ((res == bind(sock, result->ai_addr, (int)(result->ai_addrlen))) == SOCKET_ERROR)
	{
		std::cout << "bind failed. code: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		Close();
		return;
	}

	freeaddrinfo(result);

	if ((res = listen(sock, SOMAXCONN)) == SOCKET_ERROR)
	{
		std::cout << "listen failed. code: " << WSAGetLastError() << std::endl;
		Close();
		return;
	}

	active = true;
}

Socket ServerSocket::Accept()
{
	return ::accept(sock, NULL, NULL);
}

