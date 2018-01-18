#ifndef _HOOK_AUDIO_DLL_H_
#define _HOOK_AUDIO_DLL_H_

#include <windows.h>
#include <tchar.h>
#include "AudioDataHooker.h"

#define HOOK_AUDIO_EXPORTS

#ifdef HOOK_AUDIO_EXPORTS
#define HOOK_AUDIO_API __declspec(dllexport)
#else
#define HOOK_AUDIO_API __declspec(dllimport)
#endif

HOOK_AUDIO_API bool InstallHookAudio(const TCHAR* pHookPlayerFilePath);
HOOK_AUDIO_API void RemoveHookAudio();

void UninstallHook();

#endif

