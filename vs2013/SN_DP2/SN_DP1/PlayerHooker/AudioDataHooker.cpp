#include "AudioDataHooker.h"
#include "HookAudioDll.h"
#include "Utils.h"
#include <mmreg.h>
#include "detours.h"

CAPIHookLog CAudioDataHooker::ms_log(_T("C:\\PlayerHookerV6.txt"));
CHookDataPoolVector CAudioDataHooker::ms_hookDataPools;
CLock CAudioDataHooker::ms_lockHookDataPools;
CHookWaveOutMap CAudioDataHooker::ms_hookWaveOuts;

static void Scale(AudioSample* ptr, double scale, unsigned count)
{
	for(; count; count--)
	{
		(*ptr) = (AudioSample)((*ptr) * scale);
		ptr++;
	}
}

inline void ScaleChunk(IAudioChunk* pAudioChunk, double dScale) 
{
	Scale(pAudioChunk->GetData(), dScale, pAudioChunk->GetDataLength());
}

static void* GetComInterfaceAddr(void* pClass, int funcIdx)
{
	LPVOID* lpVtbl = *(LPVOID**)(pClass);

	return (void*)(*(lpVtbl + funcIdx));
}

// CAudioDataHooker
						   
CAudioDataHooker::CAudioDataHooker(): m_hook(false), m_sharedMem(pszSHARE_MAP_FILE_NAME, dwSHARE_MEM_SIZE), 
	m_pIntervalThread(NULL), m_srcSampleRate(0), m_pNotifyBuffer(NULL), m_hookDll(NULL), m_dsoundDll(NULL),
	m_winmmDll(NULL), m_origWaveOutClose(NULL), m_origWaveOutOpen(NULL), m_origWaveOutPause(NULL), m_origWaveOutReset(NULL)
	, m_origWaveOutWrite(NULL), m_origWaveOutSetVolume(NULL), m_origWaveOutRestart(NULL)
	, m_origDsoundBufferPlay(NULL), m_origDsoundBufferRelease(NULL), m_origDsoundBufferSetCurrentPosition(NULL)
	, m_origDsoundBufferSetVolume(NULL), m_origDsoundBufferStop(NULL), m_origDsoundBufferLock(NULL)
	, m_origDirectSoundCreate(NULL), m_origDirectSoundCreate8(NULL), m_origDsound8CreateBuffer(NULL)
	, m_origDsoundCreateBuffer(NULL)
{
	CAudioDataHooker::ms_log.Trace(_T("CAudioDataHooker::CAudioDataHooker()\n"));
	if (m_sharedMem.IsCreated()) 
	{
		if (!m_sharedMem.ExistValue(pszHOOK_PROCESS_COMMAND_SECTION_NAME))
		{
			m_sharedMem.AddDwordValue(pszHOOK_PROCESS_COMMAND_SECTION_NAME, dwHOOK_AUDIO_DATA_UNKNOWN);
		}

		if (!m_sharedMem.ExistValue(pszHOOK_PROCESS_INSTALL_COUNT_SECTION_NAME))
		{
			m_sharedMem.AddDwordValue(pszHOOK_PROCESS_INSTALL_COUNT_SECTION_NAME, 0);
		}

		if (!m_sharedMem.ExistValue(pszHOOK_PROCESS_START_SECTION_NAME))
		{
			m_sharedMem.AddDwordValue(pszHOOK_PROCESS_START_SECTION_NAME, 0);
		}

		if (!m_sharedMem.ExistValue(pszHOOK_PROCESS_AUDIO_DATA_SECTION_NAME))
		{
			m_sharedMem.AddValue(pszHOOK_PROCESS_AUDIO_DATA_SECTION_NAME, dwNOTIFY_SIZE * 2);
		}

		if (!m_sharedMem.ExistValue(pszHOOK_PROCESS_PATH_SECTION_NAME))
		{
			m_sharedMem.AddValue(pszHOOK_PROCESS_PATH_SECTION_NAME, 255);
		}
	}
}

CAudioDataHooker::~CAudioDataHooker()
{
	CAudioDataHooker::ms_log.Trace(_T("CAudioDataHooker::~CAudioDataHooker()\n"));
	//to make sure thread quit
	StopWork();
}

void CAudioDataHooker::OnThreadTerminate()
{
	StopWork();
}

