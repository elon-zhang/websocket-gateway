#pragma once

// ��ȡ������Ϣ ��������
int GetProfileString(const char *profile, const char *AppName, const char *KeyName, char *KeyVal );

// ��ȡ������Ϣ
int GetProfileString(const char *profile, const char *KeyName, char *KeyVal );

// ��ȡ������Ϣ ����
int GetProfileInt(const char *profile, const char *KeyName);

/********************************************************************
//func:ʮ����ת������ʮ������
//para:lnum--��Ҫת��������dest--Ŀ���ַ���
//auth:jiangjq
//date:2014/08/28
*********************************************************************/
char *dec2duo(long lnum,char *dest);

/********************************************************************
//func:������ϷID�����������ɱ�����Ϸ�ֺ�
//para:gameid--��ϷID
		deskid--����
		seq    --�ֺű��滺��
//auth:jiangjq
//date:2014/08/28
*********************************************************************/

char *getSequence(int gameid,int deskid,char *seq);
