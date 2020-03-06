#ifndef CONFIGPARS_H_
#define CONFIGPARS_H_
#include "share/singleton.h"
#include "json/json.h"
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;
using namespace Json;
#define  MAX_IP_LEN 64
//#define DEBUG_SYS_INIT
class ConfigPars:public singleton<ConfigPars>
{
public:

	ConfigPars();

	~ConfigPars();

	int read_ini(const char *name, char *val, FILE *file);

	int config_info(const  char *filenam, int partype=0);//partype:0��Ϸ��1����

	int parsjson(char* jsonfilename);

	int getGateservernum();

	int getGateindexa();

	int getGateindexb();


	//��¼��ַ
	char* getLoginip();

	//��¼�˿�
	int getLoginport();


	//���ص�ַa
	char* getGateinneripa();

	//���ض˿�a
	const int getGateinnerporta();

	//���ص�ַ
	char* getGateinneripb();

	//���ض˿�
	int getGateinnerportb();


	//��Ϸ�����ݿ�
	char* getGameserverdbip();

	char* getGameserverdbuser();

	char* getGameserverdbpwd();

	char* getGameserverdbname();

	//���������ݿ�
	char* getHallserverdbip();

	char* getHallserverdbuser();

	char* getHallserverdbpwd();

	char* getHallserverdbname();

	char* getGlo_IP3();

	int getGlo_ApplyServerPort();


	//redis�����ݿ�
	char* getRedisserverdbip();

	int getRedisserverdbname();

	char* getRedisserverdbuser();

	char* getRedisserverdbpwd();

	int getRedisServerPort();

	char* getWalleturl();

	/******************************/

	char* getApiKey();

	char* getApiseceretKey();

	/**********************************/
	//��ȡ�����±�
	int getGateinnerindex();

	//���ڵ�ַ
	char* getGateinnerip();

	//���ڶ˿�
	int getGateinnerport();


	//�����ַ
	char* getGateoutip();

	//����˿�
	int getGateoutport();

	//dbproxy
	char* getDbproxyip();

	int getDbproxyport();

public:
	Value jsonRoot; 

	char		Glo_LOGINIP[MAX_IP_LEN];			//��¼��������ַ
	int			Glo_LOGINPORT;					//��¼�������˿�

	int			Glo_GATEINDEXA;
	int			Glo_GATEINDEXB;

	int			Glo_GATESERVERNUM;

	char		Glo_GATEINNERIPA[MAX_IP_LEN];			//���ط�����A���ڵ�ַ
	int			Glo_GATEINNERPORTA;					//���ط�����A���ڶ˿�

	char		Glo_GATEINNERIPB[MAX_IP_LEN];			//���ط�����B���ڵ�ַ
	int			Glo_GATEINNERPORTB;					//���ط�����B���ڶ˿�


	char		Glo_DBHALLADDRESS[MAX_IP_LEN];	//�������ݿ��ַ
	char		Glo_DBHALLPASS[MAX_IP_LEN];		//�������ݿ�����
	char		Glo_DBHALLUSER[MAX_IP_LEN];	//�������ݿ��û�
	char		Glo_DBHALLNAME[MAX_IP_LEN];	//�������ݿ���

	char		Glo_DBGAMEADDRESS[MAX_IP_LEN];	//��Ϸ���ݿ��ַ
	char		Glo_DBGAMEPASS[MAX_IP_LEN];		//��Ϸ���ݿ�����
	char		Glo_DBGAMEUSER[MAX_IP_LEN];	//��Ϸ���ݿ��û�
	char		Glo_DBGAMENAME[MAX_IP_LEN];	//��Ϸ���ݿ���

	char        Glo_IP3[MAX_IP_LEN];            //Ӧ�÷�������ַ
	int			Glo_ApplyServerPort;			//Ӧ�÷������˿�

	char		Glo_REDISIP[MAX_IP_LEN];		//redis��������ַ
	char		Glo_REDISUSER[MAX_IP_LEN];		//redis�û���
	char		Glo_DBREDISPASS[MAX_IP_LEN];		//redis���ݿ�����
	int			Glo_DBREDISNAME;	//redis���ݿ���
	int			Glo_REDISPORT;					//redis�������˿�
	char		Glo_Walleturl[MAX_IP_LEN];		//Ǯ��������

	/***************************************/
	char		Glo_Walletapikey[MAX_IP_LEN];		//Ǯ��������

	char		Glo_Walletapisecretkey[MAX_IP_LEN];		//Ǯ��������

	/****************************************************/
	char		Glo_GATEINNERIP[MAX_IP_LEN];		//�������ط�������ַ
	int			Glo_GATEINNERPORT;					//�������ط������˿�

	char		Glo_GATEOUTIP[MAX_IP_LEN];			//�������ط�������ַ
	int			Glo_GATEOUTPORT;					//�������ط������˿�

	int         Glo_GATEINDEX;

	char		Glo_DBPROXYIP[MAX_IP_LEN];			//DB����ip
	int			Glo_DBPROXYPORT;					//DB����˿�
};
#endif