//统一输出的数据采样率44100, 32bit float, 2声道的数据
void CAudioDataHooker::OnIntervalExecute()
{
	try
	{
		DWORD hookCommand = m_sharedMem.GetDwordValue(pszHOOK_PROCESS_COMMAND_SECTION_NAME, dwHOOK_AUDIO_DATA_UNKNOWN);
// 		CAudioDataHooker::ms_log.Trace(_T("CAudioDataHooker::OnIntervalExecute: %d, %d, %d\n"), hookCommand, 
// 			ms_hookDataPools.size(), GetTickCount());
		if (hookCommand == dwHOOK_AUDIO_DATA_NEED)
		{
			INSYNC(ms_lockHookDataPools);
			bool canSetVaule = false;
			UINT notifySize = 0;
			int audioVolume = 100;
			int channel = 2;
			int bps = 16;
			int sampleRate = 44100;
			//int sampleRate = 48000;
			bool float32bit = false;
			CAudioChunk audioChunk;
			bool process = false;

			CHookDataPoolVector::iterator itr = ms_hookDataPools.begin();
			//CAudioDataHooker::ms_log.Trace(_T("CAudioDataHooker::OnIntervalExecute: %d\n"), ms_hookDataPools.size());
			for (; itr != ms_hookDataPools.end(); )
			{
				CAudioDataPool* pAudioDataPool = (*itr);
				//CAudioDataHooker::ms_log.Trace(_T("pAudioDataPool->GetUsedSize: %d, %d\n"), pAudioDataPool->GetUsedSize(), notifySize);
				notifySize = pAudioDataPool->GetNotifySize();
				if (pAudioDataPool->GetUsedSize() >= notifySize && !process)
				{
					//only work end one
					if (pAudioDataPool->Read(m_pNotifyBuffer, notifySize) > 0)
					{
						audioVolume = pAudioDataPool->GetVolume();
						channel = pAudioDataPool->GetWaveFormatEx().nChannels;
						bps = pAudioDataPool->GetWaveFormatEx().wBitsPerSample;
						sampleRate = pAudioDataPool->GetWaveFormatEx().nSamplesPerSec;
						float32bit = pAudioDataPool->GetWaveFormatEx().wFormatTag == WAVE_FORMAT_IEEE_FLOAT;

						if (pAudioDataPool->GetWaveFormatEx().wFormatTag == WAVE_FORMAT_PCM ||
							pAudioDataPool->GetWaveFormatEx().wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
						{
							canSetVaule = true;
						}
// 						CAudioDataHooker::ms_log.Trace(_T("pAudioDataPool Succ Read: %d, %d, %d, %d, %d\n"), canSetVaule,
// 							pAudioDataPool->GetWaveFormatEx().wFormatTag, channel, bps, sampleRate);
					}
				}
				else
				{
					notifySize = pAudioDataPool->GetNotifySize();
					pAudioDataPool->Read(NULL, notifySize);
				}

				if (canSetVaule && audioChunk.IsEmpty() && !process)
				{
					//转化为44100， 2
					if (sampleRate != dwHOOK_AUDIO_DATA_SAMPLERATE)
					{
						if (m_pResamplerEx != NULL)
						{
							if (m_srcSampleRate != sampleRate)
							{
								m_srcSampleRate = sampleRate;
								m_pResamplerEx->Init(channel, m_srcSampleRate, dwHOOK_AUDIO_DATA_SAMPLERATE, 
									dwHOOK_AUDIO_DATA_NOTIFY_SAMPLECOUNT);
							}
							CAudioChunk srcChunk;
							//CAudioDataHooker::ms_log.Trace(_T("srcChunk.SetData: %d\n"), notifySize);
							srcChunk.SetData(m_pNotifyBuffer, notifySize, sampleRate, channel, bps, float32bit);
							canSetVaule = ResampleAudioChunk(srcChunk, audioChunk, pAudioDataPool);
						}
						else
						{
							canSetVaule = false;
						}
					}
					else
					{
						audioChunk.SetData(m_pNotifyBuffer, notifySize, sampleRate, channel, bps, float32bit);
					}

					if (canSetVaule && channel != dwHOOK_AUDIO_DATA_CHANNEL)
					{
						ConvertMonoChunkToStereo(&audioChunk);
					}

					if (canSetVaule && audioVolume < 100)
					{
						double sacle = audioVolume / 100.0;
						ScaleChunk(&audioChunk, sacle); 
					}
					process = true;
				}

				if (pAudioDataPool->CanDestory())
				{
					CHookWaveOutMap::iterator itrHookWaveOut = ms_hookWaveOuts.begin();
					for (; itrHookWaveOut != ms_hookWaveOuts.end(); ++itrHookWaveOut)
					{
						if ((*itrHookWaveOut).second == pAudioDataPool)
						{
							ms_hookWaveOuts.erase(itrHookWaveOut);
							break;
						}
					}				

					itr = ms_hookDataPools.erase(itr);
					delete pAudioDataPool;
					//CAudioDataHooker::ms_log.Trace(_T("CAudioDataHooker::OnIntervalExecute Delete HookDataPool\n"));
				}
				else
				{
					++itr;
				}
			}

			if (canSetVaule)
			{
				if (audioChunk.GetDataSize() <= dwNOTIFY_SIZE * 2)
				{
					m_sharedMem.SetValue(pszHOOK_PROCESS_AUDIO_DATA_SECTION_NAME, audioChunk.GetData(), audioChunk.GetDataSize());
					//CAudioDataHooker::ms_log.Trace(_T("pAudioDataPool Share Data: %d\n"), audioChunk.GetDataSize());
				}
				else
				{
					CAudioDataHooker::ms_log.Trace(_T("m_sharedMem.SetValue bad: %d\n"), audioChunk.GetDataSize());
				}
			}
			else
			{
				m_sharedMem.SetDwordValue(pszHOOK_PROCESS_COMMAND_SECTION_NAME, dwHOOK_AUDIO_DATA_EMPTY);
				m_sharedMem.SetValue(pszHOOK_PROCESS_AUDIO_DATA_SECTION_NAME, m_pNotifyBuffer, 1);
				OutputDebugStringA("pszHOOK_PROCESS_COMMAND_SECTION_NAME dwHOOK_AUDIO_DATA_EMPTY\r\n");
			}
		}
		else if (hookCommand == dwHOOK_AUDIO_DATA_EMPTY)
		{
			m_sharedMem.SetValue(pszHOOK_PROCESS_AUDIO_DATA_SECTION_NAME, m_pNotifyBuffer, 1);
		}

		{
			INSYNC(m_lock);
			CHookDSounBufferMap::iterator itrHookDSounBuffer = m_hookDSounBuffers.begin();
			for (; itrHookDSounBuffer != m_hookDSounBuffers.end(); ++itrHookDSounBuffer)
			{
				itrHookDSounBuffer->second->OnTrigger();
			}
		}

		if (m_sharedMem.GetDwordValue(pszHOOK_PROCESS_START_SECTION_NAME, 0) % 2 == 0)
		{
			if (m_sharedMem.GetDwordValue(pszHOOK_PROCESS_INSTALL_COUNT_SECTION_NAME, 0) == 1)
			{
				m_pIntervalThread->Terminate();
			}
		}
	}
	catch (...)
	{
		CAudioDataHooker::ms_log.Trace(_T("CAudioDataHooker::OnIntervalExecute Error!\n"));
	}
}

bool CAudioDataHooker::ResampleAudioChunk(CAudioChunk& srcChunk, CAudioChunk& dstChunk, CAudioDataPool* pAudioDataPool)
{
	if (m_pResamplerEx->Process(&srcChunk, &dstChunk))
	{
		return true;
	}
	else
	{
		CAudioChunk tempChunk;
		int channel = pAudioDataPool->GetWaveFormatEx().nChannels;
		int bps = pAudioDataPool->GetWaveFormatEx().wBitsPerSample;
		int sampleRate = pAudioDataPool->GetWaveFormatEx().nSamplesPerSec;
		bool float32bit = pAudioDataPool->GetWaveFormatEx().wFormatTag == WAVE_FORMAT_IEEE_FLOAT;
		DWORD notifySize = pAudioDataPool->GetNotifySize() / 2;
		int temp = bps / 8 * channel;
		if (temp != 0)
		{
			notifySize -= notifySize % temp;
		}
		if (!pAudioDataPool->Read(m_pNotifyBuffer, notifySize))
		{
			return false;
		}

		tempChunk.SetData(m_pNotifyBuffer, notifySize, sampleRate, channel, bps, float32bit);
		if (m_pResamplerEx->Process(&tempChunk, &dstChunk))
		{
			return true;
		}
	}
	return false;
}

BOOL CAudioDataHooker::StartWork(const TCHAR* pHookProcessPath, HINSTANCE hModule)
{
	TCHAR buf[255];
	DWORD valueSize = 255;
	memset(buf, 0, sizeof(buf));
	if (!m_sharedMem.GetValue(pszHOOK_PROCESS_PATH_SECTION_NAME, buf, &valueSize))
	{
		return FALSE;
	}
	CAudioDataHooker::ms_log.Trace(_T("CAudioDataHooker::StartWork: [%s, %s]\n"), pHookProcessPath, buf);
	if (_tcsicmp(buf, pHookProcessPath) == 0 && !m_hook)
	{
		m_pNotifyBuffer = (char*)malloc(dwNOTIFY_SIZE * 2);

		Hook();

		m_pResamplerEx = CreateResamplerEx();

		m_pIntervalThread = new CIntervalThread(this, 20);
		DWORD installCount = m_sharedMem.GetDwordValue(pszHOOK_PROCESS_INSTALL_COUNT_SECTION_NAME, 0);
		installCount++;
		m_sharedMem.SetDwordValue(pszHOOK_PROCESS_INSTALL_COUNT_SECTION_NAME, installCount);

		//increase the reference count of the hookdll 
		TCHAR hookDllFilePath[MAX_PATH]; 
		::GetModuleFileName(hModule, hookDllFilePath, MAX_PATH);
		m_hookDll = LoadLibrary(hookDllFilePath);
		m_hook = true;

		return TRUE;
	}
	std::wstring hookPath(pHookProcessPath);
	std::wstring name = ExtractFileName(hookPath);
	if(name ==L"KwTE.exe")//
	{
		return FALSE;
	}
	return TRUE;
}

void CAudioDataHooker::StopWork()
{
	if (m_hook)
	{
		DWORD installCount = m_sharedMem.GetDwordValue(pszHOOK_PROCESS_INSTALL_COUNT_SECTION_NAME, 0);
		if (installCount > 0)
		{
			installCount--;
		}
		m_sharedMem.SetDwordValue(pszHOOK_PROCESS_INSTALL_COUNT_SECTION_NAME, installCount);

		//全部卸载完后，取消动态链接库映射
		if (installCount == 0)
		{
			if (m_pIntervalThread != NULL)
			{
				delete m_pIntervalThread;
				m_pIntervalThread = NULL;
				m_sharedMem.SetValue(pszHOOK_PROCESS_AUDIO_DATA_SECTION_NAME, m_pNotifyBuffer, 1);
			}

			if (m_pNotifyBuffer != NULL)
			{
				free(m_pNotifyBuffer);
				m_pNotifyBuffer = NULL;
			}

			if (m_pResamplerEx != NULL)
			{
				m_pResamplerEx->Destroy();
				m_pResamplerEx = NULL;
			}

			INSYNC(ms_lockHookDataPools);
			UnHook();
			m_hook = false;

			{
				INSYNC(m_lock);
				CHookDSounBufferMap::iterator itrHookDSounBuffer = m_hookDSounBuffers.begin();
				for (; itrHookDSounBuffer != m_hookDSounBuffers.end(); ++itrHookDSounBuffer)
				{
					delete itrHookDSounBuffer->second;
				}
				m_hookDSounBuffers.clear();
			}

			CHookDataPoolVector::iterator itr = ms_hookDataPools.begin();
			for (; itr != ms_hookDataPools.end(); ++itr)
			{
				CAudioDataPool* pHookDataPool = *itr;
				if (pHookDataPool != NULL)
				{
					delete pHookDataPool;
				}
			}
			ms_hookDataPools.clear();
			ms_hookWaveOuts.clear();

			if (m_hookDll != NULL)
			{
				FreeLibrary(m_hookDll);
				m_hookDll = NULL;
			}

			UninstallHook();
		}
	}
}

void CAudioDataHooker::SetHookCertainProcess(const TCHAR* pHookProcessPath)
{
	if (m_sharedMem.IsCreated()) 
	{
		m_sharedMem.SetValue(pszHOOK_PROCESS_PATH_SECTION_NAME, (void*)pHookProcessPath, 
			(DWORD)_tcsclen(pHookProcessPath) * sizeof(TCHAR));
	}
}

CAudioDataHooker* CAudioDataHooker::Instance()
{
	static CAudioDataHooker s_audioDataHooker;
	return &s_audioDataHooker;
}

CAudioDataPool* CAudioDataHooker::CreateAudioDataPool(int bufferSize)
{
	INSYNC(ms_lockHookDataPools);
	CAudioDataPool* pHookDataPool = new CAudioDataPool(bufferSize);
	ms_hookDataPools.push_back(pHookDataPool);
	return pHookDataPool;
}

void CAudioDataHooker::DeleteHookDataPool(CAudioDataPool* pHookDataPool)
{
	INSYNC(ms_lockHookDataPools);
	CHookWaveOutMap::iterator itrHookWaveOut = ms_hookWaveOuts.begin();
	for (; itrHookWaveOut != ms_hookWaveOuts.end(); ++itrHookWaveOut)
	{
		if ((*itrHookWaveOut).second == pHookDataPool)
		{
			ms_hookWaveOuts.erase(itrHookWaveOut);
			break;
		}
	}

	CHookDataPoolVector::iterator itr = find(ms_hookDataPools.begin(), ms_hookDataPools.end(), pHookDataPool);
	if (itr != ms_hookDataPools.end())
	{
		ms_hookDataPools.erase(itr);
		delete pHookDataPool;
	}
}

//dsound
HRESULT WINAPI CAudioDataHooker::HookDirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
	INSYNC(Instance()->m_createLock);
	try
	{
		CAudioDataHooker::ms_log.Trace(_T("HookDirectSoundCreate is Enter\n"));
		HRESULT hr = S_FALSE;
		hr = Instance()->m_origDirectSoundCreate(pcGuidDevice, ppDS, pUnkOuter);
		if (*ppDS != NULL)
		{
			if (Instance()->m_origDsoundCreateBuffer == NULL)
			{
				Instance()->m_origDsoundCreateBuffer = (PFN_DSOUNDCREATEBUFFER)GetComInterfaceAddr(*ppDS, 3); 

				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());	

				DetourAttach(&(PVOID&)Instance()->m_origDsoundCreateBuffer, &HookDSoundCreateBuffer);

				DetourTransactionCommit();
			}
			CAudioDataHooker::ms_log.Trace(_T("HookDirectSoundCreate: %d\n"), *ppDS);
		}
		CAudioDataHooker::ms_log.Trace(_T("HookDirectSoundCreate is Leave\n"));
		return hr;
	}
	catch (...)
	{
		return S_FALSE;
	}
}

