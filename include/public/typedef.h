#pragma  once

//��ֵ����
#define GFLOG		8				//��Ϣͷβ�ı�־����
//#define MAX_SQL_LEN	2014			// sql��䳤��
#define DAY_SEC		(60 * 60 * 24)	// һ�������
#define MAX_LENGTH	(1024*2)		// ��Ϣ������


// ȡ���ֵ [2015-5-6 16:02:25 Jacky]

#ifndef max1
	#define max1(a,b) (((a) > (b)) ? (a) : (b))
#endif

// ȡ��Сֵ [2015-5-6 16:02:40 Jacky]

#ifndef min1
	#define min1(a,b) (((a) < (b)) ? (a) : (b))
#endif


// ��������ֵ [2015-5-6 16:02:53 Jacky]
// #ifndef swap
// 	#define swap(a,b) do { if ((a) != (b)) { (a) ^= (b); (b) ^= (a); (a) ^= (b); } } while(0)
// #endif

#define myfilename(x) strrchr(x,'/')?strrchr(x,'/')+1:x

// ��ձ���
#define ZERO(v)\
	memset(&v, 0, sizeof(v));

// ������������ɹ��򷵻ش������
#define CheckCond(cond, ret)\
	if(!(cond)) return ret;

// ������������ɹ��򷵻�
#define CheckCondVoid(cond)\
	if(!(cond)) return;

// ����Ƿ�����������������������break [2015-5-6 15:30:12 Jacky]
#define CheckBreak(con)\
	if ( !(con) ) break;

// ����Ƿ�����������������������continue [2015-5-6 15:30:38 Jacky]
#define CheckContinue(con)\
	if ( !(con) ) continue;

// ���к����ݵ�������
#define setValue(value)\
	memcpy(&msgbuf[sum], &value, sizeof(value));\
	sum += sizeof(value);

// ���к����ݵ�������
#define setValueEx(value, len)\
	memcpy(&msgbuf[sum], &value, len);\
	sum += len;

#pragma pack (1)
struct MSG_HEAD		//��Ϣ�ṹ--ͷ��
{
	char			HeadPad		;       //��Ϣͷ�ı�־              1
	char			MessageFree	;		//��ϢԤ��λ                1[�ͻ��˷�����Ϊ0   �ӹܵ�Ϊ1]
	short int		MessageSize	;		//��Ϣ����ָ���          2
	short int		MessageOrder;		//��Ϣָ�����              2
	short int		MessageCmd	;		//��Ϣָ���                2
	short int		MessageSCmd	;		//��Ϣ��ָ���              2
	int				MessageUid	;		//�û�����ID				4
	int				MessageToken;		//�û�����					4
	char			MessageCheck;		//��ϢУ��λ                1
	char			MessageEnd	;		//��Ϣͷ������ʶλ          1
	
	MSG_HEAD()
	{
		memset(this, 0, sizeof(MSG_HEAD));
	}
};

#pragma pack()