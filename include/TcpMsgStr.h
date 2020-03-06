#ifndef TCPMSGSTR_H_
#define TCPMSGSTR_H_
#include <stdio.h>
#include <stdlib.h>
#define CLIENTGFLOG					8				//客户端发过来的消息头尾的标志符号
#define	MAXLENGTH			2048				//消息内容长度


enum MsgFree//MessageFree标识
{
	MSG_FREE_GATE_WITHFILL=9,//说明这个消息是从客户端过来，并且被大厅填充了玩家信息的
};
#pragma pack (1)
//消息结构体头部
struct STR_MSG_HEAD		//消息结构--头部
{
	char			HeadPad;       //消息头的标志              1
	char			MessageFree;		//消息预留位                1
	short int		MessageSize;		//消息内容指令长度          2
	short int		MessageOrder;		//消息指令序号              2
	short int		MessageCmd;		//消息指令号                2
	short int		MessageSCmd;		//消息子指令号              2  
	int				MessageUid;		//用户数字ID				4
	int				MessageToken;		//用户令牌					4
	char			MessageCheck;		//消息校验位                1
	char			MessageEnd;		//消息头结束标识位          1
};

//消息结构
struct MSG_STR
{
	struct STR_MSG_HEAD	head;			//消息头
	char				msg[MAXLENGTH];			//消息内容
};
#pragma pack ()

//消息码的定义
#define MSG_CMD_1 1
#define MSG_CMD_ONE_1 1
#define MSG_CMD_2 2
#define MSG_SCMD_TWO_1 1//登录
#define MSG_SCMD_TWO_2 2//顶号
#define MSG_SCMD_TWO_3 3//平台踢人
#define MSG_SCMD_TWO_76 76//通知客户端服务器维护
#define MSG_SCMD_TWO_100 100 //mtt报名
#define MSG_SCMD_TWO_101 101 //mtt退赛
#define MSG_SCMD_TWO_102 102 //mtt观战
#define MSG_SCMD_TWO_103 103 //mtt参赛
#define MSG_SCMD_TWO_104 104 //103大厅转游戏104
#define MSG_SCMD_TWO_105 105 //踢出玩家
#define MSG_SCMD_TWO_106 106 //申请进入房间
#define MSG_SCMD_TWO_107 107 //线下玩家坐下
#define MSG_SCMD_TWO_108 108 //玩家离开房间
#define MSG_SCMD_TWO_109 109 //更新房卡
#define MSG_SCMD_TWO_110 110 //申请创房
#define MSG_SCMD_TWO_111 111 //拉取创房配置
#define MSG_SCMD_TWO_112 112 //通知tabkey属于哪个子游戏
#define MSG_SCMD_TWO_113 113 //修改昵称
#define MSG_SCMD_TWO_115 115 //拉取红点信息
#define MSG_SCMD_TWO_116 116 //读取红点信息
#define MSG_SCMD_TWO_120 120 // 申请固定桌消息
#define MSG_SCMD_TWO_121 121 // 修改固定桌
#define MSG_SCMD_TWO_122 122 // 点击固定桌
#define MSG_SCMD_TWO_127 127 // 固定桌人数局数的变化
#define MSG_SCMD_TWO_128 128 // 捐献俱乐部钻石
#define MSG_SCMD_TWO_129 129 // 俱乐部基金的广播
#define MSG_SCMD_TWO_131 131 // 俱乐部固定桌参数变化
#define MSG_SCMD_TWO_140 140 //创建俱乐部
#define MSG_SCMD_TWO_141 141 //解散俱乐部
#define MSG_SCMD_TWO_142 142 //加入俱乐部
#define MSG_SCMD_TWO_143 143 //处理加入俱乐部
#define MSG_SCMD_TWO_144 144 //离开俱乐部
#define MSG_SCMD_TWO_145 145 //删除俱乐部成员
#define MSG_SCMD_TWO_146 146 //俱乐部成员列表请求
#define MSG_SCMD_TWO_147 147 //俱乐部列表请求
#define MSG_SCMD_TWO_148 148 //申请列表请求
#define MSG_SCMD_TWO_149 149 //屏蔽列表请求
#define MSG_SCMD_TWO_150 150 //自由桌列表请求
#define MSG_SCMD_TWO_151 151 //修改俱乐部信息
#define MSG_SCMD_TWO_152 152 //转移俱乐部群主
#define MSG_SCMD_TWO_153 153 //修改管理员

