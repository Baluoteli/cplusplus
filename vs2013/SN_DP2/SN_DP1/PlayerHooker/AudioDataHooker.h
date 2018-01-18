#ifndef _AUDIO_DATA_HOOKER_H_
#define _AUDIO_DATA_HOOKER_H_

#include "dsound.h"
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <MMSystem.h>

#include "tstring.h"
#include "SharedMem.h"
#include "APIHookLog.h"
#include "SyncObjs.h"
#include "AudioDataPool.h"
#include "Thread.h"
#include "IntervalThread.h"
#include "AudioChunk.h"
#include "ConvertPCM.h"
#include "HookDSoundBuffer.h"
#include "ResamplerEx.h"

using namespace std;

//winmm
typedef MMRESULT (WINAPI *PFN_WAVEOUTWRITE)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI *PFN_WAVEOUTCLOSE)(HWAVEOUT hwo);
typedef MMRESULT (WINAPI *PFN_WAVEOUTPAUSE)(HWAVEOUT hwo);
typedef MMRESULT (WINAPI *PFN_WAVEOUTRESTART)(HWAVEOUT hwo);
typedef MMRESULT (WINAPI *PFN_WAVEOUTRESET)(HWAVEOUT hwo);
typedef MMRESULT (WINAPI *PFN_WAVEOUTSETVOLUME)(HWAVEOUT hwo, DWORD dwVolume);
typedef MMRESULT (WINAPI *PFN_WAVEOUTOPEN)(LPHWAVEOUT phwo, UINT uDeviceID, LPWAVEFORMATEX pwfx, DWORD dwCallback,
	DWORD dwCallbackInstance, DWORD fdwOpen);

