#include "GateServer_websocket.h"
#include "TcpMsgStr.h"
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "configpars.h"
#include "log.h"
#include "GateInnerServer.h"
#include <cstring>
#include <sys/time.h>
#include <arpa/inet.h>
#include "Gate_info.h"
#include "HandleInnermsgthread.h"
#include "Handleoutmsgthread.h"
using namespace std;
GateServerModule::GateServerModule() 
{
	for (int i = 0; i < GATEMAXCONNECT;i++)
	{
		memset(&m_socketindex[i], 0, sizeof(GateSocketState));
	}
	pthread_mutex_init(&_condmutex, NULL);
}

GateServerModule::~GateServerModule()
{
	pthread_mutex_destroy(&_condmutex);
}
int GateServerModule::Init()
{
	m_lpTcpServer=NULL;
	return 0;
}
int GateServerModule::HandleAccepted(uint32_t nSockIndex, void**lppArg, const char* lpIPAddr, uint16_t wPort)
{
	//服务器不主动发送心跳
	//SetHeartbeat(nSockIndex, 15);
	//连接上来的时候设置超时为15秒
	m_lpTcpServer->SetTimeout(nSockIndex,18);
	//客户端连上来
	static int accptenum = 0;
	int index = nSockIndex&GATELOWERMASK;
	INFO("GateServerModule::HandleAccepted,index:%d acceptnum=%d", index, ++accptenum);
	resetGateSocketState(index, 0, 1, 0, nSockIndex, 0, 0);
	return 0;
}

int GateServerModule::HandleDisconnect(uint32_t nSockIndex, void* lpArg)
{
	//网关服需要维护一个数组，用nSockIndex做下标，uid为值，断线的时候把uid发给大厅服

	//客户端的连接断开
	int index = nSockIndex&GATELOWERMASK;
	INFO("HandleDisconnect nSockIndex=%d", nSockIndex);

	struct MSG_STR tmpmsg;
	memset(&tmpmsg, 0, sizeof(struct MSG_STR));
	tmpmsg.head.MessageSize = 0;
	tmpmsg.head.MessageCmd = MSG_CMD_100;
	tmpmsg.head.MessageSCmd = MSG_SCMD_HANDRED_2;
	tmpmsg.head.HeadPad = CLIENTGFLOG;
	tmpmsg.head.MessageEnd = CLIENTGFLOG;

	int uid = m_socketindex[index].uid;
	tmpmsg.head.MessageUid = uid;
	int sum = 0;
	memcpy(&tmpmsg.msg[sum], &index, 4);
	sum += 4;
	tmpmsg.head.MessageSize += 4;
	if (uid>0)//uid如果小于0，则说明，还没有发生登录，这时候没必要通知大厅
	{
		int len = sizeof(STR_MSG_HEAD)+tmpmsg.head.MessageSize;
		int fdindex = GateInnerServerModule::getInstance()->GetServerLogic()->GetuidFdconnectindex(uid);
		if (fdindex == index)//断线
			tmpmsg.msg[sum++] = 0;
		else//顶号
			tmpmsg.msg[sum++] = 1;
		tmpmsg.head.MessageSize += 1;
		GateInnerServerModule::getInstance()->GetServerLogic()->GetOutMsg(index,&tmpmsg);

		INFO("HandleDisconnect uid=%d,index=%d,findex=%d",uid,index,fdindex);
		if (fdindex == index)//这里做个相等判断，防止被顶号后清理信息，导致顶号收不到消息
			GateInnerServerModule::getInstance()->GetServerLogic()->ZeroUidInfo(uid);//断线后把玩家信息置零
	}
	resetGateSocketState(index, 0, 0, 0, 0, 0, 0);
	return 0;
}

