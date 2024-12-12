#include "pch.h"
#include "Client.h"

using namespace std;

Client::Client()
{
	log.open(".\\debug.log",ios_base::out);
	ServerDialogPort = 0;
	ServerDefaultPort = 69;
	which_mode = 1;
	fileSize = 0;
	current_block = 0;
	isEnd = false;
}

Client::~Client()
{
	log.close();
}


bool Client::initSocket(wchar_t* ip)
{
	
	wcscpy_s(this->ServerIP, ip);
	if (Create(0, SOCK_DGRAM, 63L, ip) == 0)
	{
		return false;
	}
	return true;
}




/*
* ����Ack��Ӧ����
*/
void Client::sendAck(int block_num)
{	
	// ��¼��־
	GetSystemTime(&time);
	char temp[200] = { '\0' };
	sprintf_s(temp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]    Send Ack: [block number:%d]\n",
		time.wYear, time.wMonth, time.wDay, 
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, block_num);
	
	LPWSTR record = ConvertCharToLPWSTR(temp);
	list->AddString(record);
	delete record;

	// ���챨��
	char content[4] = { 0 };
	content[1] = Ack;
	content[3] = block_num & 0x00ff;
	block_num = block_num >> 8;
	content[2] = block_num & 0x00ff;
	// ����ð��������ط�
	memcpy_s(last_packet.content, 517, content, 4);
	last_packet.len = 4;

	// ���ͱ���
	SendTo(content, 4, this->ServerDialogPort, this->ServerIP);
	log.write(temp, strlen(temp));
	timer.startTimer(hWnd);
}

/* 
* ���Ͷ��ļ�����RRQ�����ļ�������¼��־
* ��¼��ʼ����ʱ��
* mode:	1Ϊnetascii��2Ϊoctet
*/
void Client::sendGet(char* fileName)
{
	// ��¼��־
	fout.open(fileName,ios_base::binary);
	GetSystemTime(&begin);
	GetSystemTime(&time);
	char temp[200] = { '\0' };
	sprintf_s(temp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]    Send Get: [FileName:%s] [mode:%d]\n",
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, fileName, which_mode);
	log.write(temp, strlen(temp));
	LPWSTR record = ConvertCharToLPWSTR(temp);
	list->AddString(record);
	delete record;
	
	// send RRQ
	char context[100] = { '\0' };
	context[1] = Get;
	int len = 2;
	
	strcpy_s(context + 2, 100-len, fileName);
	len = len + strlen(fileName) + 1;
	if (which_mode == 1)
	{
		strcpy_s(context + len, 100 - len, "netascii\0");
		len = len + strlen("netascii\0") + 1;
	}
	else
	{
		strcpy_s(context + len, 100 - len, "octet\0");
		len = len + strlen("octet\0") + 1;
	}
	// ����ð��������ط�
	memcpy_s(last_packet.content, 517, context, len);
	last_packet.len = len;
	SendTo(context, len, 69, this->ServerIP, 0);
	timer.startTimer(hWnd);
	return;
}


/*
* ����д�ļ�����WRQ�����ļ�������¼��־
* ��¼��ʼ����ʱ��
* mode:	1Ϊnetascii��2Ϊoctet
*/
void Client::sendPut(char* filePath, char* fileName)
{
	fin.open(filePath,ios_base::binary);
	GetSystemTime(&begin);
	GetSystemTime(&time);
	char temp[200] = { '\0' };
	sprintf_s(temp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]    Send Put: [FileName:%s] [mode:%d]\n",
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, fileName, which_mode);
	log.write(temp, strlen(temp));
	LPWSTR record = ConvertCharToLPWSTR(temp);
	list->AddString(record);
	delete record;
	// send WRQ
	char context[100] = { '\0' };
	context[1] = Put;
	int len = 2;
	strcpy_s(context + 2, 100 - len, fileName);
	len = len + strlen(fileName) + 1;
	if (which_mode == 1)
	{
		strcpy_s(context + len, 100 - len, "netascii\0");
		len = len + strlen("netascii\0") + 1;
	}
	else
	{
		strcpy_s(context + len, 100 - len, "octet\0");
		len = len + strlen("octet\0") + 1;
	}
	// ����ð��������ط�
	memcpy_s(last_packet.content, 517, context, len);
	last_packet.len = len;
	SendTo(context, len, 69, this->ServerIP, 0);
	timer.startTimer(hWnd);
}


