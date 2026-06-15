#include <iostream>
#include "Type.h"
#include "DummyThread.h"
using namespace std;
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "ws2_32.lib")



const int _dummyPerThread = 50;


void StartDummy()
{
	int dummyNum = 0;
	int actionDelay = 0;
	int actionPercentage = 0;
	cout << "Number of Dummy 1 ~ 5000 " << '\n';
	cin >> dummyNum;
	cout << "action Delay (ms) " << '\n';
	cin >> actionDelay;
	cout << "Action Percentage 0~ 100 " << '\n';
	cin >> actionPercentage;

	int fullThreadNum = dummyNum / _dummyPerThread;
	int dummyInLastThread = dummyNum % _dummyPerThread;
	
	for (int i = 0; i < fullThreadNum; i++)
	{
		DummyThread* dummyThread = new DummyThread(_dummyPerThread, actionDelay, actionPercentage);
	}

	DummyThread* dummyThread = new DummyThread(dummyInLastThread, actionDelay, actionPercentage);


	while (true)
	{
		Sleep(10000);
	}
}

int main()
{
	timeBeginPeriod(1);
	srand(timeGetTime());

	WSAData wsaData;
	int initSuccess;
	initSuccess = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (initSuccess != NO_ERROR)
	{
		__debugbreak();
	}

	
	StartDummy();

	WSACleanup();
	return 0;
}