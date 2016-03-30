///*###########################################################################################
//File:		Threading.cpp
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
//unsigned int upCounter;
//unsigned int downCounter;
//bool upThreadStatus;
//bool downThreadStatus;
//
///*~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~~~~ PROTOTYPES ~~~~~~~*/
//void upThread(void *);
//void downThread(void *);
//void Sleep(int );
//
///*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N  **  M A I N
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//int main()
//{
//	upCounter = 0;
//	downCounter = 0xFFFFFFFF;
//
//	upThreadStatus = true;
//	downThreadStatus = true;
//
//	_beginthread(upThread, 0, NULL);
//	_beginthread(downThread, 0, NULL);
//
//	while (upThreadStatus || downThreadStatus)
//	{
//		system("cls");
//		cout << "Up Counter: " << upCounter << endl;
//		cout << "Down Counter: " << downCounter;
//		Sleep(1000);
//	}
//
//	system("cls");
//	cout << "Up Counter: " << upCounter << endl;
//	cout << "Down Counter: " << downCounter;
//
//	cout << endl;
//
//	// system call to wait for user input before
//	system("pause");
//
//	// return to the caller
//	return 0;
//
//} // main
//
///*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function:	upThread
//Description:	increment a counter
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//void upThread(void *param)
//{
//	while (upCounter < 0xFFFFFFFF)
//		++upCounter;
//
//	upThreadStatus = false;
//
//	_endthread();
//
//} // upThread
//
///*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function:	upThread
//Description:	increment a counter
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
//void downThread(void *param)
//{
//	while (downCounter > 0)
//		--downCounter;
//
//	downThreadStatus = false;
//
//	_endthread();
//
//} // upThread
//
///*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function:	Sleep
//Description:	sleep for a specified time in miliseconds
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
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
///*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F  **  E  O  F
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/