HRESULT _stdcall CAudioDataHooker::HookDSoundCreateBuffer(IDirectSound* pDirectSound, THIS_ LPCDSBUFFERDESC pcDSBufferDesc, 
														  LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter)
{
	INSYNC(Instance()->m_lock);
	HRESULT hr = S_FALSE;

	hr = Instance()->m_origDsoundCreateBuffer(pDirectSound, pcDSBufferDesc, ppDSBuffer, pUnkOuter);

	if (*ppDSBuffer != NULL && pcDSBufferDesc->lpwfxFormat != NULL)
	{
		CHookDSoundBuffer* pHookDSoundBuffer = new CHookDSoundBuffer(*ppDSBuffer, 
			pcDSBufferDesc->dwBufferBytes, pcDSBufferDesc->lpwfxFormat);
		Instance()->m_hookDSounBuffers[*ppDSBuffer] = pHookDSoundBuffer;

		if (Instance()->m_origDsoundBufferPlay == NULL)
		{
			Instance()->m_origDsoundBufferPlay = (PFN_DSOUNDBUFFERPLAY)GetComInterfaceAddr(*ppDSBuffer, 12); 
			Instance()->m_origDsoundBufferRelease = (PFN_DSOUNDBUFFERRELEASE)GetComInterfaceAddr(*ppDSBuffer, 2); 
			Instance()->m_origDsoundBufferSetCurrentPosition = (PFN_DSOUNDBUFFERSETCURRENTPOSITION)GetComInterfaceAddr(*ppDSBuffer, 13); 
			Instance()->m_origDsoundBufferSetVolume = (PFN_DSOUNDBUFFERSETVOLUME)GetComInterfaceAddr(*ppDSBuffer, 15); 
			Instance()->m_origDsoundBufferStop = (PFN_DSOUNDBUFFERSTOP)GetComInterfaceAddr(*ppDSBuffer, 18); 
			Instance()->m_origDsoundBufferLock = (PFN_DSOUNDBUFFERLOCK)GetComInterfaceAddr(*ppDSBuffer, 11); 

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());	

			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferPlay, &HookDSoundBufferPlay);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferRelease, &HookDSoundBufferRelease);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferSetCurrentPosition, &HookDSoundBufferSetCurrentPosition);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferSetVolume, &HookDSoundBufferSetVolume);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferStop, &HookDSoundBufferStop);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferLock, &HookDSoundBufferLock);

			DetourTransactionCommit();
		}
	}

	return hr;
}

