#include "SYSthread.h"
#include "SYStimerecord.h"

void SYSThread::ExitThread(void* arg)
{
	if(arg)
	{
	//	printf("in SYSThread::ExitThread\n");
		//SYSThread* pThread = (SYSThread*)arg;
		//pthread_mutex_lock(&pThread->_condmutex);
		//pThread->_bStarted = false;
		//pthread_cond_signal(&pThread->_cond);
		//pthread_mutex_unlock(&pThread->_condmutex);
	
	}
}
void* SYSThread::hook(void* arg)
{
	void* rc = NULL;
	if(arg)
	{
		SYSThread* pThread = (SYSThread*)arg;
		//这里不需要加锁变量是唯一的
		//pthread_mutex_lock(&pThread->_condmutex);
		//pThread->_bStarted = true;
		//pthread_cleanup_push(SYSThread::ExitThread,pThread);
		//pthread_cond_signal(&pThread->_cond);
		//pthread_mutex_unlock(&pThread->_condmutex);

		rc = (void*)pThread->Run();

		//pthread_cleanup_pop(1);
	}
	return rc;
}
SYSThread::SYSThread():_hThread(0),_bStarted(false),_bTerminate(false)
{
	pthread_mutex_init(&_condmutex, NULL);
	pthread_cond_init(&_cond, NULL);
}

SYSThread::~SYSThread()
{
	if(_bStarted)
	{
		stop(100);
	}
	pthread_mutex_destroy(&_condmutex);
	pthread_cond_destroy(&_cond);
}
int SYSThread::start()
{
	int ret = 0;
	//ret = pthread_mutex_lock(&_condmutex);
	if(ret != 0)
	{
		return ret;
	}
	if(!_bStarted)
	{
		_bStarted = true;
		_bTerminate=false;
		//用默认的线程属性就行了
		//pthread_attr_t attr;
		//pthread_attr_init(&attr);
		//pthread_attr_setstacksize(&attr,1024 * 1024 * 10);
		//pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

		ret = pthread_create(&_hThread,NULL,SYSThread::hook,(void*)this);
		if(ret == 0)
		{
			//waitCond();
		}
		//pthread_attr_destroy(&attr);
	}
	//ret = pthread_mutex_unlock(&_condmutex);
	return (int32_t)ret;
}
int SYSThread::stop(int iWaitTime)
{
	int rc = 0;
	rc = pthread_mutex_lock(&_condmutex);
	if(rc != 0)
	{
		return rc;
	}
	if(_bStarted && !pthread_equal(pthread_self(),_hThread))
	{
		Terminate();
		waitCond(iWaitTime);
		if(!_bStarted)
		{
			_bTerminate = false;
		}
		else
		{
			if(pthread_cancel(_hThread) == 0)
			{
				waitCond(10);
			}
			if(!_bStarted)
			{
				_bTerminate = false;
			}
			else
			{
				rc = -1;
			}
		}
	}
	rc = pthread_mutex_unlock(&_condmutex);
	return (int32_t)rc;
}
int SYSThread::waitCond(int iWaitTime)
{
	int rc = 0;
	if(iWaitTime < 0)
	{
		rc = pthread_cond_wait(&_cond,&_condmutex);	
	}
	else
	{
		timespec ts;
		getTimeMillisecond(iWaitTime, ts);
		rc = pthread_cond_timedwait(&_cond,&_condmutex,&ts);
	}
	return rc;
}
