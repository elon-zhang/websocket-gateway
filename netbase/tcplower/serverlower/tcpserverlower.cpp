#include <unistd.h>
#include <iostream>
#include <string>
#include "tcpserverlower.h"
using namespace std;
TcpServerLower::TcpServerLower()
	:m_lpAcceptThread(NULL),m_iSendThreadCount(0),m_lpSendThread(NULL),
	m_iRecvThreadCount(0),m_lpRecvThread(NULL),
	m_lpConnectPool(NULL),m_fd(-1), m_lpCallback(NULL),
	m_nInitSendBuffSize(0),m_bReBind(1)
{
	memset(&m_addr,0,sizeof(m_addr));
	memset(m_lpConnectArray,0,sizeof(m_lpConnectArray));
}

TcpServerLower::~TcpServerLower()
{
	//停止接收连接线程
	if( m_lpAcceptThread )
	{
		m_lpAcceptThread->stop();
	}
	//关闭监听套接字
	close(m_fd);
	//关闭在线的连接
	for(uint32_t i=0; i<MAX_ONLINE_CONNECT_COUNT; ++i)
	{
		CAcceptConnectLower* lpConnect = m_lpConnectArray[i];
		if( lpConnect )
		{
			lpConnect->Disconnect();
		}
	}

	//关闭发送数据线程
	for(int i=0; i<m_iSendThreadCount; ++i)
	{
		if( m_lpSendThread )
		{
			m_lpSendThread[i].stop();
		}

	}

	//关闭接收数据线程
	for(int i=0; i<m_iRecvThreadCount; ++i)
	{
		if( m_lpRecvThread )
		{
			m_lpRecvThread[i].stop();
		}
	}

	//删除线程对象
	if( m_lpAcceptThread )
	{
		delete m_lpAcceptThread;
	}
	if( m_lpSendThread )
	{
		delete[] m_lpSendThread;
	}
	if( m_lpRecvThread )
	{
		delete[] m_lpRecvThread;
	}
	//删除内存缓冲池以及池中的连接对象
	if( m_lpConnectPool )
	{
		delete m_lpConnectPool;
	}
}

int TcpServerLower::SendData(
						uint32_t nSockIndex,
						const void* lpData,
						int32_t iDataLen,
						int32_t iWaitTime,
						uint32_t nBlockFlag)
{
	//先判断参数是否有误
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
	int ret = TCP_ERROR_NOT_ESTABLISHED;
	uint32_t index = nSockIndex&INDEX_LOW_MASK;
	if( index < MAX_ONLINE_CONNECT_COUNT )
	{
		//获取被动连接，如果连接活跃，则发送数据
		CAcceptConnectLower* lpConnect = m_lpConnectArray[index];
		if( lpConnect && (lpConnect->GetIndex() == nSockIndex) )
		{
			ret = lpConnect->SendData((const char*)lpData, iDataLen,
				iWaitTime, nBlockFlag);
			if( 0 == ret )
			{
				++m_iSendTimes;
			}
		}
	}
	return ret;
}

int TcpServerLower::Disconnect(uint32_t nSockIndex)
{
	int ret = TCP_ERROR_WRONG_PARAM;
	uint32_t index = nSockIndex&INDEX_LOW_MASK;
	if( index < MAX_ONLINE_CONNECT_COUNT )
	{
		CAcceptConnectLower* lpConnect = m_lpConnectArray[index];
		if( lpConnect && (lpConnect->GetIndex() == nSockIndex) )
		{
			ret = lpConnect->Disconnect();
		}
	}
	return ret;
}

int TcpServerLower::SetHeartbeat(uint32_t nSockIndex, int32_t iTime)
{
	int ret = TCP_ERROR_WRONG_PARAM;
	uint32_t index = nSockIndex&INDEX_LOW_MASK;
	if( index < MAX_ONLINE_CONNECT_COUNT )
	{
		CAcceptConnectLower* lpConnect = m_lpConnectArray[index];
		if( lpConnect  && (lpConnect->GetIndex() == nSockIndex) )
		{
			ret = lpConnect->SetHeartbeat(iTime);
		}
	}
	return ret;
}

int TcpServerLower::SetTimeout(uint32_t nSockIndex, int32_t iTime)
{
	int ret = TCP_ERROR_WRONG_PARAM;
	uint32_t index = nSockIndex&INDEX_LOW_MASK;
	if( index < MAX_ONLINE_CONNECT_COUNT )
	{
		CAcceptConnectLower* lpConnect = m_lpConnectArray[index];
		if( lpConnect  && (lpConnect->GetIndex() == nSockIndex) )
		{
			ret = lpConnect->SetTimeout(iTime);
		}
	}
	return ret;
}

