#include "stdio.h"
#include <stdarg.h>
#include <string>
#include "Handleoutmsgthread.h"
#include "log.h"
#include "TcpMsgStr.h"
#include "GateInnerServer.h"

using namespace std;
HandleroutmsgThread::HandleroutmsgThread()
{
	m_msgtail = 0;
	m_msghead = 0;
	m_headround = 0;
	m_tailround = 0;
	memset(m_msgbuf, 0, MAX_MSGBUFF_LEN);
}

HandleroutmsgThread::~HandleroutmsgThread()
{
	
}

int HandleroutmsgThread::run()
{
	int msghead = 0;
	while (!isTerminated())
	{
		pthread_mutex_lock(&_condmutex);	//加锁
		while (m_msgtail == m_msghead)
		{
			//执行的时候发现消息取完了的情况，把头指针和尾指针都重新置0，减少消息转弯的情况
			INFO("HandleroutmsgThread::run all msg handled m_msgtail=%d,m_msghead=%d,m_tailround=%d,m_headround=%d", m_msgtail, m_msghead, m_tailround, m_headround);
			m_msgtail = 0;
			m_msghead = 0;
			m_tailround = 0;
			m_headround = 0;
			pthread_cond_wait(&_cond, &_condmutex);
		}
			
		msghead = m_msghead;
		pthread_mutex_unlock(&_condmutex);	//解锁
		//开始处理消息
		hanldemsg(msghead);//因为m_msghead有可能在另一个线程中被改变，所以这里加个锁，固定当时的头位置
	}
	ERROR("HandleroutmsgThread::Run END");
	return 0;
}





int HandleroutmsgThread::addonemsg(const char* msg, int len)
{
	pthread_mutex_lock(&_condmutex);	//加锁
	if (m_msghead + len >= MAX_MSGBUFF_LEN)//处理消息转弯
	{
		int firstmsglen = MAX_MSGBUFF_LEN - m_msghead;
		int secondmsglen = len - (MAX_MSGBUFF_LEN - m_msghead);
		memcpy(&m_msgbuf[m_msghead],(void*)(&msg), firstmsglen);
		m_msghead = 0;
		memcpy(&m_msgbuf[m_msghead], (void*)(&msg), secondmsglen);
		m_msghead += secondmsglen;
		m_msghead = m_msghead % (MAX_MSGBUFF_LEN);
		m_headround++;
		if (m_headround > 4000000000)
		{
			m_headround = 0;
		}
	}
	else
	{
		memcpy(&m_msgbuf[m_msghead], msg, len);
		m_msghead += len;
		m_msghead = m_msghead % (MAX_MSGBUFF_LEN);
	}
	
	pthread_cond_broadcast(&_cond);		//条件唤醒
	pthread_mutex_unlock(&_condmutex);	//解锁

	return 0;
}

void HandleroutmsgThread::hanldemsg(int msgheadpos)
{
	INFO("HandleroutmsgThread::hanldemsg ");
	MSG_STR msg;
	char msghead[sizeof(STR_MSG_HEAD)];
	memset(msghead, 0, sizeof(STR_MSG_HEAD));
	if (m_headround>m_tailround && m_msgtail < m_msghead)//这里做一个是否头指针超过尾指针一圈的判断
	{
		ERROR("hanlde msg buff is full m_headround=%d,m_tailround=%d,m_msgtail=%d,m_msghead=%d", m_headround, m_tailround, m_msgtail, m_msghead);
		m_msgtail = m_msghead;
		m_headround = 0;
		m_tailround = 0;
	}
	int nextlen = MAX_MSGBUFF_LEN - m_msgtail;//这里需要考虑一个消息转弯的问题,保证msgptr的HeadPad，MessageSize的正确性
	if (nextlen < sizeof(STR_MSG_HEAD))//这个4是消息头最开始到消息头长度字段的距离
	{
		memcpy(&msghead[0], &m_msgbuf[m_msgtail], nextlen);
		memcpy(&msghead[nextlen], &m_msgbuf[0], sizeof(STR_MSG_HEAD)-nextlen);
	}
	else
	{
		memcpy(&msghead[0], &m_msgbuf[m_msgtail], sizeof(STR_MSG_HEAD));
	}
	STR_MSG_HEAD* msgptr = (STR_MSG_HEAD*)(msghead);

	if (msgptr->HeadPad != CLIENTGFLOG && m_msgtail != msgheadpos)
	{
		ERROR("m_tail pos wrong= %d ,m_msgtail=%d,m_msghead=%d", msgptr->HeadPad, m_msgtail, msgheadpos);
		//如果出现这样的情况，说明这段消息不可用了，抛弃掉
		m_msgtail = msgheadpos;
		return;
	}
	if (m_msgtail == msgheadpos)
	{
		INFO("all msg handled!!!!!!");
		return;
	}
	int msgsize = sizeof(STR_MSG_HEAD)+msgptr->MessageSize;
	if (msgsize >= MAX_MSGBUFF_LEN)
	{
		ERROR(" wrong msgsize=%d",msgsize);
		//如果出现这样的情况，说明这段消息不可用了，抛弃掉
		m_msgtail = msgheadpos;
		return;
	}
	INFO("HandleroutmsgThread::hanldemsg FREE=%d", (msgptr->MessageFree));
	
	if (m_msgtail + msgsize >= MAX_MSGBUFF_LEN)//处理消息转弯的情况,分两部分拷贝消息
	{
		m_tailround++;
		if (m_tailround > 4000000000)
		{
			m_tailround = 0;
		}
		int firstmsglen = MAX_MSGBUFF_LEN - m_msgtail;
		int secondmsglen = msgsize - (MAX_MSGBUFF_LEN - m_msgtail);
		memcpy((void*)(&msg), &m_msgbuf[m_msgtail], firstmsglen);
		m_msgtail = 0;
		memcpy((void*)(&msg), &m_msgbuf[m_msgtail], secondmsglen);
		m_msgtail += secondmsglen;
		m_msgtail = m_msgtail % (MAX_MSGBUFF_LEN);
		INFO("HandleroutmsgThread::hanldemsg type one============= CMD=%d,SCMD=%d,m_tail=%d,m_head=%d", msg.head.MessageCmd, msg.head.MessageSCmd, m_msgtail, msgheadpos);
		GateInnerServerModule::getInstance()->GetServerLogic()->HandleOutMsg(&msg);
	}
	else
	{
		memcpy((void*)(&msg), &m_msgbuf[m_msgtail], sizeof(STR_MSG_HEAD)+msgptr->MessageSize);
		m_msgtail += sizeof(STR_MSG_HEAD)+msgptr->MessageSize;
		m_msgtail = m_msgtail % (MAX_MSGBUFF_LEN);
		INFO("HandleroutmsgThread::hanldemsg type one+++++++++++++ CMD=%d,SCMD=%d,m_tail=%d,m_head=%d", msg.head.MessageCmd, msg.head.MessageSCmd, m_msgtail, msgheadpos);
		loghex(msg.msg, msg.head.MessageSize);
		GateInnerServerModule::getInstance()->GetServerLogic()->HandleOutMsg(&msg);
	}
}
