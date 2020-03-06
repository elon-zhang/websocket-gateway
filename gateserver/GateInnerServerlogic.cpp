#include "GateInnerServerlogic.h"
#include "GateInnerServer.h"
#include "TcpMsgStr.h"
#include "GateInner_info.h"
#include "Gate_info.h"
#include "GateServer_websocket.h"
#include "Handleoutmsgthread.h"

GateInnerServerlogic::GateInnerServerlogic()
{
	pthread_mutex_init(&_condmutex, NULL);
}

GateInnerServerlogic::~GateInnerServerlogic()
{
	pthread_mutex_destroy(&_condmutex);
}

int GateInnerServerlogic::GetOutMsg(int connectindex, MSG_STR* msg)
{
	//大厅服收到来自网关服的消息
	int cmd = msg->head.MessageCmd;
	int subcmd = msg->head.MessageSCmd;
	int uid = msg->head.MessageUid;
	int msgsize = ((STR_MSG_HEAD*)msg)->MessageSize;
	if (msgsize > MAXLENGTH)
	{
		ERROR("GateInnerServerlogic::HandleGateMsg size err %d,%d,%d", cmd, subcmd,msgsize);
		return 0;
	}
	INFO("GateInnerServerlogic::HandleOutMsg %d,%d,%d,%d", cmd, subcmd, uid, ((STR_MSG_HEAD*)msg)->MessageFree);

	if (cmd == MSG_CMD_2 && subcmd == MSG_SCMD_TWO_1)
	{
		//如果是2-1登录，需要设置玩家uid
		int uid = ((STR_MSG_HEAD*)msg)->MessageUid;
		GateServerModule::getInstance()->resetGateSocketState(connectindex, uid, 1, -1, -1, -1, -1);

		//int sessiontokenlen = msg->msg[0];
		//INFO("HandleOutMsg2-1 sessiontokenlen=%d", sessiontokenlen);

		((STR_MSG_HEAD*)msg)->MessageFree = GateInnerServerModule::getInstance()->m_gateindex;//这里要修改这个字段标识网关

		loghex(msg->msg, msg->head.MessageSize);

		char buff[MAXLENGTH];
		int len = 0;
		memset(buff, 0, MAXLENGTH);
		memcpy(&buff[len], msg, sizeof(STR_MSG_HEAD));
		len += sizeof(STR_MSG_HEAD);

		memcpy(&buff[len], &connectindex, 4);
		len += 4;
		memcpy(&buff[len], (msg->msg), ((STR_MSG_HEAD*)msg)->MessageSize);
		len += ((STR_MSG_HEAD*)msg)->MessageSize;
		//  由于追加了4个字节，所以要改变下包体大小
		((STR_MSG_HEAD*)buff)->MessageSize += 4;
		loghex(buff + sizeof(STR_MSG_HEAD), msg->head.MessageSize);
		GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, (MSG_STR*)buff, HALL_SUB);
	}
	else if(cmd == MSG_CMD_100  && subcmd == MSG_SCMD_HANDRED_1)
	{
		int uid = msg->head.MessageUid;
		MSG_STR sendmsg;
		sendmsg.head.HeadPad = CLIENTGFLOG;
		sendmsg.head.MessageEnd = CLIENTGFLOG;
		sendmsg.head.MessageUid = uid;
		sendmsg.head.MessageCmd = MSG_CMD_100;
		sendmsg.head.MessageSCmd = MSG_SCMD_HANDRED_1;
		sendmsg.head.MessageSize = 8;
		int	keep_num = 0;
		memcpy(&keep_num, &msg->msg[0], 4);
		keep_num += 1;
		memcpy(&sendmsg.msg[0], &keep_num, 4);
		keep_num = Glo_Now;
		memcpy(&sendmsg.msg[4], &keep_num, 4);
		GateServerModule::getInstance()->GateSendMsgtoClient(connectindex, (char*)(&sendmsg), (sendmsg.head.MessageSize + sizeof(STR_MSG_HEAD)));
	}
	else if (cmd == MSG_CMD_100 && subcmd == MSG_SCMD_HANDRED_2)
	{
		int uid = ((STR_MSG_HEAD*)msg)->MessageUid;
		GateUserInfos::iterator it = m_GateUserInfos.find(uid);
		if (it == m_GateUserInfos.end())
		{
			ERROR("100-2 can not find uid=%d", uid);
		}
		else 
		{
			GateUserInfo userptr = it->second;
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
			INFO("handleGatemsg_100_2 ingame=%d,RoomserverIndex=%hd", userptr.ingame, userptr.RoomserverIndex);
			if (userptr.RoomserverIndex > 0)
			{
				int index = 0;
				memcpy(&index, msg->msg, 4);
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, userptr.RoomserverIndex);
			}
		}
		
	}
	else 
	{
		int len = 0;
		len += sizeof(STR_MSG_HEAD);
		len += msgsize;
		HandleroutmsgThread::getInstance()->addonemsg((char*)msg, len);
	}
	
	return 0;
}

