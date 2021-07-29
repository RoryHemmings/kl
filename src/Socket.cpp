#include <fstream>
#include <iostream>

#include "IO.h"
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

RESPONSE_CODE ClientSocket::SendFile(const std::string& path)
{
	// std::ifstream infile(path, std::ios::binary);
	Send(path.size()+1, (char*)path.c_str());
	// Algorithm
	// at any time, if something goes wrong then return the error
	// that way, the error will be returned to the main handler function
	// and the error will be handled
	// if everything gets to the end and success is returned than that will
	// also be appropriately handled
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

