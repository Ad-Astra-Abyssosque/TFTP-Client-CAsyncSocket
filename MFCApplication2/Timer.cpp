#include "pch.h"
#include "Timer.h"

Timer::Timer()
{
	endTimer = false;
	endAlready = false;
	timeout = 750;
	per_sleep = 10;
}

Timer::~Timer()
{

}

void Timer::test()
{
	MessageBoxA(NULL, "test", NULL, MB_OK);
}

void Timer::count(HWND hWnd)
{
	int n = timeout / per_sleep;
	for (int i = 0; i < n; i++)
	{
		Sleep(per_sleep);
		if (endTimer == true)
		{
			break;
		}
	}
	if (endTimer == false)
	{
		::PostMessage(hWnd, WM_MY_TIMEOUT, NULL, NULL);
	}
	endAlready = true;
}

void Timer::startTimer(HWND hWnd)
{
	endTimer = false;
	endAlready = false;
	std::thread th(&Timer::count,this,hWnd);
	//std::thread th(&Timer::test, this);
	th.detach();
	
}

void Timer::stopTimer()
{
	endTimer = true;
	while (!endAlready)
	{
		Sleep(10);
	}
}