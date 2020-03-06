#ifndef GATEINNERSERVERLOGIC_H_
#define GATEINNERSERVERLOGIC_H_
#include "stdint.h"
#include "log.h"
#include <map>
#include "TcpMsgStr.h"
using namespace std;

#pragma pack (1)
struct GateUserInfo
{
	//网关信息
	volatile int GateSocketIndex;//用户在网关服的socket连接序号

	//web服务器信息
	//short int WebserverIndex;//用户所在的web服序号

	//游戏服信息
	char RoomserverIndex;//用户所在的游戏服序号
	//int RoomDeskIndex;//用户在游戏服的房间序号
	//short Roomstate;//用户在房间的状态
	//short Roomsite;//用户在房间的座位
	//long tick;// 用户所在房间的offlinekey
	//short int user_state;//用户的玩家状态

	//上一次活跃时间
	long activetime;

	//是否在游戏中，如果是1则消息转发到游戏服
	char ingame;

	GateUserInfo()
	{
		GateSocketIndex = -1;
		//WebserverIndex = -1;
		RoomserverIndex = -1;
		//RoomDeskIndex = -1;
		//Roomstate = -1;
		//Roomsite = -1;
		//tick = -1;
		//user_state = -1;
		activetime = -1;
		ingame = 0;
	}

	GateUserInfo & operator=(const GateUserInfo& other)
	{
		GateSocketIndex = other.GateSocketIndex;
		//WebserverIndex = other.WebserverIndex;
		RoomserverIndex = other.RoomserverIndex;
		//RoomDeskIndex = other.RoomDeskIndex;
		//Roomstate = other.Roomstate;
		//Roomsite = other.Roomsite;
		//tick = other.tick;
		//user_state = other.user_state;
		activetime = other.activetime;
		ingame = other.ingame;
	}
	GateUserInfo(const GateUserInfo& other)
	{
		GateSocketIndex = other.GateSocketIndex;
		//WebserverIndex = other.WebserverIndex;
		RoomserverIndex = other.RoomserverIndex;
		//RoomDeskIndex = other.RoomDeskIndex;
		//Roomstate = other.Roomstate;
		//Roomsite = other.Roomsite;
		//tick = other.tick;
		//user_state = other.user_state;
		activetime = other.activetime;
		ingame = other.ingame;
	}
};
#pragma pack ()
typedef map<int, GateUserInfo> GateUserInfos;

class GateInnerServerlogic
{
public:
	GateInnerServerlogic();

	~GateInnerServerlogic();

	int GetOutMsg(int connectindex, MSG_STR* msg);

	//处理来自客户端的消息
	int HandleOutMsg(MSG_STR* msg);

	//处理来自内部组件的消息
	int HandleInnerMsg( MSG_STR* msg);

	int SendMsgtoClient(int uid, MSG_STR* msg);

	int resetUidInfo(int uid, GateUserInfo& info);

	int ZeroUidInfo(int uid);

	void FillMsgWithUserinfo(MSG_STR* msg);

	int GetuidFdconnectindex(int uid);

	int ServerDisconnect(int uid);

	void removeIndolentUser();//定时清理很久没登录的map值，防止map越来越大
private:
	GateUserInfos m_GateUserInfos;
	pthread_mutex_t _condmutex;
};
#endif