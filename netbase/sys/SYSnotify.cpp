#include "SYSnotify.h"
#include "SYStimerecord.h"

SYSNotify::SYSNotify()
: m_bNotify(false)
{
    /* 按缺省的属性初始化 */
    pthread_cond_init(&m_Cond, NULL);
    pthread_mutex_init(&m_Mutex, NULL);
}

SYSNotify::~SYSNotify()
{
    /* 销毁对象 */
    pthread_cond_destroy(&m_Cond);
    pthread_mutex_destroy(&m_Mutex);
}

int32_t SYSNotify::Wait(int32_t iWait /* = INFINITE */)
{
    int32_t iRet = pthread_mutex_lock(&m_Mutex);
    if (iRet != 0)
    {
        return iRet;
    }

    while (!m_bNotify)
    {
        if (iWait < 0)
        {
            /* 无限期条件等待 */
            iRet = pthread_cond_wait(&m_Cond, &m_Mutex);
        }
        else
        {
            /* 超时条件等待 */
            timespec tm;
			getTimeMillisecond(iWait, tm);
            iRet = pthread_cond_timedwait(&m_Cond, &m_Mutex, &tm);
        }

        if (iRet != 0)
        {
            pthread_mutex_unlock(&m_Mutex);
            return iRet;
        }
    }

    m_bNotify = false;
    pthread_mutex_unlock(&m_Mutex);
    return NOTIFY_OK;
}

int32_t SYSNotify::Signal()
{
    int32_t iRet = pthread_mutex_lock(&m_Mutex); //
    if (iRet != 0)
    {
        return iRet;
    }
    m_bNotify = true;

    iRet = pthread_cond_signal(&m_Cond);
    if (iRet != 0)
    {
        pthread_mutex_unlock(&m_Mutex);
        return iRet;
    }

    iRet = pthread_mutex_unlock(&m_Mutex);
    if (iRet != 0)
    {
        return iRet;
    }

    return 0;
}

int32_t SYSNotify::Reset()
{
    int32_t iRet = pthread_mutex_lock(&m_Mutex);
    if (iRet != 0)
    {
        return iRet;
    }
    m_bNotify = false;

    iRet = pthread_mutex_unlock(&m_Mutex);
    if (iRet != 0)
    {
        return iRet;
    }

    return 0;
}