HRESULT _stdcall CAudioDataHooker::HookDSoundBufferPlay(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwReserved1, 
														DWORD dwPriority, DWORD dwFlags)
{
	INSYNC(Instance()->m_lock);
	CHookDSounBufferMap::iterator itrHookDSounBuffer = Instance()->m_hookDSounBuffers.find(pDSoundBuffer);
	if (itrHookDSounBuffer != Instance()->m_hookDSounBuffers.end())
	{
		itrHookDSounBuffer->second->Play();
	}

	HRESULT hr = Instance()->m_origDsoundBufferPlay(pDSoundBuffer, dwReserved1, dwPriority, dwFlags);
	return hr;
}

ULONG _stdcall CAudioDataHooker::HookDSoundBufferRelease(IDirectSoundBuffer* pDSoundBuffer)
{
	INSYNC(Instance()->m_lock);
	ULONG refCount = Instance()->m_origDsoundBufferRelease(pDSoundBuffer);

	if (refCount == 0)
	{
		CHookDSounBufferMap::iterator itrHookDSounBuffer = Instance()->m_hookDSounBuffers.find(pDSoundBuffer);
		if (itrHookDSounBuffer != Instance()->m_hookDSounBuffers.end())
		{
			delete itrHookDSounBuffer->second;
			Instance()->m_hookDSounBuffers.erase(itrHookDSounBuffer);
		}		
	}
	return refCount;
}

