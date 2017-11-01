#include "stdafx.h"
#include "Singleton.h"


CSingleton<CSingletonInstance>::CGrabo CSingletonInstance::graboInstance;

CSingletonInstance::CSingletonInstance()
{
	printf("%s\n",__FUNCTION__);
}

CSingletonInstance::~CSingletonInstance()
{
	printf("%s\n", __FUNCTION__);
}