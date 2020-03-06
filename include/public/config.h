#pragma once

// 读取配置信息 有作用域
int GetProfileString(const char *profile, const char *AppName, const char *KeyName, char *KeyVal );

// 读取配置信息
int GetProfileString(const char *profile, const char *KeyName, char *KeyVal );

// 读取配置信息 整形
int GetProfileInt(const char *profile, const char *KeyName);

/********************************************************************
//func:十进制转换成三十二进制
//para:lnum--需要转换的数字dest--目标字符串
//auth:jiangjq
//date:2014/08/28
*********************************************************************/
char *dec2duo(long lnum,char *dest);

/********************************************************************
//func:根据游戏ID和桌号来生成本局游戏局号
//para:gameid--游戏ID
		deskid--桌号
		seq    --局号保存缓存
//auth:jiangjq
//date:2014/08/28
*********************************************************************/

char *getSequence(int gameid,int deskid,char *seq);
