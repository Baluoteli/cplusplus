#include "PlayerHookerImpl.h"
#include "Utils.h"
#include "HookAudioDll.h"
#include "HookAudioInput.h"
CPlayerHookerV6::CPlayerHookerV6()
	: mHaveHook(false)
	, mpAudioInput(NULL)
{

}

CPlayerHookerV6::~CPlayerHookerV6()
{
	stopAudioCapture();
	stopHook();
}

int CPlayerHookerV6::startHook(TCHAR* playerPath)
{
	if (IsProcessRunning(playerPath))
	{
		if (!isHooking())
		{
			Hook(playerPath);
			KillProcess(playerPath);
			StartupProcess(playerPath);
		}
	}
	else
	{
		Hook(playerPath);
		StartupProcess(playerPath);
	}

	hookexepath = playerPath;
	return 0;
}

void CPlayerHookerV6::stopHook()
{
	KillProcess(hookexepath);
	RemoveHookAudio();
}


bool CPlayerHookerV6::isHooking()
{
	return HaveHookAudioRunning();
}

int CPlayerHookerV6::startAudioCapture(IAudioCaptureCallback* callback)
{
	if (mpAudioInput == NULL)
	{
		mpAudioInput = new CHookAudioInput();
		mpAudioInput->Open(kCAPTURE_SAMPLE_RATE, kCAPTURE_CHANNEL, 16, kCAPTURE_FRAME_SIZE_IN_BYTE * 5, kCAPTURE_FRAME_SIZE_IN_BYTE);
	}
	if (mpAudioInput != NULL)
	{
		mpAudioInput->Start(callback);
	}
	return 0;
}

void CPlayerHookerV6::stopAudioCapture()
{
	if (mpAudioInput != NULL)
	{
		mpAudioInput->Stop();
		delete mpAudioInput;
		mpAudioInput = NULL;
	}

}

void CPlayerHookerV6::Hook(TCHAR* playerPath)
{
	InstallHookAudio(playerPath);
}


HOOKER_PLAYER_API IPlayerHooker* createPlayerHookerInstance() {
	return new CPlayerHookerV6();
}

HOOKER_PLAYER_API void destoryPlayerHookerInstance(IPlayerHooker* hooker) {
	delete hooker;
}