int GateServerModule::HandleRecvRawData(uint32_t nSockIndex, void* lpArg, const char* lpData, uint32_t nLen)//返回小于0的值，底层会关闭连接
{
	int nLeng = 0;
	if (nLen > 0)
	{
		if ((*lpData) == 'G')//shake
		{
			nLeng = Handshake(nSockIndex, lpData, nLen);
		}
		else
		{
			nLeng = fetch_websocket_info(nSockIndex, lpData, nLen);
		}
	}
	else
	{
		return -1;
	}
	return nLeng ;
}

int GateServerModule::HandleRecvBiz(uint32_t nSockIndex, void* lpArg, const char* msg, uint32_t nBizLen)
{
	if ((*msg) != 'G')//do not need to handle shake 
	{
		int index = nSockIndex&GATELOWERMASK;
		int msglen = m_socketindex[index].webhead_length_ + m_socketindex[index].payload_length_;
		if (msglen != nBizLen)
		{
			ERROR("HandleRecvBiz not right len nBizLen= %d,msglen=%d", nBizLen, msglen);
			ExeDisconnect(index);
			return 0;
		}

		if (m_socketindex[index].payload_length_ <= sizeof(STR_MSG_HEAD))//如果收到的消息小于消息头的长度则断开游戏，网络环境不好，玩什么游戏（这样处理有个弊端，当一个消息分两次来的时候默认就丢掉了，稳妥的做法是放回缓存区等待继续接收）
		{
			ERROR("GateServerModule msg too short:%d", m_socketindex[index].payload_length_);
			ExeDisconnect(index);
			return -1;
		}

		char buff[MAXLENGTH];
		int len = 0;
		memset(buff, 0, MAXLENGTH);
		memcpy(buff, msg, nBizLen);
		int webheadlen = m_socketindex[index].webhead_length_;
		if (m_socketindex[index].mask_ == 1)
		{
			umask((buff + webheadlen), (nBizLen - webheadlen), m_socketindex[index].masking_key_);
		}
		int msgheadpad = ((STR_MSG_HEAD*)(buff + webheadlen))->HeadPad;
		int msgtailpad = ((STR_MSG_HEAD*)(buff + webheadlen))->MessageEnd;
		int msgrorder = ((STR_MSG_HEAD*)(buff + webheadlen))->MessageOrder;

		MSG_STR* msgcontent = ((MSG_STR*)(buff + webheadlen));
		int		msgcheck = 0x017;		//检验
		msgcheck = ((msgcheck | (msgcontent->head.MessageSize & 0x0ff)) | (msgcontent->head.MessageOrder & 0x0ff)) & ((msgcontent->head.MessageSCmd & 0x0ff) | (msgcontent->head.MessageUid & 0x0ff)) | (msgcontent->head.MessageToken & 0x0ff);
		msgcheck = msgcheck & 0x0ff;

		//int index = nSockIndex&GATELOWERMASK;
		if (msgheadpad != CLIENTGFLOG || msgtailpad != CLIENTGFLOG)
		{
			ERROR("GateServerModule msg pad wrong:%d,%d", msgheadpad, msgtailpad);
			ExeDisconnect(index);
			return -1;
		}
		else if (msgrorder != m_socketindex[index].fdrindex)//消息序号检查
		{
			ERROR("GateServerModule msgorder wrong:%d,%d", msgrorder, m_socketindex[index].fdrindex);
			ExeDisconnect(index);
			return -1;
		}
		else if (msgcheck != (msgcontent->head.MessageCheck & 0xff))//消息检验位置
		{
			ERROR("GateServerModule msgcheck wrong:%d,%d", msgcheck, (msgcontent->head.MessageCheck & 0xff));
			ExeDisconnect(index);
			return -1;
		}


		int nLeng = ((STR_MSG_HEAD*)(buff + webheadlen))->MessageSize + sizeof(STR_MSG_HEAD);
		if (nLeng != m_socketindex[index].payload_length_)
		{
			ERROR("GateServerModule::HandleRecvRawData %d,%d", nLeng, m_socketindex[index].payload_length_);
			ExeDisconnect(index);
			return -1;
		}

		
		int fdrindex = m_socketindex[index].fdrindex + 1;
		resetGateSocketState(index, -1, -1, -1, -1, fdrindex, -1);
		if (m_socketindex[index].fdrindex >= MSG_RMAX)
			resetGateSocketState(index, -1, -1, -1, -1, 0, -1);
		INFO("GateServerModule::HandleRecvBiz index=%d,senduid=%d", index, m_socketindex[index].uid);
		GateInnerServerModule::getInstance()->GetServerLogic()->GetOutMsg(index, (MSG_STR*)(buff + webheadlen));
	}
	return 0;
}



