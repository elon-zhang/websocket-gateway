#include "tcpclientlower.h"

extern TcpClientCreatorLower *glpTcpClientCreator;


TcpClientLower::TcpClientLower(ITcpClientModule* lpCallBack, uint32_t nSendBuffSize, bool bAutoReConnect, int32_t iRecvBuffSize)
:m_Connect(nSendBuffSize, this, iRecvBuffSize), m_lpCallback(lpCallBack), m_nConnectTimes(0)
	,m_nCheckReConnectTimes(0),	m_bAutoReConnect(bAutoReConnect)
{
}

int TcpClientLower::Connect(
						const char*	lpServerAddr,
						uint16_t	wServerPort,
						bool bBlockConnect,
						const char* lpLocalAddr )
{
	m_Connect.Disconnect();
	//初始化ip地址和端口
	if( lpServerAddr )
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr((char*)lpServerAddr);
		addr.sin_port = htons(wServerPort);
		m_Connect.Init(-1, &addr);
	}
	//设置本地ip
	if( lpLocalAddr )
	{
		m_localaddr.sin_family = AF_INET;
		m_localaddr.sin_addr.s_addr = inet_addr((char*)lpLocalAddr);
		m_localaddr.sin_port = 0;
	}
	//使用阻塞方式连接
	int ret = 0;
	if( bBlockConnect )
	{
		ret = ConnectBlock();
	}
	else
	{
		ret = ConnectNonBlock();
	}
	//放入EPOLL中
	if( !ret )
	{
		ret = Add2Epoll();
	}
	return ret;
}

int TcpClientLower::SendData(
							const void* lpData,
							int32_t iDataLen,
							int32_t iWaitTime,
							uint32_t nBlockFlag)
{
	if( !lpData || iDataLen<=0 )
	{
		return TCP_ERROR_WRONG_PARAM;
	}
	if( iWaitTime<0 )
	{
		iWaitTime = 0;
	}
	else if( iWaitTime>MAX_WAIT_TIME )
	{
		iWaitTime = MAX_WAIT_TIME;
	}
	return m_Connect.SendData((const char*)lpData, iDataLen, iWaitTime, nBlockFlag);
}

int TcpClientLower::Disconnect()
{
	return m_Connect.Disconnect();
}

int TcpClientLower::ReConnect(
							const char*	lpServerAddr,
							uint16_t	wServerPort,
							bool bBlockConnect,
							const char* lpLocalAddr )
{
	return Connect(lpServerAddr, wServerPort, bBlockConnect, lpLocalAddr);
}

int TcpClientLower::SetHeartbeat(int32_t iTime)
{
	return m_Connect.SetHeartbeat(iTime);
}

int TcpClientLower::SetTimeout(int32_t iTime)
{
	return m_Connect.SetTimeout(iTime);
}

int TcpClientLower::SetQuickAck(bool bQuickAck)
{
	m_Connect.SetQuickAck(bQuickAck);
	return 0;
}

int TcpClientLower::SetTcpNoDelay(bool bDelay)
{
	int delay = bDelay;
	if( setsockopt(m_Connect.GetSocket(), IPPROTO_TCP, TCP_NODELAY,(char *)&delay,sizeof(delay)) )
	{
		return TCP_ERROR_SETSOCKOPT_FAILED;
	}
	return 0;
}

int TcpClientLower::SetTcpKeepAlive(bool bKeepAlive)
{
	int iKeepAlive = bKeepAlive;
	if( setsockopt(m_Connect.GetSocket(), SOL_SOCKET, SO_KEEPALIVE,
		(void *)&iKeepAlive, sizeof(iKeepAlive)) )
	{
		return TCP_ERROR_SETSOCKOPT_FAILED;
	}
	return 0;
}

int TcpClientLower::ConnectNonBlock()
{
	//增加连接次数
	++m_nConnectTimes;
	m_nCheckReConnectTimes=0;
	//创建socket
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd == -1 )
	{
		return TCP_ERROR_CREATE_SOCKET_FAILED;
	}
	//设置客户端使用的网卡
	if( m_localaddr.sin_addr.s_addr )
	{
		if( bind(fd, (sockaddr*)&m_localaddr, (socklen_t)sizeof(m_localaddr)) )
		{
			return TCP_ERROR_BIND_FAILED;
		}
	}
	//设置CConnect类的fd
	m_Connect.Init(fd, m_Connect.GetAddr());
	//设置套接字非阻塞
	SetNonBlocking(1);
	//连接服务端
	if( connect(fd, (sockaddr*)m_Connect.GetAddr(),(socklen_t)sizeof(sockaddr_in))
		&& errno != EINPROGRESS )
	{
		return TCP_ERROR_CONNECT_FAILED;
	}
	return 0;
}

