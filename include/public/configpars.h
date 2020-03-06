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

	int config_info(const  char *filenam, int partype=0);//partype:0游戏，1网关

	int parsjson(char* jsonfilename);

	int getGateservernum();

	int getGateindexa();

	int getGateindexb();


	//登录地址
	char* getLoginip();

	//登录端口
	int getLoginport();


	//网关地址a
	char* getGateinneripa();

	//网关端口a
	const int getGateinnerporta();

	//网关地址
	char* getGateinneripb();

	//网关端口
	int getGateinnerportb();


	//游戏服数据库
	char* getGameserverdbip();

	char* getGameserverdbuser();

	char* getGameserverdbpwd();

	char* getGameserverdbname();

	//大厅服数据库
	char* getHallserverdbip();

	char* getHallserverdbuser();

	char* getHallserverdbpwd();

	char* getHallserverdbname();

	char* getGlo_IP3();

	int getGlo_ApplyServerPort();


	//redis服数据库
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
	//获取网关下标
	int getGateinnerindex();

	//对内地址
	char* getGateinnerip();

	//对内端口
	int getGateinnerport();


	//对外地址
	char* getGateoutip();

	//对外端口
	int getGateoutport();

	//dbproxy
	char* getDbproxyip();

	int getDbproxyport();

public:
	Value jsonRoot; 

	char		Glo_LOGINIP[MAX_IP_LEN];			//登录服务器地址
	int			Glo_LOGINPORT;					//登录服务器端口

	int			Glo_GATEINDEXA;
	int			Glo_GATEINDEXB;

	int			Glo_GATESERVERNUM;

	char		Glo_GATEINNERIPA[MAX_IP_LEN];			//网关服务器A对内地址
	int			Glo_GATEINNERPORTA;					//网关服务器A对内端口

	char		Glo_GATEINNERIPB[MAX_IP_LEN];			//网关服务器B对内地址
	int			Glo_GATEINNERPORTB;					//网关服务器B对内端口


	char		Glo_DBHALLADDRESS[MAX_IP_LEN];	//大厅数据库地址
	char		Glo_DBHALLPASS[MAX_IP_LEN];		//大厅数据库密码
	char		Glo_DBHALLUSER[MAX_IP_LEN];	//大厅数据库用户
	char		Glo_DBHALLNAME[MAX_IP_LEN];	//大厅数据库名

	char		Glo_DBGAMEADDRESS[MAX_IP_LEN];	//游戏数据库地址
	char		Glo_DBGAMEPASS[MAX_IP_LEN];		//游戏数据库密码
	char		Glo_DBGAMEUSER[MAX_IP_LEN];	//游戏数据库用户
	char		Glo_DBGAMENAME[MAX_IP_LEN];	//游戏数据库名

	char        Glo_IP3[MAX_IP_LEN];            //应用服务器地址
	int			Glo_ApplyServerPort;			//应用服务器端口

	char		Glo_REDISIP[MAX_IP_LEN];		//redis服务器地址
	char		Glo_REDISUSER[MAX_IP_LEN];		//redis用户名
	char		Glo_DBREDISPASS[MAX_IP_LEN];		//redis数据库密码
	int			Glo_DBREDISNAME;	//redis数据库名
	int			Glo_REDISPORT;					//redis服务器端口
	char		Glo_Walleturl[MAX_IP_LEN];		//钱包服域名

	/***************************************/
	char		Glo_Walletapikey[MAX_IP_LEN];		//钱包服域名

	char		Glo_Walletapisecretkey[MAX_IP_LEN];		//钱包服域名

	/****************************************************/
	char		Glo_GATEINNERIP[MAX_IP_LEN];		//对内网关服务器地址
	int			Glo_GATEINNERPORT;					//对内网关服务器端口

	char		Glo_GATEOUTIP[MAX_IP_LEN];			//对外网关服务器地址
	int			Glo_GATEOUTPORT;					//对外网关服务器端口

	int         Glo_GATEINDEX;

	char		Glo_DBPROXYIP[MAX_IP_LEN];			//DB代理ip
	int			Glo_DBPROXYPORT;					//DB代理端口
};
#endif