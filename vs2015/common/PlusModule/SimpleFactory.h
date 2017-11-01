#ifndef __SIMPLEFACTORY_H__
#define __SIMPLEFACTORY_H__

class CSimpleFactoryBase
{
public:
	virtual ~CSimpleFactoryBase() = 0;
	virtual void fun1() = 0;
	virtual void fun2() = 0;
	virtual void fun3() = 0;
};

class CSimpleFactoryDerived1:public CSimpleFactoryBase
{
public:
	CSimpleFactoryDerived1();
	~CSimpleFactoryDerived1();

	virtual void fun1();
	virtual void fun2();
	virtual void fun3();
};

class CSimpleFactoryDerived2 :public CSimpleFactoryBase
{
public:
	CSimpleFactoryDerived2();
	~CSimpleFactoryDerived2();

	virtual void fun1();
	virtual void fun2();
	virtual void fun3();
};

class CSimpleFactoryDerived3 :public CSimpleFactoryBase
{
public:
	CSimpleFactoryDerived3();
	~CSimpleFactoryDerived3();

	virtual void fun1();
	virtual void fun2();
	virtual void fun3();
};

class CSimpleFactoryInstance
{
public:
	CSimpleFactoryInstance();
	~CSimpleFactoryInstance();

	void createInstance(int Type);
	void fun1();
	void fun2();	

private:
	CSimpleFactoryBase *pFactoryBase;
};

#endif
