/*
File: Proxy.cpp

Author : Jackson Walker
Company : Principia College
Course : CSCI 333 - Computer Networking
Date : March 5, 2016

Description : Simple proxy to forward requests

WSAGetLastError() Codes :
https ://msdn.microsoft.com/en-us/library/windows/desktop/ms740668(v=vs.85).aspx

############################################################################################*//*###########################################################################################


/*~~~~~ INCLUDES ~~~~~~~~ INCLUDES ~~~~~~~~ INCLUDES ~~~~~~~~ INCLUDES ~~~~~~~~ INCLUDES ~~~*/
#include <iostream>		// used for system()
#include <time.h>		// used for clock(), CLOCKS_PER_SEC, localtime, struct tm, time_t
#include <Windows.h>	// used for Winsock
#include <string>		// used for string functions
#include <fstream>		// used for file input
#include <process.h>	// used for threading 
#include <stdio.h>		// why is printf breaking


using namespace std;

/*~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~*/
#define DEFAULT_PORT		9050				// Default Server Port Number
#define SERVER_PORT			1080				// Default for the actual server
#define MAX_BUFFER			100					// Maximum Buffer Size
#define MAX_CONNECTIONS		3					// Mazimum # of client connections
#define PROHIBITED_CLIENT	"10.130.35.174"		// used for testing on my own computer
#define DEFAULT_SERVER		"10.130.35.173"		// used for testing on own computer
#define _CRT_SECURE_NO_WARNINGS					// disable warning for time functions lol
#define FAV_ICON			"HTTP/1.0 favicon\nContent-Type:image/x-icon\n\n"
#define FAIL_IMAGE			"HTTP/1.0 200 OK\nContent-Type:image/jpg\n\n"
#define MESS_403			"<html><body><h1>403 ACCESS DENIED</h1></body></html>"
#define NOTOK_404			"HTTP/1.0 404 Not Found\nContent-Type:text/html\n\n"
#define MESS_404			"<html><body><h1>FILE NOT FOUND</h1></body></html>"
#define OK_IMAGE			"HTTP/1.0 200 OK\nContent-Type:image/gif\n\n"
#define OK_TEXT				"HTTP/1.0 200 OK\nContent-Type:text/html\n\n"
#define OK_PDF				"HTTP/1.0 200 OK\nContent-Type:application/pdf\n\n"


// make the linker include wsock32.lib
#pragma comment(lib, "wsock32.lib")

/*~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~*/
void Sleep(int);								// sleep for a specified time in miliseconds
void service403(void *);
void serviceFromClient(void *);
void serviceFromServer(void *);

