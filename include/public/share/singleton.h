#ifndef SINGLETON_H
#define SINGLETON_H
#include <iostream>
template <typename singleclass>
class singleton{
public:
	static singleclass* getInstance()
	{
		if (!m_instance)
		{
			m_instance = new singleclass;
		}
		return m_instance;
	}

	static void delInstance()
	{
		if (m_instance)
		{
			delete m_instance;
		}
		m_instance = NULL;
	}

protected:

	singleton(){}

	~singleton(){}

	singleton(const singleton&) {}//��ֹ��������һ��ʵ��  

	singleton& operator=(const singleton&){} //��ֹ��ֵ����һ��ʵ�� 

private:
	static singleclass* m_instance ;
};
template <typename singleclass>
singleclass* singleton<singleclass>::m_instance = NULL;
#endif