int GateServerModule::Handshake(uint32_t nSockIndex, const char* lpData, uint32_t nLen)
{
	string shakestr(lpData);
	size_t endpos = shakestr.find("\r\n\r\n");
	if (endpos != string::npos)
	{
		size_t  magicbeginpos = shakestr.find("Sec-WebSocket-Key: ");
		if (magicbeginpos == string::npos)
		{
			ERROR("can not find Sec-WebSocket-Key %s", shakestr.c_str());
			return -1;
		}
		char recvShakeKey[1024] = { 0 };
		size_t magicendpos = shakestr.find("\r",magicbeginpos);
		int seckeylen = strlen("Sec-WebSocket-Key: ");
		int lpkeylen = magicbeginpos + seckeylen;
		memcpy(recvShakeKey, lpData + lpkeylen, (magicendpos - lpkeylen));
		Askshake(nSockIndex, recvShakeKey);
		return (endpos + 4);
	}
	else
	{
		return (nLen + 1);//not complete need to recv again
	}
}

int GateServerModule::Askshake(uint32_t nSockIndex,const char* magickey)
{
	char request[1024] = { 0 };
	int len = 0;
	strcat(request, "HTTP/1.1 101 Switching Protocols\r\n");
	strcat(request, "Connection: upgrade\r\n");
	strcat(request, "Sec-WebSocket-Accept: ");
	std::string server_key = magickey;
	server_key += MAGIC_KEY;

	SHA1 sha;
	unsigned int message_digest[5];
	sha.Reset();
	sha << server_key.c_str();

	sha.Result(message_digest);
	for (int i = 0; i < 5; i++) {
		message_digest[i] = htonl(message_digest[i]);
	}
	server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
	server_key += "\r\n";
	strcat(request, server_key.c_str());
	strcat(request, "Upgrade: websocket\r\n\r\n");
	//int index = nSockIndex&GATELOWERMASK;
	len = strlen(request);
	m_lpTcpServer->SendData(nSockIndex, request, len);
	return 0;
}


int GateServerModule::fetch_websocket_info(uint32_t nSockIndex, const char *msg,int nLen)
{
	if (nLen < 2 )
	{
		return 2;
	}
	int nLeng = 0;
	int index = nSockIndex&GATELOWERMASK;
	int pos = 0;
	nLeng = fetch_fin(nSockIndex, msg, nLen, pos);
	nLeng = fetch_opcode(nSockIndex, msg, nLen, pos);
	nLeng = fetch_mask(nSockIndex, msg, nLen, pos);
	nLeng = fetch_payload_length(nSockIndex, msg, nLen, pos);
	if (nLeng == 0)nLeng = fetch_masking_key(nSockIndex, msg, nLen, pos);
	if (nLeng == 0)nLeng = fetch_payload(nSockIndex, msg, nLen, pos);
	return nLeng;
}

void GateServerModule::reset(uint32_t nSockIndex)
{
	int index = nSockIndex&GATELOWERMASK;
	m_socketindex[index].fin_ = 0;
	m_socketindex[index].opcode_ = 0;
	m_socketindex[index].mask_ = 0;
	memset(m_socketindex[index].masking_key_, 0, sizeof(m_socketindex[index].masking_key_));
	m_socketindex[index].payload_length_ = 0;
	m_socketindex[index].webhead_length_ = 0;
}