int TcpClientLower::ConnectBlock()
{
	m_nCheckReConnectTimes=0;
	//创建socket
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if( fd == -1 )
	{
		return TCP_ERROR_CREATE_SOCKET_FAILED;
	}
	//设置客户端使用的网卡
	if( m_localaddr.sin_addr.s_addr )
	{
		if(bind(fd, (sockaddr*)&m_localaddr, (socklen_t)sizeof(m_localaddr)))
		{
			return TCP_ERROR_BIND_FAILED;
		}
	}
	//连接服务端
	if( connect(fd, (sockaddr*)m_Connect.GetAddr(), (socklen_t)sizeof(sockaddr_in)) )
	{
		return TCP_ERROR_CONNECT_FAILED;
	}
	//设置CConnect类的fd并记录连接的时间
	m_Connect.Init(fd, m_Connect.GetAddr());
	//设置套接字非阻塞
	SetNonBlocking(1);
	//避免回调上层OnConnected接口
	m_Connect.SetConnectStatus(true);
	return 0;
}

int TcpClientLower::Add2Epoll()
{
	int ret = glpTcpClientCreator->GetSendThread(m_iSendThreadIndex)->AddConnect(&m_Connect);
	if( !ret )
	{
		ret = glpTcpClientCreator->GetRecvThread(m_iRecvThreadIndex)->AddConnect(&m_Connect);
	}
	return ret;
}

int TcpClientLower::DelFromEpoll()
{
	glpTcpClientCreator->GetSendThread(m_iSendThreadIndex)->DelConnect(&m_Connect);
	glpTcpClientCreator->GetRecvThread(m_iRecvThreadIndex)->DelConnect(&m_Connect);
	return 0;
}

bool TcpClientLower::NeedReConnect()
{
	if( m_Connect.GetSocket() == -1 && m_bAutoReConnect)
	{
		++m_nCheckReConnectTimes;
		//断开1.5秒后，进行重连
		if( m_nCheckReConnectTimes >= 15 )
		{
			return true;
		}
	}
	return false;
}

int TcpClientLower::SetConnectTimes()
{
	m_nConnectTimes=0;
	return 0;
}

int TcpClientLower::SetNonBlocking(int nonblock)
{
	int fd = m_Connect.GetSocket();
	if (-1 == fd)
		return -1;

	int nRecvBuf = 32 * 1024;//设置为32K
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));

	int nSendBuf = 32 * 1024;//设置为32K
	setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));

	int fflags = fcntl(fd, F_GETFL);
	if (nonblock) fflags |= O_NONBLOCK;
	else fflags &=~ O_NONBLOCK;
	return fcntl(fd, F_SETFL, fflags);
}

int TcpClientLower::Init(
						int32_t iSendIndex,
						int32_t iRecvIndex,
						const char* lpServerAddr,
						uint16_t wServerPort,
						const char* lpLocalAddr )
{
	m_iSendThreadIndex = iSendIndex;
	m_iRecvThreadIndex = iRecvIndex;
	//本地ip地址配置
	memset(&m_localaddr, 0, sizeof(m_localaddr));
	if( lpLocalAddr )
	{
		m_localaddr.sin_family = AF_INET;
		m_localaddr.sin_addr.s_addr = inet_addr((char*)lpLocalAddr);
		m_localaddr.sin_port = 0;
	}
	//初始化ip地址和端口
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr((char*)lpServerAddr);
	addr.sin_port = htons(wServerPort);
	//初始化连接，申请内存
	return m_Connect.Init(-1, &addr);
}

bool TcpClientLower::GetConnectStatus()
{
	return m_Connect.IsConnected();
}

int TcpClientLower::GetConnectCount()
{
	return m_nConnectTimes;
}

int TcpClientLower::SetAutoReConnect(bool bAutoReConnect)
{
	m_bAutoReConnect = bAutoReConnect;
	return 0;
}

bool TcpClientLower::GetAutoReConnect()
{
	return m_bAutoReConnect;
}

