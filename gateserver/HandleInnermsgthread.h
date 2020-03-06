#ifndef HANDLEINNERMSGTHREAD_H_
#define HANDLEINNERMSGTHREAD_H_
#include <queue>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include "Gate_info.h"
#include "share/singleton.h"
#include "publicthread.h"
using namespace std;

class HandlerinnermsgThread :public PublicThread, public singleton<HandlerinnermsgThread>
{
public:
	HandlerinnermsgThread();

	~HandlerinnermsgThread();

	virtual int run();

	int addonemsg(const char* msg, int len);

	void hanldemsg(int msgheadpos);
private:

	char m_msgbuf[MAX_MSGBUFF_LEN];
	int m_msghead;//消息队列的头指针
	int m_msgtail;//消息队列的尾指针
	unsigned int m_headround;
	int m_tailround;
};
#endif