HRESULT _stdcall CAudioDataHooker::HookDSoundBufferSetCurrentPosition(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwNewPosition)
{
	INSYNC(Instance()->m_lock);
	CHookDSounBufferMap::iterator itrHookDSounBuffer = Instance()->m_hookDSounBuffers.find(pDSoundBuffer);
	if (itrHookDSounBuffer != Instance()->m_hookDSounBuffers.end())
	{
		itrHookDSounBuffer->second->SetCurrentPosition(dwNewPosition);
	}

	HRESULT hr = Instance()->m_origDsoundBufferSetCurrentPosition(pDSoundBuffer, dwNewPosition);
	return hr;
}

HRESULT _stdcall CAudioDataHooker::HookDSoundBufferSetVolume(IDirectSoundBuffer* pDSoundBuffer, THIS_ LONG lVolume)
{
	INSYNC(Instance()->m_lock);
	CHookDSounBufferMap::iterator itrHookDSounBuffer = Instance()->m_hookDSounBuffers.find(pDSoundBuffer);
	if (itrHookDSounBuffer != Instance()->m_hookDSounBuffers.end())
	{
		itrHookDSounBuffer->second->SetVolume(lVolume);
	}

	HRESULT hr = Instance()->m_origDsoundBufferSetVolume(pDSoundBuffer, lVolume);
	return hr;
}

HRESULT _stdcall CAudioDataHooker::HookDSoundBufferStop(IDirectSoundBuffer* pDSoundBuffer)
{
	INSYNC(Instance()->m_lock);
	CHookDSounBufferMap::iterator itrHookDSounBuffer = Instance()->m_hookDSounBuffers.find(pDSoundBuffer);
	if (itrHookDSounBuffer != Instance()->m_hookDSounBuffers.end())
	{
		itrHookDSounBuffer->second->Stop();
	}

	HRESULT hr = Instance()->m_origDsoundBufferStop(pDSoundBuffer);
	return hr;
}

