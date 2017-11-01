#ifndef __FACTORYMODEL_H__
#define __FACTORYMODEL_H__

class ICapture
{
public:
	virtual ~ICapture() = 0;

	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
};

ICapture::~ICapture()
{
	printf("%s\n",__FUNCTION__);
}

typedef void* (*pFun)();

class CInstanceMgr
{
public:
	static void* getInstance(std::string classname)
	{
		std::map<std::string, pFun>::iterator it = mapClassFun_.find(classname);
		if (mapClassFun_.end() != it)
		{
			return (it->second)();//每个实例只用容许调用一次.
		}

		return nullptr;
	}

	static void registerClass(const std::string &className, pFun classFun)
	{
		std::map<std::string, pFun>::iterator it = mapClassFun_.end();
		if (mapClassFun_.end() ==  (it = mapClassFun_.find(className)))
		{
			mapClassFun_[className] = classFun;
		}
	}

private:
	static std::map<std::string, pFun > mapClassFun_;
};
std::map<std::string, pFun> CInstanceMgr::mapClassFun_;

class CRegisterClass
{
public:
	CRegisterClass(const std::string &className, pFun InstanceFun)	
	{
		CInstanceMgr::registerClass(className, InstanceFun);
	}
};

#define REGISTER_CLASS(className)\
class register_##className\
{\
public:\
static void* newInstance()\
{\
return new className;\
}\
private:\
static const CRegisterClass reg##className;\
};\
const CRegisterClass register_##className::reg##className(#className,register_##className::newInstance);


class CCaptureDerived1:public ICapture
{
public:
	CCaptureDerived1() { ; }
	~CCaptureDerived1() { printf("%s\n",__FUNCTION__); }

protected:
	virtual void start()
	{
		printf("%s\n", __FUNCTION__);
	}
	virtual void stop()
	{
		printf("%s\n", __FUNCTION__);
	}
	virtual void pause()
	{
		printf("%s\n", __FUNCTION__);
	}

private:
};

REGISTER_CLASS(CCaptureDerived1)


class CCaptureDerived2 :public ICapture
{
public:
	CCaptureDerived2() { ; }
	~CCaptureDerived2() { printf("%s\n",__FUNCTION__); }

protected:
	virtual void start()
	{
		printf("%s\n", __FUNCTION__);
	}
	virtual void stop()
	{
		printf("%s\n", __FUNCTION__);
	}
	virtual void pause()
	{
		printf("%s\n", __FUNCTION__);
	}

private:
};

REGISTER_CLASS(CCaptureDerived2)


class CCaptureDerived3 :public ICapture
{
public:
	CCaptureDerived3() { ; }
	~CCaptureDerived3() { printf("%s\n",__FUNCTION__); }

protected:
	virtual void start()
	{
		printf("%s\n", __FUNCTION__);
	}
	virtual void stop()
	{
		printf("%s\n", __FUNCTION__);
	}
	virtual void pause()
	{
		printf("%s\n", __FUNCTION__);
	}

private:
};

REGISTER_CLASS(CCaptureDerived3)

class CCaptureInstance
{
public:
	CCaptureInstance(const std::string &captureName) :captureObserver_(nullptr)
	{
		captureObserver_ = (ICapture*)CInstanceMgr::getInstance(captureName);
		if (nullptr == captureObserver_)
		{
			printf("!!!!!!Error ,Cloud'not find the interface of captureName: %s\n",captureName.c_str());
		}
	}

	~CCaptureInstance()
	{
		if (captureObserver_)
		{
			delete captureObserver_;
			captureObserver_ = nullptr;
		}
	}

	ICapture* createCapture(const std::string &captureName)
	{
		return (captureObserver_ = (ICapture*)CInstanceMgr::getInstance(captureName));
	}

	void start()
	{
		if (captureObserver_)
		{
			captureObserver_->start();
		}
	}

	void stop()
	{
		if (captureObserver_)
		{
			captureObserver_->stop();
		}
	}

	void pause()
	{
		if (captureObserver_)
		{
			captureObserver_->pause();
		}
	}

private:
	ICapture* captureObserver_;
};

#endif
