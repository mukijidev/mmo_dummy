#pragma once

#include "Type.h"
#include <vector>
#include <thread>

class Dummy;

class DummyThread
{
public:
	DummyThread(int dummyNum, int actionDelay, int actionPercentage);
	~DummyThread();

private:
	void Run();
	void NetworkRun();
	void DummyRun();

private:
	std::vector<Dummy*> _dummyList;
	std::thread _dummyThread;
	int _actionDelay;
	bool _dummyRun = true;
	//0~100;
	int _actionPercentage;
};

