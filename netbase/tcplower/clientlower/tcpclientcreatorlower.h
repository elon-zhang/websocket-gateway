#ifndef TCPCLIENTCREATORLOWER_H_
#define TCPCLIENTCREATORLOWER_H_
#include "tcpreconnectthreadlower.h"
#include "tcpclientlower.h"
#include <include/tcpclientfunction.h>
#include <tcpbase/tcpbasethread.h>
#include <vector>

///主动连接对象工厂类
class TcpClientCreatorLower:public ITcpClientCreator
{
public:
	//应用层接口
	virtual int Init(int32_t iSendThreadCount=1, int32_t iRecvThreadCount=1);
	virtual int CreateTcpClient(
								ITcpClientFunction** lppClient,
								const char*  lpServerAddr,
								uint16_t     wServerPort,
								ITcpClientModule* lpCallback,
								uint32_t nSendBuffSize=2048,
								const char* lpLocalAddr=NULL,
								int32_t iRecvBuffSize=2048);

public:
	TcpClientCreatorLower();
	///增加工厂的引用计数
	int AddRef();
	///释放工厂，减小引用计数，计数为0时删除本工厂
	int Release();

	///检测连接是否断开，并根据自动重连标志进行自动重连
	int CheckConnection();
	///获取发送线程指针
	CTcpSendThread* GetSendThread(int index){return &m_lpSendThread[index];}
	///获取接收线程指针
	CTcpRecvThread* GetRecvThread(int index){return &m_lpRecvThread[index];}
private:
	///析构函数
	~TcpClientCreatorLower();
	///清理内部状态，停止线程等
	void Cleanup();
public:
	static SYSThreadMutex	sMutex;
protected:
	///主动连接的线程对象指针
	ReConnectThreadLower*	m_lpReConnectThread;
	///发送数据线程对象数组指针
	CTcpSendThread*		m_lpSendThread;
	///接收数据线程对象数组指针
	CTcpRecvThread*		m_lpRecvThread;
	///工厂引用计数
	int				m_iRefCount;
	///发送线程个数
	int32_t			m_iSendThreadCount;
	///接收线程个数
	int32_t			m_iRecvThreadCount;
	///存储主动连接对象
	std::vector<TcpClientLower*> m_vInitiator;
};

#endif