#define MSG_SCMD_TWO_154 154 //刷新请求列表
#define MSG_SCMD_TWO_155 155 //刷新自由桌

#define MSG_SCMD_TWO_160 160 //俱乐部上下分
#define MSG_SCMD_TWO_161 161 //俱乐部上下分记录日志
#define MSG_SCMD_TWO_162 162 //俱乐部积分设置
#define MSG_SCMD_TWO_163 163 //查找俱乐部

#define MSG_SCMD_TWO_200 200 //拉取战绩
#define MSG_SCMD_TWO_201 201 //拉取战绩详情

#define MSG_SCMD_TWO_210 210 //texas拉取房间即时信息(时长，座位人数，状态)


#define MSG_CMD_3 3
#define MSG_SCMD_THREE_1 1
#define MSG_SCMD_THREE_2 2 //通知玩家状态改变
#define MSG_SCMD_THREE_3 3//进入牌桌后通知玩家其他玩家信息
#define MSG_SCMD_THREE_10 10//聊天
#define MSG_SCMD_THREE_72 72//玩家申请解散牌桌
#define MSG_SCMD_THREE_73 73//处理解散牌桌申请

#define MSG_CMD_4 4
#define MSG_SCMD_FOUR_1 1//客户端准备
#define MSG_SCMD_FOUR_2 2//客户端断线重连
#define MSG_SCMD_FOUR_5 5//开始下一局
#define MSG_SCMD_FOUR_7 7//更新玩家状态
#define MSG_SCMD_FOUR_9 9//房主点击开始
#define MSG_SCMD_FOUR_10 10//房主点击开始
#define MSG_SCMD_FOUR_11 11//准备开始
#define MSG_SCMD_FOUR_14 14//通知抢庄
#define MSG_SCMD_FOUR_15 15//通知谁抢了庄
#define MSG_SCMD_FOUR_16 16//确定庄家
#define MSG_SCMD_FOUR_17 17//通知出牌(理牌)
#define MSG_SCMD_FOUR_18 18//通知出牌(理牌)结果
#define MSG_SCMD_FOUR_19 19//翻倍(下注)
#define MSG_SCMD_FOUR_20 20// 四倍抢庄翻倍
#define MSG_SCMD_FOUR_21 21// 通知选手牌
#define MSG_SCMD_FOUR_22 22// 发牌
#define MSG_SCMD_FOUR_23 23// 发牌
#define MSG_SCMD_FOUR_24 24//通知玩家抢地主(欢乐)
#define MSG_SCMD_FOUR_25 25//通知玩家抢地主结果(欢乐)
#define MSG_SCMD_FOUR_27 27//通知玩家加倍(欢乐)
#define MSG_SCMD_FOUR_28 28//牌局结束
#define MSG_SCMD_FOUR_29 29//广播玩家加倍(欢乐)
#define MSG_SCMD_FOUR_30 30//比牌
#define MSG_SCMD_FOUR_31 31//托管
#define MSG_SCMD_FOUR_32 32//弃牌
#define MSG_SCMD_FOUR_33 33//提示弃牌
#define MSG_SCMD_FOUR_34 34//提示推注
#define MSG_SCMD_FOUR_36 36//买码
#define MSG_SCMD_FOUR_41 41//下一局退出
#define MSG_SCMD_FOUR_43 43//强制退出
#define MSG_SCMD_FOUR_50 50//发送战绩
#define MSG_SCMD_FOUR_63 63//牌局回顾
#define MSG_SCMD_FOUR_29 29//配牌
#define MSG_SCMD_FOUR_100 100//下一局
#define MSG_SCMD_FOUR_160 160//积分更新

#define MSG_CMD_5	5
#define MSG_SCMD_FIVE_1		1	//玩家发送准备
#define MSG_SCMD_FIVE_2		2	//游戏开始
#define MSG_SCMD_FIVE_3		3	//发牌
#define MSG_SCMD_FIVE_4		4	//定庄
#define MSG_SCMD_FIVE_5		5	//玩家操作
#define MSG_SCMD_FIVE_6		6	//通知玩家牌型
#define MSG_SCMD_FIVE_7		7	//广播玩家操作
#define MSG_SCMD_FIVE_8		8	//结算
#define MSG_SCMD_FIVE_9		9	//广播亮牌
#define MSG_SCMD_FIVE_10	10	//玩家断线
#define MSG_SCMD_FIVE_11	11	//牌局结束通知状态
#define MSG_SCMD_FIVE_12	12	//玩家在桌子上断线重连

