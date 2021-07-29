#ifndef SOCKET_H
#define SOCKET_H

// #undef _WINSOCKAPI_
// #define _WINSOCKAPI_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

enum RESPONSE_CODE
{
	FAILURE = 0,
	SUCCESS = 1,
	KILL	= 2
};

/**
 * Windows Socket Wrapper class
 **/
class Socket
{

public:
	/**
	 * Initializes Winsock
	 */
	Socket();

	// Conversion constructor from SOCKET
	Socket(SOCKET);

	/**
	 * Sends len amount of data to this socket
	 */
	int Send(size_t len, char* data);
	
	/**
	 * Blocks until able to read len bytes from internal socket
	 * Returns -1 on error,
	 * 0 on socket close,
	 * and length otherwise
	 */
	size_t Recv(size_t len, char* in) const;

	/** 
	 * Closes socket
	 */
	void Close();

	bool IsActive() const { return active; }

protected:
	SOCKET sock;

	bool active = false;

};

class ClientSocket : public Socket
{

public:
	/**
	 * Initializes winsock, creates socket, and attempts
	 * to connect to server at adress on port
	 */
	ClientSocket(const std::string& adress, int port);

	/**
	 * Sends data using internal socket
	 * and then blocks until a response is received
	 * 
	 * Then the response is returned in the form of a RESPONSE_CODE
	 */
	RESPONSE_CODE SendFile(const std::string& path); 



private:
	RESPONSE_CODE recv();

};

class ServerSocket : public Socket
{

public:
	ServerSocket(const std::string& port);
	Socket Accept();

private:
	std::string port;

};

#endif