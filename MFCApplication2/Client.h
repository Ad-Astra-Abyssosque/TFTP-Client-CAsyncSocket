#pragma once
#include <afxsock.h>
#include <iostream>
#include "telegram.h"
#include <fstream>
#include "Timer.h"



// 编写实现一个 TFTP 客户端程序，要求如下：
// 严格按照 TFTP 协议与标准 TFTP 服务器通信;
// 能够实现两种不同的传输模式 netascii 和 octet；
// 能够将文件上传到 TFTP 服务器;
// 能够从 TFTP 服务器下载指定文件；
// 能够向用户展现文件操作的结果：文件传输成功 / 传输失败;
// 针对传输失败的文件，能够提示失败的具体原因;
// 能够显示文件上传与下载的吞吐量;
// 能够记录日志，对于用户操作、传输成功，传输失败，超时重传等行为记录日志；
// 人机交互友好（图形界面 / 命令行界面均可）；
class Client :
    public CAsyncSocket
{
private:
	enum operation_code { Zero, Get, Put, Data, Ack, Error };
	
	wchar_t ServerIP[20];
	LPTSTR clientIP;
	UINT clientPort;
	UINT ServerDefaultPort;
	UINT ServerDialogPort;
	
	
	SYSTEMTIME time;
	SYSTEMTIME begin;
	SYSTEMTIME end;
	int fileSize;
	
	std::ofstream log;
	std::ofstream fout;		/*写入文件*/
	std::ifstream fin;		/*读文件*/
	struct LastPacket
	{
		char content[517];
		int len;
	}last_packet;
	
	int current_block;
	bool isEnd;

	void calcSpeed();
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	void printLog();
	LPWSTR ConvertCharToLPWSTR(const char* srcString);

public:
	Client();
	~Client();
	bool initSocket(wchar_t*);
	void sendAck(int block_num);
	void sendGet(char*);
	void sendPut(char*, char*);
	bool sendData(int block_num, int mode);
	void resend();
	void shutdown();
	int which_mode;
	CListBox* list;
	CEdit* speed_display;
	Timer timer;
	HWND hWnd;
	//Request request;
	

};

