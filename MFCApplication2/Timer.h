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
	void count(HWND hWnd);		// ��ʱ����
	void test();
public:
	Timer();
	~Timer();
	void startTimer(HWND hWnd);	// ������ʱ��
	void stopTimer();			// �رռ�ʱ��
};