int GateServerModule::fetch_fin(uint32_t nSockIndex, const char *msg, int nLen, int &pos)
{
	int index = nSockIndex&GATELOWERMASK;
	m_socketindex[index].fin_ = (unsigned char)msg[pos] >> 7;
	if (m_socketindex[index].fin_ != 1)
	{
		ERROR("fetch_fin has not complete %d", m_socketindex[index].fin_);
	}
	m_socketindex[index].webhead_length_ = 2;
	return 0;
}

int GateServerModule::fetch_opcode(uint32_t nSockIndex, const char *msg, int nLen, int &pos)
{
	int index = nSockIndex&GATELOWERMASK;
	m_socketindex[index].opcode_ = msg[pos] & 0x0f;
	pos++;
	m_socketindex[index].webhead_length_ = 2;
	return 0;
}

int GateServerModule::fetch_mask(uint32_t nSockIndex, const char *msg, int nLen, int &pos)
{
	int index = nSockIndex&GATELOWERMASK;
	m_socketindex[index].mask_ = (unsigned char)msg[pos] >> 7;
	return 0;
}

int GateServerModule::fetch_payload_length(uint32_t nSockIndex, const char *msg, int nLen, int &pos)
{
	int index = nSockIndex&GATELOWERMASK;
	m_socketindex[index].payload_length_ = msg[pos] & 0x7f;
	pos++;
	if (m_socketindex[index].payload_length_ == 126){
		if (nLen < (2+2))
		{
			return (2 + 2);
		}
		uint16_t length = 0;
		memcpy(&length, msg + pos, 2);
		pos += 2;
		m_socketindex[index].payload_length_ = ntohs(length);
		m_socketindex[index].webhead_length_ += 2;
	}
	else if (m_socketindex[index].payload_length_ == 127){
		if (nLen < (2 + 4))
		{
			return (2 + 4);
		}
		uint32_t length = 0;
		memcpy(&length, msg + pos, 4);
		pos += 4;
		m_socketindex[index].payload_length_ = ntohl(length);
		m_socketindex[index].webhead_length_ += 4;
	}
	return 0;
}

int GateServerModule::fetch_masking_key(uint32_t nSockIndex, const char *msg, int nLen, int &pos)
{
	int index = nSockIndex&GATELOWERMASK;
	if (m_socketindex[index].mask_ != 1)
		return 0;
	if (nLen < (m_socketindex[index].webhead_length_ + 4))
	{
		return (m_socketindex[index].webhead_length_ + 4);
	}
	for (int i = 0; i < 4; i++)
		m_socketindex[index].masking_key_[i] = msg[pos + i];
	pos += 4;
	m_socketindex[index].webhead_length_ += 4;
	return 0;
}

void GateServerModule::umask(char *data, int len, char *mask)
{
	int i;
	for (i = 0; i < len; ++i)
		*(data + i) ^= *(mask + (i % 4));
}

int GateServerModule::fetch_payload(uint32_t nSockIndex, const char *msg, int nLen, int &pos)
{
	int index = nSockIndex&GATELOWERMASK;
	int completedatalen = m_socketindex[index].payload_length_ + m_socketindex[index].webhead_length_;
	return completedatalen;
}

int GateServerModule::pack_frame_head(int msg_len, char* msgbuf)
{
	char response_head[20] = {0};
	int head_length = 0;
	if (msg_len < 126)
	{
		response_head[0] = 0x82;
		response_head[1] = msg_len;
		head_length = 2;
	}
	else if (msg_len < 0xFFFF)
	{
		response_head[0] = 0x82;
		response_head[1] = 126;
		response_head[2] = (msg_len >> 8 & 0xFF);
		response_head[3] = (msg_len & 0xFF);
		head_length = 4;
	}
	else
	{
		response_head[0] = 0x82;
		response_head[1] = 127;
		head_length = 12;
	}
	memcpy(msgbuf, response_head, head_length);
	return head_length;
}


int GateServerModule::HandleError(int32_t iErrNo, uint32_t nSockIndex, void* lpArg, const char* lpErrMsg)
{
	cout << "GateServerModule::HandleError"  << endl;
	return 0;
}

