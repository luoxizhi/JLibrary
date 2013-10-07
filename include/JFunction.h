// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JFunction_H
#define JFunction_H

/************************************************************************/
/* Null Parameter Function                                              */
/************************************************************************/
class JNullArguBase
{
public:
	virtual ~JNullArguBase(){}
	virtual void CallFunction() = 0;
};

template<typename T> 
class JNullArguFunctionBase : public JNullArguBase
{
	typedef void (T::*PtrFunction)();
public:
	JNullArguFunctionBase(T* t, PtrFunction pFunc): m_object(t), m_func(pFunc){}
	~JNullArguFunctionBase(){}

	void CallFunction(){
		if( m_object && m_func ){
			(m_object->*m_func)();
		}
	}

private:
	T* m_object;
	PtrFunction m_func;
};

class JNullArguFunction
{
public:
	JNullArguFunction(): m_handle(NULL){}
	~JNullArguFunction(){
		Detach();
	}

public:
	template<typename T> 
	void Attach(T* t, void (T::*pFun)()){ 
		Detach();
		try
		{
			m_handle = new JNullArguFunctionBase<T>(t, pFun);
		}
		catch(...)
		{
			Detach();
		}
	}
	void Detach(){ 
		if( m_handle ){
			try
			{
				delete m_handle;
			}
			catch(...){}	
			m_handle = NULL;
		}		
	} 

	void operator()(){
		if( m_handle ){
			m_handle->CallFunction();
		}
	}

	bool operator !() const { return m_handle != NULL; }
	operator void *() const { return m_handle; }

private:
	JNullArguBase *m_handle;
};

/************************************************************************/
/* One Parameter Function                                               */
/************************************************************************/
template <typename Param1>
class JOneArguBase
{
public:
	virtual void CallFunction(Param1 param1) = 0;
};

template<typename T, typename Param1> 
class JOneArguFunctionBase : public JOneArguBase<Param1>
{
	typedef void (T::*PtrFunction)(Param1);
public:
	JOneArguFunctionBase(T* t, PtrFunction pFunc): m_object(t), m_func(pFunc){}
	~JOneArguFunctionBase(){}

	void CallFunction(Param1 param1){
		if( m_object && m_func ){
			(m_object->*m_func)(param1);
		}
	}

private:
	T* m_object;
	PtrFunction m_func;
};

template <typename Param1>
class JOneArguFunction
{
public:
	JOneArguFunction(): m_handle(NULL){}
	~JOneArguFunction(){
		Detach();
	}

public:
	template<typename T> 
	void Attach(T* t, void (T::*pFun)(Param1)){ 
		Detach();
		try
		{
			m_handle = new JOneArguFunctionBase<T, Param1>(t, pFun);
		}
		catch(...)
		{
			Detach();
		}
	}
	void Detach(){ 
		if( m_handle ){
			try
			{
				delete m_handle;
			}
			catch(...){}	
			m_handle = NULL;
		}		
	} 

	void operator()(Param1 param1){
		if( m_handle ){
			m_handle->CallFunction(param1);
		}
	}

	bool operator !() const { return m_handle != NULL; }
	operator void *() const { return m_handle; }

private:
	JOneArguBase<Param1> *m_handle;
};

/************************************************************************/
/* Two Parameters Function                                              */
/************************************************************************/
template < typename Param1, typename Param2 >
class JTwoArguBase
{
public:
	virtual void CallFunction(Param1, Param2) = 0;
};

template<typename T, typename Param1, typename Param2> 
class JTwoArguFunctionBase : public JTwoArguBase <Param1, Param2>
{
	typedef void(T::*PtrFunction)(Param1, Param2);
public:
	JTwoArguFunctionBase(T* t, PtrFunction pFunc): m_object(t), m_func(pFunc){}
	~JTwoArguFunctionBase(){}

	void CallFunction(Param1 param1, Param2 param2){
		if( m_object && m_func ){
			(m_object->*m_func)(param1, param2);
		}
	}

private:
	T* m_object;
	PtrFunction m_func;
};

template <typename Param1, typename Param2>
class JTwoArguFunction
{
public:
	JTwoArguFunction(): m_handle(NULL){}
	~JTwoArguFunction(){
		Detach();
	}

public:
	template<typename T> 
	void Attach(T* t, void (T::*pFun)(Param1, Param2)){ 
		Detach();
		try
		{
			m_handle = new JTwoArguFunctionBase<T, Param1, Param2>(t, pFun);
		}
		catch(...)
		{
			Detach();
		}
	}
	void Detach(){ 
		if( m_handle ){
			try
			{
				delete m_handle;
			}
			catch(...){}	
			m_handle = NULL;
		}		
	} 

	void operator()(Param1 param1, Param2 param2){
		if( m_handle ){
			m_handle->CallFunction(param1, param2);
		}
	}

	bool operator !() const { return m_handle != NULL; }
	operator void *() const { return m_handle; }

private:
	JTwoArguBase<Param1, Param2> *m_handle;
};

/************************************************************************/
/* Three Parameters Function                                            */
/************************************************************************/
template <typename Param1, typename Param2, typename Param3>
class JThreeArguBase
{
public:
	virtual void CallFunction(Param1, Param2, Param3) = 0;
};

template<typename T, typename Param1, typename Param2, typename Param3> 
class JThreeArguFunctionBase : public JThreeArguBase<Param1, Param2, Param3>
{
	typedef void (T::*PtrFunction)(Param1, Param2, Param3);
public:
	JThreeArguFunctionBase(T* t, PtrFunction pFunc): m_object(t), m_func(pFunc){}
	~JThreeArguFunctionBase(){}

