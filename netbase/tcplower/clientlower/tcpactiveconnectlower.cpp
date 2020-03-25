#include "tcpactiveconnectlower.h"
#include "tcpclientlower.h"
ActiveConnectLower::ActiveConnectLower(uint32_t nInitSendBuffSize, TcpClientLower* lpTcpClient, uint32_t iRecvBuffSize)
:CTcpBaseConnect(nInitSendBuffSize, iRecvBuffSize), m_lpTcpClient(lpTcpClient), m_bIsConnected(false)
{
}

int ActiveConnectLower::OnConnected()
{
	bool bCallback=false;
	int ret = 0;
	m_RecvDataMutex.lock();
	m_SendDataMutex.lock();
	//需要判断套接字和连接状态
	if( m_fd != -1 && !m_bIsConnected )
	{
		bCallback=true;
		m_bIsConnected = true;
		m_nSendDataOffset = 0;
		m_nSendDataLen = 0;
		m_iPacketLen = 0;
		m_nRecvDataLen	=0;
		m_SendDataNotify.Reset();
		gettimeofday(&m_nSendTimestamp, NULL);
		gettimeofday(&m_nRecvTimestamp, NULL);
	}
	m_lpTcpClient->SetConnectTimes();
	m_SendDataMutex.unlock();
	m_RecvDataMutex.unlock();
	//只有在原来连接是断开时，才回调应用层，保证只回调一次
	if( bCallback )
	{
		ret = m_lpTcpClient->GetCallback()->OnConnected();
	}
	return ret;
}

int ActiveConnectLower::HandleDisconnect()
{
	return m_lpTcpClient->GetCallback()->HandleDisconnect();
}

int ActiveConnectLower::HandleRecvRawData(const char* lpData, uint32_t nLen)
{
	return m_lpTcpClient->GetCallback()->HandleRecvRawData(lpData, nLen);
}

int ActiveConnectLower::HandleRecvBiz(const char* lpBizData, uint32_t nBizLen)
{
	return m_lpTcpClient->GetCallback()->HandleRecvBiz(lpBizData, nBizLen);
}

int ActiveConnectLower::HandleError(int32_t iErrNo, const char* lpErrMsg)
{
	return m_lpTcpClient->GetCallback()->HandleError(iErrNo, lpErrMsg);
}

bool ActiveConnectLower::IsConnected()
{
	return m_bIsConnected;
}

bool ActiveConnectLower::DealDisconnect()
{
	if( m_bIsConnected )
	{
		m_lpTcpClient->DelFromEpoll();
		m_bIsConnected = false;
		return true;
	}
	return false;
}