int GateInnerServerlogic::HandleOutMsg(MSG_STR* msg)
{
	//大厅服收到来自网关服的消息
	int cmd = msg->head.MessageCmd;
	int subcmd = msg->head.MessageSCmd;
	int uid = msg->head.MessageUid;
	int msgsize = ((STR_MSG_HEAD*)msg)->MessageSize;
	if (msgsize > MAXLENGTH)
	{
		ERROR("GateInnerServerlogic::HandleGateMsg size err %d,%d,%d", cmd, subcmd, msgsize);
		return 0;
	}
	INFO("GateInnerServerlogic::HandleOutMsg %d,%d,%d,%d", cmd, subcmd, uid, ((STR_MSG_HEAD*)msg)->MessageFree);
	switch (cmd)
	{
	case MSG_CMD_2:
	{
		switch (subcmd)
		{
		case MSG_SCMD_TWO_100://mtt报名
		case MSG_SCMD_TWO_101://mtt退赛
		case MSG_SCMD_TWO_102://mtt观赛
		case MSG_SCMD_TWO_103://mtt参赛
		case MSG_SCMD_TWO_106://玩家进入房间,先去大厅找到对应的游戏服id
		{
			int uid = msg->head.MessageUid;
			GateUserInfos::iterator it = m_GateUserInfos.find(uid);
			if (it == m_GateUserInfos.end())
			{
				ERROR("%d-%d can not find uid=%d", cmd, subcmd, uid);
			}
			else
			{
				GateUserInfo userptr = it->second;
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
			}

		}
		break;
		case MSG_SCMD_TWO_107://玩家坐下
		case MSG_SCMD_TWO_108://玩家离开房间
		{
			int uid = msg->head.MessageUid;
			GateUserInfos::iterator it = m_GateUserInfos.find(uid);
			if (it == m_GateUserInfos.end())
			{
				ERROR("%d-%d can not find uid=%d", cmd, subcmd, uid);
			}
			else
			{
				GateUserInfo userptr = it->second;
				//FillMsgWithUserinfo(msg);
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, userptr.RoomserverIndex);
			}

		}
		break;
		case MSG_SCMD_TWO_109://玩家申请房卡更新
		{
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
		}
		break;
		case MSG_SCMD_TWO_110://申请创房
		case MSG_SCMD_TWO_111://拉取创房配置
		case MSG_SCMD_TWO_112://请求tabkey属于哪个子游戏
		case MSG_SCMD_TWO_113://修改昵称
		case MSG_SCMD_TWO_115://红点信息
		case MSG_SCMD_TWO_116://读取红点信息
		case MSG_SCMD_TWO_200:
		case MSG_SCMD_TWO_201:
		{
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
		}
		break;
		case MSG_SCMD_TWO_120:// 俱乐部消息
		case MSG_SCMD_TWO_121:
		case MSG_SCMD_TWO_122:
		case MSG_SCMD_TWO_128:
		case MSG_SCMD_TWO_140:// 俱乐部消息
		case MSG_SCMD_TWO_141:
		case MSG_SCMD_TWO_142:
		case MSG_SCMD_TWO_143:
		case MSG_SCMD_TWO_144:
		case MSG_SCMD_TWO_145:
		case MSG_SCMD_TWO_146:
		case MSG_SCMD_TWO_147:
		case MSG_SCMD_TWO_148:
		case MSG_SCMD_TWO_149:
		case MSG_SCMD_TWO_150:
		case MSG_SCMD_TWO_151:
		case MSG_SCMD_TWO_152:
		case MSG_SCMD_TWO_153:
		case MSG_SCMD_TWO_160:
		case MSG_SCMD_TWO_161:
		case MSG_SCMD_TWO_162:
		case MSG_SCMD_TWO_163:
		{
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, WEB_SUB);
		}
		break;
		case MSG_SCMD_TWO_210:
		{
			int subnum = 0;
			memcpy(&subnum, &msg->msg[0], 4);
			msg->head.MessageFree= GateInnerServerModule::getInstance()->m_gateindex;//这里要修改这个字段标识网关
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, subnum);
		}
		default:

			break;
		}
	}
	break;
	case MSG_CMD_3:
	{
		switch (subcmd)
		{
		case MSG_SCMD_THREE_1:
		{

		}
		break;
		case MSG_SCMD_THREE_10:
		case MSG_SCMD_THREE_72:
		case MSG_SCMD_THREE_73:
		{
			int uid = msg->head.MessageUid;
			GateUserInfos::iterator it = m_GateUserInfos.find(uid);
			if (it == m_GateUserInfos.end())
			{
				ERROR("%d-%d can not find uid=%d", cmd, subcmd, uid);
			}
			else
			{
				GateUserInfo userptr = it->second;
				//FillMsgWithUserinfo(msg);
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, userptr.RoomserverIndex);
			}

		}
		break;
		default:
			break;
		}
	}
	break;
	case MSG_CMD_4:
	case MSG_CMD_5:
	case MSG_CMD_7:
	{
		/* switch (subcmd)
		{
		case MSG_SCMD_FOUR_1:
		case MSG_SCMD_FOUR_9:
		case MSG_SCMD_FOUR_10:
		case MSG_SCMD_FOUR_14:
		case MSG_SCMD_FOUR_16:
		case MSG_SCMD_FOUR_17:
		case MSG_SCMD_FOUR_18:
		case MSG_SCMD_FOUR_20:
		case MSG_SCMD_FOUR_21:
		case MSG_SCMD_FOUR_22:
		case MSG_SCMD_FOUR_24:
		case MSG_SCMD_FOUR_25:
		case MSG_SCMD_FOUR_28:
		case MSG_SCMD_FOUR_31:
		case MSG_SCMD_FOUR_32:
		case MSG_SCMD_FOUR_33:
		case MSG_SCMD_FOUR_36:
		case MSG_SCMD_FOUR_63:
		case MSG_SCMD_FOUR_29:
		{*/
		int uid = msg->head.MessageUid;
		GateUserInfos::iterator it = m_GateUserInfos.find(uid);
		if (it == m_GateUserInfos.end())
		{
			ERROR("%d-%d can not find uid=%d", cmd, subcmd, uid);
		}
		else
		{
			GateUserInfo userptr = it->second;
			//FillMsgWithUserinfo(msg);
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, userptr.RoomserverIndex);
		}

		/*}

		break;
		default:
		break;
		}*/
	}
	break;
	case MSG_CMD_100:
	{
		switch (subcmd)
		{
		case MSG_SCMD_HANDRED_2://客户端断线
		{
			int uid = msg->head.MessageUid;
			GateUserInfos::iterator it = m_GateUserInfos.find(uid);
			if (it == m_GateUserInfos.end())
			{
				ERROR("100-2 can not find uid=%d", uid);
			}
			else
			{
				//通知web服
				//GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, WEB_SUB);

				//通知大厅
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);

				GateUserInfo userptr = it->second;
				//在桌子中并且不是旁观, 通知游戏
				//if (userptr.RoomDeskIndex > 0 && (userptr.Roomsite >= 0 && userptr.Roomsite < GAME_USER_MAX))
				//if (userptr.RoomDeskIndex > 0 && (userptr.Roomstate >= U_STATE_IN_DESK && userptr.Roomstate < U_STATE_OVER) )
				/*INFO("handleGatemsg_100_2 ingame=%d,RoomserverIndex=%hd",userptr.ingame, userptr.RoomserverIndex);
				if (userptr.RoomserverIndex > 0)
				{
					int index = 0;
					memcpy(&index, msg->msg, 4);
					GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, userptr.RoomserverIndex);
				}*/
			}

		}
		break;
		default:
			break;
		}
	}
	break;

	default:
		break;
	}
	return 0;
}

