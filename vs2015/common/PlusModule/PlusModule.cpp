// PlusModule.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "FactoryModel.h"
#include "reflectionModel.h"
#include "reflectionEx.h"
#include "SimpleFactory.h"

char* InstanceDesc[] = {
	"VGA","Tea1","Tea2","Tea3","Stu1","Stu2","Stu3"
};

void MyTest()
{
	CCaptureInstance Obj("CCaptureDerived");
	Obj.start();
	Obj.stop();
	Obj.pause();

	CCaptureInstance obj1("CCaptureDerived2");
	obj1.start();
	obj1.stop();
	obj1.pause();
}

void Reflection()
{
	CInstance obj("CDerived1");
	obj.fun1();
	obj.fun2();
	obj.fun3();

	CInstance obj1("CDerived2");
	obj1.fun1();
	obj1.fun2();
	obj1.fun3();
}

void reflectionEx()
{
// 	CFactoryImpMgr::getInstance()->fun1(KFactoryDerived,InstanceVGA);
// 	CFactoryImpMgr::getInstance()->fun2(KFactoryDerived,InstanceTea1);
// 	CFactoryImpMgr::getInstance()->fun2(KFactoryDerived, InstanceTea2);

	//delete CFactoryImpMgr::getInstance();
	//delete CFactoryClassImp::getInstance();

	CSingleton<CFactoryImpMgr>::getInstance()->fun1(KFactoryDerived, InstanceVGA);
	CSingleton<CFactoryImpMgr>::getInstance()->fun2(KFactoryDerived1, InstanceTea1);
 	CSingleton<CFactoryImpMgr>::getInstance()->fun2(KFactoryDerived, InstanceTea2);
}

void singleton()
{
	CSingleton<CSingletonInstance>::getInstance()->fun1();
}

void simpleFactory()
{
	CSimpleFactoryInstance factoryInstance;
	factoryInstance.createInstance(1);
}

int main()
{
	simpleFactory();
	system("pause");
    return 0;
}

