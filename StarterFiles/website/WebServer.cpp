/*###########################################################################################
File: WebServer.cpp

A super light weight HTTP server

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Notes:
1) Compiles for Winsock only since uses Windows threads. Generates no warnings.
2) Serves: htm, html, gif, pdf only. (may be some more, but it is limited)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Execution notes:
1) Execute this program in the directory which will be the root for
	all file references (i.e., the directory that is considered at "public.html").
2) Open a Web browser and surf to http://xxx.xxx.xxx.xxx:pppp/yyy where	xxx.xxx.xxx.xxx is
	the IP address or hostname of the machine that WebServer is executing on, pppp is the
	port port number, and yyy is the requested object.
3) Output (to console) from WebServer include files being sent, size of files, and how many
	chunks the files are being broken up into.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
WSAGetLastError() Codes:
https://msdn.microsoft.com/en-us/library/windows/desktop/ms740668(v=vs.85).aspx

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
History:	KJC (12/29/2000) - Genesis from (server.c)
			HF	(09/06/2003) - Ported to simple (WIN32 dedicated)
			JWB (02/24/2016) - Error control, cleanup, and changes to reading files

############################################################################################*/

/*~~~~~ INCLUDES ~~~~~~~~ INCLUDES ~~~~~~~~ INCLUDES ~~~~~~~~ INCLUDES ~~~~~~~~ INCLUDES ~~~*/
#include <iostream>         // Needed for printf()
#include <string.h>         // Needed for strcpy() and strlen()
#include <process.h>        // Needed for _beginthread() and _endthread()
#include <fstream>			// Needed for file input
#include <windows.h>        // Needed for all Winsock stuff

using namespace std;

/*~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~*/
#define MAX_BUFFER			4096    // Buffer size (big enough for a GET)
#define PORT_NUM			1080    // Port number for a Web server
#define MAX_CONNECTIONS		100		// max number of connections
#define FAV_ICON			"HTTP/1.0 favicon\nContent-Type:image/x-icon\n\n"
#define OK_IMAGE			"HTTP/1.0 200 OK\nContent-Type:image/gif\n\n"
#define OK_TEXT				"HTTP/1.0 200 OK\nContent-Type:text/html\n\n"
#define OK_PDF				"HTTP/1.0 200 OK\nContent-Type:application/pdf\n\n"
#define NOTOK_404			"HTTP/1.0 404 Not Found\nContent-Type:text/html\n\n"
#define MESS_404			"<html><body><h1>FILE NOT FOUND</h1></body></html>"

// make the linker include wsock32.lib
#pragma comment(lib, "wsock32.lib")

