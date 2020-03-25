#include "tcpacceptthreadlower.h"
#include "tcpserverlower.h"
#include "tcpconnectpoollower.h"
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;
TcpAcceptThreadLower::TcpAcceptThreadLower(TcpServerLower* lpServer)
	:m_hEpoll(-1),m_lpServer(lpServer)
{
}

TcpAcceptThreadLower::~TcpAcceptThreadLower()
{
	close(m_hEpoll);
}

int TcpAcceptThreadLower::Init()
{
	m_hEpoll = epoll_create(EPOLL_SIZE);
	if( m_hEpoll < 0 )
	{
		//DBG(("epoll_create error:%d %s\n", errno, strerror(errno)));
		return TCP_ERROR_CREATE_EPOLL_FAILED;
	}
	return 0;
}
long TcpAcceptThreadLower::Run()
{
	//cout << "TcpAcceptThreadLower::Run" << endl;
	char errbuff[ERROR_BUFF_SIZE];
	THREAD_SIGNAL_BLOCK
	while(!isTerminated())
	{
		epoll_event ev[10];
		int nfds = epoll_wait(m_hEpoll, ev, sizeof(ev) / sizeof(epoll_event), -1);
		for (int i = 0; i < nfds;i++)
		{
			TcpServerLower *lpServer = (TcpServerLower*)ev[i].data.ptr;
			if (ev[i].events & EPOLLIN)
			{
				lpServer->OnAccept();
			}
			if (ev[i].events & EPOLLERR)
			{
				snprintf(errbuff, ERROR_BUFF_SIZE, "epoll wait error:0x%x", ev[i].events);
				lpServer->GetCallback()->HandleError(OE_EPOLL_WAIT_EPOLLERR,(uint32_t)-1, NULL, errbuff);
			}
		}
	}
	return 0;
}
int TcpAcceptThreadLower::AddAcceptor(TcpServerLower *lpServer)
{
	//cout << "AddAcceptor" << endl;
	int ret = TCP_ERROR_EPOLL_ADD_FAILED;
	char errbuff[ERROR_BUFF_SIZE];
	m_lpServer = lpServer;
	if( m_hEpoll != -1 && lpServer)
	{
		epoll_event ev;
		ev.events = EPOLLIN|EPOLLET;// 设置为边缘触发
		ev.data.ptr = lpServer;
		ret = epoll_ctl(m_hEpoll, EPOLL_CTL_ADD, lpServer->GetSocket(), &ev);
		if( ret )
		{
			ret = TCP_ERROR_EPOLL_ADD_FAILED;
			snprintf(errbuff, ERROR_BUFF_SIZE,"epoll ctl add socket[%d]failed,errno:%d strerror:%s",
					lpServer->GetSocket(), errno, strerror(errno));
			lpServer->GetCallback()->HandleError(OE_SYSTEM_ERROR,(uint32_t)-1, NULL, errbuff);
		}
	}
	return ret;
}

