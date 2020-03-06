#ifndef GATESERVER_H_
#define GATESERVER_H_
#include <tcpservervirfunciton.h>
#include "share/singleton.h"
#include <pthread.h>
#include "sha1.h"
#include "base64.h"
#define GATEMAXCONNECT 10000 //一个网关最多连接10000个
#define GATELOWERMASK 0xFFFF //连接序号做位与后取得网络中连接的索引
#define MSG_RMAX 30000//消息读取最大序号


#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

//typedef std::map<std::string, std::string> HEADER_MAP;

struct GateSocketState
{
	int uid;//连接对应的玩家uid
	int clientstate;//客户端的状态：0断开，1正常
	int hallstate;//跟大厅的同步:0大厅还没同步,1大厅已经同步
	int nSocketIndex;//网络连接序号
	int fdrindex;//消息读取序号
	int fdsindex;//消息发送序号
	uint8_t fin_;
	uint8_t opcode_;
	uint8_t mask_;
	char masking_key_[4];
	uint64_t payload_length_;
	uint64_t webhead_length_;
};//验证一个大厅发过来的消息是否能转发给客户端，需要uid对应上，并且clientstate和hallstate都为1,这样防止断线重连的时候发错客户端


class GateServerModule : public ITcpServerModule, public singleton<GateServerModule>
{
public:
	GateServerModule();

	~GateServerModule();
public:
	int Init();

	int HandleAccepted(uint32_t nSockIndex,void**lppArg,const char* lpIPAddr,uint16_t wPort);

    int HandleDisconnect(uint32_t nSockIndex, void* lpArg);

	int HandleRecvRawData(uint32_t nSockIndex,void* lpArg,const char* lpData,uint32_t nLen);

	int HandleRecvBiz(uint32_t nSockIndex,void* lpArg,const char* lpBizData,uint32_t nBizLen);

	int HandleError(int32_t iErrNo,uint32_t nSockIndex,void* lpArg,const char* lpErrMsg);

	int HandleHeartbeat(uint32_t nSockIndex, void* lpArg, uint32_t nTime);

	int HandleTimeout(uint32_t nSockIndex, void* lpArg, uint32_t nTime);

	//websocket parse
	int Handshake(uint32_t nSockIndex, const char* lpData, uint32_t nLen);

	int Askshake(uint32_t nSockIndex, const char* magickey);

	int fetch_websocket_info(uint32_t nSockIndex, const char *msg, int nLen);

	void reset(uint32_t nSockIndex);

	int fetch_fin(uint32_t nSockIndex, const char *msg, int nLen, int &pos);

	int fetch_opcode(uint32_t nSockIndex, const char *msg, int nLen, int &pos);

	int fetch_mask(uint32_t nSockIndex, const char *msg, int nLen, int &pos);

	int fetch_masking_key(uint32_t nSockIndex, const char *msg, int nLen, int &pos);

	void umask(char *data, int len, char *mask);

	int fetch_payload_length(uint32_t nSockIndex, const char *msg, int nLen, int &pos);

	int fetch_payload(uint32_t nSockIndex, const char *msg, int nLen, int &pos);

	int pack_frame_head(int msg_len, char* msgbuf);

public:
	//获取连接状态:0不正常，1正常。必须是clientstate 和hallstate同时为1才能发送消息，说明大厅服已经同步了网关服的连接状态
	int getSocketIndexState(uint32_t Index);

	int GateSendMsgtoClient(uint32_t Index,const char* lpData,int32_t iDataLen);

	void resetGateSocketState(int index,int uid,int clientstate,int hallstate,int nSocketIndex,int fdrindex,int fdsindex);

	void ExeDisconnect(int index);
public:
	ITcpServerFunction* m_lpTcpServer;
	GateSocketState m_socketindex[GATEMAXCONNECT];
private:
	pthread_mutex_t _condmutex;
};





#endif 