/*################################# GLOBAL SOCKETS BECAUSE FUCK MULTIPLE PARAMETERS IN BEGIN THREAD ###############################################*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
int main()
{
	WSADATA	wsaData;						// windows socket...used for WSA functions

	// define socket client
	SOCKET mySocket = INVALID_SOCKET;		// client socket

	struct	sockaddr_in		proxyAddr;		// self server IP
	struct	sockaddr_in		clientAddr;		// client IP
	struct  sockaddr_in		serverAddr;		// server IP info declared globally


	struct Sockets
	{
		SOCKET serverSocket = INVALID_SOCKET;	//create connection socket for server
		SOCKET proxySocket = INVALID_SOCKET;
		SOCKET clientSocket = INVALID_SOCKET;
	} sockets;

	char	sendBuf[MAX_BUFFER] = "";			// output buffer
	char	recvBuf[MAX_BUFFER] = "";			// input buffer

	int    buffer_size = MAX_BUFFER;

	time_t rawtime = time(NULL);	//time object
	//struct tm * timeinfo;			//tm struct

	//Set color and such
	system("Color 2");
	system("cls");
	printf("Walker Proxy Server");
	printf("\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	printf("\n");

	//Ask for new prohibited IP
	printf("Default Prohibited Client IP Address: %s\n", PROHIBITED_CLIENT);
	printf("Please enter '1' for default or new IP: ");
	string sNewProhibitedIP;
	cin >> sNewProhibitedIP;
	if (sNewProhibitedIP == to_string(1))
		sNewProhibitedIP = PROHIBITED_CLIENT;
	printf("Prohibited IP address is: %s\n", sNewProhibitedIP.c_str());	//output new prohibited IP
	
	printf("Default Server IP Address: %s\n", DEFAULT_SERVER);
	printf("Please enter '1' for default or new IP: ");
	string sNewServerIP;
	cin >> sNewServerIP;
	if (sNewServerIP == to_string(1))
		sNewServerIP = DEFAULT_SERVER;
	printf("Server IP address is: %s\n", sNewServerIP.c_str());	//output new server IP

	//startup winsock
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) < 0)
	{
		printf("WSA Initialization Failed: %ld\n\n", WSAGetLastError());	//if error
		return 0;
	}
	else
		printf("WinSock initialization successful! \n");

	//socket()
	mySocket = socket(AF_INET, SOCK_STREAM, 0);	//create socket using ip v4 and tcp 

	if (mySocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());		//If socket creation is invalid, end Wnsock
		WSACleanup();
		return 0;														//Thanks to TenoUK for this socket related test
	}
	else
		printf("Socket creation successful! \n");

	/* Lingerring Socket - Important for multiple requests                  */
	LINGER ling;       // Create the LINGER object
	ling.l_onoff = 1;  // Linger = ON
	ling.l_linger = 0; // Hard disconnect
	if (setsockopt(mySocket, SOL_SOCKET, SO_LINGER, (LPSTR)&ling, sizeof(ling)) < 0)
	{
		printf("Unsuccessfull linger setting .....\n");
		system("pause"); // system call to wait for user input
		return 1;
	}

	//set ip address
	//Set values for serverAddr for the bind

	proxyAddr.sin_family = AF_INET;					// Address Family To Be Used
	proxyAddr.sin_port = htons(int(DEFAULT_PORT));		// Port number to use --default 9050 --
	proxyAddr.sin_addr.s_addr = htonl(INADDR_ANY);		// server IP address

	//bind()
	if (bind(mySocket, (SOCKADDR*)&proxyAddr, sizeof(proxyAddr)) == SOCKET_ERROR)	//bind socket to set local associations
	{
		printf("bind() failed: %ld.\n", WSAGetLastError());							//again, thanks to TenoUK for this bind test
		if (closesocket(mySocket) < 0)	//if close fails
		{
			printf("Server: closesocket() error %ld \n", WSAGetLastError());
			return 0;
		}//close socket
		return 0;
	}
	else
		printf("Bind() successful \n");	//return success message!

	//listen()
	if (listen(mySocket, MAX_CONNECTIONS) == SOCKET_ERROR)	//listen on server socket for incoming TCP connections
	{
		printf("error listening on socket: %ld.\n", WSAGetLastError());
		if (closesocket(mySocket) < 0)	//if close fails
		{
			printf("Server: closesocket() error %ld \n", WSAGetLastError());
			return 0;
		}//close socket
		return 0;
	}
	else
		printf("Listening on port %d\n", DEFAULT_PORT);	//return successful listen

	//accept()
	while (true)	//loop until temporary socket is connected
	{
		int clientLength = sizeof(clientAddr);	//get client length
		sockets.clientSocket = INVALID_SOCKET;	//create temporary return socket
		sockets.clientSocket = accept(mySocket, (struct sockaddr *)&clientAddr, &clientLength);	//set the temporary port to accept incoming connection, I don't fully understand the middle parameter
		if (sockets.clientSocket == INVALID_SOCKET)	//if failure,
		{

			printf("Server: Connection Error: %ld \n", WSAGetLastError());
			if (closesocket(sockets.clientSocket) < 0)	//if close fails
			{
				printf("Server: closesocket() error %ld \n", WSAGetLastError());
				return 0;
			}//close socket
			return 0;
		}
		else
			printf("A connection request has arrived! \n");	//let user know that client is connected

		string clientIP = inet_ntoa(clientAddr.sin_addr);	//get client ip which was assigned in the accept
		if (clientIP.compare(sNewProhibitedIP) == 0)	//if comparison is true (prohib is client)
		{
			_beginthread(service403, 0, (void *)sockets.clientSocket);
			//Sleep(10);
			printf("Created new thread to service 403 request \n");
		}//if
		else
		{
			if ((sockets.serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)	//if the socket fails
			{
				printf("Could not create socket : %d \n", WSAGetLastError());
			}
			printf("Socket Created. \n");
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_addr.s_addr = inet_addr(DEFAULT_SERVER);	//assign information to server Addr object
			serverAddr.sin_port = htons(SERVER_PORT);

			if (connect(sockets.serverSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) < 0)		//connection instructions from binary tides
			{
				printf("connect error: %d \n", WSAGetLastError());	//failure
				system("PAUSE");
			}

			printf("Connected \n");	//connected 
			//_beginthread(serviceFromClient, 0, (void *)sockets.clientSocket);	//call service from client to pass messages from client to 
			//rest of program here please

		} Sleep(10);
	}//while
}//main

