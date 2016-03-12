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
#include "stdafx.h"		// used for misc fixes
#include <iostream>		// used for system()
#include <time.h>		// used for clock(), CLOCKS_PER_SEC, localtime, struct tm, time_t
#include <Windows.h>	// used for Winsock
#include <string>		// used for string functions


using namespace std;

/*~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~~~~~~ DEFINES ~~~~*/
#define DEFAULT_PORT		9050				// Default Server Port Number
#define MAX_BUFFER			100					// Maximum Buffer Size
#define MAX_CONNECTIONS		3					// Mazimum # of client connections
#define PROHIBITED_CLIENT	"10.200.134.18"		// used for testing on my own computer
#define _CRT_SECURE_NO_WARNINGS					// disable warning for time functions lol

// make the linker include wsock32.lib
#pragma comment(lib, "wsock32.lib")

/*~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~*/
void Sleep(int);								// sleep for a specified time in miliseconds

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
int main()
{
	WSADATA	wsaData;						// windows socket...used for WSA functions

	// define socket client
	SOCKET mySocket = INVALID_SOCKET;		// client socket

	struct	sockaddr_in		serverAddr;		// server IP
	struct	sockaddr_in		clientAddr;		// client IP


	char	sendBuf[MAX_BUFFER] = "";			// output buffer
	char	recvBuf[MAX_BUFFER] = "";			// input buffer

	int    buffer_size = MAX_BUFFER;

	time_t rawtime = time(NULL);	//time object
	//struct tm * timeinfo;			//tm struct

	//Set color and such
	system("Color 2");
	system("cls");
	printf("Proxy Server");
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

	serverAddr.sin_family = AF_INET;					// Address Family To Be Used
	serverAddr.sin_port = htons(int(DEFAULT_PORT));		// Port number to use --default 9050 --
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);		// server IP address

	//bind()
	if (bind(mySocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)	//bind socket to set local associations
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
	SOCKET clientSocket;	//create temporary socket object for incoming connections
	while (true)	//loop until temporary socket is connected
	{
		int clientLength = sizeof(clientAddr);	//get client length
		clientSocket = INVALID_SOCKET;	//create temporary return socket
		clientSocket = accept(mySocket, (struct sockaddr *)&clientAddr, &clientLength);	//set the temporary port to accept incoming connection, I don't fully understand the middle parameter
		if (clientSocket == INVALID_SOCKET)	//if failure,
		{
			printf("Server: Connection Error: %ld \n", WSAGetLastError());
			if (closesocket(clientSocket) < 0)	//if close fails
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
			if (send(clientSocket, "Prohibited IP, you've been rejected", MAX_BUFFER, 0)< 0)		//rejection
			{
				printf("Server: send() error: %ld \n", WSAGetLastError());	//if send fails
				return 0;
			}
			if (closesocket(clientSocket) < 0)	//if close fails
			{
				printf("Server: closesocket() error %ld \n", WSAGetLastError());
				return 0;
			}//close socket
		}