/*
* �������ݱ��ģ�����¼��־
* �ۼ��ļ���СfileSize
* block_num:���ݱ��ı��
* resend:	�Ƿ�Ϊ�ط�����
*			���Ϊ�ط����ģ�������һ������
*			���򣬶��µ��ļ����ݲ�����
*/
bool Client::sendData(int block_num, int resend)
{
	GetSystemTime(&time);
	char temp[200] = { '\0' };
	
	if (resend == 1)
	{
		sprintf_s(temp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]    Resend Data: [block number:%d]\n",
			time.wYear, time.wMonth, time.wDay,
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, block_num);
		log.write(temp, strlen(temp));
		LPWSTR record = ConvertCharToLPWSTR(temp);
		list->AddString(record);
		delete record;
		//
		fileSize += last_packet.len;
		SendTo(last_packet.content, last_packet.len, this->ServerDialogPort, this->ServerIP);
		timer.startTimer(hWnd);
		if (last_packet.len < 512)
		{
			return true;
		}
		return false;
	}
	else
	{
		sprintf_s(temp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]    Send Data: [block number:%d]\n",
			time.wYear, time.wMonth, time.wDay,
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, block_num);
		log.write(temp, strlen(temp));
		LPWSTR record = ConvertCharToLPWSTR(temp);
		list->AddString(record);
		delete record;
		//
		char content[517] = { '\0' };
		content[0] = 0;
		content[1] = Data;
		content[3] = block_num & 0x00ff;
		block_num = block_num >> 8;
		content[2] = block_num & 0x00ff;
		// ���ļ�
		fin.read(content + 4, 512);
		int data_len = fin.gcount();
		//int data_len = strlen(content + 4);
		fileSize += data_len;
		// ����ð��������ط�
		memcpy_s(last_packet.content, 517, content, data_len+4);
		last_packet.len = data_len + 4;
		
		SendTo(content, 4 + data_len, this->ServerDialogPort, this->ServerIP);
		timer.startTimer(hWnd);
		// ���һ���
		if (data_len < 512)
		{
			return true;
		}
		return false;
	}
}


