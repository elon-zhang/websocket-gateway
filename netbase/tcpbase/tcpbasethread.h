#ifndef TCPBASETHREAD_H_
#define TCPBASETHREAD_H_

#include "tcpbaseconnect.h"
#include <sys/SYSthread.h>
///发送数据线程和接收数据线程的基类
class TcpBaseThread:public SYSThread
{
public:
	TcpBaseThread(uint32_t ev);
	~TcpBaseThread();
	///初始化线程类
	int Init();
	///将连接放入到线程的epoll中
	int AddConnect(CTcpBaseConnect*);
	///将连接从epoll中删除
	int DelConnect(CTcpBaseConnect*);

protected:
	///epoll的fd
	int			m_hEpoll;
	///加入本线程的连接关注的epoll事件
	uint32_t	m_nEpollEvent;
};

///负责发送数据（积压下来的）的线程类
class CTcpSendThread:public TcpBaseThread
{
public:
	CTcpSendThread():TcpBaseThread(EPOLLOUT|EPOLLET){}  //边缘触发
	///线程执行函数
	virtual long Run();
};

///负责接收数据的线程类
class CTcpRecvThread:public TcpBaseThread
{
public:
	CTcpRecvThread() :TcpBaseThread(EPOLLIN | EPOLLET){}   //边缘触发
	//CTcpRecvThread() :TcpBaseThread(EPOLLIN){}   //水平触发
	///线程执行函数
	virtual long Run();
};




#endif /* TCP_THREAD_H_ */
