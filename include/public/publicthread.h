#ifndef PUBLICTHREAD_H_
#define PUBLICTHREAD_H_
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>

class PublicThread
{
public:
	PublicThread();

	~PublicThread();

	int start();

	///线程执行函数
	static void *hook(void* arg);

	static void ExitThread(void* arg);
	
	bool isTerminated();

	virtual int run() = 0;
protected:
	pthread_mutex_t _condmutex;
	pthread_cond_t _cond;
	pthread_t _hThread;
	volatile bool _bStarted;
	volatile bool _bTerminate;
};
#endif