int GateInnerServerlogic::HandleInnerMsg(MSG_STR* msg)
{
	//大厅服收到来自游戏服的消息
	int cmd = msg->head.MessageCmd;
	int subcmd = msg->head.MessageSCmd;
	int uid = msg->head.MessageUid;
	int msgsize = ((STR_MSG_HEAD*)msg)->MessageSize;
	if (msgsize > MAXLENGTH)
	{
		ERROR("GateInnerServerlogic::HandleGameMsg size err %d,%d,%d", cmd, subcmd, msgsize);
		return 0;
	}
	INFO("GateInnerServerlogic::HandleInnerMsg cmd=%d,scmd=%d,uid=%d,msgsize=%d", cmd, subcmd, uid, msgsize);
	switch (cmd)
	{
	case MSG_CMD_2:
	{
	  	switch (subcmd)
	  	{
	  	case MSG_SCMD_TWO_1:
	  	{
			int len = 0;
			GateUserInfo tempgateuserinfo;
			int tempGateSocketIndex = 0;
			memcpy(&tempGateSocketIndex, &msg->msg[len], 4);
			len += 4;
			tempgateuserinfo.GateSocketIndex = tempGateSocketIndex;
			//memcpy(&tempgateuserinfo.WebserverIndex, &msg->msg[len], 2);
			//len += 2;
			// short roomserverindex = 4;
			memcpy(&tempgateuserinfo.RoomserverIndex, &msg->msg[len], 1);
			len += 1;
			//memcpy(&tempgateuserinfo.RoomDeskIndex, &msg->msg[len], 4);
			//len += 4;
			//memcpy(&tempgateuserinfo.Roomstate, &msg->msg[len], 2);
			//len += 2;
			//memcpy(&tempgateuserinfo.Roomsite, &msg->msg[len], 2);
			//len += 2;
			//memcpy(&tempgateuserinfo.tick, &msg->msg[len], 8);
			//len += 8;
			//memcpy(&tempgateuserinfo.user_state, &msg->msg[len], 2);
			//len += 2;
			memcpy(&tempgateuserinfo.ingame, &msg->msg[len], 1);
			len += 1;
			tempgateuserinfo.activetime = Glo_Now;
			if (tempgateuserinfo.GateSocketIndex >= GATEMAXCONNECT)
			{
				ERROR("GateInnerServerlogic::HandleInnerMsg index wrong %d,%d,%d,%d", cmd, subcmd, uid, tempgateuserinfo.GateSocketIndex);
			}
			else
			{
				//收到大厅的2-1回包，网关玩家信息需要同步大厅的信息
				GateServerModule::getInstance()->resetGateSocketState(tempgateuserinfo.GateSocketIndex, uid, -1, 1, -1, -1, -1);
				resetUidInfo(uid, tempgateuserinfo);

				// 通知初大厅外的内部组件


				//回包客户端
				char buff[MAXLENGTH];
				int sendlen = 0;
				memset(buff, 0, MAXLENGTH);
				((STR_MSG_HEAD*)msg)->MessageSize -= len;
				memcpy(&buff[sendlen], msg, sizeof(STR_MSG_HEAD));
				sendlen += sizeof(STR_MSG_HEAD);
				memcpy(&buff[sendlen], (char*)(&msg->msg[len]), ((STR_MSG_HEAD*)msg)->MessageSize);
				SendMsgtoClient(uid, (MSG_STR*)(buff));
			 }

	  	}
		break;
	  	case MSG_SCMD_TWO_3:
	  	case MSG_SCMD_TWO_76:
	  	case MSG_SCMD_TWO_100:
	  	case MSG_SCMD_TWO_101:
	  	case MSG_SCMD_TWO_102:
	  	case MSG_SCMD_TWO_103:
	  	case MSG_SCMD_TWO_105:
	  	case MSG_SCMD_TWO_106:
	  	case MSG_SCMD_TWO_107:
	  	case MSG_SCMD_TWO_108:
	  	case MSG_SCMD_TWO_120:
	  	case MSG_SCMD_TWO_121:
	  	case MSG_SCMD_TWO_122:
	  	case MSG_SCMD_TWO_127:
	  	case MSG_SCMD_TWO_128:
	  	case MSG_SCMD_TWO_129:
	  	case MSG_SCMD_TWO_131:
	  	case MSG_SCMD_TWO_140:
	  	case MSG_SCMD_TWO_141:
	  	case MSG_SCMD_TWO_142:
	  	case MSG_SCMD_TWO_143:
	  	case MSG_SCMD_TWO_144:
	  	case MSG_SCMD_TWO_145:
	  	case MSG_SCMD_TWO_146:
	  	case MSG_SCMD_TWO_147:
	  	case MSG_SCMD_TWO_148:
	  	case MSG_SCMD_TWO_149:
	  	case MSG_SCMD_TWO_150:
	  	case MSG_SCMD_TWO_151:
	  	case MSG_SCMD_TWO_152:
	  	case MSG_SCMD_TWO_153:
	  	case MSG_SCMD_TWO_154:
	  	case MSG_SCMD_TWO_155:
	  	case MSG_SCMD_TWO_160:
	  	case MSG_SCMD_TWO_161:
	  	case MSG_SCMD_TWO_162:
	  	case MSG_SCMD_TWO_163:
	  	case MSG_SCMD_TWO_200:
	  	case MSG_SCMD_TWO_201:
	  	case MSG_SCMD_TWO_210:
	  	{
			SendMsgtoClient(uid, msg);
	  	}
		break;
	  	case MSG_SCMD_TWO_109://更新房卡
	  	{
			SendMsgtoClient(uid, msg);
	  	}
		break;
		case MSG_SCMD_TWO_110://申请创房大厅这个阶段结果通知
		case MSG_SCMD_TWO_111://拉取创房配置
		case MSG_SCMD_TWO_112:
		case MSG_SCMD_TWO_113://修改昵称
		case MSG_SCMD_TWO_115://红点信息
		case MSG_SCMD_TWO_116://读取红点信息
		{
			SendMsgtoClient(uid, msg);
		}
		break;
	  	case MSG_SCMD_TWO_2://通知在其他地方登陆
	  	{

		  	int len = 0;
		  	int tempGateSocketIndex = 0;
		  	memcpy(&tempGateSocketIndex, &msg->msg[len], 4);
		  	len += 4;

		  	//回包客户端
		  	char buff[MAXLENGTH];
		  	int sendlen = 0;
		  	memset(buff, 0, MAXLENGTH);
		  	((STR_MSG_HEAD*)msg)->MessageSize -= len;
		  	memcpy(&buff[sendlen], msg, sizeof(STR_MSG_HEAD));
		  	sendlen += sizeof(STR_MSG_HEAD);
		  	memcpy(&buff[sendlen], (char*)(&msg->msg[len]), ((STR_MSG_HEAD*)msg)->MessageSize);
		  	sendlen += ((STR_MSG_HEAD*)msg)->MessageSize;

		  	//2-2由于在2-1后面，所以只能通过连接索引去发送
		  	if (tempGateSocketIndex >= 0 && tempGateSocketIndex < GATEMAXCONNECT)
		  	{
			  	GateServerModule::getInstance()->GateSendMsgtoClient(tempGateSocketIndex, buff, sendlen);
		  	}
	  	}
		break;
	  	default:
		break;
	  	}
		}
		break;
	case MSG_CMD_3:
	{
		switch (subcmd)
		{
		case MSG_SCMD_THREE_1:
		{

		}
		break;
		case MSG_SCMD_THREE_3:
		case MSG_SCMD_THREE_10:
		case MSG_SCMD_THREE_72:
		case MSG_SCMD_THREE_73:
		{
			SendMsgtoClient(uid, msg);
		}
		break;
		default:
		break;
		}
	}
	break;
	case MSG_CMD_4:
	case MSG_CMD_5:
	case MSG_CMD_7:
	{
		SendMsgtoClient(uid, msg);
	}
	break;
	case MSG_CMD_1000:
	{
		switch (subcmd)
		{
		case MSG_SCMD_THOURAND_1://内部组件登录
		{
		  	int subnum = msg->head.MessageFree;
		  	int socketindex = 0;
		  	memcpy(&socketindex, msg->msg, 4);
		  	INFO("inner sub login sub=%d,socketindex=%d", subnum, socketindex);
		  	if (subnum < MAX_SUB_NUM)
		  	{
			  	GateInnerServerModule::getInstance()->setsubSokcetindex(subnum, socketindex);
		  	}


		  	//服务器个别组件重启的时候把内存的玩家信息置零
		  	if (subnum != HALL_SUB && subnum != WEB_SUB)
		  	{
			  	GateUserInfos::iterator it = m_GateUserInfos.begin();
			  	for (; it != m_GateUserInfos.end(); it++)
			  	{
				  	GateUserInfo userptr = it->second;
				  	if (userptr.RoomserverIndex == subnum)
				  	{
					  	ServerDisconnect(it->first);//先断掉在房间里的连接
					  	ZeroUidInfo(it->first);
				  	}
			  	}

			  	//通知大厅组件重启
			  	struct MSG_STR tmpmsg;
			  	memset(&tmpmsg, 0, sizeof(struct MSG_STR));
			  	tmpmsg.head.MessageSize = 1;
			  	tmpmsg.head.MessageCmd = MSG_CMD_1000;
			  	tmpmsg.head.MessageSCmd = MSG_SCMD_THOURAND_12;
			  	tmpmsg.head.HeadPad = CLIENTGFLOG;
			  	tmpmsg.head.MessageEnd = CLIENTGFLOG;
			  	int sum = 0;
			  	tmpmsg.msg[sum++] = subnum;
			  	GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, &tmpmsg, HALL_SUB);
		  	}
		}
		break;
	 	case MSG_SCMD_THOURAND_9://大厅通知游戏服创建房间
	 	{
			int uid = msg->head.MessageUid;
			int gamesubid = 0;
			memcpy(&gamesubid, &msg->msg[0], 4);
			GateUserInfos::iterator it = m_GateUserInfos.find(uid);
			if (it == m_GateUserInfos.end())
			{
				ERROR("%d-%d can not find uid=%d", cmd, subcmd, uid);
			}
			else
			{
				GateUserInfo userptr = it->second;
				userptr.RoomserverIndex = gamesubid;//开始创房了就把玩家的RoomserverIndex设置为请求的创房服务器
				resetUidInfo(uid, userptr);
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, gamesubid);
			}

	 	}
		break;
		case MSG_SCMD_THOURAND_10://大厅通知游戏服进入房间
		{
			int uid = msg->head.MessageUid;
			int gamesubid = 0;
		   	int rlen = 0;
		   	memcpy(&gamesubid, &msg->msg[0], 4);
		   	GateUserInfos::iterator it = m_GateUserInfos.find(uid);
		   	//if (it == m_GateUserInfos.end())
		   	//{
			  	// //回包2-106到客户端
			  	// MSG_STR sendmsg;
			  	// sendmsg.head.MessageCmd = MSG_CMD_2;
			  	// sendmsg.head.MessageSCmd = MSG_SCMD_TWO_106;
			  	// sendmsg.head.HeadPad = CLIENTGFLOG;
			  	// sendmsg.head.MessageEnd = CLIENTGFLOG;
			  	// sendmsg.head.MessageUid = uid;
			  	// sendmsg.head.MessageSize = 4;
			  	// int enterret = 67;
			  	// memcpy(sendmsg.msg, &enterret, 4);
			  	// SendMsgtoClient(uid, &sendmsg);
			  	// ERROR("%d-%d can not find uid=%d", cmd, subcmd, uid);
		   	//}
		   	//else
		   	//{
			if (gamesubid>0)//说明找到桌子
			{
				GateUserInfo userptr = it->second;
				userptr.RoomserverIndex = gamesubid;//开始创房了就把玩家的RoomserverIndex设置为请求的创房服务器
				resetUidInfo(uid, userptr);

				//转发2-106到游戏服
				MSG_STR sendmsg;
				sendmsg.head.MessageCmd = MSG_CMD_2;
				sendmsg.head.MessageSCmd = MSG_SCMD_TWO_106;
				sendmsg.head.HeadPad = CLIENTGFLOG;
				sendmsg.head.MessageEnd = CLIENTGFLOG;
				sendmsg.head.MessageUid = uid;
				sendmsg.head.MessageSize = msgsize;
				sendmsg.head.MessageFree= GateInnerServerModule::getInstance()->m_gateindex;//这里要修改这个字段标识网关
				memcpy(sendmsg.msg, &(msg->msg[rlen]), msgsize);
				INFO("sendmsg.head.MessageFree=%d", sendmsg.head.MessageFree);
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, &sendmsg, gamesubid);
			}
			// else
			// {
			//  //回包2-106到客户端
			//  MSG_STR sendmsg;
			//  sendmsg.head.MessageCmd = MSG_CMD_2;
			//  sendmsg.head.MessageSCmd = MSG_SCMD_TWO_106;
			//  sendmsg.head.HeadPad = CLIENTGFLOG;
			//  sendmsg.head.MessageEnd = CLIENTGFLOG;
			//  sendmsg.head.MessageUid = uid;
			//  sendmsg.head.MessageSize = 4;
			//  int enterret = 67;
			//  memcpy(sendmsg.msg, &enterret, 4);
			//  SendMsgtoClient(uid, &sendmsg);
			// }
		    //}

		}
		break;
		case MSG_SCMD_THOURAND_2://游戏服通知大厅玩家进入牌桌后在牌桌的信息
		{
			int subnum = 0;//msg->head.MessageFree;
			int sum = 0;
			int userid = 0;
			memcpy(&subnum, &(msg->msg[sum]), 4);
			sum += 4;
			memcpy(&userid, &(msg->msg[sum]), 4);
			sum += 4;
			int ustate = 0;
			memcpy(&ustate, &(msg->msg[sum]), 4);
			sum += 4;
			int dindex = 0;
			memcpy(&dindex, &(msg->msg[sum]), 4);
			sum += 4;
			int site = 0;
			memcpy(&site, &(msg->msg[sum]), 4);
			sum += 4;
			long tick = 0;
			memcpy(&tick, &(msg->msg[sum]), 8);
			sum += 8;

			int ingame = 0;
			memcpy(&ingame, &(msg->msg[sum]), 4);
			sum += 4;

			GateUserInfos::iterator it = m_GateUserInfos.find(userid);
			if (it != m_GateUserInfos.end())
			{
				GateUserInfo userptr = it->second;
				userptr.RoomserverIndex = subnum;
				//userptr.Roomstate = ustate;
				//userptr.RoomDeskIndex = dindex;
				//userptr.Roomsite = site;
				//userptr.tick = tick;
				userptr.ingame = ingame;
				resetUidInfo(userid, userptr);
				INFO("HandleInnerMsg_1000_2 UID=%d,subnum=%d,ustate=%d,dindex=%d,site=%d,tick=%ld", userid, subnum, ustate, dindex, site, tick);
			}
			else
			{
				ERROR("HandleInnerMsg_1000_2 can not find UID=%d", userid);
			}
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
		}
		break;
		case MSG_SCMD_THOURAND_3://游戏服通知大厅玩家在牌桌坐下后在牌桌的信息
		{
			int subnum = 0; //msg->head.MessageFree;
			int sum = 0;
			int userid = 0;
			memcpy(&subnum, &(msg->msg[sum]), 4);
			sum += 4;
			memcpy(&userid, &(msg->msg[sum]), 4);
			sum += 4;
			int ustate = 0;
			memcpy(&ustate, &(msg->msg[sum]), 4);
			sum += 4;
			int dindex = 0;
			memcpy(&dindex, &(msg->msg[sum]), 4);
			sum += 4;
			int site = 0;
			memcpy(&site, &(msg->msg[sum]), 4);
			sum += 4;
			int ingame = 0;
			memcpy(&ingame, &(msg->msg[sum]), 4);
			sum += 4;

			GateUserInfos::iterator it = m_GateUserInfos.find(userid);
			if (it != m_GateUserInfos.end())
			{
				GateUserInfo userptr = it->second;
				userptr.RoomserverIndex = subnum;
				//userptr.Roomstate = ustate;
				//userptr.RoomDeskIndex = dindex;
				//userptr.Roomsite = site;
				//userptr.ingame = ingame;
				resetUidInfo(userid, userptr);
				INFO("handleGamemsg_1000_3 UID=%d,subnum=%d,ustate=%d,dindex=%d,site=%d", userid, subnum, ustate, dindex, site);
			}
			else
			{
				ERROR("handleGamemsg_1000_3 can not find UID=%d", userid);
			}
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
		}
		break;
	 	case MSG_SCMD_THOURAND_4:
	 	{
			/*int userid = msg->head.MessageUid;

			GateUserInfos::iterator it = m_GateUserInfos.find(userid);
			if (it != m_GateUserInfos.end())
			{
				GateUserInfo userptr = it->second;
				userptr.RoomserverIndex = -1;
				userptr.Roomstate = -1;
				userptr.RoomDeskIndex = -1;
				userptr.Roomsite = -1;
				userptr.ingame = 0;
				resetUidInfo(userid, userptr);
				INFO("handleGamemsg_1000_4 UID=%d", userid);
			}
			else
			{
				ERROR("handleGamemsg_1000_4 can not find UID=%d", userid);
			}*/
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
	 	}
		break;
		case MSG_SCMD_THOURAND_31://游戏服通知大厅牌桌解散
		{
			int uid = 0;
			memcpy(&uid, &msg->msg[0], 4);
			ServerDisconnect(uid);
		}
		break;
		case MSG_SCMD_THOURAND_6://游戏服通知大厅创房成功
		{
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
		}
		break;
		case MSG_SCMD_THOURAND_7://大厅创房俱乐部自由桌成功，通知web
		case MSG_SCMD_THOURAND_8://玩家断线重连，通知web
		case MSG_SCMD_THOURAND_11://通知俱乐部去更新自由桌
		{
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, WEB_SUB);
		}
		break;
	    case MSG_SCMD_THOURAND_5://玩家断线重连，通知game
	    {
		    GateUserInfos::iterator it = m_GateUserInfos.find(uid);
		  	if (it != m_GateUserInfos.end())
		  	{
			  	int gamesub = 0;
			  	memcpy(&gamesub, &msg->msg[0], 4);
			  	if (gamesub > 0)
			  	{
				  	msg->head.MessageFree = GateInnerServerModule::getInstance()->m_gateindex;//这里要修改这个字段标识网关
				  	INFO("reconnect %d-%d,%d", msg->head.MessageCmd, msg->head.MessageSCmd, gamesub);
				  	GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, gamesub);
			  	}
			  	else
			  	{
				  	ERROR("gamesub wrong %d-%d,%d", msg->head.MessageCmd, msg->head.MessageSCmd, gamesub);
			  	}
		  	}else
		  	{
			  	ERROR("1000-5 not uid=%d",uid);
		  	}					  
	 	}
		break;
	 	case MSG_SCMD_THOURAND_15:    // 俱乐部创建固定桌
	 	{
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, HALL_SUB);
	 	}
		break;
	 	case MSG_SCMD_THOURAND_16:		// 固定桌人数局数变化
	 	{
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, WEB_SUB);
	 	}
		break;
		case MSG_SCMD_THOURAND_160://俱乐部积分变更通知游戏服
		{
			if (uid > 0)
			{
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, uid);
			}
			else
			{
				ERROR("gamesub wrong %d-%d,%d", msg->head.MessageCmd, msg->head.MessageSCmd, uid);
			}
		}
			break;
		case MSG_SCMD_THOURAND_32://游戏服通知大厅创房成功
		{
			GateUserInfos::iterator it = m_GateUserInfos.find(uid);
			if (it != m_GateUserInfos.end())
			{
				int gamesub = 0;
				memcpy(&gamesub, &msg->msg[0], 4);
				if (gamesub > 0)
				{
					GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, msg, gamesub);
				}
				else
				{
					ERROR("gamesub wrong %d-%d,%d", msg->head.MessageCmd, msg->head.MessageSCmd, gamesub);
				}
			}
			else
			{
				ERROR("1000-32 not uid=%d", uid);
			}
		}
		break;
		case MSG_SCMD_THOURAND_33://大厅通知游戏服更新point(抽水点)
		{
		 	int gamesub = 0;
			memcpy(&gamesub, &msg->msg[0], 4);
			GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(0, msg, gamesub);
		}
		break;
		case MSG_SCMD_THOURAND_102://大厅通知游戏服进入mtt房间(观赛)
		{
			int uid = msg->head.MessageUid;
		   	int gamesubid 	= 0;
		   	int idx 		= 0;
		   	int sum         = 0;
		   	int rlen 		= 0;
		   	memcpy(&gamesubid, &msg->msg[sum], 4);
		   	sum += 4;
		   	memcpy(&idx, &msg->msg[sum], 4);
		   	sum += 4;

		   	GateUserInfos::iterator it = m_GateUserInfos.find(uid);
		   
			if (gamesubid>0)//说明找到桌子
			{
				GateUserInfo userptr = it->second;
				userptr.RoomserverIndex = gamesubid;//开始创房了就把玩家的RoomserverIndex设置为请求的创房服务器
				resetUidInfo(uid, userptr);

				//转发2-103到游戏服
				MSG_STR sendmsg;
				sendmsg.head.MessageCmd 	= MSG_CMD_2;
				sendmsg.head.MessageSCmd 	= MSG_SCMD_TWO_104;
				sendmsg.head.HeadPad 		= CLIENTGFLOG;
				sendmsg.head.MessageEnd 	= CLIENTGFLOG;
				sendmsg.head.MessageUid 	= uid;
				sendmsg.head.MessageSize 	= msgsize;
				sendmsg.head.MessageFree= GateInnerServerModule::getInstance()->m_gateindex;//这里要修改这个字段标识网关
				memcpy(sendmsg.msg, &(msg->msg[rlen]), msgsize);
				INFO("MSG_SCMD_THOURAND_102 sendmsg.head.MessageFree=%d", sendmsg.head.MessageFree);
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, &sendmsg, gamesubid);
			}
		}
		break;
		case MSG_SCMD_THOURAND_103://大厅通知游戏服进入mtt房间(参赛)
		{
		   	int uid = msg->head.MessageUid;
		   	int gamesubid 	= 0;
		   	int idx 		= 0;
		   	int coin 		= 0;
		   	int rebuy 		= 0;
		   	int addbuy 		= 0;
		   	int sum         = 0;
		   	int rlen 		= 0;
		   	memcpy(&gamesubid, &msg->msg[sum], 4);
		   	sum += 4;
		   	memcpy(&idx, &msg->msg[sum], 4);
		   	sum += 4;
		   	memcpy(&coin, &msg->msg[sum], 4);
		   	sum += 4;
		   	memcpy(&rebuy, &msg->msg[sum], 4);
		   	sum += 4;
		   	memcpy(&addbuy, &msg->msg[sum], 4);
		   	sum += 4;

		   	GateUserInfos::iterator it = m_GateUserInfos.find(uid);
		   
			if (gamesubid>0)//说明找到桌子
			{
				GateUserInfo userptr = it->second;
				userptr.RoomserverIndex = gamesubid;//开始创房了就把玩家的RoomserverIndex设置为请求的创房服务器
				resetUidInfo(uid, userptr);

				//转发2-103到游戏服
				MSG_STR sendmsg;
				sendmsg.head.MessageCmd 	= MSG_CMD_2;
				sendmsg.head.MessageSCmd 	= MSG_SCMD_TWO_104;
				sendmsg.head.HeadPad 		= CLIENTGFLOG;
				sendmsg.head.MessageEnd 	= CLIENTGFLOG;
				sendmsg.head.MessageUid 	= uid;
				sendmsg.head.MessageSize 	= msgsize;
				sendmsg.head.MessageFree= GateInnerServerModule::getInstance()->m_gateindex;//这里要修改这个字段标识网关
				memcpy(sendmsg.msg, &(msg->msg[rlen]), msgsize);
				INFO("MSG_SCMD_THOURAND_103 sendmsg.head.MessageFree=%d", sendmsg.head.MessageFree);
				GateInnerServerModule::getInstance()->SendMsgtoSubMoudle(uid, &sendmsg, gamesubid);
			}
		}
		break;
		default:
		break;
	}
	}
	break;
	case MSG_CMD_10000:
	{
		switch (subcmd)
		{
	  		case MSG_SCMD_TENTHOURAND_1://测试用
	  		{

	  		}
			break;
	  		default:
			break;
		}
	}
	break;
	default:
	break;
	}
	return 0;
}