int GateServerModule::HandleHeartbeat(uint32_t nSockIndex, void* lpArg, uint32_t nTime)//返回0不断开连接，非0断开连接
{
	cout<<"GateServerModule::HandleHeartbeat"<<endl;
	return 0;
}

int GateServerModule::HandleTimeout(uint32_t nSockIndex, void* lpArg, uint32_t nTime)//返回0不断开连接，非0断开连接
{
	cout << "GateServerModule::HandleTimeout" << nSockIndex << endl;
	return 1;
}


int GateServerModule::getSocketIndexState(uint32_t Index)
{
	if (Index < GATEMAXCONNECT)
	{
		if (m_socketindex[Index].clientstate == 1 && m_socketindex[Index].hallstate == 1)
		{
			return 1;
		}
	}
	//INFO("GateServerModule::getSocketIndexState %d %d", m_socketindex[index].clientstate, m_socketindex[index].hallstate);
	return 0;
}

int GateServerModule::GateSendMsgtoClient(uint32_t Index, const char* lpData, int32_t iDataLen)
{
	if (Index < 0 || Index > GATEMAXCONNECT)
	{
		ERROR("GateServerModule::GateSendMsgtoClient cmd=%d,scmd=%d,index is out of range %d", ((STR_MSG_HEAD*)lpData)->MessageCmd, ((STR_MSG_HEAD*)lpData)->MessageSCmd, Index);
		return 0;
	}
	
	int nSocketIndex = m_socketindex[Index].nSocketIndex;
	int clientstate = m_socketindex[Index].clientstate;
	int hallstate = m_socketindex[Index].hallstate;
	int connectuid = m_socketindex[Index].uid;
	int msguid = ((STR_MSG_HEAD*)lpData)->MessageUid;
	if (nSocketIndex >= 0 && (connectuid == msguid) && hallstate == 1 && clientstate == 1)
	{
		((STR_MSG_HEAD*)lpData)->MessageOrder = m_socketindex[Index].fdsindex;
		int fdsindex = m_socketindex[Index].fdsindex + 1;
		resetGateSocketState(Index, -1, -1, -1, -1, -1,fdsindex);
		if (m_socketindex[Index].fdsindex >= MSG_RMAX)
			resetGateSocketState(Index, -1, -1, -1, -1, -1, 0);
		INFO("GateServerModule::GateSendMsgtoClient%d-%d,uid =%d", ((STR_MSG_HEAD*)lpData)->MessageCmd, ((STR_MSG_HEAD*)lpData)->MessageSCmd,msguid);

		char sendbuff[2048] = { 0 };
		int len = 0;
		int webhead=pack_frame_head(iDataLen, sendbuff);
		len += webhead;
		memcpy(sendbuff + len, lpData, iDataLen);
		len += iDataLen;
		return m_lpTcpServer->SendData(nSocketIndex, (const void*)sendbuff, len);
	}
	else
	{
		INFO("GateServerModule::GateSendMsgtoClient%d-%d connect is reset msguid=%d,connectuid=%d", ((STR_MSG_HEAD*)lpData)->MessageCmd, ((STR_MSG_HEAD*)lpData)->MessageSCmd,msguid,connectuid);
	}
	return 0;
}

void GateServerModule::ExeDisconnect(int index)
{
	ERROR("GateServerModule::ExeDisconnect index=%d", index);
	if (index < 0 || index > GATEMAXCONNECT)
	{
		return ;
	}

	int nSocketIndex = m_socketindex[index].nSocketIndex;
	m_lpTcpServer->Disconnect(nSocketIndex);
}