/*~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~*/
void ProcessGet(void *);       // Thread function to handle GET

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
int main()
{
	// initialize winsock...0=successful
	WSADATA	wsaData;	// windows socket...used for WSA functions

	// define sockets for proxy and remotes, and initialize them
	SOCKET serverSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	struct sockaddr_in serverAddr;	// server IP
	struct sockaddr_in clientAddr;	// client IP

	// lengths of the addresses
	int lengthServerAddr = sizeof(serverAddr);
	int lengthClientAddr = sizeof(clientAddr);

	char serverName[256];		// server name
	struct in_addr serverIP;	// server IP address

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// set color of console to Green and clear it
	system("Color E");
	system("cls");
	printf("Web Server\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// < 0 if error
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) < 0)
	{
		printf("WSA Initialization Failed: %ld\n\n", WSAGetLastError());
		system("pause");
		return -1;
	}
	printf("WSAStartup Successful...\n");

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// initialize server socket
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Socket Initialization Failed: %ld\n\n", WSAGetLastError());
		WSACleanup();
		system("pause");
		return -1;
	}
	printf("Socket Successful...\n");

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// set proxy IP Address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// < 0 if error
	if (bind(serverSocket, (struct sockaddr *) &serverAddr, lengthServerAddr) < 0)
	{
		printf("Bind Failed: %d\n\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		system("pause");
		return -1;
	}
	printf("Bind Successful...\n");

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// listen...less than 0 = error
	if (listen(serverSocket, MAX_CONNECTIONS) < 0)
	{
		printf("Listen Failed: %d\n\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		system("pause");
		return -1;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// display info about server
	gethostname(serverName, sizeof(serverName));

	// get IP address of server computer
	memcpy(&serverIP, gethostbyname(serverName)->h_addr_list[0], sizeof(struct in_addr));

	printf("%s (%s) - listening on port %d\n",
			serverName,
			inet_ntoa(serverIP),
			ntohs(serverAddr.sin_port));
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	while (true)
	{
		clientSocket = accept(serverSocket,
							  (struct sockaddr *)&clientAddr,
							  &lengthClientAddr);
		if (clientSocket < 0)
			printf("Accept Failed: %d\n\n", WSAGetLastError());

		// report to server console
		printf("*** Profile for a connecting client ***\n");
		printf("\tClient IP: %s, client TCP port: %d ..\n",
				inet_ntoa(clientAddr.sin_addr),
				ntohs(clientAddr.sin_port));

		// Spin-off a thread to handle this request (pass only client_s)
		if (_beginthread(ProcessGet, MAX_BUFFER, (void *)clientSocket) < 0)
		{
			printf("ERROR - Unable to create thread\n\n");
			closesocket(clientSocket);
		}
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// program will never get here, but just in case it does for some reason...we want to
	//	clean things up

	// system call to wait for user input before
	system("Color 0");
	system("cls");

	// Close the server socket and clean-up winsock
	closesocket(serverSocket);
	WSACleanup();

	// To make sure this "main" returns an integer.
	return 0;

} // main

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Function:	ProcessGet
Description:	this thread function is to handle GET requests
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
void ProcessGet(void *param)
{
	SOCKET		clientSocket = (SOCKET)param;	// Client socket descriptor
	char		sendBuf[MAX_BUFFER];			// output buffer
	char		recvBuf[MAX_BUFFER];			// input buffer
	char		*fileName = NULL;				// File name
	char		*tokens = NULL;					// For holding token info
	ifstream	fin;							// reading in the file that was requested
	int			streamSize;						// number of bytes actually transfered
	int			fileSize = 0;					// we need to keep track of the file size
	int			numChunks = 0;					// we want to display the # of chunks

	// Receive and handle the GET if there is one
	if ((streamSize = recv(clientSocket, recvBuf, MAX_BUFFER, 0)) > 0)
	{
		//printf("\n=====================================================================\n%.*s",
		//	streamSize, recvBuf);
		//printf("=====================================================================\n");

		// Parse the GET request...first line produces the tokens
		strtok_s(recvBuf, " ", &tokens);			// consume the GET
		fileName = strtok_s(NULL, " ", &tokens);	// next token is the file name

		// report on the file requested
		printf("\tRequested: %s\n", &fileName[1]);

		// Open the requested file
		//  - Start at 2nd char to get rid of leading '\'
		fin.open(&fileName[1], ios::in | ios::binary);

		// Generate and send the response (404 if could not open the file)
		if (fin)
		{ // file was successfully opened
			
			// figure out what type of file we are sending
			if (strstr(fileName, ".gif") != NULL)
				strcpy_s(sendBuf, OK_IMAGE);
			else if (strstr(fileName, "favicon.ico") != NULL)
				strcpy_s(sendBuf, FAV_ICON);
			else if (strstr(fileName, ".pdf") != NULL)
				strcpy_s(sendBuf, OK_PDF);
			else
				strcpy_s(sendBuf, OK_TEXT);

			// (< 0 if error)
			if (send(clientSocket, sendBuf, strlen(sendBuf), 0) < 0)
				printf("Send Failed: %d\n\n", WSAGetLastError());

			// loop through and send the file in chunks up to MAX_BUFFER size
			while (!fin.eof())
			{
				// read in from the file up to MAX_BUFFER size
				fin.read(sendBuf, MAX_BUFFER);
				
				// get the size that has just been read in from the file
				streamSize = (int)fin.gcount();

				// (< 0 if error)
				if (send(clientSocket, sendBuf, streamSize, 0) < 0)
					printf("Send Failed: %d\n\n", WSAGetLastError());
				else
				{ // successful send
					fileSize += streamSize;
					numChunks++;
				}

			} // while !eof

			// report on the file that has been sent: size and number of chunks
			printf("\tSent: %s (%d chunk(s) - %d Bytes)\n",
					&fileName[1],
					numChunks,
					fileSize);

			// close file...we are done reading it
			fin.close();
		}
		else
		{ // we could not open the file
			
			printf("\tFile %s not found - sending HTTP 404 \n", &fileName[1]);
			
			strcpy_s(sendBuf, NOTOK_404);
			
			// (< 0 if error)
			if (send(clientSocket, sendBuf, strlen(sendBuf), 0) < 0)
				printf("Send Failed: %d\n\n", WSAGetLastError());
			
			strcpy_s(sendBuf, MESS_404);
			
			// (< 0 if error)
			if (send(clientSocket, sendBuf, strlen(sendBuf), 0) < 0)
				printf("Send Failed: %d\n\n", WSAGetLastError());
		}
	}

	// Close the client socket and end the thread
	closesocket(clientSocket);
	_endthread();

} // ProcessGet

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/