#ifndef TCPACCEPTCONNECTLOWER_H_
#define TCPACCEPTCONNECTLOWER_H_
#include <tcpbase/tcpbaseconnect.h>
#include <include/tcpservervirfunciton.h>
#include "tcpacceptconnectlower.h"
class ConnectPoolLower;
class TcpServerLower;
class CAcceptConnectLower:public CTcpBaseConnect
{
public:
	friend class ConnectPoolLower;
	CAcceptConnectLower(uint32_t nInitSendBuffSize,
			uint32_t nIndex,
			ITcpServerModule* lpCallback,
			ConnectPoolLower* lpPool,
			uint32_t nInitRecvBuffSize);
	int Release();
	virtual int OnConnected();
	virtual int HandleDisconnect();
	virtual int HandleRecvRawData(const char* lpData, uint32_t nLen);
	virtual int HandleRecvBiz(const char* lpBizData, uint32_t nBizLen);
	virtual int HandleError(int32_t iErrNo, const char* lpErrMsg);
	virtual bool IsConnected();
	virtual bool DealDisconnect();

	///获取连接编号
	uint32_t GetIndex(){return m_nIndex;}
	///获取存放应用层指针的指针
	void**	GetArgPointer(){return &m_lpArg;}

protected:
		///本链接对象的编号，编号分高16位和低16位
	uint32_t		m_nIndex;
		///在HandleAccepted时，应用层设置的指针
	void*			m_lpArg;
		///回调应用层的回调接口
	ITcpServerModule*	m_lpCallback;
		///连接池
	ConnectPoolLower*		m_lpPool;
		///在连接池中，指向下一个空闲链接
	CAcceptConnectLower*	m_lpNext;

};
#endif