int TcpServerLower::SetQuickAck(uint32_t nSockIndex, bool bQuickAck)
{
	int ret = TCP_ERROR_WRONG_PARAM;
	uint32_t index = nSockIndex&INDEX_LOW_MASK;
	if( index < MAX_ONLINE_CONNECT_COUNT )
	{
		CAcceptConnectLower* lpConnect = m_lpConnectArray[index];
		if( lpConnect  && (lpConnect->GetIndex() == nSockIndex) )
		{
			lpConnect->SetQuickAck(bQuickAck);
			ret = 0;
		}
	}
	return ret;
}

int TcpServerLower::SetTcpNoDelay(uint32_t nSockIndex, bool bDelay)
{
	int ret = TCP_ERROR_WRONG_PARAM;
	int delay = bDelay;
	uint32_t index = nSockIndex&INDEX_LOW_MASK;
	if( index < MAX_ONLINE_CONNECT_COUNT )
	{
		CAcceptConnectLower* lpConnect = m_lpConnectArray[index];
		if( lpConnect  && (lpConnect->GetIndex() == nSockIndex) )
		{
			ret = setsockopt(lpConnect->GetSocket(), IPPROTO_TCP, TCP_NODELAY,
				(char *)&delay, sizeof(delay));
			if( ret )
			{
				ret = TCP_ERROR_SETSOCKOPT_FAILED;
			}
		}
	}
	return ret;
}

int TcpServerLower::SetTcpKeepAlive(uint32_t nSockIndex, bool bKeepAlive)
{
	int ret = TCP_ERROR_WRONG_PARAM;
	int iKeepAlive = bKeepAlive;
	uint32_t index = nSockIndex&INDEX_LOW_MASK;
	if( index < MAX_ONLINE_CONNECT_COUNT )
	{
		CAcceptConnectLower* lpConnect = m_lpConnectArray[index];
		if( lpConnect  && (lpConnect->GetIndex() == nSockIndex) )
		{
			ret = setsockopt(lpConnect->GetSocket(), SOL_SOCKET, SO_KEEPALIVE,
				(void *)&iKeepAlive, sizeof(iKeepAlive));
			if( ret )
			{
				ret = TCP_ERROR_SETSOCKOPT_FAILED;
			}
		}
	}
	return ret;
}

int TcpServerLower::SetReuseAddr(int reuse)
{
	return setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
}

int TcpServerLower::CreateThread()
{
	int ret = 0;
	//创建监听线程并初始化
	m_lpAcceptThread = NEW TcpAcceptThreadLower(this);
	if( !m_lpAcceptThread )
	{
		return TCP_ERROR_ALLOC_MEMORY_FAILED;
	}
	ret = m_lpAcceptThread->Init();
	if( ret )
	{
		return ret;
	}

	//创建发送线程并初始化
	m_lpSendThread = NEW CTcpSendThread[m_iSendThreadCount];
	if( !m_lpSendThread )
	{
		return TCP_ERROR_ALLOC_MEMORY_FAILED;
	}
	for(int i=0; i<m_iSendThreadCount; ++i)
	{
		ret = m_lpSendThread[i].Init();
		if( ret )
		{
			return ret;
		}
	}

	//创建接收线程并初始化
	m_lpRecvThread = NEW CTcpRecvThread[m_iRecvThreadCount];
	if( !m_lpRecvThread )
	{
		return TCP_ERROR_ALLOC_MEMORY_FAILED;
	}
	for(int i=0; i<m_iRecvThreadCount; ++i)
	{
		ret = m_lpRecvThread[i].Init();
		if( ret )
		{
			return ret;
		}
	}
	//cout << "CreateTcpServer start ret =" << ret << endl;
	//启动监听线程、发送和接收线程
	m_lpAcceptThread->start();
	//cout << "CreateTcpServer start ret =" << ret << endl;
	for(int i=0; i<m_iSendThreadCount; ++i)
	{
		m_lpSendThread[i].start();
	}
	//cout << "CreateTcpServer start ret =" << ret << endl;
	for(int i=0; i<m_iRecvThreadCount; ++i)
	{
		m_lpRecvThread[i].start();
	}
	//cout << "CreateTcpServer start ret =" << ret << endl;
	return 0;
}

int TcpServerLower::SetNonBlocking(int fd, int nonblock)
{
	int fflags = fcntl(fd, F_GETFL);
	if( nonblock )
	{
		fflags |= O_NONBLOCK;
	}
	else
	{
			fflags &=~ O_NONBLOCK;
	}
	int nRecvBuf = 32*1024;//设置为32K
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));

	int nSendBuf = 32*1024;//设置为32K
	setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));
	return fcntl(fd, F_SETFL, fflags);
}

