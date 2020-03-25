#ifndef SYSNOTIFY_H_
#define SYSNOTIFY_H_

#include "common.h"
///等待事件时返回值
#define NOTIFY_OK        0   /* 有事件发生 */
#define NOTIFY_TIMEOUT   110 /* 等待时间超时 */

///事件对象定义
class SYSNotify
{
public:
    ///构造函数
	SYSNotify();

    ///析构函数
    ~SYSNotify();

    ///等待一个事件
    /**
     *@param iWait 等待的时间，单位毫秒，默认为直到有事件发生
     *@return EVENT_OK表示成功等到该事件激活，EVENT_TIMEOUT表示超时，其他失败
     */
	int32_t Wait(int32_t iWait = INFINITE);

    ///激发该事件
    /**
     * @return EVENT_OK表示成功，其他表示失败
     */
	int32_t Signal(void);

    ///重置事件状态为false
    /**
     * @return EVENT_OK表示成功，其他表示失败
    */
    int32_t Reset(void);

private:
    pthread_mutex_t m_Mutex; /* 保护条件变量的互斥子 */
    pthread_cond_t m_Cond; /* 条件变量句柄 */
    bool m_bNotify;

    ///封闭掉下面函数
    SYSNotify(const SYSNotify& one);
    SYSNotify& operator=(const SYSNotify& one);
};




#endif
