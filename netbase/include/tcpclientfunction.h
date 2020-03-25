#ifndef TCPCLIENTFUNCTION_H_
#define TCPCLIENTFUNCTION_H_
#include <stdint.h>
#include <stdio.h>
class ITcpClientFunction
{
protected:
	virtual ~ITcpClientFunction(){}
public:
	virtual int Connect(const char* lpServerAddr = NULL,
			uint16_t wServerPort = 0,
			bool bBlockConnect = false,
			const char* lpLocalAddr = NULL) = 0;
	 virtual int SendData(
								const void* lpData,
								int32_t iDataLen,
								int32_t iWaitTime = 500,
								uint32_t nBlockFlag = 0 ) = 0;
	 virtual int Disconnect()=0;
	 virtual int ReConnect(
	 							const char*	lpServerAddr = NULL,
	 							uint16_t	wServerPort = 0,
	 							bool bBlockConnect = false,
	 							const char* lpLocalAddr = NULL ) = 0;
	 virtual int SetHeartbeat(int32_t iTime)=0;
	 virtual int SetTimeout(int32_t iTime)=0;
	 virtual int SetQuickAck(bool bQuickAck)=0;
	 virtual int SetTcpNoDelay(bool bNoDelay)=0;

	 virtual int SetTcpKeepAlive(bool bKeepAlive)=0;

	virtual int SetAutoReConnect(bool bAutoReConnect)=0;

	virtual bool GetAutoReConnect()=0;
	virtual bool GetConnectStatus()=0;

	virtual int GetConnectCount()=0;
};
class ITcpClientModule{
public:
	virtual ~ITcpClientModule(){}
	virtual int OnConnected()=0;

	virtual int HandleDisconnect()=0;

	virtual int HandleRecvRawData(const char* lpData, uint32_t nLen)=0;

	virtual int HandleRecvBiz(const char* lpBizData, uint32_t nBizLen)=0;

	virtual int HandleError(int32_t iErrNo, const char* lpErrMsg)=0;

	virtual int HandleHeartbeat(uint32_t nTime)=0;

	virtual int HandleTimeout(uint32_t nTime)=0;
};
class ITcpClientCreator{
protected:
	virtual ~ITcpClientCreator(){}
public:
	 virtual int Init(int32_t iSendThreadCount = 1, int32_t iRecvThreadCount = 1 ) = 0;

    virtual int CreateTcpClient(
										ITcpClientFunction** lppClient,
										const char*  lpServerAddr,
										uint16_t     wServerPort,
										ITcpClientModule* lpCallback,
										uint32_t nSendBuffSize = 2048,
										const char* lpLocalAddr = NULL,
										int32_t iRecvBuffSize = 2048) = 0;
};

#ifdef __cplusplus
extern "C"{
#endif

ITcpClientCreator* GetTcpClientCreator();//
int DestroyTcpClientCreator(ITcpClientCreator* lpClientCreator);

#ifdef __cplusplus
}
#endif
#endif