//dsound
typedef HRESULT (WINAPI *PFN_DIRECTSOUNDCREATE)(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
typedef HRESULT (WINAPI *PFN_DIRECTSOUNDCREATE8)(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter);

typedef HRESULT (WINAPI *PFN_DSOUNDCREATEBUFFER)(IDirectSound* pDirectSound, THIS_ LPCDSBUFFERDESC pcDSBufferDesc, 
											   LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter);

typedef HRESULT (WINAPI *PFN_DSOUNDBUFFERPLAY)(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwReserved1, 
											 DWORD dwPriority, DWORD dwFlags);

typedef ULONG (WINAPI *PFN_DSOUNDBUFFERRELEASE)(IDirectSoundBuffer* pDSoundBuffer);

typedef HRESULT (WINAPI *PFN_DSOUNDBUFFERSETCURRENTPOSITION)(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwNewPosition);

typedef HRESULT (WINAPI *PFN_DSOUNDBUFFERSETVOLUME)(IDirectSoundBuffer* pDSoundBuffer, THIS_ LONG lVolume);

typedef HRESULT (WINAPI *PFN_DSOUNDBUFFERSTOP)(IDirectSoundBuffer* pDSoundBuffer);

typedef HRESULT (WINAPI *PFN_DSOUNDBUFFERLOCK)(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwOffset, DWORD dwBytes, 
											 LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
//dsound8
typedef HRESULT (WINAPI *PFN_DSOUND8CREATEBUFFER)(IDirectSound8* pDirectSound8, THIS_ LPCDSBUFFERDESC pcDSBufferDesc, 
												LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter);

static const DWORD dwNOTIFY_SIZE = 8192;
//static const DWORD dwNOTIFY_SIZE = 4096;
static const DWORD dwSHARE_MEM_SIZE = dwNOTIFY_SIZE * 2 + 4096;
static const DWORD dwHOOK_AUDIO_DATA_SAMPLERATE = 44100;
static const DWORD dwHOOK_AUDIO_DATA_CHANNEL = 2;
static const DWORD dwHOOK_AUDIO_DATA_BPS = 16;
static const DWORD dwHOOK_AUDIO_DATA_SAMPLESIZE = (dwHOOK_AUDIO_DATA_BPS / 8) * dwHOOK_AUDIO_DATA_CHANNEL;
static const DWORD dwHOOK_AUDIO_DATA_NOTIFY_SAMPLECOUNT = 2048;
static const DWORD dwHOOK_AUDIO_DATA_AVGBYTESPERSEC = 176400;
static const TCHAR* pszSHARE_MAP_FILE_NAME = _T("_YDQ_CAUDIODATAHOOKER_SHARE_MAP_FILE_NAME_");

static const TCHAR* pszHOOK_PROCESS_PATH_SECTION_NAME          = _T("YDQ_HOOK_PROCESS_PATH");
static const TCHAR* pszHOOK_PROCESS_AUDIO_DATA_SECTION_NAME    = _T("YDQ_HOOK_PROCESS_AUDIO_DATA");
static const TCHAR* pszHOOK_PROCESS_COMMAND_SECTION_NAME       = _T("YDQ_HOOK_PROCESS_COMMAND");
static const TCHAR* pszHOOK_PROCESS_INSTALL_COUNT_SECTION_NAME = _T("YDQ_HOOK_PROCESS_INSTALL_COUNT");
static const TCHAR* pszHOOK_PROCESS_START_SECTION_NAME         = _T("YDQ_HOOK_PROCESS_START");

static const DWORD dwHOOK_AUDIO_DATA_WAIT    = 0x00000000;
static const DWORD dwHOOK_AUDIO_DATA_NEED    = 0x00000001;
static const DWORD dwHOOK_AUDIO_DATA_EMPTY   = 0x00000010;
static const DWORD dwHOOK_AUDIO_DATA_UNKNOWN = 0xFFFFFFFF;

typedef map<HWAVEOUT, CWaveOutAudioDataPool*> CHookWaveOutMap;
typedef vector<CAudioDataPool*> CHookDataPoolVector;
typedef map<void*, CHookDSoundBuffer*> CHookDSounBufferMap;

class CAudioDataHooker: public IIntervalThreadNotify
{
public:
	static CAudioDataHooker* Instance();

	virtual BOOL StartWork(const TCHAR* pHookProcessPath, HINSTANCE hModule);
	virtual void StopWork();
	virtual void SetHookCertainProcess(const TCHAR* pHookProcessPath);

	CAudioDataPool* CreateAudioDataPool(int bufferSize);
	void DeleteHookDataPool(CAudioDataPool* pHookDataPool);

	static CAPIHookLog ms_log;

	static bool CanHandle(int sampleRate, int channel);

protected:
	void OnIntervalExecute();
	void OnThreadTerminate();

private:
	CAudioDataHooker();
	~CAudioDataHooker();

	static HRESULT WINAPI HookDirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
	static HRESULT WINAPI HookDirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter);
	static MMRESULT WINAPI HookWaveOutClose(HWAVEOUT hwo);
	static MMRESULT WINAPI HookWaveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
	static MMRESULT WINAPI HookWaveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID, LPWAVEFORMATEX pwfx, DWORD dwCallback, 
		DWORD dwCallbackInstance, DWORD fdwOpen);
	static MMRESULT WINAPI HookWaveOutPause(HWAVEOUT hwo);
	static MMRESULT WINAPI HookWaveOutRestart(HWAVEOUT hwo);
	static MMRESULT WINAPI HookWaveOutReset(HWAVEOUT hwo);
	static MMRESULT WINAPI HookWaveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);

	bool ResampleAudioChunk(CAudioChunk& srcChunk, CAudioChunk& dstChunk, CAudioDataPool* pAudioDataPool);
	void Hook();
	void UnHook();

	PFN_WAVEOUTOPEN m_origWaveOutOpen;
	PFN_WAVEOUTCLOSE m_origWaveOutClose;
	PFN_WAVEOUTWRITE m_origWaveOutWrite;
	PFN_WAVEOUTPAUSE m_origWaveOutPause;
	PFN_WAVEOUTRESTART m_origWaveOutRestart;
	PFN_WAVEOUTRESET m_origWaveOutReset;
	PFN_WAVEOUTSETVOLUME m_origWaveOutSetVolume;

	PFN_DIRECTSOUNDCREATE m_origDirectSoundCreate;
	PFN_DSOUNDCREATEBUFFER m_origDsoundCreateBuffer;

	PFN_DIRECTSOUNDCREATE8 m_origDirectSoundCreate8;
	PFN_DSOUND8CREATEBUFFER m_origDsound8CreateBuffer;

	PFN_DSOUNDBUFFERLOCK m_origDsoundBufferLock;
	PFN_DSOUNDBUFFERPLAY m_origDsoundBufferPlay;
	PFN_DSOUNDBUFFERRELEASE m_origDsoundBufferRelease;
	PFN_DSOUNDBUFFERSETCURRENTPOSITION m_origDsoundBufferSetCurrentPosition;
	PFN_DSOUNDBUFFERSETVOLUME m_origDsoundBufferSetVolume;
	PFN_DSOUNDBUFFERSTOP m_origDsoundBufferStop;

	//dsound
	static HRESULT _stdcall HookDSoundCreateBuffer(IDirectSound* pDirectSound, THIS_ LPCDSBUFFERDESC pcDSBufferDesc, 
		LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter);

	static HRESULT _stdcall HookDSoundBufferPlay(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwReserved1, 
		DWORD dwPriority, DWORD dwFlags);

	static ULONG _stdcall HookDSoundBufferRelease(IDirectSoundBuffer* pDSoundBuffer);

	static HRESULT _stdcall HookDSoundBufferSetCurrentPosition(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwNewPosition);

	static HRESULT _stdcall HookDSoundBufferSetVolume(IDirectSoundBuffer* pDSoundBuffer, THIS_ LONG lVolume);

	static HRESULT _stdcall HookDSoundBufferStop(IDirectSoundBuffer* pDSoundBuffer);

	static HRESULT _stdcall HookDSoundBufferLock(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwOffset, DWORD dwBytes, 
		LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);

	//dsound8
	static HRESULT _stdcall HookDSound8CreateBuffer(IDirectSound8* pDirectSound8, THIS_ LPCDSBUFFERDESC pcDSBufferDesc, 
		LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter);

	CSharedMem m_sharedMem;
	CLock m_lock;
	CLock m_createLock;
	static CLock ms_lockHookDataPools;
	static CHookDataPoolVector ms_hookDataPools;
	static CHookWaveOutMap ms_hookWaveOuts;
	bool m_hook;
	char* m_pNotifyBuffer;
	CIntervalThread* m_pIntervalThread;
	IResamplerEx* m_pResamplerEx;
	int m_srcSampleRate;
	CHookDSounBufferMap m_hookDSounBuffers;
	HMODULE m_hookDll;
	HMODULE m_dsoundDll;
	HMODULE m_winmmDll;
};

#endif