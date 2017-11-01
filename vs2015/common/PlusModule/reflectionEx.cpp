#include "stdafx.h"
#include "reflectionEx.h"


#define REGISTERCLASS(className)\
void* newInstance##className(const std::string &InstanceDesc){\
return new className(InstanceDesc);}\
CRegisterFactoryClass reg##className(#className,newInstance##className);

REGISTERCLASS(CFactoryDerived);
REGISTERCLASS(CFactoryDerived1);
REGISTERCLASS(CFactoryDerived2);

CFactoryClassImp::~CFactoryClassImp()
{
	printf("%s\n",__FUNCTION__);
}

CSingleton<CFactoryClassImp>::CGrabo CFactoryClassImp::graboInstanceImp_;

void* CFactoryClassImp::getInstance(const std::string &className,const std::string &InstanceDesc)
{
	auto it = mapReflection_.find(className);
	if (mapReflection_.end() != it)
	{
		return (it->second)(InstanceDesc);
	}
	return nullptr;
}

void CFactoryClassImp::registerClass(const std::string &className, pReflectionFun pfun)
{
	mapReflection_[className] = pfun;
	
	printf("%s: %s\n",__FUNCTION__,className.c_str());
}

CFactoryClassImp::CFactoryClassImp()
{
	printf("%s\n",__FUNCTION__);
}

CRegisterFactoryClass::CRegisterFactoryClass(const std::string &className, pReflectionFun pfun)
{
	CSingleton<CFactoryClassImp>::getInstance()->registerClass(className,pfun);
}

CFactoryBase::~CFactoryBase()
{
	printf("%s\n",__FUNCTION__);
}

CFactoryDerived::CFactoryDerived(const std::string &InstanceDesc)
{
	InstanDesc_ = InstanceDesc;
	printf("%s %s\n",__FUNCTION__,InstanceDesc.c_str());
}

CFactoryDerived::~CFactoryDerived()
{
	printf("%s %s\n", __FUNCTION__, InstanDesc_.c_str());
}

void CFactoryDerived::fun1()
{
	printf("%s\n", __FUNCTION__);
}

void CFactoryDerived::fun2()
{
	printf("%s\n", __FUNCTION__);
}

void CFactoryDerived::fun3()
{
	printf("%s\n", __FUNCTION__);
}

CFactoryDerived1::CFactoryDerived1(const std::string &InstanceDesc)
{
	InstanDesc_ = InstanceDesc;
	printf("%s %s\n",__FUNCTION__,InstanceDesc.c_str());
}

CFactoryDerived1::~CFactoryDerived1()
{
	printf("%s %s\n", __FUNCTION__, InstanDesc_.c_str());
}

void CFactoryDerived1::fun1()
{
	printf("%s\n",__FUNCTION__);
}

void CFactoryDerived1::fun2()
{
	printf("%s\n", __FUNCTION__);
}

void CFactoryDerived1::fun3()
{
	printf("%s\n",__FUNCTION__);
}

CFactoryDerived2::CFactoryDerived2(const std::string &InstanceDesc)
{
	InstanDesc_ = InstanceDesc;
	printf("%s %s\n", __FUNCTION__,InstanceDesc.c_str());
}

CFactoryDerived2::~CFactoryDerived2()
{
	printf("%s %s\n", __FUNCTION__,InstanDesc_.c_str());
}

void CFactoryDerived2::fun1()
{
	printf("%s\n", __FUNCTION__);
}

void CFactoryDerived2::fun2()
{
	printf("%s\n", __FUNCTION__);
}

void CFactoryDerived2::fun3()
{
	printf("%s\n", __FUNCTION__);
}

CSingleton<CFactoryImpMgr>::CGrabo CFactoryImpMgr::graboMgr;
CFactoryImpMgr::CFactoryImpMgr()
{
	printf("%s\n",__FUNCTION__);
}

CFactoryImpMgr::~CFactoryImpMgr()
{
	std::map<std::string, CFactoryBase*>::iterator it = mapFactoryMgr_.begin();
	for (; mapFactoryMgr_.end() != it; it++)
	{
		delete it->second;
		it->second = nullptr;
	}

	mapFactoryMgr_.clear();

	printf("%s\n",__FUNCTION__);
}

void CFactoryImpMgr::fun1(const std::string &className, const std::string &InstanceDesc)
{
	std::map<std::string, CFactoryBase*>::iterator it = mapFactoryMgr_.find(InstanceDesc);
	if (mapFactoryMgr_.end() != it)
	{
		return it->second->fun1();
	}

	CFactoryBase* pBaseTemp = (CFactoryBase*)CSingleton<CFactoryClassImp>::getInstance()->getInstance(className,InstanceDesc);
	pBaseTemp->fun1();
	mapFactoryMgr_[InstanceDesc] = pBaseTemp;
}

void CFactoryImpMgr::fun2(const std::string &className, const std::string &InstanceDesc)
{
	std::map<std::string, CFactoryBase*>::iterator it = mapFactoryMgr_.find(InstanceDesc);
	if (mapFactoryMgr_.end() != it)
	{
		return it->second->fun2();
	}

	CFactoryBase* pBaseTemp = (CFactoryBase*)CSingleton<CFactoryClassImp>::getInstance()->getInstance(className,InstanceDesc);
	pBaseTemp->fun2();
	mapFactoryMgr_[InstanceDesc] = pBaseTemp;
}

void CFactoryImpMgr::fun3(const std::string &className, const std::string &InstanceDesc)
{
	std::map<std::string, CFactoryBase*>::iterator it = mapFactoryMgr_.find(InstanceDesc);
	if (mapFactoryMgr_.end() != it)
	{
		return it->second->fun3();
	}

	CFactoryBase* pBaseTemp = (CFactoryBase*)CSingleton<CFactoryClassImp>::getInstance()->getInstance(className,InstanceDesc);
	pBaseTemp->fun3();
	mapFactoryMgr_[InstanceDesc] = pBaseTemp;
}