#define MSG_CMD_7	7
#define MSG_SCMD_SEVEN_1		1	//玩家发送准备
#define MSG_SCMD_SEVEN_2		2	//游戏开始
#define MSG_SCMD_SEVEN_3		3	//发牌
#define MSG_SCMD_SEVEN_4		4	//定庄
#define MSG_SCMD_SEVEN_5		5	//玩家操作
#define MSG_SCMD_SEVEN_6		6	//通知玩家牌型
#define MSG_SCMD_SEVEN_7		7	//广播玩家操作
#define MSG_SCMD_SEVEN_8		8	//结算
#define MSG_SCMD_SEVEN_9		9	//广播亮牌
#define MSG_SCMD_SEVEN_10		10	//玩家断线
#define MSG_SCMD_SEVEN_11		11	//牌局结束通知状态
#define MSG_SCMD_SEVEN_12		12	//玩家在桌子上断线重连
#define MSG_SCMD_SEVEN_13		13	//通知购买保险
#define MSG_SCMD_SEVEN_14      	14	//加钱
#define MSG_SCMD_SEVEN_15      	15	//通知可能主动显示自己的手牌
#define MSG_SCMD_SEVEN_16      	16	//玩家主动显示的手牌通知
#define MSG_SCMD_SEVEN_17		17	//牌局结束后，玩家没有足够的钱，而退出桌子通知
#define MSG_SCMD_SEVEN_19		19	//是否中保险
#define MSG_SCMD_SEVEN_20		20	//获取旁观玩家信息
#define MSG_SCMD_SEVEN_21       21  //玩家桌子上的金币因为自动代入的更新
#define MSG_SCMD_SEVEN_22		22	//自动操作(弃牌，看牌)
#define MSG_SCMD_SEVEN_23		23  //jackport
#define MSG_SCMD_SEVEN_24		24  //jackportget
#define MSG_SCMD_SEVEN_25       25  //奖池
#define MSG_SCMD_SEVEN_29       29  //配牌
#define MSG_SCMD_SEVEN_30		30  //rebuy
#define MSG_SCMD_SEVEN_31		31  //addbuy
#define MSG_SCMD_SEVEN_32		32  //放弃rebuy addbuy
#define MSG_SCMD_SEVEN_33		33  //mtt合桌通知
#define MSG_SCMD_SEVEN_34       34  //mtt升盲通知

#define MSG_CMD_100 100
#define MSG_SCMD_HANDRED_1  1//心跳协议
#define MSG_SCMD_HANDRED_2  2//通知网关服务器玩家断线

#define MSG_CMD_1000 1000//服务器组件内部消息
#define MSG_SCMD_THOURAND_1  1//内部组件(网关服和游戏服)连上大厅的时候告诉大厅socket索引
#define MSG_SCMD_THOURAND_2 2//游戏服通知大厅玩家进入牌桌信息
#define MSG_SCMD_THOURAND_3 3////通知大厅玩家坐下成功
#define MSG_SCMD_THOURAND_4 4////通知大厅打到大鱼
#define MSG_SCMD_THOURAND_5 5////通知游戏服打牌玩家断线重连
#define MSG_SCMD_THOURAND_6 6////通知大厅创房成功
#define MSG_SCMD_THOURAND_7 7//大厅创房俱乐部自由桌成功，通知web
#define MSG_SCMD_THOURAND_8 8//玩家断线重连，通知web
#define MSG_SCMD_THOURAND_9 9//大厅通知游戏服创建房间
#define MSG_SCMD_THOURAND_10 10//大厅通知游戏服进入房间
#define MSG_SCMD_THOURAND_11 11//大厅俱乐部自由桌成功解散，通知web
#define MSG_SCMD_THOURAND_12 12//游戏服重启，通知大厅
#define MSG_SCMD_THOURAND_15 15		// 通知大厅固定桌创建房间
#define MSG_SCMD_THOURAND_16 16		// 通知俱乐部固定桌人数局数变化 
#define MSG_SCMD_THOURAND_31 31		// 通知网关断开连接
#define MSG_SCMD_THOURAND_32 32		// 通知玩家被踢出
#define MSG_SCMD_THOURAND_33 33		// 通知游戏服更新(抽水点，平台或游戏服状态)

#define MSG_SCMD_THOURAND_102 102    //大厅通知游戏服进入mtt房间(观赛)
#define MSG_SCMD_THOURAND_103 103   //大厅通知游戏服进入mtt房间(参赛)