HRESULT _stdcall CAudioDataHooker::HookDSoundBufferLock(IDirectSoundBuffer* pDSoundBuffer, THIS_ DWORD dwOffset, 
														DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, 
														LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)
{
	INSYNC(Instance()->m_lock);
	HRESULT hr = Instance()->m_origDsoundBufferLock(pDSoundBuffer, dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);

	CHookDSounBufferMap::iterator itrHookDSounBuffer = Instance()->m_hookDSounBuffers.find(pDSoundBuffer);
	if (itrHookDSounBuffer != Instance()->m_hookDSounBuffers.end())
	{
		itrHookDSounBuffer->second->CapturePlayData(true);
	}

	return hr;
}

//dsound8
HRESULT WINAPI CAudioDataHooker::HookDirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
{
	INSYNC(Instance()->m_lock);
	try
	{
		CAudioDataHooker::ms_log.Trace(_T("HookDirectSoundCreate8 is Enter\n"));
		HRESULT hr = S_FALSE;
		hr = Instance()->m_origDirectSoundCreate8(pcGuidDevice, ppDS8, pUnkOuter);
		if (*ppDS8 != NULL)
		{
			if (Instance()->m_origDsound8CreateBuffer == NULL)
			{
				Instance()->m_origDsound8CreateBuffer = (PFN_DSOUND8CREATEBUFFER)GetComInterfaceAddr(*ppDS8, 3); 

				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());	

				DetourAttach(&(PVOID&)Instance()->m_origDsound8CreateBuffer, &HookDSound8CreateBuffer);

				DetourTransactionCommit();
			}
		}
		CAudioDataHooker::ms_log.Trace(_T("HookDirectSoundCreate8 is Leave\n"));
		return hr;
	}
	catch (...)
	{
		return S_FALSE;
	}
}

HRESULT _stdcall CAudioDataHooker::HookDSound8CreateBuffer(IDirectSound8* pDirectSound8, THIS_ LPCDSBUFFERDESC pcDSBufferDesc, 
										 LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter)
{
	INSYNC(Instance()->m_lock);
	HRESULT hr = Instance()->m_origDsound8CreateBuffer(pDirectSound8, pcDSBufferDesc, ppDSBuffer, pUnkOuter);

	if (*ppDSBuffer != NULL && pcDSBufferDesc->lpwfxFormat != NULL)
	{
		CHookDSoundBuffer* pHookDSoundBuffer = new CHookDSoundBuffer(*ppDSBuffer, 
			pcDSBufferDesc->dwBufferBytes, pcDSBufferDesc->lpwfxFormat);
		Instance()->m_hookDSounBuffers[*ppDSBuffer] = pHookDSoundBuffer;

		if (Instance()->m_origDsoundBufferPlay == NULL)
		{
			Instance()->m_origDsoundBufferPlay = (PFN_DSOUNDBUFFERPLAY)GetComInterfaceAddr(*ppDSBuffer, 12); 
			Instance()->m_origDsoundBufferRelease = (PFN_DSOUNDBUFFERRELEASE)GetComInterfaceAddr(*ppDSBuffer, 2); 
			Instance()->m_origDsoundBufferSetCurrentPosition = (PFN_DSOUNDBUFFERSETCURRENTPOSITION)GetComInterfaceAddr(*ppDSBuffer, 13); 
			Instance()->m_origDsoundBufferSetVolume = (PFN_DSOUNDBUFFERSETVOLUME)GetComInterfaceAddr(*ppDSBuffer, 15); 
			Instance()->m_origDsoundBufferStop = (PFN_DSOUNDBUFFERSTOP)GetComInterfaceAddr(*ppDSBuffer, 18); 
			Instance()->m_origDsoundBufferLock = (PFN_DSOUNDBUFFERLOCK)GetComInterfaceAddr(*ppDSBuffer, 11); 

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());	

			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferPlay, &HookDSoundBufferPlay);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferRelease, &HookDSoundBufferRelease);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferSetCurrentPosition, &HookDSoundBufferSetCurrentPosition);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferSetVolume, &HookDSoundBufferSetVolume);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferStop, &HookDSoundBufferStop);
			DetourAttach(&(PVOID&)Instance()->m_origDsoundBufferLock, &HookDSoundBufferLock);

			DetourTransactionCommit();
		}
	}

	ATLTRACE2(_T("HookDSound8CreateBuffer: %d\n"), *ppDSBuffer);

	return hr;
}

MMRESULT WINAPI CAudioDataHooker::HookWaveOutClose(HWAVEOUT hwo)
{
	INSYNC(Instance()->m_lock);
	try
	{
		MMRESULT hr = MMSYSERR_ERROR;
		hr = Instance()->m_origWaveOutClose(hwo);
		{
			INSYNC(ms_lockHookDataPools);

			CHookWaveOutMap::iterator itr = ms_hookWaveOuts.find(hwo);
			if (itr != ms_hookWaveOuts.end())
			{
				itr->second->SetEndHook(true);
			}
		}
		return hr;
	}
	catch (...)
	{
		return MMSYSERR_ERROR;
	}
}

