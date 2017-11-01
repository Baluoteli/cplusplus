#ifndef __REFLECTIONEX_H__
#define __REFLECTIONEX_H__

typedef void* (*pReflectionFun)(const std::string &InstanceDesc);

class CFactoryClassImp
{
public:
	~CFactoryClassImp();
	static CFactoryClassImp* getInstance();
	void* getInstance(const std::string &className,const std::string &InstanceDesc);
	void registerClass(const std::string &className,pReflectionFun pfun);

	struct CGrabo
	{
		~CGrabo()
		{
			CRITICAL_SECTION criticalSection;
			InitializeCriticalSection(&criticalSection);
			EnterCriticalSection(&criticalSection);

			if (pInstance_)
			{
				delete pInstance_;
				pInstance_ = nullptr;
			}

			LeaveCriticalSection(&criticalSection);
			DeleteCriticalSection(&criticalSection);
		}
	};
	static CGrabo graboInstanceImp_;

private:
	CFactoryClassImp();
	std::map<std::string, pReflectionFun> mapReflection_;
	static CFactoryClassImp* pInstance_;
};

class CRegisterFactoryClass
{
public:
	CRegisterFactoryClass(const std::string &className, pReflectionFun pfun);
};

class CFactoryBase
{
public:
	virtual ~CFactoryBase() = 0;

	virtual void fun1() = 0;
	virtual void fun2() = 0;
	virtual void fun3() = 0;

public:
	std::string InstanDesc_;
};

class CFactoryDerived:public CFactoryBase
{
public:;
	CFactoryDerived(const std::string &InstanceDesc);
	~CFactoryDerived();

	virtual void fun1();
	virtual void fun2();
	virtual void fun3();
};

class CFactoryDerived1 :public CFactoryBase
{
public:
	CFactoryDerived1(const std::string &InstanceDesc);
	~CFactoryDerived1();

	virtual void fun1();
	virtual void fun2();
	virtual void fun3();
};

class CFactoryDerived2:public CFactoryBase
{
public:
	CFactoryDerived2(const std::string &InstanceDesc);
	~CFactoryDerived2();

	virtual void fun1();
	virtual void fun2();
	virtual void fun3();
};

class CFactoryImpMgr
{
public:
	~CFactoryImpMgr();
	static CFactoryImpMgr* getInstance();

	struct CGrabo
	{
		~CGrabo()
		{
			CRITICAL_SECTION criticalSection;
			InitializeCriticalSection(&criticalSection);
			EnterCriticalSection(&criticalSection);
			if (pFactoryMgrInstance_)
			{
				delete pFactoryMgrInstance_;
				pFactoryMgrInstance_ = nullptr;
			}
			LeaveCriticalSection(&criticalSection);
			DeleteCriticalSection(&criticalSection);
		}
	};
	static CGrabo graboMgr;

	//Interface
	void fun1(const std::string &className,const std::string &InstanceDesc);
	void fun2(const std::string &className, const std::string &InstanceDesc);
	void fun3(const std::string &className, const std::string &InstanceDesc);

private:
	CFactoryImpMgr();
	static CFactoryImpMgr* pFactoryMgrInstance_;

	struct compType{
	public:
		bool operator()(const std::string& str1, const std::string& str2)const {
			return (str1 > str2);
		}
	};
	std::map<std::string,CFactoryBase*, compType> mapFactoryMgr_;
};

#endif