int TcpServerLower::Listen(bool bReBind)
{
	m_fd = socket(AF_INET,SOCK_STREAM,0);
	if(m_fd == -1)
	{
		return TCP_ERROR_CREATE_SOCKET_FAILED;
	}
	SetNonBlocking(m_fd,1);
	if( bReBind )
	{
		SetReuseAddr(1);
	}
	if( bind(m_fd, (struct sockaddr*)&m_addr, (socklen_t)sizeof(m_addr)))
	{
		return TCP_ERROR_BIND_FAILED;
	}
    if( listen(m_fd, 1000) )
	{
		return TCP_ERROR_LISTEN_FAILED;
	}
	return 0;
}

int TcpServerLower::Init(
					const char* lpIpAddr,
					uint16_t wPort,
					ITcpServerModule* lpCallback,
					bool bReBind,
					int32_t iSendBuffSize,
					int32_t iSendThreadCount,
					int32_t iRecvThreadCount,
					int32_t iRecvBuffSize)
{
	m_lpCallback = lpCallback;
	m_bReBind = bReBind;
	m_nInitSendBuffSize = iSendBuffSize*1024;
	m_iSendThreadCount = iSendThreadCount;
	m_iRecvThreadCount = iRecvThreadCount;

	//初始化连接池
	m_lpConnectPool = NEW ConnectPoolLower(this, m_nInitSendBuffSize, iRecvBuffSize*1024);
	if( !m_lpConnectPool || m_lpConnectPool->Init() )
	{
		return TCP_ERROR_ALLOC_MEMORY_FAILED;
	}

	//初始化监听IP地址和端口
	m_addr.sin_family = AF_INET;
	if( lpIpAddr )
	{
		m_addr.sin_addr.s_addr = inet_addr((char*)lpIpAddr);
	}
	else
	{
		m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	m_addr.sin_port = htons(wPort);

	//开始监听
	int ret = Listen(bReBind);
	//cout << "CreateTcpServer Listen ret =" << ret << endl;
	if( !ret )
	{
		ret = CreateThread();
	}
	return ret;
}

int TcpServerLower::DelConnect(uint32_t nIndex)
{
	uint32_t index = nIndex&INDEX_LOW_MASK;
	if( index < MAX_ONLINE_CONNECT_COUNT )
	{
		m_lpConnectArray[index] = NULL;
	}
	return 0;
}

int TcpServerLower::CheckConnection()
{
	for(uint32_t i=0; i<MAX_ONLINE_CONNECT_COUNT; ++i)
	{
		CAcceptConnectLower* lpConnect = m_lpConnectArray[i];
		if( lpConnect )
		{
			//检查该连接最近发送数据和接收数据的时间
			timeval tnow;
			gettimeofday(&tnow, NULL);
			int32_t nSendInterval = lpConnect->GetTimeDiff(lpConnect->GetSendTimestamp(), &tnow);
			int32_t nRecvInterval = lpConnect->GetTimeDiff(lpConnect->GetRecvTimestamp(), &tnow);
			int32_t nHeartbeatInterval = lpConnect->GetHeartbeatInterval();
			int32_t nTimeoutInterval = lpConnect->GetTimeoutInterval();
			int ret = 0;
			if( nHeartbeatInterval )
			{
				if(	nSendInterval>=nHeartbeatInterval &&
					nRecvInterval>=nHeartbeatInterval )
				{
					ret = m_lpCallback->HandleHeartbeat(lpConnect->GetIndex(),
					*lpConnect->GetArgPointer(),
					nSendInterval>nRecvInterval?nRecvInterval:nSendInterval );
				}
				else if(nSendInterval>=nHeartbeatInterval )
				{
					ret = m_lpCallback->HandleHeartbeat(lpConnect->GetIndex(),
						*lpConnect->GetArgPointer(), nSendInterval );
				}
			}
			if( (ret==0) && nTimeoutInterval )
			{
				if( nRecvInterval >= nTimeoutInterval )
				{
					ret = m_lpCallback->HandleTimeout(lpConnect->GetIndex(),
						*lpConnect->GetArgPointer(), nRecvInterval );
				}
			}
			//心跳或者超时处理出错，关闭连接
			if( ret )
			{
				lpConnect->Disconnect();
			}
		}
	}
	return 0;
}

int TcpServerLower::OnAccept()
{
	int ret = 0;
	struct sockaddr_in addr;
	socklen_t clilen = sizeof(addr);
	int fd = 0;
	while (true)
	{
		fd = accept(m_fd, (struct sockaddr*)&addr, &clilen);
		if (fd <= 0)
		{
			//close(fd);
			break;
		}
		char ipaddr[50];
		uint16_t port = ntohs( addr.sin_port );
		CAcceptConnectLower* lpConnect = m_lpConnectPool->GetConnect();
		if( lpConnect )
		{
			lpConnect->Init(fd, &addr);
			SetNonBlocking(fd, 1);
			lpConnect->GetPeerIpAddr(ipaddr, sizeof(ipaddr));
			//从连接池中获取出来后，连接编号就已经确定
			//连接编号的低16位就是在m_lpConnectArray中的索引
			int iArrayIndex = lpConnect->GetIndex()&INDEX_LOW_MASK;
			//在回调之前，需要设置在线以备应用层在HandleAccepted中发送数据
			m_lpConnectArray[iArrayIndex] = lpConnect;
			//在回调之前，需要将套接字放入发送数据线程中
			m_lpSendThread[iArrayIndex%m_iSendThreadCount].AddConnect(lpConnect);
			//回调应用层，如果返回不为0，则关闭连接
			int accept = m_lpCallback->HandleAccepted(lpConnect->GetIndex(),
				lpConnect->GetArgPointer(), ipaddr, port);
			if( accept )
			{
				close(fd);
				DelConnect(lpConnect->GetIndex());
				lpConnect->Release();
			}
			else
			{
				//在回调成功后，将套接字放入接收数据线程中
				m_lpRecvThread[iArrayIndex%m_iRecvThreadCount].AddConnect(lpConnect);
			}
		}
		else
		{
			uint32_t ip = ntohl(addr.sin_addr.s_addr);
			char errbuff[ERROR_BUFF_SIZE];
			snprintf(errbuff,ERROR_BUFF_SIZE,"failed to establish the connection to client[%d.%d.%d.%d:%d],no memory or connection to much",
				(ip>>24)&0xff,(ip>>16)&0xff,(ip>>8)&0xff,(ip&0xff),port);
			m_lpCallback->HandleError(OE_ALLOC_MEMORY_FAILED, (uint32_t)-1, NULL, errbuff);
			close(fd);
		}
	}

	if (fd < 0)
	{
		if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR){
			printf("%s: bad accept\n");
		}
	}

	return ret;
}


