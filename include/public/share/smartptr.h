#ifndef SMARTPTR_H
#define SMARTPTR_H
#include <stdio.h>
using namespace std;
template <typename T>
class SmartPtr
{
public:
	// 构造函数 默认为空
	SmartPtr() : pointer(0), counter(0)
	{
	}

	// 形参为指针的构造函数
	SmartPtr(T* p) : pointer(0), counter(0){
		*this = p;
	}

	// 复制构造函数
	SmartPtr(const SmartPtr<T> &p) :
		pointer(p.pointer), counter(p.counter){
		Increase();
	}

	~SmartPtr(){
		Decrease();
	}

	// 指针的赋值操作符，类型不同，不是自赋值
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

	// 智能指针赋值操作符
	SmartPtr operator=(SmartPtr<T> &p){
		// 处理自赋值
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

	// ×操作符重载
	T* operator*() const{
		return this;
	}

	// ->操作符重载
	T* operator->() const{
		return pointer;
	}

	// 返回底层指针
	int getPtrPointer() const{
		return *pointer;
	}

	// 返回引用计数
	int getPtrCounter() const{
		return *counter;
	}

	// 处理父类子类的情况， ptr<derived>不能访问 ptr<based>的内部对象
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

	// 处理内部使用 dynamic_cast做判断的转换，失败则空指针
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