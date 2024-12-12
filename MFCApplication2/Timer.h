#pragma once
#include <thread>
#include <Windows.h>

#define WM_MY_TIMEOUT WM_USER+100

class Timer
{
private:
	bool endTimer;
	bool endAlready;

	int timeout;
	int per_sleep;
	void count(HWND hWnd);		// 计时函数
	void test();
public:
	Timer();
	~Timer();
	void startTimer(HWND hWnd);	// 开启计时器
	void stopTimer();			// 关闭计时器
};

