#include "GateInnerServer.h"
#include "TcpMsgStr.h"
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "configpars.h"
#include "log.h"
#include "GateInner_info.h"
#include "HandleInnermsgthread.h"
using namespace std;
GateInnerServerModule::GateInnerServerModule() :m_Gateserverlogic(NULL)
{
	m_Gateserverlogic = new GateInnerServerlogic;
	for (int i = 0; i < 20;i++)
	{
		m_serversubSockIndex[i] = -1;
	}
}

GateInnerServerModule::~GateInnerServerModule()
{
	if (m_Gateserverlogic)
	{
		delete m_Gateserverlogic;
	}
	m_Gateserverlogic = NULL;
}
int GateInnerServerModule::Init()
{
	m_lpTcpServer=NULL;
	return 0;
}
int GateInnerServerModule::HandleAccepted(uint32_t nSockIndex, void**lppArg, const char* lpIPAddr, uint16_t wPort)
{
	//连接上来的时候主动把nSockIndex发给连接的组件,连接组件收到后再把这个连接索引发过来，通过消息的msgfree位来区别保存
	MSG_STR tmpmsg;
	tmpmsg.head.MessageUid = 0;
	tmpmsg.head.MessageSize = 4;
	tmpmsg.head.MessageCmd = MSG_CMD_1000;
	tmpmsg.head.MessageSCmd = MSG_SCMD_THOURAND_1;
	tmpmsg.head.HeadPad = CLIENTGFLOG;
	tmpmsg.head.MessageEnd = CLIENTGFLOG;
	tmpmsg.head.MessageFree = m_gateindex;
	memcpy(&tmpmsg.msg[0], &nSockIndex, 4);
	INFO("GateInnerServerModule::HandleAccepted =%d",nSockIndex);
	int packlen = tmpmsg.head.MessageSize + sizeof(STR_MSG_HEAD);
	m_lpTcpServer->SendData(nSockIndex, (void*)(&tmpmsg), packlen);
	return 0;
}

int GateInnerServerModule::HandleDisconnect(uint32_t nSockIndex, void* lpArg)
{
	static int disnum = 0;
	disnum++;
	cout<<"GateInnerServerModule::HandleDisconnect,num is:"<<disnum<<endl;
	for (int i = 0; i < 20; i++)
	{
		if (m_serversubSockIndex[i] == nSockIndex)
		{
			m_serversubSockIndex[i] = -1;
			break;
		}

	}
	return 0;
}

int GateInnerServerModule::HandleRecvRawData(uint32_t nSockIndex, void* lpArg, const char* lpData, uint32_t nLen)
{
	//这样一个包一个包接收的时候会导致底层消息缓存满，之前设置的65k，改成200k.
	if (nLen<sizeof(STR_MSG_HEAD))//如果收到的消息小于消息头的长度则返回消息头长度，让继续接收
	{
		return sizeof(STR_MSG_HEAD);
	}
	
	int msgheadpad = ((STR_MSG_HEAD*)lpData)->HeadPad;
	int msgtailpad = ((STR_MSG_HEAD*)lpData)->MessageEnd;
	if (msgheadpad != CLIENTGFLOG || msgtailpad != CLIENTGFLOG)
	{
		int msgfree = ((STR_MSG_HEAD*)lpData)->MessageFree;
		int msgsize = ((STR_MSG_HEAD*)lpData)->MessageSize;
		cout << "GateInnerServerModule msg pad wrong:" << msgtailpad << msgheadpad << msgfree << msgsize << endl;
		return nLen;//说明这个消息出错了，返回nLen，这段消息不要
	}
	
	int nLeng = ((STR_MSG_HEAD*)lpData)->MessageSize + sizeof(STR_MSG_HEAD);
	//发现如果返回nLen会导致粘包,所以在网关中把MessageSize加4
	
	return nLeng;
}

int GateInnerServerModule::HandleRecvBiz(uint32_t nSockIndex, void* lpArg, const char* lpBizData, uint32_t nBizLen)
{

	if (nBizLen >= sizeof(STR_MSG_HEAD))
	{
		int nLeng = ((STR_MSG_HEAD*)lpBizData)->MessageSize + sizeof(STR_MSG_HEAD);
		int msgheadpad = ((STR_MSG_HEAD*)lpBizData)->HeadPad;
		int msgtailpad = ((STR_MSG_HEAD*)lpBizData)->MessageEnd;
		if (msgheadpad != CLIENTGFLOG || msgtailpad != CLIENTGFLOG)return 0;
		if (nLeng >= MAXLENGTH)
		{
			return 0;
		}
		
		if (nLeng == nBizLen)
		{
			/*char buff[MAXLENGTH];
			memset(buff, 0, MAXLENGTH);
			memcpy(buff, lpBizData, nBizLen);*/
			HandlerinnermsgThread::getInstance()->addonemsg(lpBizData,nBizLen);
		}
	}
	
	return 0;
}

int GateInnerServerModule::HandleError(int32_t iErrNo, uint32_t nSockIndex, void* lpArg, const char* lpErrMsg)
{
	cout << "GateInnerServerModule::HandleError" << lpErrMsg << endl;
	return 0;
}

int GateInnerServerModule::HandleHeartbeat(uint32_t nSockIndex, void* lpArg, uint32_t nTime)//返回0不断开连接，非0断开连接
{
	cout<<"GateInnerServerModule::HandleHeartbeat"<<endl;
	return 0;
}

int GateInnerServerModule::HandleTimeout(uint32_t nSockIndex, void* lpArg, uint32_t nTime)//返回0不断开连接，非0断开连接
{
	cout<<"GateInnerServerModule::HandleTimeout"<<endl;
	return 0;
}


int GateInnerServerModule::SendMsgtoSubMoudle(int uid, MSG_STR *msg, int subid, int toallsub, int sec)
{
	if (subid < 0 || subid >= 20 || m_serversubSockIndex[subid] < 0)
	{
		ERROR("SendMsgtoSubMoudle wrong subid=%d", subid);
		return 0;
	}
	int packlen = ((STR_MSG_HEAD*)msg)->MessageSize + sizeof(STR_MSG_HEAD);
	int msglen = ((STR_MSG_HEAD*)msg)->MessageSize;
	if (msglen > MAXLENGTH)
	{
		ERROR("SendMsgtoSubMoudle out of siez=%d", msglen);
		return 0;
	}
	//((STR_MSG_HEAD*)msg)->MessageFree = SUB_NUM;
	m_lpTcpServer->SendData(m_serversubSockIndex[subid], (void*)(msg), packlen);
	return 0;
}
