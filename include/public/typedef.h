#pragma  once

//数值定义
#define GFLOG		8				//消息头尾的标志符号
//#define MAX_SQL_LEN	2014			// sql语句长度
#define DAY_SEC		(60 * 60 * 24)	// 一天的秒数
#define MAX_LENGTH	(1024*2)		// 消息包长度


// 取最大值 [2015-5-6 16:02:25 Jacky]

#ifndef max1
	#define max1(a,b) (((a) > (b)) ? (a) : (b))
#endif

// 取最小值 [2015-5-6 16:02:40 Jacky]

#ifndef min1
	#define min1(a,b) (((a) < (b)) ? (a) : (b))
#endif


// 交换两个值 [2015-5-6 16:02:53 Jacky]
// #ifndef swap
// 	#define swap(a,b) do { if ((a) != (b)) { (a) ^= (b); (b) ^= (a); (a) ^= (b); } } while(0)
// #endif

#define myfilename(x) strrchr(x,'/')?strrchr(x,'/')+1:x

// 清空变量
#define ZERO(v)\
	memset(&v, 0, sizeof(v));

// 检查条件若不成功则返回错误代码
#define CheckCond(cond, ret)\
	if(!(cond)) return ret;

// 检查条件若不成功则返回
#define CheckCondVoid(cond)\
	if(!(cond)) return;

// 检查是否满足条件，不满足条件就break [2015-5-6 15:30:12 Jacky]
#define CheckBreak(con)\
	if ( !(con) ) break;

// 检查是否满足条件，不满足条件就continue [2015-5-6 15:30:38 Jacky]
#define CheckContinue(con)\
	if ( !(con) ) continue;

// 序列号数据到缓存中
#define setValue(value)\
	memcpy(&msgbuf[sum], &value, sizeof(value));\
	sum += sizeof(value);

// 序列号数据到缓存中
#define setValueEx(value, len)\
	memcpy(&msgbuf[sum], &value, len);\
	sum += len;

#pragma pack (1)
struct MSG_HEAD		//消息结构--头部
{
	char			HeadPad		;       //消息头的标志              1
	char			MessageFree	;		//消息预留位                1[客户端发来的为0   接管的为1]
	short int		MessageSize	;		//消息内容指令长度          2
	short int		MessageOrder;		//消息指令序号              2
	short int		MessageCmd	;		//消息指令号                2
	short int		MessageSCmd	;		//消息子指令号              2
	int				MessageUid	;		//用户数字ID				4
	int				MessageToken;		//用户令牌					4
	char			MessageCheck;		//消息校验位                1
	char			MessageEnd	;		//消息头结束标识位          1
	
	MSG_HEAD()
	{
		memset(this, 0, sizeof(MSG_HEAD));
	}
};

#pragma pack()