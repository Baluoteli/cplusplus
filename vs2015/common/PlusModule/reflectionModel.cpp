#include "stdafx.h"
#include "reflectionModel.h"

#define REGISTER_CLASS(className)\
class CRegister##className{\
public:\
	static void* newInstance() {\
	return new className;}\
private:\
	static const CRegisterClassReflection reg; \
}; \
const CRegisterClassReflection CRegister##className::reg(#className,CRegister##className::newInstance);

CBase::~CBase()
{
	printf("%s\n",__FUNCTION__);
}

std::map<std::string, pFunDerived> CDerivedInstanceMgr::mapClassInstance_;
REGISTER_CLASS(CDerived1)

void* CDerivedInstanceMgr::getDerivedInstance(const std::string &className)
{
	std::map<std::string, pFunDerived>::iterator it = CDerivedInstanceMgr::mapClassInstance_.begin();
	if (CDerivedInstanceMgr::mapClassInstance_.end() != it)
	{
		return (it->second)();
	}

	printf("!!!!!Error . Cloud't find DerivedInstance of className : %s\n",className.c_str());
	return nullptr;
}

void CDerivedInstanceMgr::registerClass(const std::string &className,pFunDerived pFun)
{
	std::map<std::string, pFunDerived>::iterator it = CDerivedInstanceMgr::mapClassInstance_.end();
	if (CDerivedInstanceMgr::mapClassInstance_.end() == (it = CDerivedInstanceMgr::mapClassInstance_.find(className)))
	{
		CDerivedInstanceMgr::mapClassInstance_[className] = pFun;
		return;
	}

	printf("!!!!Error. registerClass Name: %s Already exist...\n",className.c_str());
}

CRegisterClassReflection::CRegisterClassReflection(const std::string &className, pFunDerived pfun)
{
	CDerivedInstanceMgr::registerClass(className, pfun);
}

//派生实例对象.
CDerived1::CDerived1()
{
}

CDerived1::~CDerived1()
{
	printf("%s\n",__FUNCTION__);
}

void CDerived1::fun1()
{
	printf("%s\n",__FUNCTION__);
}

void CDerived1::fun2()
{
	printf("%s\n",__FUNCTION__);
}

void CDerived1::fun3()
{
	printf("%s\n",__FUNCTION__);
}

CDerived2::CDerived2()
{

}

CDerived2::~CDerived2()
{
	printf("%s\n",__FUNCTION__);
}

void CDerived2::fun1()
{
	printf("%s\n",__FUNCTION__);
}

void CDerived2::fun2()
{
	printf("%s\n", __FUNCTION__);
}

void CDerived2::fun3()
{
	printf("%s\n", __FUNCTION__);
}

//实例化.
CInstance::CInstance():pBaseObserver_(nullptr)
{

}

CInstance::CInstance(const std::string &className)
{
	pBaseObserver_ = (CBase*)CDerivedInstanceMgr::getDerivedInstance(className);
	if (nullptr == pBaseObserver_) {
		printf("Instance failed... className: %s\n",className.c_str());
	}
}

CInstance::~CInstance()
{
	if (pBaseObserver_)
	{
		delete pBaseObserver_;
		pBaseObserver_ = nullptr;
	}
}

void CInstance::fun1()
{
	if (pBaseObserver_)
	{
		pBaseObserver_->fun1();
	}
}
void CInstance::fun2()
{
	if (pBaseObserver_)
	{
		pBaseObserver_->fun2();
	}
}

void CInstance::fun3()
{
	if (pBaseObserver_)
	{
		pBaseObserver_->fun3();
	}
}