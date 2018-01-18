#ifndef __PLAYER_HOOKER_H__
#define __PLAYER_HOOKER_H__

#ifdef HOOKER_PLAYER_EXPORTS
#define HOOKER_PLAYER_API __declspec(dllexport)
#else
#define HOOKER_PLAYER_API __declspec(dllimport)
#endif

#include <wtypes.h>
#include <mmreg.h>

class IAudioCaptureCallback
{
public:
	virtual void onCaptureStart() = 0;
	virtual void onCaptureStop() = 0;
	virtual void onCapturedData(void* data, UINT dataLen, WAVEFORMATEX* format) = 0;
};

class IPlayerHooker
{
public:
	virtual ~IPlayerHooker(){}

	virtual int startHook(TCHAR* playerPath) = 0;
	virtual void stopHook() = 0;
	virtual int startAudioCapture(IAudioCaptureCallback* callback) = 0;
	virtual void stopAudioCapture() = 0;
};


HOOKER_PLAYER_API IPlayerHooker* createPlayerHookerInstance();
HOOKER_PLAYER_API void destoryPlayerHookerInstance(IPlayerHooker* hooker);

#endif