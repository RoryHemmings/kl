#pragma once

#ifndef SOCKET_H
#define SOCKET_H

#include <string>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

enum RESPONSE_CODE
{
	FAILURE: 0,
	SUCCESS: 1,
	KILL: 2
};

/**
 * Windows Socket Wrapper class
 **/
class Socket
{

public:
	/**
	 * Initializes winsock, creates socket, and attempts
	 * to connect to server at adress on port
	 */
	Socket(const std::string& adress, int port);

	/** 
	 * Closes socket
	 */
	void Close();

	/**
	 * Sends data using internal socket
	 * and then blocks until a response is received
	 * 
	 * Then the response is returned in the form of a RESPONSE_CODE
	 */
	RESPONSE_CODE SendFile(const std::string& path); 

	bool IsActive() const { return active; }

private:
	WSADATA wsa_data;
	SOCKET sock;
	struct sockaddr_in addr_in;

	bool active = false;

	int send(size_t len, void* data);
	RESPONSE_CODE recv();
	

};

#endif