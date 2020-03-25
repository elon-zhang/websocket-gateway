#include "tcpclientcreatorlower.h"
TcpClientCreatorLower*	glpTcpClientCreator=NULL;
TcpClientCreatorLower::TcpClientCreatorLower()
	:m_lpReConnectThread(NULL), m_lpSendThread(NULL)
	,m_lpRecvThread(NULL), m_iRefCount(0)
	,m_iSendThreadCount(0), m_iRecvThreadCount(0)
{
}

TcpClientCreatorLower::~TcpClientCreatorLower()
{
	if( m_iRefCount != 0 )
	{
		//BUG(("%s:%d %s 引用计数错误\n",__FILE__,__LINE__,__FUNCTION__));
	}
	Cleanup();
}

void TcpClientCreatorLower::Cleanup()
{
	///首先停止重连线程
	if( m_lpReConnectThread )
	{
		m_lpReConnectThread->stop();
	}
	///然后断开主动连接对象
	for( int i=0; i<(int)m_vInitiator.size(); ++i)
	{
		m_vInitiator[i]->Disconnect();
	}
	///停止发送线程
	for(int i=0; i<m_iSendThreadCount; ++i)
	{
		if( m_lpSendThread )
		{
			m_lpSendThread[i].stop();
		}
	}
	///停止接收线程
	for(int i=0; i<m_iRecvThreadCount; ++i)
	{
		if( m_lpRecvThread )
		{
			m_lpRecvThread[i].stop();
		}
	}
	///释放线程类的内存
	if( m_lpReConnectThread )
	{
		delete m_lpReConnectThread;
		m_lpReConnectThread = NULL;
	}
	if( m_lpSendThread )
	{
		delete[] m_lpSendThread;
		m_lpSendThread = NULL;
	}
	if( m_lpRecvThread )
	{
		delete[] m_lpRecvThread;
		m_lpRecvThread = NULL;
	}
	///释放主动连接对象的内存
	for( int i=0; i<(int)m_vInitiator.size(); ++i)
	{
		delete m_vInitiator[i];
	}
	m_vInitiator.clear();
}

int TcpClientCreatorLower::Init(int32_t iSendThreadCount, int32_t iRecvThreadCount)
{
	int ret = TCP_ERROR_INIT_AGAIN;
	sMutex.lock();
	if( !m_lpReConnectThread )
	{
		//设置发送线程个数
		if( iSendThreadCount < MIN_THREAD_COUNT )
		{
			iSendThreadCount = MIN_THREAD_COUNT;
		}
		else if( iSendThreadCount > MAX_THREAD_COUNT )
		{
			iSendThreadCount = MAX_THREAD_COUNT;
		}
		m_iSendThreadCount = iSendThreadCount;

		//设置接收线程个数
		if( iRecvThreadCount < MIN_THREAD_COUNT )
		{
			iRecvThreadCount = MIN_THREAD_COUNT;
		}
		else if( iRecvThreadCount > MAX_THREAD_COUNT )
		{
			iRecvThreadCount = MAX_THREAD_COUNT;
		}
		m_iRecvThreadCount = iRecvThreadCount;

		//创建线程类
		m_lpReConnectThread = NEW ReConnectThreadLower;
		m_lpSendThread = NEW CTcpSendThread[m_iSendThreadCount];
		m_lpRecvThread = NEW CTcpRecvThread[m_iRecvThreadCount];
		//初始化各个线程
		if(m_lpReConnectThread && m_lpSendThread && m_lpRecvThread)
		{
			//初始化发送线程，并启动
			for(int i=0; i<m_iSendThreadCount; ++i)
			{
				ret = m_lpSendThread[i].Init();
				if( !ret )
				{
					m_lpSendThread[i].start();
				}
				else
				{
					break;
				}
			}
			if( !ret )
			{
				//初始化接收线程并启动
				for(int i=0; i<m_iRecvThreadCount; ++i)
				{
					ret = m_lpRecvThread[i].Init();
					if( !ret )
					{
						m_lpRecvThread[i].start();
					}
					else
					{
						break;
					}
				}
				if( !ret )
				{
					m_lpReConnectThread->start();
				}
			}
		}
		else
		{
			ret = TCP_ERROR_ALLOC_MEMORY_FAILED;
		}
		//如果有线程初始化失败，则清理线程类
		if( ret )
		{
			Cleanup();
		}
	}
	sMutex.unlock();
	return ret;
}

