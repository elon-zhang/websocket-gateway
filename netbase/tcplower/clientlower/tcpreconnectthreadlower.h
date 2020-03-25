#ifndef TCPRECONNECTTHREADLOWER_H_
#define TCPRECONNECTTHREADLOWER_H_
#include <sys/SYSthread.h>
class ReConnectThreadLower:public SYSThread
{
public:
	virtual long Run();
};
#endif
