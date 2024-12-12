#pragma once
#include <afxsock.h>
#include <iostream>
#include "telegram.h"
#include <fstream>
#include "Timer.h"



// ��дʵ��һ�� TFTP �ͻ��˳���Ҫ�����£�
// �ϸ��� TFTP Э�����׼ TFTP ������ͨ��;
// �ܹ�ʵ�����ֲ�ͬ�Ĵ���ģʽ netascii �� octet��
// �ܹ����ļ��ϴ��� TFTP ������;
// �ܹ��� TFTP ����������ָ���ļ���
// �ܹ����û�չ���ļ������Ľ�����ļ�����ɹ� / ����ʧ��;
// ��Դ���ʧ�ܵ��ļ����ܹ���ʾʧ�ܵľ���ԭ��;
// �ܹ���ʾ�ļ��ϴ������ص�������;
// �ܹ���¼��־�������û�����������ɹ�������ʧ�ܣ���ʱ�ش�����Ϊ��¼��־��
// �˻������Ѻã�ͼ�ν��� / �����н�����ɣ���
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
	std::ofstream fout;		/*д���ļ�*/
	std::ifstream fin;		/*���ļ�*/
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