	void CallFunction(Param1 param1, Param2 param2, Param3 param3){
		if( m_object && m_func ){
			(m_object->*m_func)(param1, param2, param3);
		}
	}

private:
	T* m_object;
	PtrFunction m_func;
};

template <typename Param1, typename Param2, typename Param3>
class JThreeArguFunction
{
public:
	JThreeArguFunction(): m_handle(NULL){}
	~JThreeArguFunction(){
		Detach();
	}

public:
	template<typename T> 
	void Attach(T* t, void (T::*pFun)(Param1, Param2, Param3)){ 
		Detach();
		try
		{
			m_handle = new JThreeArguFunctionBase<T, Param1, Param2, Param3>(t, pFun);
		}
		catch(...)
		{
			Detach();
		}
	}
	void Detach(){ 
		if( m_handle ){
			try
			{
				delete m_handle;
			}
			catch(...){}	
			m_handle = NULL;
		}		
	} 

	void operator()(Param1 param1, Param2 param2, Param3 param3){
		if( m_handle ){
			m_handle->CallFunction(param1, param2, param3);
		}
	}

	bool operator !() const { return m_handle != NULL; }
	operator void *() const { return m_handle; }

private:
	JThreeArguBase<Param1, Param2, Param3> *m_handle;
};

/************************************************************************/
/* Four Parameters Function                                             */
/************************************************************************/
template <typename Param1, typename Param2, typename Param3, typename Param4>
class JFourArguBase
{
public:
	virtual void CallFunction(Param1, Param2, Param3, Param4) = 0;
};

template<typename T, typename Param1, typename Param2, typename Param3, typename Param4> 
class JFourArguFunctionBase : public JFourArguBase<Param1, Param2, Param3, Param4>
{
	typedef void (T::*PtrFunction)(Param1, Param2, Param3, Param4);
public:
	JFourArguFunctionBase(T* t, PtrFunction pFunc): m_object(t), m_func(pFunc){}
	~JFourArguFunctionBase(){}

	void CallFunction(Param1 param1, Param2 param2, Param3 param3, Param4 param4){
		if( m_object && m_func ){
			(m_object->*m_func)(param1, param2, param3, param4);
		}
	}

private:
	T* m_object;
	PtrFunction m_func;
};

template <typename Param1, typename Param2, typename Param3, typename Param4>
class JFourArguFunction
{
public:
	JFourArguFunction(): m_handle(NULL){}
	~JFourArguFunction(){
		Detach();
	}

public:
	template<typename T> 
	void Attach(T* t, void (T::*pFun)(Param1, Param2, Param3, Param4)){ 
		Detach();
		try
		{
			m_handle = new JFourArguFunctionBase<T, Param1, Param2, Param3, Param4>(t, pFun);
		}
		catch(...)
		{
			Detach();
		}
	}
	void Detach(){ 
		if( m_handle ){
			try
			{
				delete m_handle;
			}
			catch(...){}	
			m_handle = NULL;
		}		
	} 

	void operator()(Param1 param1, Param2 param2, Param3 param3, Param4 param4){
		if( m_handle ){
			m_handle->CallFunction(param1, param2, param3, param4);
		}
	}

	bool operator !() const { return m_handle != NULL; }
	operator void *() const { return m_handle; }

private:
	JFourArguBase<Param1, Param2, Param3, Param4> *m_handle;
};

/************************************************************************/
/* Five Parameters Function                                             */
/************************************************************************/
template <typename Param1, typename Param2, typename Param3, typename Param4, typename Param5>
class JFiveArguBase
{
public:
	virtual void CallFunction(Param1, Param2, Param3, Param4, Param5) = 0;
};

template<typename T, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5> 
class JFiveArguFunctionBase : public JFiveArguBase<Param1, Param2, Param3, Param4, Param5>
{
	typedef void (T::*PtrFunction)(Param1, Param2, Param3, Param4, Param5);
public:
	JFiveArguFunctionBase(T* t, PtrFunction pFunc): m_object(t), m_func(pFunc){}
	~JFiveArguFunctionBase(){}

	void CallFunction(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5){
		if( m_object && m_func ){
			(m_object->*m_func)(param1, param2, param3, param4, param5);
		}
	}

private:
	T* m_object;
	PtrFunction m_func;
};

template <typename Param1, typename Param2, typename Param3, typename Param4, typename Param5>
class JFiveArguFunction
{
public:
	JFiveArguFunction(): m_handle(NULL){}
	~JFiveArguFunction(){}

public:
	template<typename T> 
	void Attach(T* t, void (T::*pFun)(Param1, Param2, Param3, Param4, Param5)){ 
		Detach();
		try
		{
			m_handle = new JFiveArguFunctionBase<T, Param1, Param2, Param3, Param4, Param5>(t, pFun);
		}
		catch(...)
		{
			Detach();
		}
	}
	void Detach(){ 
		if( m_handle ){
			try
			{
				delete m_handle;
			}
			catch(...){}	
			m_handle = NULL;
		}		
	} 

	void operator()(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5){
		if( m_handle ){
			m_handle->CallFunction(param1, param2, param3, param4, param5);
		}
	}

	bool operator !() const { return m_handle != NULL; }
	operator void *() const { return m_handle; }

private:
	JFiveArguBase<Param1, Param2, Param3, Param4, Param5> *m_handle;
};
#endif // JFunction_H