#define MSG_SCMD_THOURAND_85 85		// 更新金额
#define MSG_SCMD_THOURAND_109 109		// 查询金额
#define MSG_SCMD_THOURAND_111 111		// 取一个机器人uid,balance


#define MSG_SCMD_THOURAND_160 160//俱乐部积分变更通知游戏服


#define MSG_CMD_10000 10000//压力测试协议
#define MSG_SCMD_TENTHOURAND_1 1


//金币流水类型定义
#define USER_COIN_CODE_NON          0    //
#define USER_COIN_CODE_WIN 			1    //赢
#define USER_COIN_CODE_LOSE  		2    //输
#define USER_COIN_CODE_BUY   		3    //充值
#define USER_COIN_CODE_TAKE_FOR  	4    //提现申请
#define USER_COIN_CODE_TAKE_OK  	5	 //提现成功
#define USER_COIN_CODE_TAKE_CANCEL  6	 //提现撤消
#define USER_COIN_CODE_EXCHANGE     7    //兑换银币
#define USER_COIN_CODE_MAX          8


//数据表的宏定义
#define TABLE_Q_USERJINB "q_user"
#define TABLE_Q_ROBOTJINB "q_robot"
#define TABLE_Q_ROBOTINDEX "q_robot_%d"
#define TABLE_Q_USER "q_user"
#define TABLE_Q_TRADE_CHARGE "q_trade_charge"
#define TABLE_Q_TABLES "q_tables"
#define TABLE_Q_TABLES7 "q_tables_7"
#define TABLE_Q_TABLES_HIS "q_tables_his"
#define TABLE_Q_TABLE_INFO "q_table_info"
#define TABLE_Q_GROUP_USER "q_club_user"
#define TABLE_Q_GROUP "q_club"
#define TABLE_Q_GROUP_SCORE "q_group_score"
#define TABLE_Q_GROUP_FAXED "q_group_faxedtable"
#define TABLE_Q_GROUP_CTR "q_group_contribute"
#define TABLE_Q_GROUP_INTEGRATION "q_group_pointrecord"
#define TABLE_Q_WORD "q_word"
#define TABLE_Q_TABLESCONFIG "q_table_config"
#define TABLE_Q_TABLESJINBCONFIG "q_tablejinb_config"
#define TABLE_Q_TABLESJINBZJHCONFIG "q_tablejinb_zjh_config"
#define TABLE_Q_TABLESDDZCONFIG "q_tableddz_config"
#define TABLE_Q_TABLESNIUCONFIG "q_tableniu_config"
#define TABLE_Q_TABLESNIUPUTCONFIG "q_tableniu_putconfig"
#define TABLE_Q_TABLESZJHCONFIG "q_tablezjh_config"
#define TABLE_Q_TABLEFIGHT "q_fight"
#define TABLE_T_ACCOUNT_REPORT "t_account_report"
#define TABLE_T_LOGIN "t_login"
#define TABLE_T_USER "t_user"
#define TABLE_T_GAME "t_game"
#define TABLE_T_GCOIN "t_gcoin"
#define T_M_SYS  "t_m_sys"
#define TABLE_Q_CLUB_USER "q_club_user"
#define TABLE_D_DOCK_WHITE_LIST "d_dock_white_list"
#define TABLE_D_DOCK "d_dock"
#define TABLE_T_SUM "t_sum"
#define T_M_GAME "t_m_game"
#define T_GAME_INFO_4 "t_info_4"
#define T_GAME_INFO_5 "t_info_5"
#define T_ROBOT_CHARG_RECORD "t_robot_charge_record"
#define	T_DDZGTYPE_CONFIG		"t_ddzgtype_config"
#define	T_DDZGTYPE_STOCK		"t_ddzgtype_stock"
#define TABLE_F_DISABLE_GAME	"f_disable_game"
#define TABLE_STAGE	"q_stage"
#define TABLE_MSG_REDPOINT	"t_msg_redpoint"

//组件subid定义
#define HALL_SUB 0
#define WEB_SUB 1
#define JFNN_SUB 12
#define MAX_SUB_NUM 20
#define MAX_CLIENTMOUDLE_NUM 5//最大跟网关的连接数量
#define MAX_CLIENTMOUDLE_DBINDEX 4//跟db代理的连接

//机器人uid
#define	GAME_ROBERT_MAX		1000	//最大机器人数
#define	GAME_ROBERT_BINDEX	10000	//机器人最小索引
#define	GAME_ROBERT_EINDEX	11000	//机器人最大索引

#endif