MMRESULT WINAPI CAudioDataHooker::HookWaveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	INSYNC(Instance()->m_lock);
	try
	{
		MMRESULT hr = MMSYSERR_ERROR;
		{
			INSYNC(ms_lockHookDataPools);
			CHookWaveOutMap::iterator itr = ms_hookWaveOuts.find(hwo);
			if (itr != ms_hookWaveOuts.end())
			{
				itr->second->WriteWaveHdr(pwh);
			}
		}
		hr = Instance()->m_origWaveOutWrite(hwo, pwh, cbwh);
		return hr;
	}
	catch (...)
	{
		return MMSYSERR_ERROR;
	}
}

MMRESULT WINAPI CAudioDataHooker::HookWaveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID, LPWAVEFORMATEX pwfx, 
	DWORD dwCallback, DWORD dwCallbackInstance, DWORD fdwOpen)
{		
	INSYNC(Instance()->m_lock);
	try
	{
		MMRESULT hr = MMSYSERR_ERROR;
		hr = Instance()->m_origWaveOutOpen(phwo, uDeviceID, pwfx, dwCallback, dwCallbackInstance, fdwOpen);
		if (CAudioDataHooker::CanHandle(pwfx->nSamplesPerSec, pwfx->nChannels))
		{
			INSYNC(ms_lockHookDataPools);
			if (phwo != NULL && ms_hookWaveOuts.find(*phwo) == ms_hookWaveOuts.end())
			{
				CAudioDataPool* pHookDataPool = new CWaveOutAudioDataPool();
				pHookDataPool->SetWaveFormatEx(pwfx);
				ms_hookDataPools.push_back(pHookDataPool);
				ms_hookWaveOuts[*phwo] = (CWaveOutAudioDataPool*)(pHookDataPool);
			}
		}
		return hr;
	}
	catch (...)
	{
		return MMSYSERR_ERROR;
	}
}

MMRESULT WINAPI CAudioDataHooker::HookWaveOutPause(HWAVEOUT hwo)
{
	INSYNC(Instance()->m_lock);
	try
	{
		MMRESULT hr = MMSYSERR_ERROR;
		hr = Instance()->m_origWaveOutPause(hwo);
		{
			INSYNC(ms_lockHookDataPools);
			CHookWaveOutMap::iterator itr = ms_hookWaveOuts.find(hwo);
			if (itr != ms_hookWaveOuts.end())
			{
				itr->second->SetEndWrite(true);
				itr->second->SetCanRead(false);
			}
		}
		return hr;
	}
	catch (...)
	{
		return MMSYSERR_ERROR;
	}
}

MMRESULT WINAPI CAudioDataHooker::HookWaveOutRestart(HWAVEOUT hwo)
{
	INSYNC(Instance()->m_lock);
	try
	{
		MMRESULT hr = MMSYSERR_ERROR;
		hr = Instance()->m_origWaveOutRestart(hwo);

		{
			INSYNC(ms_lockHookDataPools);
			CHookWaveOutMap::iterator itr = ms_hookWaveOuts.find(hwo);
			if (itr != ms_hookWaveOuts.end())
			{
				itr->second->SetEndWrite(false);
				itr->second->SetCanRead(true);
			}
		}
		return hr;
	}
	catch (...)
	{
		return MMSYSERR_ERROR;
	}
}

MMRESULT WINAPI CAudioDataHooker::HookWaveOutReset(HWAVEOUT hwo)
{
	INSYNC(Instance()->m_lock);
	try
	{
		MMRESULT hr = MMSYSERR_ERROR;
		hr = Instance()->m_origWaveOutReset(hwo);
		{
			INSYNC(ms_lockHookDataPools);
			CHookWaveOutMap::iterator itr = ms_hookWaveOuts.find(hwo);
			if (itr != ms_hookWaveOuts.end())
			{
				itr->second->Flush();
			}
		}
		return hr;
	}
	catch (...)
	{
		return MMSYSERR_ERROR;
	}
}

MMRESULT WINAPI CAudioDataHooker::HookWaveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
	MMRESULT hr = MMSYSERR_ERROR;
	DWORD curVolume;
	hr = waveOutGetVolume(hwo, &curVolume);
	if (hr != MMSYSERR_NOERROR)
	{ 
		return hr;
	}

	INSYNC(Instance()->m_lock);
	try
	{
		hr = MMSYSERR_ERROR;
		hr = Instance()->m_origWaveOutSetVolume(hwo, dwVolume);
		if (SUCCEEDED(hr))
		{
			INSYNC(ms_lockHookDataPools);
			CHookWaveOutMap::iterator itr = ms_hookWaveOuts.find(hwo);
			if (itr != ms_hookWaveOuts.end())
			{
				WORD audioVolumeLeft = 100;
				WORD audioVolumeRight = 100; 
				audioVolumeLeft = int(LOWORD(dwVolume) / 655.35);
				audioVolumeRight = int(HIWORD(dwVolume) / 655.35);	
				itr->second->SetVolume(__MAX(audioVolumeLeft, audioVolumeRight));
			}
		}
		return hr;
	}
	catch (...)
	{
		return MMSYSERR_ERROR;
	}
}