int GateInnerServerlogic::SendMsgtoClient(int uid, MSG_STR* msg)
{
	GateUserInfos::iterator it = m_GateUserInfos.find(uid);
	if (it == m_GateUserInfos.end())
	{
		ERROR("GateInnerServerlogic::SendMsgtoClient can not find uid=%d",uid);
		return 0;
	}
	int connectindex = m_GateUserInfos[uid].GateSocketIndex;
	int len = msg->head.MessageSize + sizeof(STR_MSG_HEAD);
	if (len > MAXLENGTH)
	{
		ERROR("GateInnerServerlogic::SendMsgtoClient msg is out of range cmd=%d,scmd=%d,size=%d", msg->head.MessageCmd, msg->head.MessageSCmd, msg->head.MessageSize);
		return 0;
	}
	if (connectindex >= 0 && connectindex < GATEMAXCONNECT)
	{
		GateServerModule::getInstance()->GateSendMsgtoClient(connectindex, (char*)(msg), len);
	}
	else
	{
		ERROR("SendMsgtoClient connect out of range cmd=%d,scmd=%d, uid=%d,connecindex=%d",msg->head.MessageCmd,msg->head.MessageSCmd,uid,connectindex);
	}
	
	return 0;
}

int GateInnerServerlogic::ServerDisconnect(int uid)
{
	ERROR("GateInnerServerlogic::ServerDisconnect UID=%d", uid);
	GateUserInfos::iterator it = m_GateUserInfos.find(uid);
	if (it == m_GateUserInfos.end())
	{
		ERROR("GateInnerServerlogic::SendMsgtoClient can not find uid=%d", uid);
		return 0;
	}
	int connectindex = m_GateUserInfos[uid].GateSocketIndex;
	
	if (connectindex >= 0 && connectindex < GATEMAXCONNECT)
	{
		GateServerModule::getInstance()->ExeDisconnect(connectindex);
	}
	return 0;
}

