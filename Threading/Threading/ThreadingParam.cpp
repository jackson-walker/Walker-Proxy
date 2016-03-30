///*###########################################################################################
//File:		ThreadingParam.cpp
//
//Author:	John Broere
//Company:	Principia College
//Course:	CSCI 333: Computer Networking
//Date:		February 23, 2016
//
//Description: A sample program for multi-threaded applications.
//
//############################################################################################*/
//#include <iostream>		// used for system()
//#include <process.h>	// for thread calls (_beginthread, ect.)
//#include <time.h>		// used for clock(), CLOCKS_PER_SEC, localtime, struct tm, time_t
//
//using namespace std;
//
///*~~~~~ GLOBALS ~~~~~ GLOBALS ~~~~~ GLOBALS ~~~~~ GLOBALS ~~~~~ GLOBALS ~~~~~ GLOBALS ~~~~~~*/
//bool upThreadStatus;
//bool downThreadStatus;
//
///*~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~*/
//void upThread(void *);
//void downThread(void *);
//void Sleep(int);
//
///*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//int main()
//{
//	unsigned int counter;
//
//	upThreadStatus = true;
//	counter = 0;
//
//	_beginthread(upThread, 0, (void *)&counter);
//	
//	Sleep(3);
//
//	downThreadStatus = true;
//	counter = 10;
//
//	_beginthread(downThread, 0, (void *)&counter);
//
//	while (upThreadStatus || downThreadStatus)
//		;
//
//	// system call to wait for user input before
//	system("pause");
//
//	// return to the caller
//	return 0;
//
//} // main
//
//  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Function:	upThread
//  Description:	increment a counter
//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//void upThread(void *param)
//{
//	unsigned int counter = *(int*)param;
//
//	while (counter < 10)
//	{
//		++counter;
//		cout << "upThread: " << counter << endl;
//	}
//
//	upThreadStatus = false;
//
//	_endthread();
//
//} // upThread
//
//  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Function:	upThread
//  Description:	increment a counter
//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//void downThread(void *param)
//{
//	unsigned int counter = *(int*)param;
//
//	while (counter > 0)
//	{
//		--counter;
//		cout << "downThread: " << counter << endl;
//	}
//
//	downThreadStatus = false;
//
//	_endthread();
//
//} // upThread
//
//  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Function:	Sleep
//  Description:	sleep for a specified time in miliseconds
//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//void Sleep(int msec)
//{
//	// calculate the endTime we are looking for
//	clock_t endTime = clock() + msec * CLOCKS_PER_SEC / 1000;
//
//	// loop and do nothing until the endTime has been reached
//	while (clock() < endTime) {}
//
//} // Sleep
//
//  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F
//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/