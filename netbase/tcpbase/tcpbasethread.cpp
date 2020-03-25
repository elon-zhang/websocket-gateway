#include "tcpbasethread.h"
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

TcpBaseThread::TcpBaseThread(uint32_t ev):
	m_hEpoll(-1), m_nEpollEvent(ev)
{
}

TcpBaseThread::~TcpBaseThread()
{
	close(m_hEpoll);
}

int TcpBaseThread::Init()
{
	m_hEpoll = epoll_create(EPOLL_SIZE);
	if( m_hEpoll < 0 )
	{
		//DBG(("epoll_create error:%d %s\n", errno, strerror(errno)));
		return TCP_ERROR_CREATE_EPOLL_FAILED;
	}
	return 0;
}

int TcpBaseThread::AddConnect(CTcpBaseConnect* lpConnect)
{
	int ret = TCP_ERROR_EPOLL_ADD_FAILED;
	char errbuff[ERROR_BUFF_SIZE];
	if( m_hEpoll != -1 && lpConnect)
	{
		epoll_event ev;
		ev.events = m_nEpollEvent;
		ev.data.ptr = lpConnect;
		int fd = lpConnect->GetSocket();
		ret = epoll_ctl(m_hEpoll, EPOLL_CTL_ADD, fd, &ev);
		if( ret < 0 )
		{
			ret = TCP_ERROR_EPOLL_ADD_FAILED;
			snprintf(errbuff,ERROR_BUFF_SIZE,"epoll ctl add failed,socket:%d event:0x%x,errno:%d strerror:%s",
				fd, m_nEpollEvent, errno, strerror(errno));
			lpConnect->HandleError(OE_SYSTEM_ERROR, errbuff);
		}
	}
	return ret;
}

int TcpBaseThread::DelConnect(CTcpBaseConnect* lpConnect)
{
	int ret = TCP_ERROR_EPOLL_ADD_FAILED;
	char errbuff[ERROR_BUFF_SIZE];
	if( m_hEpoll != -1 && lpConnect)
	{
		epoll_event ev;
		ev.events = m_nEpollEvent;
		ev.data.ptr = lpConnect;
		int fd = lpConnect->GetSocket();
		ret = epoll_ctl(m_hEpoll, EPOLL_CTL_DEL, fd, &ev);
		if( ret < 0 )
		{
			ret = TCP_ERROR_EPOLL_ADD_FAILED;
			snprintf(errbuff,ERROR_BUFF_SIZE,"epoll ctl del failed,socket:%d event:0x%x,errno:%d strerror:%s",
				fd, m_nEpollEvent, errno, strerror(errno));
			lpConnect->HandleError(OE_SYSTEM_ERROR, errbuff);
		}
	}
	return ret;
}

long CTcpSendThread::Run()
{
	//cout << "CTcpSendThread::Run" << endl;
	THREAD_SIGNAL_BLOCK;
	while(!isTerminated())
	{
		epoll_event ev[EPOLL_SIZE];
		int nfds = epoll_wait(m_hEpoll, ev, sizeof(ev)/sizeof(epoll_event), EPOLL_WAIT_TIME);
		//DBG(("CTcpSendThread nfds:%d errno:%d strerror:%s\n", nfds, errno, strerror(errno)));
		for(int i=0; i<nfds; ++i)
		{
			CTcpBaseConnect* lpConnect = (CTcpBaseConnect*)ev[i].data.ptr;
			if(ev[i].events & EPOLLOUT)
			{
				if(lpConnect->IsConnected())
				{
					lpConnect->SendBlockData();
				}
				else
				{
					if( lpConnect->OnConnected() )//在第一次连接上的时候会进入该行
					{
						lpConnect->Disconnect();
					}
				}
			}
		}
	}
	return 0;
}

long CTcpRecvThread::Run()
{
	//cout << "CTcpRecvThread::Run"  << endl;
	THREAD_SIGNAL_BLOCK;
	while(!isTerminated())
	{
		epoll_event ev[EPOLL_SIZE];
		//int nfds = epoll_wait(m_hEpoll, ev, sizeof(ev)/sizeof(epoll_event), EPOLL_WAIT_TIME);
		 int nfds = epoll_wait(m_hEpoll, ev, sizeof(ev) / sizeof(epoll_event), -1);
		//DBG(("CTcpPassiveRecvThread nfds:%d errno:%d strerror:%s\n", nfds, errno, strerror(errno)));
		for(int i=0; i<nfds; ++i)
		{
			CTcpBaseConnect* lpConnect = (CTcpBaseConnect*)ev[i].data.ptr;
			if(ev[i].events & EPOLLIN)
			{
				//cout << "recv data=" << nfds << endl;
				if( lpConnect->OnRecvData() )
				{
					lpConnect->Disconnect();
				}
			}
		}
	}
	return 0;
}


