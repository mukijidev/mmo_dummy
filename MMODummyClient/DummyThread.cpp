#include "DummyThread.h"
#include "Dummy.h"
#include <stdio.h>



DummyThread::DummyThread(int dummyNum, int actionDelay, int actionPercentage)
	: _actionDelay(actionDelay), _actionPercentage(actionPercentage)
{
	static int dummyId = 1;
	// 할거 : dummyNum에 맞게 dummy 생성

	for (int i = 0; i < dummyNum; i++)
	{
		Dummy* dummy = new Dummy(dummyId++);
		_dummyList.push_back(dummy);
	}

	/*_actionDelay = actionDelay;
	_actionPercentage = actionPercentage;*/
	// 쓰레드 생성
	_dummyThread = std::thread(&DummyThread::Run, this);
}

DummyThread::~DummyThread()
{
	_dummyRun = false;
	_dummyThread.join();
	for (auto dummy : _dummyList)
	{
		delete dummy;
	}
	_dummyList.clear();
}


void DummyThread::Run()
{
	// 스레드별로 srand설정
	srand(timeGetTime() + std::hash<std::thread::id>{}(std::this_thread::get_id()));

	// 스레드별로 랜덤시작
	int initialDelay = rand() % 1000; // 최대 1초 지연

	Sleep(initialDelay);
	while (_dummyRun)
	{
		for (auto dummy : _dummyList)
		{
			//dummy->Run();
		}

		Sleep(_actionDelay);
		NetworkRun();
		DummyRun();
	}

	return;
}

void DummyThread::NetworkRun()
{
	TIMEVAL tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_SET rest, wset;
	FD_ZERO(&rest);
	FD_ZERO(&wset);
	// dummy들 상대로 select돌리기
	int registeredNum = 0;
	
	for (auto dummy : _dummyList)
	{
		if (!dummy->bConnected)
		{
			//연결 안된애들은 넘어가고
			continue;
		}

		FD_SET(dummy->_socket, &rest);
		FD_SET(dummy->_socket, &wset);
		registeredNum++;
	}

	if (registeredNum == 0)
	{
		return;
	}
	
	int selectNum = 0;
	selectNum = select(0, &rest, &wset, NULL, &tv);
	if (selectNum == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"select failed : %d\n", errorCode);
		return;
	}

	if (selectNum == 0)
	{
		return;
	}

	for (auto dummy : _dummyList)
	{
		if (!dummy->bConnected)
			continue;

		if (FD_ISSET(dummy->_socket, &rest))
		{
			dummy->RecvProcess();
			//dummy->Recv();
		}

		if (!dummy->bConnected)
			continue;

		if (FD_ISSET(dummy->_socket, &wset))
		{
			dummy->SendProcess();
		}
	}
	

}

void DummyThread::DummyRun()
{
	for (auto dummy : _dummyList)
	{
		int percentageVal = rand() % 100;
		if (_actionPercentage >= percentageVal)
		{
			dummy->Run();
		}
	}
}

