#ifndef THREAD_MUTEX_H_
#define THREAD_MUTEX_H_
#include "common.h"
class SYSThreadMutex{
public:
	SYSThreadMutex(){
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&_mutex,&attr);
		pthread_mutexattr_destroy(&attr);
	
	}
	~SYSThreadMutex(){
		pthread_mutex_destroy(&_mutex);
	}
	void lock(){
		pthread_mutex_lock(&_mutex);
	}
	int trylock(){
		return pthread_mutex_trylock(&_mutex);
	}
	void unlock(){
		pthread_mutex_unlock(&_mutex);
	}
private:
	pthread_mutex_t _mutex;
};
class SYSThreadGuard
{
public:
	SYSThreadGuard(SYSThreadMutex *mutex)
	{
		_mutex = NULL;
		if(mutex){
			_mutex=mutex;
			_mutex->lock();
		}
	}
		~SYSThreadGuard(){
			if(_mutex){
				_mutex->unlock();
			}
		}
	

private:
	SYSThreadMutex *_mutex;
};
#endif