int GateInnerServerlogic::resetUidInfo(int uid, GateUserInfo& info)
{
	pthread_mutex_lock(&_condmutex);	//加锁
	m_GateUserInfos[uid] = info;
	pthread_mutex_unlock(&_condmutex);	//解锁
	return 0;
}

int GateInnerServerlogic::ZeroUidInfo(int uid)
{
	pthread_mutex_lock(&_condmutex);	//加锁
	m_GateUserInfos[uid].GateSocketIndex = -1;
	//m_GateUserInfos[uid].WebserverIndex = -1;
	m_GateUserInfos[uid].RoomserverIndex = -1;
	//m_GateUserInfos[uid].RoomDeskIndex = -1;
	//m_GateUserInfos[uid].Roomstate = -1;
	//m_GateUserInfos[uid].Roomsite = -1;
	//m_GateUserInfos[uid].tick = -1;
	//m_GateUserInfos[uid].user_state = -1;
	m_GateUserInfos[uid].activetime = -1;
	m_GateUserInfos[uid].ingame = 0;
	pthread_mutex_unlock(&_condmutex);	//解锁
	return 0;
}

void  GateInnerServerlogic::removeIndolentUser()
{
	pthread_mutex_lock(&_condmutex);	//加锁
	GateUserInfos::iterator it = m_GateUserInfos.begin();
	for (; it != m_GateUserInfos.end();)
	{
		if (((Glo_Now - it->second.activetime) >= 259200))//超过三天没有登录的清除
		{
			m_GateUserInfos.erase(it++);
			INFO("GateInnerServerlogic::removeIndolentUser uid=%d", it->first);
		}
		else
		{
			it++;
		}
	}
	pthread_mutex_unlock(&_condmutex);	//解锁
}