void GateServerModule::resetGateSocketState(int index, int uid, int clientstate, int hallstate, int nSocketIndex, int fdrindex, int fdsindex)
{
	pthread_mutex_lock(&_condmutex);	//加锁
	if (index <GATEMAXCONNECT)
	{
		if(uid!=-1)m_socketindex[index].uid = uid;
		if (clientstate != -1)m_socketindex[index].clientstate = clientstate;
		if (hallstate != -1)m_socketindex[index].hallstate = hallstate;
		if (nSocketIndex != -1)m_socketindex[index].nSocketIndex = nSocketIndex;
		if (fdrindex != -1)m_socketindex[index].fdrindex = fdrindex;
		if (fdsindex != -1)m_socketindex[index].fdsindex = fdsindex;
		INFO("index=%d,uid=%d,clientstate=%d,hallstate=%d,nsocketindex=%d,fdrindex=%d,fdsindex=%d",index, m_socketindex[index].uid, m_socketindex[index].clientstate, m_socketindex[index].hallstate, m_socketindex[index].nSocketIndex, m_socketindex[index].fdrindex, m_socketindex[index].fdsindex);
	}
	pthread_mutex_unlock(&_condmutex);	//解锁
}




bool g_bStart = false;
void sigint_handler(int sig)
{
	cout << "sigint_handler="<<sig << endl;
	switch (sig)
	{
	case SIGINT:
		g_bStart = false;
		break;
	default:
		break;
	}
}

int main(int argc,char* argv[])
{
	//signal(SIGINT,sigint_handler);
	init_log(argv[1]);
	//读取配置文件
	ConfigPars::getInstance()->config_info(argv[1],1);

	//开启处理消息线程
	HandlerinnermsgThread::getInstance();
	HandlerinnermsgThread::getInstance()->start();

	//开启处理消息线程
	HandleroutmsgThread::getInstance();
	HandleroutmsgThread::getInstance()->start();

	if (GateServerModule::getInstance() == NULL)
	{
		cout << "GateServerModule create failed!" << endl;
		return 1;
	}
	//创建服务器模块
	//网关服务器发送缓存区10k够用了,因为每个链接有一个缓冲区
	int ret = CreateTcpServer(&(GateServerModule::getInstance()->m_lpTcpServer), ConfigPars::getInstance()->getGateoutip(), ConfigPars::getInstance()->getGateoutport(), GateServerModule::getInstance(),1,10,1,1,10);

	

	if (GateInnerServerModule::getInstance() == NULL)
	{
		cout << "GateInnerServerModule create failed!" << endl;
		return 1;
	}
	//创建内部服务器模块
	ret = CreateTcpServer(&(GateInnerServerModule::getInstance()->m_lpTcpServer), ConfigPars::getInstance()->getGateinnerip(), ConfigPars::getInstance()->getGateinnerport(), GateInnerServerModule::getInstance(),1, 512, 1, 1, 512);

	int gateindex = ConfigPars::getInstance()->getGateinnerindex();
	GateInnerServerModule::getInstance()->m_gateindex = gateindex;

	if (GateInnerServerModule::getInstance()->m_lpTcpServer != NULL && GateServerModule::getInstance()->m_lpTcpServer != NULL)
	{
		cout << "GateServerModule start!" << endl;
		g_bStart = true;
	}
	while (g_bStart)
	{
		sleep(1);//每1秒检查一次
		Glo_Now = time(NULL);


		time_t time_seconds = time(0);
		struct tm* now_time = localtime(&time_seconds);
		if (now_time->tm_hour == 3 && now_time->tm_min == 0 && (now_time->tm_sec == 0 || now_time->tm_sec == 1 || now_time->tm_sec == 2))//每天凌晨3点清理用户map
		{
			GateInnerServerModule::getInstance()->GetServerLogic()->removeIndolentUser();
		}
	}
	cout << "GateServerModule end!" << endl;
	DestroyTcpServer(GateServerModule::getInstance()->m_lpTcpServer);

	if (GateServerModule::getInstance() != NULL)
	{
		GateServerModule::delInstance();
	}

	DestroyTcpServer(GateInnerServerModule::getInstance()->m_lpTcpServer);

	if (GateInnerServerModule::getInstance() != NULL)
	{
		GateInnerServerModule::delInstance();
	}
}
