#ifndef GATEINNERSERVER_H_
#define GATEINNERSERVER_H_
#include <tcpservervirfunciton.h>
#include "share/singleton.h"
#include "GateInnerServerlogic.h"
#include "TcpMsgStr.h"

class GateInnerServerModule : public ITcpServerModule, public singleton<GateInnerServerModule>
{
public:
	GateInnerServerModule();

	~GateInnerServerModule();
public:
	int Init();

	int HandleAccepted(uint32_t nSockIndex,void**lppArg,const char* lpIPAddr,uint16_t wPort);

    int HandleDisconnect(uint32_t nSockIndex, void* lpArg);

	int HandleRecvRawData(uint32_t nSockIndex,void* lpArg,const char* lpData,uint32_t nLen);

	int HandleRecvBiz(uint32_t nSockIndex,void* lpArg,const char* lpBizData,uint32_t nBizLen);

	int HandleError(int32_t iErrNo,uint32_t nSockIndex,void* lpArg,const char* lpErrMsg);

	int HandleHeartbeat(uint32_t nSockIndex, void* lpArg, uint32_t nTime);

	int HandleTimeout(uint32_t nSockIndex, void* lpArg, uint32_t nTime);

	GateInnerServerlogic* GetServerLogic(){ return m_Gateserverlogic; }

	void setsubSokcetindex(int subnum, int subSockIndex){ m_serversubSockIndex[subnum] = subSockIndex; }

public:
	//向内部组件转发消息
	int SendMsgtoSubMoudle(int uid, MSG_STR *msg, int subid, int toallsub = 0, int sec = 0);
	
public:
	ITcpServerFunction* m_lpTcpServer;
	GateInnerServerlogic* m_Gateserverlogic;
	int m_serversubSockIndex[20];//连接内部网关的服务器组件连接索引
	int m_gateindex;
};
#endif 