void GateInnerServerlogic::FillMsgWithUserinfo(MSG_STR* msg)
{
	int uid = msg->head.MessageUid;
	GateUserInfos::iterator it = m_GateUserInfos.find(uid);
	if (it == m_GateUserInfos.end())
	{
		ERROR("GateInnerServerlogic::FillMsgWithUserinfo can not find uid=%d", uid);
		return;
	}
	GateUserInfo userptr = it->second;
	//if (userptr)
	{
		char tempbuf[MAXLENGTH] = { 0 };
		msg->head.MessageCheck = MSG_FREE_GATE_WITHFILL;
		//INFO("FillMsgWithUserinfo FREE=%d,uid=%d,cmd=%d,scmd=%d,RoomDeskIndex=%d,Roomstate=%d,Roomsite=%d,tick=%d", msg->head.MessageCheck, uid, msg->head.MessageCmd, msg->head.MessageSCmd, userptr.RoomDeskIndex, userptr.Roomstate, userptr.Roomsite);
		memcpy(tempbuf, msg->msg, msg->head.MessageSize);
		int filllen = 0;
		//memcpy((void*)(&msg->msg[filllen]), &(userptr.RoomDeskIndex), 4);
		//filllen += 4;
		//memcpy((void*)(&msg->msg[filllen]), &(userptr.Roomstate), 2);
		//filllen += 2;
		//memcpy((void*)(&msg->msg[filllen]), &(userptr.Roomsite), 1);
		//filllen += 1;
		//memcpy((void*)(&msg->msg[filllen]), &(userptr.tick), 8);
		//filllen += 8;

		memcpy((void*)(&msg->msg[filllen]), tempbuf, msg->head.MessageSize);
		msg->head.MessageSize += filllen;
	}
}

int GateInnerServerlogic::GetuidFdconnectindex(int uid)
{
	GateUserInfos::iterator it = m_GateUserInfos.find(uid);
	if (it == m_GateUserInfos.end())
	{
		ERROR("GateInnerServerlogic::GetuidFdconnectindex can not find uid=%d", uid);
		return 0;
	}
	volatile int connectindex = m_GateUserInfos[uid].GateSocketIndex;
	return connectindex;
}