int TcpClientCreatorLower::CreateTcpClient(
										ITcpClientFunction** lppTcpClient,
										const char*  lpServerAddr,
										uint16_t     wServerPort,
										ITcpClientModule* lpCallback,
										uint32_t nSendBuffSize,
										const char* lpLocalAddr,
										int32_t iRecvBuffSize)
{
	int ret = 0;
	TcpClientLower* lpTcpClient = NULL;
	if( !lppTcpClient || !lpServerAddr || !lpCallback || !wServerPort )
	{
		return TCP_ERROR_WRONG_PARAM;
	}
	//首先查看一下工厂是否已经初始化
	sMutex.lock();
	if( !m_lpReConnectThread )
	{
		ret = TCP_ERROR_NOT_INIT;
	}
	sMutex.unlock();
	if( !ret )
	{
		//设置发送缓存
		if(nSendBuffSize < 1)
		{
			nSendBuffSize = 1;
		}
		nSendBuffSize *= 1024;

		//创建CInitiator对象
		lpTcpClient = NEW TcpClientLower(lpCallback, nSendBuffSize, false, iRecvBuffSize*1024);
		if( lpTcpClient )
		{
			sMutex.lock();
			ret = lpTcpClient->Init( m_vInitiator.size()%m_iSendThreadCount,
									m_vInitiator.size()%m_iRecvThreadCount,
									lpServerAddr, wServerPort, lpLocalAddr );
			if( ret )
			{
				delete lpTcpClient;
				lpTcpClient = NULL;
			}
			else
			{
				try{
					m_vInitiator.push_back(lpTcpClient);
				}
				catch(std::bad_alloc){
					delete lpTcpClient;
					lpTcpClient=NULL;
					ret = TCP_ERROR_ALLOC_MEMORY_FAILED;
				}
			}
			sMutex.unlock();
		}
		else
		{
			ret = TCP_ERROR_ALLOC_MEMORY_FAILED;
		}
	}
	*lppTcpClient = lpTcpClient;
	return ret;
}

int TcpClientCreatorLower::AddRef()
{
	 return __sync_add_and_fetch(&m_iRefCount,1);
}

int TcpClientCreatorLower::Release()
{
	int ref = __sync_sub_and_fetch(&m_iRefCount,1);
	if( ref == 0 )
	{
		///删除工厂，并重置glpTcpClientCreator指针
		delete glpTcpClientCreator;
		glpTcpClientCreator = NULL;
	}
	return ref;
}

int TcpClientCreatorLower::CheckConnection()
{
	char errbuff[ERROR_BUFF_SIZE];
	sMutex.lock();
	for(uint32_t i=0; i<m_vInitiator.size(); ++i)
	{
		TcpClientLower *lpTcpClient = m_vInitiator[i];
		if( lpTcpClient->NeedReConnect() )
		{
			int ret = lpTcpClient->ConnectNonBlock();
			if( ret )
			{
				snprintf(errbuff,ERROR_BUFF_SIZE,"connect error,connect times:%d,return code:%d,errno:%d strerror:%s",
						lpTcpClient->GetConnectTimes(), ret, errno, strerror(errno));
				lpTcpClient->GetCallback()->HandleError(OE_SYSTEM_ERROR, errbuff);
				lpTcpClient->Disconnect();
			}
			else
			{
				lpTcpClient->Add2Epoll();
			}
		}
		else if( lpTcpClient->GetConnectStatus() )
		{
			//检查该连接最近发送数据和接收数据的时间
			timeval tnow;
			gettimeofday(&tnow, NULL);
			int32_t nSendInterval = lpTcpClient->m_Connect.GetTimeDiff(
					lpTcpClient->m_Connect.GetSendTimestamp(), &tnow );
			int32_t nRecvInterval = lpTcpClient->m_Connect.GetTimeDiff(
					lpTcpClient->m_Connect.GetRecvTimestamp(), &tnow );
			int32_t nHeartbeatInterval = lpTcpClient->m_Connect.GetHeartbeatInterval();
			int32_t nTimeoutInterval = lpTcpClient->m_Connect.GetTimeoutInterval();

			//回调上层应用，需要释放锁，防止可能存在的死锁
			sMutex.unlock();
			int ret = 0;
			if( nHeartbeatInterval )
			{
				if(nSendInterval>=nHeartbeatInterval &&
					nRecvInterval>=nHeartbeatInterval )
				{
					ret = lpTcpClient->GetCallback()->HandleHeartbeat(nSendInterval>nRecvInterval?nRecvInterval:nSendInterval);

				}
				else if(nSendInterval>=nHeartbeatInterval )
				{
					ret = lpTcpClient->GetCallback()->HandleHeartbeat(nSendInterval);
				}
			}
			if( !ret && nTimeoutInterval )
			{
				if( nRecvInterval >= nTimeoutInterval )
				{
					ret = lpTcpClient->GetCallback()->HandleTimeout(nRecvInterval);
				}
			}
			//心跳或者超时处理出错，关闭连接
			if( ret )
			{
				lpTcpClient->Disconnect();
			}
			sMutex.lock();
		}
	}
	sMutex.unlock();
	return 0;
}

SYSThreadMutex TcpClientCreatorLower::sMutex;
ITcpClientCreator* GetTcpClientCreator()
{
	TcpClientCreatorLower::sMutex.lock();
	if( !glpTcpClientCreator )
	{
		glpTcpClientCreator = NEW TcpClientCreatorLower;
	}
	if( glpTcpClientCreator )
	{
		glpTcpClientCreator->AddRef();
	}
	TcpClientCreatorLower::sMutex.unlock();
	return glpTcpClientCreator;
}

int DestroyTcpClientCreator(ITcpClientCreator* lpTcpClientCreator)
{
	if( lpTcpClientCreator == glpTcpClientCreator )
	{
		//此处不能加锁，否则可能死锁
		glpTcpClientCreator->Release();
		return 0;
	}
	return TCP_ERROR_WRONG_PARAM;
}
