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
* 发送Ack响应报文
*/
void Client::sendAck(int block_num)
{	
	// 记录日志
	GetSystemTime(&time);
	char temp[200] = { '\0' };
	sprintf_s(temp, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]    Send Ack: [block number:%d]\n",
		time.wYear, time.wMonth, time.wDay, 
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, block_num);
	
	LPWSTR record = ConvertCharToLPWSTR(temp);
	list->AddString(record);
	delete record;

	// 构造报文
	char content[4] = { 0 };
	content[1] = Ack;
	content[3] = block_num & 0x00ff;
	block_num = block_num >> 8;
	content[2] = block_num & 0x00ff;
	// 保存该包，用于重发
	memcpy_s(last_packet.content, 517, content, 4);
	last_packet.len = 4;

	// 发送报文
	SendTo(content, 4, this->ServerDialogPort, this->ServerIP);
	log.write(temp, strlen(temp));
	timer.startTimer(hWnd);
}

/* 
* 发送读文件请求RRQ，打开文件流，记录日志
* 记录开始发送时间
* mode:	1为netascii，2为octet
*/
void Client::sendGet(char* fileName)
{
	// 记录日志
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
	// 保存该包，用于重发
	memcpy_s(last_packet.content, 517, context, len);
	last_packet.len = len;
	SendTo(context, len, 69, this->ServerIP, 0);
	timer.startTimer(hWnd);
	return;
}


/*
* 发送写文件请求WRQ，打开文件流，记录日志
* 记录开始发送时间
* mode:	1为netascii，2为octet
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
	// 保存该包，用于重发
	memcpy_s(last_packet.content, 517, context, len);
	last_packet.len = len;
	SendTo(context, len, 69, this->ServerIP, 0);
	timer.startTimer(hWnd);
}


/*
* 发送数据报文，并记录日志
* 累加文件大小fileSize
* block_num:数据报的编号
* resend:	是否为重发报文
*			如果为重发报文，则发送上一个报文
*			否则，读新的文件内容并发送
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
		// 读文件
		fin.read(content + 4, 512);
		int data_len = fin.gcount();
		//int data_len = strlen(content + 4);
		fileSize += data_len;
		// 保存该包，用于重发
		memcpy_s(last_packet.content, 517, content, data_len+4);
		last_packet.len = data_len + 4;
		
		SendTo(content, 4 + data_len, this->ServerDialogPort, this->ServerIP);
		timer.startTimer(hWnd);
		// 最后一组包
		if (data_len < 512)
		{
			return true;
		}
		return false;
	}
}


/**
* 处理接收到的报文
* Data：数据报文，则判断序号是否正确，并发送ack
*		累加文件大小
*		如果收到的报文小于512B，则结束。在结束时获取时间，与开始时间和文件大小一起计算传输速率
* 
* Ack：	接收到响应报文，判断序号是否正确，如果正确则发送下一个报文；错误则重发
*		发送Data报文时，如果发送的报文小于512B，则将isEnd标志位置1
*		当接收到正确报文时，如果isEnd为1，说明结束，停止发送；否则，继续发送
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
	// 从服务器Get文件到本地
	case Data: 
	{
		int recv_block = content[2];
		recv_block = recv_block << 8;
		recv_block += content[3];
		// 正常接收
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
		// 只要接收到的数据号不是期望号码，则重传current block
		else 
		{
			sendAck(current_block);
		}
		break;
	}
	// 从本地Put文件到服务器
	case Ack: 
	{
		int recv_block = content[2];
		recv_block = recv_block << 8;
		recv_block += content[3];
		// 正常接收
		if (recv_block == current_block)
		{
			current_block++;
			// 未结束则继续发送
			if (!isEnd)
			{
				// 当最后一个小于512B的包被发送后，isEnd = true
				isEnd = sendData(current_block, 0);
			}
			// 结束则关闭，并恢复原状态
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
	// 处理错误(uncomplete)
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
			speed_display->SetWindowTextW(L"未定义 Not defined, see error message");
			break;
		}
		case 1: {
			speed_display->SetWindowTextW(L"文件未找到 File not found");
			break;
		}
		case 2: {
			speed_display->SetWindowTextW(L"访问被拒绝 Access violation");
			break;
		}
		case 3: {
			speed_display->SetWindowTextW(L"磁盘满或超出可分配空间 Disk full or allocation exceeded");
			break;
		}
		case 4: {
			speed_display->SetWindowTextW(L"非法的 TFTP 操作 Illegal TFTP operation");
			break;
		}
		case 5: {
			speed_display->SetWindowTextW(L"未知的传输 ID Unknown transfer ID");
			break;
		}
		case 6: {
			speed_display->SetWindowTextW(L"文件已经存在 File already exists");
			break;
		}
		case 7: {
			speed_display->SetWindowTextW(L"没有该用户 No such user");
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
	swprintf(content,100, L"传输完成    文件大小:%dB    用时:%.4fs    平均速率:%.3fKB/s",
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
	int nwLen = MultiByteToWideChar(CP_ACP, 0, srcString, srcLen, NULL, 0);//算出合适的长度
	LPWSTR str = new WCHAR[srcLen];
	MultiByteToWideChar(CP_ACP, 0, srcString, srcLen, str, nwLen);
	return str;
}
