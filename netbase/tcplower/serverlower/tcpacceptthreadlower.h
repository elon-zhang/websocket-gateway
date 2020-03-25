#ifndef TCPACCEPTTHREADLOWER_H_
#define TCPACCEPTTHREADLOWER_H_
#include <tcpbase/tcpbasethread.h>
class TcpServerLower;
class ConnectPoolLower;
class TcpAcceptThreadLower:public SYSThread
{
public:
	TcpAcceptThreadLower(TcpServerLower* lpServer);
	~TcpAcceptThreadLower();
	int Init();
	virtual long Run();
	int AddAcceptor(TcpServerLower* lpServer);
protected:
	int m_hEpoll;
	TcpServerLower* m_lpServer;
};



#endif /* TCP_ACCEPT_THREAD_H_ */