int CreateTcpServer(
					ITcpServerFunction** lppServer,
					const char* lpIpAddr,
					uint16_t wPort,
					ITcpServerModule* lpCallback,
					bool bReBind,
					int32_t iSendBuffSize,
					int32_t iSendThreadCount,
					int32_t iRecvThreadCount,
					int32_t iRecvBuffSize)
{
	int ret = TCP_ERROR_ALLOC_MEMORY_FAILED;
	//先检查传入的参数
	if( !lppServer || !lpCallback || !wPort)
	{
		return TCP_ERROR_WRONG_PARAM;
	}
	//发送缓存最小为10K
	if( iSendBuffSize < 10 )
	{
		iSendBuffSize = 10;
	}

	//发送缓存最小为10K
	if (iRecvBuffSize < 10)
	{
		iRecvBuffSize = 10;
	}
	//检查发送线程数
	if( iSendThreadCount < MIN_THREAD_COUNT )
	{
		iSendThreadCount = MIN_THREAD_COUNT;
	}
	else if( iSendThreadCount>MAX_THREAD_COUNT )
	{
		iSendThreadCount = MAX_THREAD_COUNT;
	}
	//检查接收线程数
	if( iRecvThreadCount < MIN_THREAD_COUNT )
	{
		iRecvThreadCount = MIN_THREAD_COUNT;
	}
	else if( iRecvThreadCount>MAX_THREAD_COUNT )
	{
		iRecvThreadCount = MAX_THREAD_COUNT;
	}
	//cout << "CreateTcpServer ret =" << ret << endl;
	//创建侦听对象
	TcpServerLower* lpServer = NEW TcpServerLower;
	if( lpServer )
	{
		ret = lpServer->Init(lpIpAddr, wPort, lpCallback, bReBind,
			iSendBuffSize, iSendThreadCount, iRecvThreadCount, iRecvBuffSize);
		if( ret )
		{
			delete lpServer;
			lpServer = NULL;
		}
	}
	//cout << "CreateTcpServer ret =" << ret << endl;
	*lppServer = lpServer;
	if( lpServer )
	{
		//开始接收连接
		ret = lpServer->GetAcceptThread()->AddAcceptor(lpServer);
		if( ret )
		{
			delete lpServer;
			*lppServer = NULL;
		}
	}

	//cout << "CreateTcpServer ret =" << ret << endl;
	return ret;
}

int DestroyTcpServer(ITcpServerFunction* lpServer)
{
	int ret = TCP_ERROR_WRONG_PARAM;
	if( lpServer )
	{
		TcpServerLower* pServer = dynamic_cast<TcpServerLower*>(lpServer);
		if( pServer )
		{
			delete pServer;
			ret = 0;
		}
	}
	return ret;
}
