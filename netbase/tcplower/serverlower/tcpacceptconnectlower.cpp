#include "tcpacceptconnectlower.h"
#include "tcpconnectpoollower.h"
#include "tcpserverlower.h"
CAcceptConnectLower::CAcceptConnectLower(
	uint32_t nInitSendBuffSize,
	uint32_t nIndex,
	ITcpServerModule*lpCallback,
	ConnectPoolLower*lpPool,
	uint32_t nInitRecvBuffSize)
	:CTcpBaseConnect(nInitSendBuffSize, nInitRecvBuffSize), m_nIndex(nIndex), m_lpArg(NULL),
	m_lpCallback(lpCallback),m_lpPool(lpPool), m_lpNext(NULL)
{
}

int CAcceptConnectLower::Release()
{
	return m_lpPool->ReleaseConnect(this);
}

int CAcceptConnectLower::OnConnected()
{
	return 0;
}

int CAcceptConnectLower::HandleDisconnect()
{
	m_lpCallback->HandleDisconnect(m_nIndex, m_lpArg);
	this->Release();
	return 0;
}

int CAcceptConnectLower::HandleRecvRawData(const char* lpData, uint32_t nLen)
{
	return m_lpCallback->HandleRecvRawData(m_nIndex, m_lpArg, lpData, nLen);
}

int CAcceptConnectLower::HandleRecvBiz(const char* lpBizData, uint32_t nBizLen)
{
	return m_lpCallback->HandleRecvBiz(m_nIndex, m_lpArg, lpBizData, nBizLen);
}

int CAcceptConnectLower::HandleError(int32_t iErrNo, const char* lpErrMsg)
{
	return m_lpCallback->HandleError(iErrNo, m_nIndex, m_lpArg, lpErrMsg);
}

bool CAcceptConnectLower::IsConnected()
{
	return m_fd != -1;
}

bool CAcceptConnectLower::DealDisconnect()
{
	m_lpPool->m_lpServer->DelConnect(m_nIndex);
	return true;
}
