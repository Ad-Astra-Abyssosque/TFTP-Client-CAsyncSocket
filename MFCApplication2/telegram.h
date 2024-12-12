#pragma once
#include <iostream>

struct Request
{
	short operationCode;
	char fileName[100];
	char mode[10];
};
struct Data
{
	short operationCode;
	short blockNum;
	std::string content;
};
struct Ack
{
	short operationCode;
	short blockNum;
};
struct Error
{
	short operationCode;
	short errorCode;
	std::string message;
};
