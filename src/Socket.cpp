#include <fstream>

#include "IO.h"
#include "Socket.h"

Socket::Socket(const std::string& adress, int port)
{
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		IO::WriteAppLog("Failed to initialize winsock");
		WSACleanup();
		return;
	}
	
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		IO::WriteAppLog("Failed to create socket. code: " + WSAGetLastError());
		WSACleanup();
		return;
	}

	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_addr.s_addr = inet_addr(adress.c_str());
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);

	if (connect(sock, (SOCKADDR*)&addr_in, sizeof(addr_in)) == SOCKET_ERROR)
	{
		IO::WriteAppLog("Failed to connect to server");
		WSACleanup();
		close();
		return;
	}

	active = true;
}

void Socket::Close()
{
	closesocket(sock);
	active = false;
}

RESPONSE_CODE Socket::SendFile(const std::string& path)
{
	std::ifstream infile(path, std::ios::binary);
	// Algorithm
	// at any time, if something goes wrong then return the error
	// that way, the error will be returned to the main handler function
	// and the error will be handled
	// if everything gets to the end and success is returned than that will
	// also be appropriately handled

}

int Socket::send(size_t len, void* data) 
{
	int status = ::send(sock, data, len, 0);	
	if (status == SOCKET_ERROR)
	{
		close();
		WSACleanup();

		return FAILURE;
	}

	return SUCCESS;
}

/**
 * Since the only thing the client should ever receive is 
 * one of three numbers, we can store its response in a single 
 * uint8_t
 */
RESPONSE_CODE Socket::recv()
{
	uint8_t* res_code = NULL;

	int status = ::recv(sock, res_code, 1, 0);
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

	return &res_code;
}
