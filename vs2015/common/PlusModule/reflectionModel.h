#ifndef __REFLECTION_H__
#define __REFLECTION_H__

class CBase
{
public:
	virtual ~CBase() = 0;
	virtual void fun1() = 0;
	virtual void fun2() = 0;
	virtual void fun3() = 0;
};

typedef void* (*pFunDerived)();
class CDerivedInstanceMgr
{
public:
	static void* getDerivedInstance(const std::string &className);
	static void registerClass(const std::string &className, pFunDerived pFun);

private:
	static std::map<std::string, pFunDerived> mapClassInstance_;
};

class CRegisterClassReflection
{
public:
	CRegisterClassReflection(const std::string &className,pFunDerived pfun);
};

class CDerived1 :public CBase
{
public:
	CDerived1();
	~CDerived1();

protected:
	virtual void fun1();
	virtual void fun2();
	virtual void fun3();
};

class CDerived2 :public CBase
{
public:
	CDerived2();
	~CDerived2();

protected:
	virtual void fun1();
	virtual void fun2();
	virtual void fun3();
};

class CInstance
{
public:
	CInstance();
	CInstance(const std::string &className);
	~CInstance();

	void fun1();
	void fun2();
	void fun3();

private:
	CBase* pBaseObserver_;
};

#endif