bool CAudioDataHooker::CanHandle(int sampleRate, int channel)
{
	return sampleRate <= dwHOOK_AUDIO_DATA_SAMPLERATE * 2 && channel <= dwHOOK_AUDIO_DATA_CHANNEL;
}

void CAudioDataHooker::Hook()
{
	INSYNC(m_lock);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	m_dsoundDll = LoadLibrary(_T("dsound.dll"));
	m_origDirectSoundCreate = (PFN_DIRECTSOUNDCREATE)GetProcAddress(m_dsoundDll, "DirectSoundCreate");
	DetourAttach(&(PVOID&)m_origDirectSoundCreate, &HookDirectSoundCreate);


	m_origDirectSoundCreate8 = (PFN_DIRECTSOUNDCREATE8)GetProcAddress(m_dsoundDll, "DirectSoundCreate8");
	DetourAttach(&(PVOID&)m_origDirectSoundCreate8, &HookDirectSoundCreate8);

	m_winmmDll = LoadLibrary(_T("winmm.dll"));
	m_origWaveOutClose = (PFN_WAVEOUTCLOSE)GetProcAddress(m_winmmDll, "waveOutClose");
	DetourAttach(&(PVOID&)m_origWaveOutClose, &HookWaveOutClose);

	m_origWaveOutWrite = (PFN_WAVEOUTWRITE)GetProcAddress(m_winmmDll, "waveOutWrite");
	DetourAttach(&(PVOID&)m_origWaveOutWrite, &HookWaveOutWrite);

	m_origWaveOutOpen = (PFN_WAVEOUTOPEN)GetProcAddress(m_winmmDll, "waveOutOpen");
	DetourAttach(&(PVOID&)m_origWaveOutOpen, &HookWaveOutOpen);

	m_origWaveOutPause = (PFN_WAVEOUTPAUSE)GetProcAddress(m_winmmDll, "waveOutPause");
	DetourAttach(&(PVOID&)m_origWaveOutPause, &HookWaveOutPause);

	m_origWaveOutRestart = (PFN_WAVEOUTRESTART)GetProcAddress(m_winmmDll, "waveOutRestart");
	DetourAttach(&(PVOID&)m_origWaveOutRestart, &HookWaveOutRestart);

	m_origWaveOutReset = (PFN_WAVEOUTRESET)GetProcAddress(m_winmmDll, "waveOutReset");
	DetourAttach(&(PVOID&)m_origWaveOutReset, &HookWaveOutReset);

	m_origWaveOutSetVolume = (PFN_WAVEOUTSETVOLUME)GetProcAddress(m_winmmDll, "waveOutSetVolume");
	DetourAttach(&(PVOID&)m_origWaveOutSetVolume, &HookWaveOutSetVolume);

	DetourTransactionCommit();
}

void CAudioDataHooker::UnHook()
{
	INSYNC(m_lock);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());


	DetourDetach(&(PVOID&)m_origWaveOutClose, &HookWaveOutClose);
	DetourDetach(&(PVOID&)m_origWaveOutWrite, &HookWaveOutWrite);
	DetourDetach(&(PVOID&)m_origWaveOutOpen, &HookWaveOutOpen);
	DetourDetach(&(PVOID&)m_origWaveOutPause, &HookWaveOutPause);
	DetourDetach(&(PVOID&)m_origWaveOutRestart, &HookWaveOutRestart);
	DetourDetach(&(PVOID&)m_origWaveOutReset, &HookWaveOutReset);
	DetourDetach(&(PVOID&)m_origWaveOutSetVolume, &HookWaveOutSetVolume);

	//dsound
	DetourDetach(&(PVOID&)m_origDsoundBufferLock, &HookDSoundBufferLock);
	DetourDetach(&(PVOID&)m_origDsoundBufferPlay, &HookDSoundBufferPlay);
	DetourDetach(&(PVOID&)m_origDsoundBufferRelease, &HookDSoundBufferRelease);
	DetourDetach(&(PVOID&)m_origDsoundBufferSetVolume, &HookDSoundBufferSetVolume);
	DetourDetach(&(PVOID&)m_origDsoundBufferStop, &HookDSoundBufferStop);
	DetourDetach(&(PVOID&)m_origDsoundBufferSetCurrentPosition, &HookDSoundBufferSetCurrentPosition);

	DetourDetach(&(PVOID&)m_origDsoundCreateBuffer, &HookDSoundCreateBuffer);
	DetourDetach(&(PVOID&)m_origDsound8CreateBuffer, &HookDSound8CreateBuffer);
	DetourDetach(&(PVOID&)m_origDirectSoundCreate, &HookDirectSoundCreate);
	DetourDetach(&(PVOID&)m_origDirectSoundCreate8, &HookDirectSoundCreate8);

	DetourTransactionCommit();

	if (m_dsoundDll != NULL)
	{
		FreeLibrary(m_dsoundDll);
		m_dsoundDll = NULL;
	}

	if (m_winmmDll != NULL)
	{
		FreeLibrary(m_winmmDll);
		m_winmmDll = NULL;
	}
}


