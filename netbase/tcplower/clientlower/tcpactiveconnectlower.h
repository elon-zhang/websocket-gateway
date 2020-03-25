#ifndef TCPACTIVECONNECTLOWER_H_
#define TCPACTIVECONNECTLOWER_H_
#include <tcpbase/tcpbaseconnect.h>
//#include "tcpclientlower.h"
class TcpClientLower;
class ActiveConnectLower: public CTcpBaseConnect
{
public:
	ActiveConnectLower(uint32_t nInitSendBuffSize, TcpClientLower* lpInitiator, uint32_t iRecvBuffSize);

public:
	//CTcpBaseConnect回调的接口函数实现
	virtual int OnConnected();
	virtual int HandleDisconnect();
	virtual int HandleRecvRawData(const char* lpData, uint32_t nLen);
	virtual int HandleRecvBiz(const char* lpBizData, uint32_t nBizLen);
	virtual int HandleError(int32_t iErrNo, const char* lpErrMsg);
	virtual bool IsConnected();
	virtual bool DealDisconnect();

	///设置连接状态
	void SetConnectStatus(bool status){m_bIsConnected=status;}
protected:
	///主动连接对象指针
	TcpClientLower* m_lpTcpClient;
	///连接状态，是否连接上
	bool		m_bIsConnected;
};
#endif
