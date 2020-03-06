#ifndef SMARTPTR_H
#define SMARTPTR_H
#include <stdio.h>
using namespace std;
template <typename T>
class SmartPtr
{
public:
	// ���캯�� Ĭ��Ϊ��
	SmartPtr() : pointer(0), counter(0)
	{
	}

	// �β�Ϊָ��Ĺ��캯��
	SmartPtr(T* p) : pointer(0), counter(0){
		*this = p;
	}

	// ���ƹ��캯��
	SmartPtr(const SmartPtr<T> &p) :
		pointer(p.pointer), counter(p.counter){
		Increase();
	}

	~SmartPtr(){
		Decrease();
	}

	// ָ��ĸ�ֵ�����������Ͳ�ͬ�������Ը�ֵ
	SmartPtr operator=(T* p){
		Decrease();
		if (p){
			pointer = p;
			counter = new int(1);
		}
		else {
			pointer = 0;
			counter = 0;
		}
		return *this;
	}

	// ����ָ�븳ֵ������
	SmartPtr operator=(SmartPtr<T> &p){
		// �����Ը�ֵ
		if (this != &p){
			Decrease();
			pointer = p.pointer;
			counter = p.counter;
			Increase();
		}
		return *this;
	}

	operator bool() const{
		return counter != 0;
	}

	// ������������
	T* operator*() const{
		return this;
	}

	// ->����������
	T* operator->() const{
		return pointer;
	}

	// ���صײ�ָ��
	int getPtrPointer() const{
		return *pointer;
	}

	// �������ü���
	int getPtrCounter() const{
		return *counter;
	}

	// ���������������� ptr<derived>���ܷ��� ptr<based>���ڲ�����
	template<typename C> friend class SmartPtr;

	template<typename C>
	SmartPtr(const SmartPtr<C> &p) :
		pointer(p.pointer), counter(p.counter){
		Increase();
	}

	template<typename C>
	SmartPtr<T> & operator=(const SmartPtr<C> &p){
		Decrease();
		pointer = p.pointer;
		counter = p.counter;
		Increase();
		return *this;
	}

	// �����ڲ�ʹ�� dynamic_cast���жϵ�ת����ʧ�����ָ��
	template<typename C>
	SmartPtr<C> Cast() const{
		C* converted = dynamic_cast<C*>(pointer);
		SmartPtr<C> result;
		if (converted){
			result.pointer = converted;
			result.counter = counter;
			result.Increase();
		}
		return result;
	}

private:
	T*      pointer;
	int*    counter;

	void Increase(){
		if (counter)
			++*counter;
	}

	void Decrease(){
		if (counter && --*counter == 0){
			delete pointer;
			delete counter;
			counter = 0;
			pointer = 0;
		}
	}
};
#endif