void service403(void *param)
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
	string		whatSent;						//what did we send?

	printf("Thread ID: %i opened \n", _getpid());

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

		// figure out what type of file we are sending
		if (strstr(fileName, "frown.jpg") != NULL)
			strcpy_s(sendBuf, FAIL_IMAGE);

		else if (strstr(fileName, "favicon.ico") != NULL)
			strcpy_s(sendBuf, FAV_ICON);
		else
		{
			strcpy_s(sendBuf, OK_TEXT);
			fin.open("http_403.html", ios::in | ios::binary);	//open 403 if it's not a jpg or ico
			fileName = "http_403.html";
		}

		if (!fin.is_open())
		{
			fin.open(&fileName[1], ios::in | ios::binary);
			
		}

		// Open the requested file
		//  - Start at 2nd char to get rid of leading '\'
		
		// Generate and send the response (404 if could not open the file)
		if (fin)
		{ // file was successfully opened						
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
				fileName[1],
				numChunks,
				fileSize);

			// close file...we are done reading it
			fin.close();
		}//end if
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
	}//endif

	// Close the client socket and end the thread
	printf("Closing thread ID: %i \n ############################################################################### \n", _getpid());
	closesocket(clientSocket);
	_endthread();

}

void Sleep(int msec)
{
	// calculate the endTime we are looking for
	clock_t endTime = clock() + msec * CLOCKS_PER_SEC / 1000;

	// loop and do nothing until the endTime has been reached
	while (clock() < endTime) {}

} // Sleep

/*
//Handles recv from client and forwards requests to server
void serviceFromClient(void *param)
{
	Sockets sockets = *(Sockets*)param;			// client socket descriptor
	char		sendBuf[MAX_BUFFER];			// output bufferi
	char		recvBuf[MAX_BUFFER];			// input buffer
	char		*fileName = NULL;				// File name
	char		*tokens = NULL;					// For holding token info
	int			streamSize;						// number of bytes actually transfered
	int			fileSize = 0;					// we need to keep track of the file size
	int			numChunks = 0;					// we want to display the # of chunks

	printf("serviceFromClient thread successfully initialized id: %i \n", _getpid());// debug for breaking
	int loopInt = 0;
	// Receive GET and transfer
	while ((streamSize = recv(clientSocket, recvBuf, MAX_BUFFER, 0)) > 0)
	{
		string saveBuf = recvBuf;
		strtok_s(recvBuf, " ", &tokens);			// consume the GET
		fileName = strtok_s(NULL, " ", &tokens);	// next tokebn is the file name
		printf("\tRequested from server: %s\n", &fileName[1]);	//print request

		if (send(serverSocket, saveBuf.c_str(), streamSize, 0) < 0)
			printf("Send Failed: %d\n\n", WSAGetLastError());
		else
		{ // successful send 
			loopInt++;
			printf("%d send \n", loopInt);
			fileSize += streamSize;
			printf("filesize inc, filesize: %d \n", fileSize);
			numChunks++;	
			printf("numchunks inc numchunks = %d", numChunks);
		}// end ifelse
							//DIES SOMEWHERE IN HERE
		printf("\tSent: %s (%d chunk(s) - %d Bytes to server)\n",	//send math for sending to sever
			fileName[1],
			numChunks,
			fileSize);
	}//end while
	closesocket(serverSocket);
	_endthread();
}
*/
//Handles recv from server and forwards data to client
void serviceFromServer(void *param)
{

}