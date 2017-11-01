#include "stdafx.h"
#include "SimpleFactory.h"

CSimpleFactoryBase::~CSimpleFactoryBase()
{
	printf("%s\n",__FUNCTION__);
}

CSimpleFactoryDerived1::CSimpleFactoryDerived1()
{
	printf("%s\n",__FUNCTION__);
}

CSimpleFactoryDerived1::~CSimpleFactoryDerived1()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived1::fun1()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived1::fun2()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived1::fun3()
{
	printf("%s\n",__FUNCTION__);
}

CSimpleFactoryDerived2::CSimpleFactoryDerived2()
{
	printf("%s\n", __FUNCTION__);
}

CSimpleFactoryDerived2::~CSimpleFactoryDerived2()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived2::fun1()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived2::fun2()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived2::fun3()
{
	printf("%s\n",__FUNCTION__);
}

CSimpleFactoryDerived3::CSimpleFactoryDerived3()
{
	printf("%s\n",__FUNCTION__);
}

CSimpleFactoryDerived3::~CSimpleFactoryDerived3()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived3::fun1()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived3::fun2()
{
	printf("%s\n",__FUNCTION__);
}

void CSimpleFactoryDerived3::fun3()
{
	printf("%s\n",__FUNCTION__);
}

CSimpleFactoryInstance::CSimpleFactoryInstance():pFactoryBase(nullptr)
{
	printf("%s\n",__FUNCTION__);
}

CSimpleFactoryInstance::~CSimpleFactoryInstance()
{
	printf("%s\n",__FUNCTION__);
	if (pFactoryBase)
	{
		delete pFactoryBase;
		pFactoryBase = nullptr;
	}
}

void CSimpleFactoryInstance::createInstance(int Type)
{
	if (1 == Type)
	{
		pFactoryBase = new CSimpleFactoryDerived1;
	}
	else if (2 == Type)
	{
		pFactoryBase = new CSimpleFactoryDerived2;
	}
	else if (3 == Type)
	{
		pFactoryBase = new CSimpleFactoryDerived3;
	}
}

void CSimpleFactoryInstance::fun1()
{
	pFactoryBase->fun1();
}

void CSimpleFactoryInstance::fun2()
{
	pFactoryBase->fun2();
}