#include "tcpreconnectthreadlower.h"
#include <tcpbase/tcpbasevariable.h>
#include "tcpclientcreatorlower.h"
extern TcpClientCreatorLower*	glpTcpClientCreator;
long ReConnectThreadLower::Run()
{
	uint32_t nCount=0;
	THREAD_SIGNAL_BLOCK
	while(!isTerminated())
	{
		//sleep10毫秒
		sleep(5);
		++nCount;
		//每隔0.1秒，检查一次
		//if( (nCount%10) == 0 )
		{
			glpTcpClientCreator->CheckConnection();
		}
	}
	return 0;
}