/**
* ������յ��ı���
* Data�����ݱ��ģ����ж�����Ƿ���ȷ��������ack
*		�ۼ��ļ���С
*		����յ��ı���С��512B����������ڽ���ʱ��ȡʱ�䣬�뿪ʼʱ����ļ���Сһ����㴫������
* 
* Ack��	���յ���Ӧ���ģ��ж�����Ƿ���ȷ�������ȷ������һ�����ģ��������ط�
*		����Data����ʱ��������͵ı���С��512B����isEnd��־λ��1
*		�����յ���ȷ����ʱ�����isEndΪ1��˵��������ֹͣ���ͣ����򣬼�������
*/
void Client::OnReceive(int nErrorCode)
{
	unsigned char content[516] = { '\0' };
	CString ip = L"192.168.43.13";
	UINT port = 0;
	
	int size = this->ReceiveFrom(content, 516, ip, this->ServerDialogPort);
	timer.stopTimer();
	
	int opcode = content[1];
	switch (opcode)
	{
	// �ӷ�����Get�ļ�������
	case Data: 
	{
		int recv_block = content[2];
		recv_block = recv_block << 8;
		recv_block += content[3];
		// ��������
		if (recv_block == current_block + 1)
		{
			current_block++;
			fout.write((char*)content + 4, size - 4);
			//fout << content + 4;
			fileSize += size - 4;
			sendAck(current_block);
			// complete
			if (size - 4 < 512)
			{
				fout.close();
				GetSystemTime(&end);
				timer.stopTimer();
				calcSpeed();
				current_block = 0;
				fileSize = 0;
			}
		}
		// ֻҪ���յ������ݺŲ����������룬���ش�current block
		else 
		{
			sendAck(current_block);
		}
		break;
	}
	// �ӱ���Put�ļ���������
	case Ack: 
	{
		int recv_block = content[2];
		recv_block = recv_block << 8;
		recv_block += content[3];
		// ��������
		if (recv_block == current_block)
		{
			current_block++;
			// δ�������������
			if (!isEnd)
			{
				// �����һ��С��512B�İ������ͺ�isEnd = true
				isEnd = sendData(current_block, 0);
			}
			// ������رգ����ָ�ԭ״̬
			else
			{
				fin.close();
				GetSystemTime(&end);
				calcSpeed();
				current_block = 0;
				isEnd = false;
				fileSize = 0;
			}
		}
		else
		{
			isEnd = sendData(current_block, 1);
		}
		break;
	}
	// �������(uncomplete)
	case Error:
	{
		int errCode = content[2];
		errCode = errCode << 8;
		errCode += content[3];
		int data_len = strlen((char*)content + 4);
		// stop
		if (fin.is_open()) fin.close();
		if (fout.is_open()) fout.close();
		current_block = 0;
		fileSize = 0;
		isEnd = false;

		switch (errCode){
		case 0: {
			speed_display->SetWindowTextW(L"δ���� Not defined, see error message");
			break;
		}
		case 1: {
			speed_display->SetWindowTextW(L"�ļ�δ�ҵ� File not found");
			break;
		}
		case 2: {
			speed_display->SetWindowTextW(L"���ʱ��ܾ� Access violation");
			break;
		}
		case 3: {
			speed_display->SetWindowTextW(L"�������򳬳��ɷ���ռ� Disk full or allocation exceeded");
			break;
		}
		case 4: {
			speed_display->SetWindowTextW(L"�Ƿ��� TFTP ���� Illegal TFTP operation");
			break;
		}
		case 5: {
			speed_display->SetWindowTextW(L"δ֪�Ĵ��� ID Unknown transfer ID");
			break;
		}
		case 6: {
			speed_display->SetWindowTextW(L"�ļ��Ѿ����� File already exists");
			break;
		}
		case 7: {
			speed_display->SetWindowTextW(L"û�и��û� No such user");
			break;
		}
		}
		break;
	}
	default:
	{
		break;
	}
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

void Client::calcSpeed()
{
	int msec = end.wMilliseconds - begin.wMilliseconds;
	int sec = 0;
	int min = 0;
	int hour = 0;
	if (msec < 0)
	{
		msec += 1000;
		sec = end.wSecond - begin.wSecond - 1;
	}
	else
	{
		sec = end.wSecond - begin.wSecond ;
	}
	if (sec < 0)
	{
		sec += 60;
		min = end.wMinute - begin.wMinute - 1;
	}
	else
	{
		min = end.wMinute - begin.wMinute;
	}
	if (min < 0)
	{
		min += 60;
		hour = end.wHour - begin.wHour - 1;
	}
	else
	{
		hour = end.wHour - begin.wHour;
	}
	if (hour < 0) hour += 24;
	float cost = msec * 0.001 + sec + 60 * min + 3600 * hour;
	float speed = fileSize / cost / 1000;
	wchar_t content[100];
	swprintf(content,100, L"�������    �ļ���С:%dB    ��ʱ:%.4fs    ƽ������:%.3fKB/s",
		fileSize, cost, speed);
	speed_display->SetWindowTextW(content);
}

void Client::resend()
{
	GetSystemTime(&time);
	char temp[200] = { '\0' };
	sprintf_s(temp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]    Resend Packet!\n",
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	log.write(temp, strlen(temp));
	LPWSTR record = ConvertCharToLPWSTR(temp);
	list->AddString(record);
	delete record;
	//
	if (last_packet.content[1] == Get || last_packet.content[1] == Put){
		SendTo(last_packet.content, last_packet.len, this->ServerDefaultPort, this->ServerIP);
	}
	else { 
		SendTo(last_packet.content, last_packet.len, this->ServerDialogPort, this->ServerIP); 
	}
	timer.startTimer(hWnd);
}

void Client::OnSend(int nErrorCode)
{
	//onReceive(0);

}

void Client::shutdown()
{
	if (fin.is_open()) fin.close();
	if (fout.is_open()) fout.close();
	current_block = 0;
	
	isEnd = false;
	GetSystemTime(&end);
	calcSpeed();
	fileSize = 0;
	timer.stopTimer();
}

LPWSTR Client::ConvertCharToLPWSTR(const char* srcString)
{
	int srcLen = strlen(srcString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, srcString, srcLen, NULL, 0);//������ʵĳ���
	LPWSTR str = new WCHAR[srcLen];
	MultiByteToWideChar(CP_ACP, 0, srcString, srcLen, str, nwLen);
	return str;
}
