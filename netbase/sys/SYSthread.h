#ifndef SYSTHREAD_H_
#define SYSTHREAD_H_
#include "common.h"
class SYSThread{
public:
	SYSThread();
	virtual ~SYSThread();
	int start();
	int stop(int nWaitTime=-1);
	bool isRunning()const
	{
		return _bStarted;
	}
	bool isTerminated() const
	{
		return _bTerminate;
	}
	void Terminate()
	{
		_bTerminate = true;
	}
	static void *hook(void* arg);
	static void ExitThread(void* arg);
protected:
	virtual long Run() = 0;
private:
	SYSThread(const SYSThread& );
	SYSThread& operator=(const SYSThread&);
	int waitCond(int nWaitTime=-1);
private:
	pthread_mutex_t _condmutex;
	pthread_cond_t _cond;
	pthread_t _hThread;
	volatile bool _bStarted;
	volatile bool _bTerminate;



};
#endif
